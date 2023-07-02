#ifndef SIMPLE_NOTES_NOTEWIDGET_H
#define SIMPLE_NOTES_NOTEWIDGET_H

#include <QWidget>

#include "backend/Note.h"

class NoteWidget : public QWidget {
    // needed in order to call findChildren
    Q_OBJECT

public:
    explicit NoteWidget(const Note &note, QWidget *parent = nullptr);

    void display(const Note &note);

    const QString& getNoteId() const;

private:
    QLayout* mkHeader();
    static QLayout* mkContent();
    static QLayout* mkFooter();

private:
    QString m_noteId;
};

#endif //SIMPLE_NOTES_NOTEWIDGET_H
