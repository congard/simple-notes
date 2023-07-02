#include "HelpDialog.h"
#include <QMessageBox>

HelpDialog::HelpDialog(QWidget *parent)
    : QMessageBox(parent)
{
    QString translatedTextHelp = tr(
        "<p>New note can be created by clicking add (\"+\") button, by clicking "
        "\"File\" => \"New note\" or via shortcut <i>Ctrl + N</i>.</p>"

        "<p>You can delete your note by clicking remove (\"-\") button near the note "
        "you want to remove.</p>"

        "<p>In order to synchronize your local notes with remote server, you need:</p>"
        "<ol>"
        "<li>Click \"File\" => \"Sign in / up\"</li>"
        "<li>Enter your data (email and password) and click \"Sign in\" if you have "
        "already created an account or \"Sign up\" if you haven't</li>"
        "<li>Now you can sync your notes: click \"File\" => \"Synchronize\" or "
        "sync button in the toolbar</li>"
        "<li>Now all your notes are synchronized with the server</li>"
        "</ol>"
        "<p><b>Note:</b> this app can be used offline (without need to log in), but in that "
        "case you won't be able to synchronize your notes.</p>");

    setWindowTitle(tr("Help"));
    setText(translatedTextHelp);
}
