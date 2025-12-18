#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static bool openDatabase();
    static void closeDatabase();
    static bool createSchema();
    static void ensureDefaultUser();


private:
    static QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
