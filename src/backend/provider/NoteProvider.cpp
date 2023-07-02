#include "NoteProvider.h"
#include "AuthProvider.h"
#include "DatabaseProvider.h"
#include "backend/StatusHandler.h"
#include "backend/IdGenerator.h"
#include "Details.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QMutexLocker>
#include <QSettings>

#include <algorithm>
#include <variant>

constexpr static auto SettingsKey = "NoteProvider_notes";

namespace Key {
constexpr static auto Notes = "notes";
constexpr static auto AddedNotes = "addedNotes";
constexpr static auto RemovedNoteIds = "removedIds";
}

namespace RemoteKey {
constexpr static auto Notes = "notes";
}

std::unique_ptr<NoteProvider> NoteProvider::m_instance {nullptr};

NoteProvider& NoteProvider::getInstance() {
    if (m_instance == nullptr)
        m_instance.reset(new NoteProvider());
    return *m_instance;
}

NoteProvider::~NoteProvider() {
    save();
}

void NoteProvider::addNote(const QString &title, const QString &body) {
    Note note;
    note.setTitle(title);
    note.setBody(body);
    note.setDate(QDateTime::currentDateTime());
    addNote(note);
}

void NoteProvider::addNote(Note &note) {
    {
        QMutexLocker locker(&m_syncMutex);
        m_addedNotes.push_back(std::move(note));
    }

    notesUpdated();
}

void NoteProvider::removeNote(const QString &id) {
    auto removeFrom = [&id](QVector<Note> &notes) -> bool {
        auto predId = [&id](const Note &note) {
            return note.getId() == id;
        };

        if (auto it = std::find_if(notes.begin(), notes.end(), predId); it != notes.end()) {
            notes.remove(static_cast<int>(it - notes.begin()));
            return true;
        }

        return false;
    };

    {
        QMutexLocker locker(&m_syncMutex);

        if (removeFrom(m_notes)) {
            m_removedIds.push_back(id);
        } else {
            assert(removeFrom(m_addedNotes));
        }
    }

    notesUpdated();
}

QVector<Note*> NoteProvider::getNotes() {
    QMutexLocker locker(&m_syncMutex);

    QVector<Note*> result;
    result.reserve(m_notes.size() + m_addedNotes.size());

    for (auto &note : m_notes)
        result.push_back(&note);

    for (auto &note : m_addedNotes)
        result.push_back(&note);

    return result;
}

static QJsonObject toJsonObject(const QByteArray &bytes) {
    QJsonDocument doc = QJsonDocument::fromJson(bytes);

    // check validity of the document
    if (!doc.isNull()) {
        if (doc.isObject()) {
            return doc.object();
        } else {
            qDebug() << "Document is not an object\n";
        }
    } else {
        qDebug() << "Invalid JSON:\n" << bytes << "\n";
    }

    return {};
}

template<typename T>
static void fromJson(T &out, const QJsonValue&);

template<>
void fromJson(QString &out, const QJsonValue &val) {
    out = val.toString();
}

template<>
void fromJson(Note &out, const QJsonValue &val) {
    out = Note(val.toObject());
}

template<typename T>
void fromJson(QVector<T> &out, const QJsonValue &jsonValue) {
    out = {};

    if (jsonValue.isNull())
        return;

    for (auto el : jsonValue.toArray()) {
        T val;
        fromJson(val, el);
        out.push_back(std::move(val));
    }
}

void NoteProvider::load() {
    QSettings settings(Details::Organization, Details::AppName);

    auto notes = settings.value(SettingsKey).toByteArray();

    if (notes.isEmpty())
        return;

    auto obj = toJsonObject(notes);

    fromJson(m_notes, obj.value(Key::Notes));
    fromJson(m_addedNotes, obj.value(Key::AddedNotes));
    fromJson(m_removedIds, obj.value(Key::RemovedNoteIds));

    notesUpdated();
}

void NoteProvider::save() {
    QJsonDocument doc(toJson());
    QByteArray data(doc.toJson(QJsonDocument::Compact));

    QSettings settings(Details::Organization, Details::AppName);
    settings.setValue(SettingsKey, data);
}

/**
 * Reads notes from DataSnapshot
 * @tparam T supported types: `Note`, `QString`.
 * If `T` is `QString`, then list of ids will be returned
 * @param snapshot
 * @return vector of notes or vector of note ids, depending on the `T` type
 */
