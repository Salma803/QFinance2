#include "mainwindow.h"
#include "./database/databasemanager.h"
#include "./repository/categorierepository.h"

#include <QDebug>
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "QFinance2_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    if (!DatabaseManager::openDatabase())
        return -1;

    DatabaseManager::createSchema();
    DatabaseManager::ensureDefaultUser();
    CategorieRepository::creerCategoriesParDefaut();


    MainWindow w;
    w.show();
    return a.exec();
}
