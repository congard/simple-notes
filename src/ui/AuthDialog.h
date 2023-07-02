#ifndef SIMPLE_NOTES_AUTHDIALOG_H
#define SIMPLE_NOTES_AUTHDIALOG_H

#include <QDialog>

/**
 * AuthDialog
 * @note: designed to use only as modal dialog
 * @ref: https://doc.qt.io/qt-6/qdialog.html#code-examples
 */
class AuthDialog : public QDialog {
public:
    explicit AuthDialog(QWidget *parent = nullptr);
};

#endif //SIMPLE_NOTES_AUTHDIALOG_H
