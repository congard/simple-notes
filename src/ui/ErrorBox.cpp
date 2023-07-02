#include "ErrorBox.h"
#include "App.h"

ErrorBox::ErrorBox(const Error &error, QWidget *parent)
    : QMessageBox(parent)
{
    setWindowTitle(tr("Error"));

    QString errorTypeStr = [&]() {
        switch (error.type()) {
            case Error::Type::Unknown: return tr("unknown");
            case Error::Type::Internal: return tr("internal");
            case Error::Type::Firebase: return tr("firebase");
        }
    }();

    setText(error.message());

    setDetailedText(tr("%1\nError code: %2\nType: %3")
        .arg(error.message())
        .arg(error.code())
        .arg(errorTypeStr));
}

void ErrorBox::handler(const Error &error) {
    App::getInstance()->invokeLater(QRunnable::create([error]() {
        ErrorBox box(error);
        box.exec();
    }));
}
