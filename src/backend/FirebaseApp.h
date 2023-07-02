#ifndef SIMPLE_NOTES_FIREBASEAPP_H
#define SIMPLE_NOTES_FIREBASEAPP_H

#include <memory>

#include <firebase/app.h>

class FirebaseApp {
public:
    ~FirebaseApp();

    static FirebaseApp& getInstance();

    firebase::App& getApp();
    firebase::App* operator->();

    static void destroy();

private:
    FirebaseApp();

private:
    static std::unique_ptr<FirebaseApp> m_instance;
    std::unique_ptr<firebase::App> m_app;
};

#endif //SIMPLE_NOTES_FIREBASEAPP_H
