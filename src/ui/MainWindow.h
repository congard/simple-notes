#ifndef SIMPLE_NOTES_MAINWINDOW_H
#define SIMPLE_NOTES_MAINWINDOW_H

#include <QMainWindow>

#include "backend/provider/AuthProvider.h"

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void statusChanged(AuthProvider::Status status);
    void notesUpdated();

    static void signAction();

    static void showAddNoteWindow();

    void showAboutDialog();
    void showHelpDialog();

private:
    QMenuBar* mkMenuBar();
    QToolBar* mkToolBar();

    static QStatusBar* mkStatusBar();
    static QWidget* mkContent();

private:
    QAction* getMenuBarAction(const QString &name);

private:
    void signedIn();
    void signedOut();

    static void showAuthWindow();
    static void checkAuthStatus();

    void sync();

private:
    void saveSettings();
    void loadSettings();
};

#endif //SIMPLE_NOTES_MAINWINDOW_H
