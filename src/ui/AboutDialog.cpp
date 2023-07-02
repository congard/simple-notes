#include "AboutDialog.h"
#include <QMessageBox>

AboutDialog::AboutDialog(QWidget *parent)
    : m_parent(parent) {}

void AboutDialog::exec() {
    QString translatedTextAbout = QMessageBox::tr(
        "<p>This project is developed by congard within the subject "
        "of C++ Programming Language at the AGH University of Science and Technology</p>"
        "<p>Distributed under the BSD 4-clause license</p>"
        "<p>Homepage: <a href=\"https://github.com/congard/simple-notes\">simple-notes</a>"
        "<br>My GitHub: <a href=\"https://github.com/congard\">congard</a></p>");
    QMessageBox::about(m_parent, QMessageBox::tr("About SimpleNotes"), translatedTextAbout);
}
