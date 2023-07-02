#ifndef SIMPLE_NOTES_NOTEEDITORDIALOG_H
#define SIMPLE_NOTES_NOTEEDITORDIALOG_H

#include <QDialog>

#include "backend/Note.h"

/**
 * NoteEditorDialog
 * @note: designed to use only as modal dialog
 * @ref: https://doc.qt.io/qt-6/qdialog.html#code-examples
 */
class NoteEditorDialog : public QDialog {
public:
    explicit NoteEditorDialog(Note &note, QWidget *parent = nullptr);

private:
    Note &m_note;
};

#endif //SIMPLE_NOTES_NOTEEDITORDIALOG_H
