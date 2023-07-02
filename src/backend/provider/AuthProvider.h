#ifndef SIMPLE_NOTES_AUTHPROVIDER_H
#define SIMPLE_NOTES_AUTHPROVIDER_H

#include <QObject>
#include <QString>

#include <memory>
#include <firebase/auth.h>

#include "backend/IStatusHandler.h"

class AuthProvider : public QObject {
    Q_OBJECT

    friend class FirebaseApp;

public:
    enum class Status {
        Unknown,
        SignedOut,
        SignedIn
    };

public:
    static AuthProvider& getInstance();

    void signIn(const QString &email, const QString &password, const IStatusHandlerPtr<> &statusHandler);
    void signUp(const QString &email, const QString &password, const IStatusHandlerPtr<> &statusHandler);
    void signOut();

    /**
     * Checks whether the user is still logged in
     * @param statusHandler
     */
    void refresh(const IStatusHandlerPtr<> &statusHandler);

    bool isValid() const;
    QString uid() const;
    QString email() const;

    Status status() const;

signals:
    void statusChanged(AuthProvider::Status status);

private:
    AuthProvider();

    void changeStatus(Status status);

    std::function<void(const firebase::Future<firebase::auth::AuthResult>&)>
            getAuthCallback(const IStatusHandlerPtr<> &statusHandler);

    static void destroy();

private:
    static std::unique_ptr<AuthProvider> m_instance;
    std::unique_ptr<firebase::auth::Auth> m_auth;
    Status m_status;
};

#endif //SIMPLE_NOTES_AUTHPROVIDER_H
