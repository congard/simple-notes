#ifndef SIMPLE_NOTES_DATABASEPROVIDER_H
#define SIMPLE_NOTES_DATABASEPROVIDER_H

#include <memory>
#include <firebase/database.h>

class DatabaseProvider {
    friend class FirebaseApp;

public:
    static DatabaseProvider& getInstance();

    firebase::database::Database* getDb() const;
    firebase::database::Database* operator->() const;

private:
    DatabaseProvider();

    static void destroy();

private:
    static std::unique_ptr<DatabaseProvider> m_instance;
    std::unique_ptr<firebase::database::Database> m_db;
};

#endif //SIMPLE_NOTES_DATABASEPROVIDER_H
