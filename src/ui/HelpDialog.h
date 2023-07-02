#ifndef SIMPLE_NOTES_HELPDIALOG_H
#define SIMPLE_NOTES_HELPDIALOG_H

#include <QMessageBox>

/**
 * HelpDialog
 * @note: designed to use only as modal dialog
 * @ref: https://doc.qt.io/qt-6/qdialog.html#code-examples
 */
class HelpDialog : public QMessageBox {
public:
    explicit HelpDialog(QWidget *parent = nullptr);
};

#endif //SIMPLE_NOTES_HELPDIALOG_H
