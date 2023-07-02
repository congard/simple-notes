#include "AuthDialog.h"

#include "backend/provider/AuthProvider.h"
#include "backend/StatusHandler.h"
#include "ErrorBox.h"
#include "App.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

AuthDialog::AuthDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Auth"));

    auto edEmail = new QLineEdit();
    edEmail->setPlaceholderText(tr("Email"));

    auto edPassword = new QLineEdit();
    edPassword->setPlaceholderText(tr("Password"));
    edPassword->setEchoMode(QLineEdit::Password);

    auto btnSignIn = new QPushButton(tr("Sign in"));
    auto btnSignUp = new QPushButton(tr("Sign up"));

    auto buttonBox = new QDialogButtonBox();
    buttonBox->addButton(btnSignIn, QDialogButtonBox::ActionRole);
    buttonBox->addButton(btnSignUp, QDialogButtonBox::ActionRole);

    auto layout = new QVBoxLayout();
    layout->addWidget(edEmail);
    layout->addWidget(edPassword);
    layout->addWidget(buttonBox);

    setLayout(layout);

    auto statusHandler = StatusHandler<>::both([this]() {
        App::getInstance()->invokeLater(QRunnable::create([this]() {
            close();
        }));
    }, ErrorBox::handler);

    connect(btnSignIn, &QPushButton::clicked, [=]() {
        auto &auth = AuthProvider::getInstance();
        auth.signIn(edEmail->text(), edPassword->text(), statusHandler);
    });

    connect(btnSignUp, &QPushButton::clicked, [=]() {
        auto &auth = AuthProvider::getInstance();
        auth.signUp(edEmail->text(), edPassword->text(), statusHandler);
    });
}
