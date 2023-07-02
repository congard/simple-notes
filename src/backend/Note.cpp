#include "Note.h"

#include <utility>

namespace Key {
constexpr static auto Id = "id";
constexpr static auto Title = "title";
constexpr static auto Body = "body";
constexpr static auto Date = "date";
}

Note::Note()
    : m_id(), m_title(), m_body(), m_date(QDateTime::currentDateTime()) {}

Note::Note(const QJsonObject &obj)
    : m_id(obj.value(Key::Id).toString("")),
      m_title(obj.value(Key::Title).toString("")),
      m_body(obj.value(Key::Body).toString("")),
      m_date(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(obj.value(Key::Date).toDouble()))) {}

Note::Note(QString id, const std::map<firebase::Variant, firebase::Variant> &data)
    : m_id(std::move(id))
{
    for (const auto & [vKey, vValue] : data) {
        std::string_view key = vKey.string_value();

        if (key == Key::Title) {
            setTitle(vValue.string_value());
        } else if (key == Key::Body) {
            setBody(vValue.string_value());
        } else if (key == Key::Date) {
            setDate(QDateTime::fromMSecsSinceEpoch(vValue.int64_value()));
        } else {
            fprintf(stderr, "Warning: unknown key '%s'\n", key.data());
        }
    }
}

bool Note::operator==(const Note &rhs) const {
    return getId() == rhs.getId() &&
           getTitle() == rhs.getTitle() &&
           getBody() == rhs.getBody() &&
           getDate() == rhs.getDate();
}

bool Note::operator!=(const Note &rhs) const {
    return !(*this == rhs);
}

void Note::setId(const QString &id) {
    m_id = id;
}

void Note::setTitle(const QString &title) {
    m_title = title;
}

void Note::setBody(const QString &body) {
    m_body = body;
}

void Note::setDate(const QDateTime &date) {
    m_date = date;
}

const QString& Note::getId() const {
    return m_id;
}

const QString& Note::getTitle() const {
    return m_title;
}

const QString& Note::getBody() const {
    return m_body;
}

const QDateTime& Note::getDate() const {
    return m_date;
}

QJsonObject Note::toJson() const {
    QJsonObject result;
    result[Key::Id] = m_id;
    result[Key::Title] = m_title;
    result[Key::Body] = m_body;
    result[Key::Date] = m_date.toMSecsSinceEpoch();
    return result;
}

std::map<std::string, firebase::Variant> Note::toFirebaseMap() const {
    return { // map
        { // pair
            m_id.toStdString(), std::map<std::string, firebase::Variant> {
                {Key::Title, m_title.toStdString()},
                {Key::Body, m_body.toStdString()},
                {Key::Date, static_cast<int64_t>(m_date.toMSecsSinceEpoch())}
            }
        }
    };
}
