#ifndef SIMPLE_NOTES_NOTE_H
#define SIMPLE_NOTES_NOTE_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

#include <firebase/database.h>
#include <firebase/variant.h>

#include "IStatusHandler.h"

class Note {
public:
    Note();
    explicit Note(const QJsonObject &obj);
    Note(QString id, const std::map<firebase::Variant, firebase::Variant> &data);

    bool operator==(const Note &rhs) const;
    bool operator!=(const Note &rhs) const;

    void setId(const QString &id);
    void setTitle(const QString &title);
    void setBody(const QString &body);
    void setDate(const QDateTime &date);

    const QString& getId() const;
    const QString& getTitle() const;
    const QString& getBody() const;
    const QDateTime& getDate() const;

    QJsonObject toJson() const;

    std::map<std::string, firebase::Variant> toFirebaseMap() const;

private:
    QString m_id;
    QString m_title;
    QString m_body;
    QDateTime m_date;
};

#endif //SIMPLE_NOTES_NOTE_H
