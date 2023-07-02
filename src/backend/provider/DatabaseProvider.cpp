#include "DatabaseProvider.h"
#include "backend/FirebaseApp.h"

std::unique_ptr<DatabaseProvider> DatabaseProvider::m_instance;

DatabaseProvider& DatabaseProvider::getInstance() {
    if (m_instance == nullptr)
        m_instance.reset(new DatabaseProvider());
    return *m_instance;
}

firebase::database::Database* DatabaseProvider::getDb() const {
    return m_db.get();
}

firebase::database::Database* DatabaseProvider::operator->() const {
    return getDb();
}

DatabaseProvider::DatabaseProvider()
    : m_db(firebase::database::Database::GetInstance(&FirebaseApp::getInstance().getApp())) {}

void DatabaseProvider::destroy() {
    m_instance.reset();
}
