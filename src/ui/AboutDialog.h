#ifndef SIMPLE_NOTES_ABOUTDIALOG_H
#define SIMPLE_NOTES_ABOUTDIALOG_H

#include <QWidget>

class AboutDialog {
public:
    explicit AboutDialog(QWidget *parent = nullptr);

    void exec();

private:
    QWidget *m_parent;
};

#endif //SIMPLE_NOTES_ABOUTDIALOG_H
