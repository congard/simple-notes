#include "FirebaseApp.h"
#include "provider/AuthProvider.h"
#include "provider/DatabaseProvider.h"
#include "provider/NoteProvider.h"

std::unique_ptr<FirebaseApp> FirebaseApp::m_instance {nullptr};

/*
 * Providers must be destroyed no later than the FirebaseApp is destroyed.
 * Otherwise, it will lead to undefined behaviour and / or segmentation fault.
 */
FirebaseApp::~FirebaseApp() {
    NoteProvider::destroy();
    DatabaseProvider::destroy();
    AuthProvider::destroy();
}

FirebaseApp& FirebaseApp::getInstance() {
    if (m_instance == nullptr)
        m_instance.reset(new FirebaseApp());
    return *m_instance;
}

firebase::App& FirebaseApp::getApp() {
    return *m_app;
}

firebase::App* FirebaseApp::operator->() {
    return &getApp();
}

void FirebaseApp::destroy() {
    m_instance.reset();
}

FirebaseApp::FirebaseApp()
    : m_app(firebase::App::Create()) {}
