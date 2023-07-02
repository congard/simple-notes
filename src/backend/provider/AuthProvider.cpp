#include "AuthProvider.h"
#include "backend/FirebaseApp.h"

std::unique_ptr<AuthProvider> AuthProvider::m_instance;

AuthProvider& AuthProvider::getInstance() {
    if (m_instance == nullptr)
        m_instance.reset(new AuthProvider());
    return *m_instance;
}

void AuthProvider::signIn(const QString &email, const QString &password, const IStatusHandlerPtr<> &statusHandler) {
    m_auth->SignInWithEmailAndPassword(qPrintable(email), qPrintable(password))
        .OnCompletion(getAuthCallback(statusHandler));
}

void AuthProvider::signUp(const QString &email, const QString &password, const IStatusHandlerPtr<> &statusHandler) {
    m_auth->CreateUserWithEmailAndPassword(qPrintable(email), qPrintable(password))
        .OnCompletion(getAuthCallback(statusHandler));
}

void AuthProvider::signOut() {
    m_auth->SignOut();
    changeStatus(Status::SignedOut);
}

void AuthProvider::refresh(const IStatusHandlerPtr<> &statusHandler) {
    if (!m_auth->current_user().is_valid()) {
        changeStatus(Status::SignedOut);
        return;
    }

    m_auth->current_user().Reload().OnCompletion([statusHandler, this](const firebase::Future<void> &res) {
        if (res.error() != 0) {
            statusHandler->onError(Error::firebase(res.error(), res.error_message()));
            changeStatus(Status::Unknown);
        } else {
            statusHandler->onSuccess();
            changeStatus(Status::SignedIn);
        }
    });
}

bool AuthProvider::isValid() const {
    return m_auth->current_user().is_valid();
}

QString AuthProvider::uid() const {
    return QString::fromStdString(m_auth->current_user().uid());
}

QString AuthProvider::email() const {
    return QString::fromStdString(m_auth->current_user().email());
}

AuthProvider::Status AuthProvider::status() const {
    return m_status;
}

AuthProvider::AuthProvider()
    : m_auth(firebase::auth::Auth::GetAuth(&FirebaseApp::getInstance().getApp())),
      m_status(Status::Unknown)
{
    qRegisterMetaType<AuthProvider::Status>("AuthProvider::Status");

    if (!isValid()) {
        m_status = Status::SignedOut;
    }
}

void AuthProvider::changeStatus(Status status) {
    m_status = status;
    statusChanged(status);
}

std::function<void(const firebase::Future<firebase::auth::AuthResult> &)>
AuthProvider::getAuthCallback(const IStatusHandlerPtr<> &statusHandler) {
    return [this, statusHandler](const firebase::Future<firebase::auth::AuthResult> &res) {
        if (res.error() != 0) {
            statusHandler->onError(Error::firebase(res.error(), res.error_message()));
            changeStatus(Status::Unknown);
        } else {
            statusHandler->onSuccess();
            changeStatus(Status::SignedIn);
        }
    };
}

void AuthProvider::destroy() {
    m_instance.reset();
}
