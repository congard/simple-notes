#ifndef SIMPLE_NOTES_NOTEPROVIDER_H
#define SIMPLE_NOTES_NOTEPROVIDER_H

#include <QMutex>

#include "backend/Note.h"

class NoteProvider : public QObject {
    Q_OBJECT

public:
    static NoteProvider& getInstance();

    ~NoteProvider() override;

    /**
     * Adds a note. Thread-safe.
     * @param title of the note
     * @param body of the note
     */
    void addNote(const QString &title, const QString &body);

    /**
     * Adds a note. Thread-safe.
     * @param note to add
     */
    void addNote(Note &note);

    /**
     * Removes the note with the specified id. Thread-safe.
     * @param id of the note
     */
    void removeNote(const QString &id);

    /**
     * Thread-safe function.
     * @return all notes
     */
    QVector<Note*> getNotes();

    void load();
    void save();

    /**
     * Performs synchronization. Warning: it will rebase changes, not merge.
     * Thread safe.
     * @param statusHandler
     */
    void sync(const IStatusHandlerPtr<> &statusHandler);

    /**
     * Deletes instance of NoteProvider
     */
    static void destroy();

signals:
    void notesUpdated();

private:
    NoteProvider();

    QJsonObject toJson() const;

private:
    static std::unique_ptr<NoteProvider> m_instance;

    QVector<Note> m_notes;
    QVector<Note> m_addedNotes;
    QVector<QString> m_removedIds;

    QMutex m_syncMutex;

    bool m_isSyncing;
};

#endif //SIMPLE_NOTES_NOTEPROVIDER_H