template<typename T>
static std::variant<QVector<T>, Error> readNotes(const firebase::database::DataSnapshot *snapshot) {
    constexpr bool isNote = std::is_same_v<T, Note>;
    constexpr bool isString = std::is_same_v<T, QString>;

    static_assert(isNote || isString, "Unsupported type: only Note and QString are supported");

    auto remoteNotes = snapshot->value();

    if (remoteNotes.is_null())
        return QVector<T> {};

    if (!remoteNotes.is_map())
        return Error::internal(ErrorCode::InvalidType, "Invalid type: map expected");

    QVector<T> notes;

    for (auto & [vKey, vValue] : remoteNotes.map()) {
        if (!vValue.is_map())
            return Error::internal(ErrorCode::InvalidType, "Invalid type: map expected");

        auto noteId = vKey.string_value();
        auto &note = vValue.map();

        if constexpr (std::is_same_v<T, Note>) {
            notes.push_back({noteId, note});
        } else if constexpr (std::is_same_v<T, QString>) {
            notes.push_back(noteId);
        }
    }

    return notes;
}

void NoteProvider::sync(const IStatusHandlerPtr<> &statusHandler) {
    auto &auth = AuthProvider::getInstance();

    if (!auth.isValid()) {
        statusHandler->onError(Error::internal(ErrorCode::InvalidState, "Invalid user"));
        return;
    }

    if (m_isSyncing) {
        statusHandler->onError(Error::internal(ErrorCode::InvalidState, "Synchronization is active now"));
        return;
    }

    m_isSyncing = true;

    auto resultCallback = [statusHandler, this](const firebase::Future<firebase::database::DataSnapshot> &res) {
        m_isSyncing = false;

        if (res.error() != 0) {
            statusHandler->onError(Error::firebase(res.error(), res.error_message()));
            return;
        }

        if (auto notesRes = readNotes<Note>(res.result()); std::holds_alternative<QVector<Note>>(notesRes)) {
            QMutexLocker locker(&m_syncMutex);
            auto &notes = std::get<QVector<Note>>(notesRes);
            std::swap(m_notes, notes);
        } else {
            statusHandler->onError(std::get<Error>(notesRes));
        }

        // Warning: m_syncMutex must be unlocked here, otherwise it can cause deadlock:
        //   sync { sync { ... } }
        //          ^^^^
        //          here

        notesUpdated();
        statusHandler->onSuccess();
    };

    auto dbRef = DatabaseProvider::getInstance()->GetReference(RemoteKey::Notes)
        .Child(qPrintable(auth.uid()));

    dbRef.GetValue().OnCompletion([=](const firebase::Future<firebase::database::DataSnapshot> &res) mutable {
        auto remoteNotesRes = readNotes<QString>(res.result());

        if (std::holds_alternative<Error>(remoteNotesRes)) {
            statusHandler->onError(std::get<Error>(remoteNotesRes));
            return;
        }

        auto &vecRemoteNotes = std::get<QVector<QString>>(remoteNotesRes);
        QSet<QString> remoteNotes(vecRemoteNotes.begin(), vecRemoteNotes.end());

        m_syncMutex.lock();

        // detect deleted on remote
        // if local note doesn't exist in remote - remove it
        m_notes.erase(std::remove_if(m_notes.begin(), m_notes.end(), [&remoteNotes](const Note &note) {
            return remoteNotes.find(note.getId()) == remoteNotes.end();
        }), m_notes.end());

        // create map of changes
        std::map<std::string, firebase::Variant> updatedNotes;

        // 1st stage: remove
        for (auto &id : m_removedIds) {
            updatedNotes[id.toStdString()] = firebase::Variant::Null();
        }

        m_removedIds.clear();

        // 2nd stage: add
        for (auto &note : m_addedNotes) {
            QString id = IdGenerator::generate();
            note.setId(id);
            m_notes.push_back(std::move(note));
        }

        m_addedNotes.clear();

        // 3rd stage: push & rebase
        for (auto &note : m_notes) {
            updatedNotes.merge(note.toFirebaseMap());
        }

        m_syncMutex.unlock();

        dbRef.UpdateChildren(updatedNotes)
            .OnCompletion([=](const firebase::Future<void> &res) mutable {
                if (res.error() != 0) {
                    statusHandler->onError(Error::firebase(res.error(), res.error_message()));
                } else {
                    dbRef.GetValue().OnCompletion(resultCallback);
                }
            });
    });
}

void NoteProvider::destroy() {
    m_instance.reset();
}

NoteProvider::NoteProvider()
    : m_isSyncing(false)
{
    load();
}

template<typename T>
static QJsonValue toJson(const T&);

// Explicit specializations are not allowed to use storage class specifiers,
// such as static. They should have the same linkage as the primary template.

template<>
QJsonValue toJson(const QString &str) {
    return str;
}

template<>
QJsonValue toJson(const Note &note) {
    return note.toJson();
}

template<typename T>
QJsonValue toJson(const QVector<T> &qVector) {
    QJsonArray result;

    for (auto &value : qVector)
        result.push_back(toJson(value));

    return result;
}

QJsonObject NoteProvider::toJson() const {
    QJsonObject result;
    result[Key::Notes] = ::toJson(m_notes);
    result[Key::AddedNotes] = ::toJson(m_addedNotes);
    result[Key::RemovedNoteIds] = ::toJson(m_removedIds);
    return result;
}
