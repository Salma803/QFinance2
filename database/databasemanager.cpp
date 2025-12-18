#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QSqlDatabase DatabaseManager::db;

bool DatabaseManager::openDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("finance.db");
    }

    if (!db.open()) {
        qDebug() << "Erreur ouverture DB:" << db.lastError().text();
        return false;
    }

    QSqlQuery pragma;
    pragma.exec("PRAGMA foreign_keys = ON");

    qDebug() << "Base de données ouverte";
    return true;
}

void DatabaseManager::closeDatabase()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "Base de données fermée";
    }
}

bool DatabaseManager::createSchema()
{
    QSqlQuery query;

    /* =========================
       UTILISATEUR
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Utilisateur ("
            "id TEXT PRIMARY KEY,"
            "nom TEXT NOT NULL"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    /* =========================
       COMPTE (abstrait)
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Compte ("
            "id TEXT PRIMARY KEY,"
            "utilisateur_id TEXT NOT NULL,"
            "nom TEXT NOT NULL,"
            "solde REAL NOT NULL DEFAULT 0,"
            "type TEXT NOT NULL,"
            "FOREIGN KEY (utilisateur_id) REFERENCES Utilisateur(id)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    /* =========================
       CATEGORIE (hiérarchique)
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Categorie ("
            "id TEXT PRIMARY KEY,"
            "nom TEXT NOT NULL,"
            "parent_id TEXT,"
            "utilisateur_id TEXT,"
            "FOREIGN KEY (parent_id) REFERENCES Categorie(id)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }


    /* =========================
       OPERATION (abstraite)
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Operation ("
            "id TEXT PRIMARY KEY,"
            "compte_id TEXT NOT NULL,"
            "categorie_id TEXT NOT NULL,"
            "nom TEXT NOT NULL,"
            "date TEXT NOT NULL,"
            "montant REAL NOT NULL,"
            "type TEXT NOT NULL,"
            "FOREIGN KEY (compte_id) REFERENCES Compte(id),"
            "FOREIGN KEY (categorie_id) REFERENCES Categorie(id)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    /* =========================
       REVENU
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Revenu ("
            "operation_id TEXT PRIMARY KEY,"
            "provenance TEXT NOT NULL,"
            "FOREIGN KEY (operation_id) REFERENCES Operation(id)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    /* =========================
       DEPENSE
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Depense ("
            "operation_id TEXT PRIMARY KEY,"
            "est_recurrente INTEGER NOT NULL,"
            "frequence TEXT,"
            "FOREIGN KEY (operation_id) REFERENCES Operation(id)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    /* =========================
       BUDGET
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Budget ("
            "id TEXT PRIMARY KEY,"
            "categorie_id TEXT NOT NULL,"
            "mois INTEGER NOT NULL,"
            "annee INTEGER NOT NULL,"
            "montant REAL NOT NULL,"
            "FOREIGN KEY (categorie_id) REFERENCES Categorie(id)"
            "UNIQUE (categorie_id, mois, annee)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    /* =========================
       TRANSFERT
       ========================= */
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS Transfert ("
            "id TEXT PRIMARY KEY,"
            "montant REAL NOT NULL,"
            "date TEXT NOT NULL,"
            "source_id TEXT NOT NULL,"
            "destination_id TEXT NOT NULL,"
            "FOREIGN KEY (source_id) REFERENCES Compte(id),"
            "FOREIGN KEY (destination_id) REFERENCES Compte(id)"
            ");")) {
        qDebug() << query.lastError();
        return false;
    }

    qDebug() << "Schéma de base de données créé avec succès";
    return true;
}
void DatabaseManager::ensureDefaultUser()
{
    QSqlQuery query;

    query.prepare("SELECT COUNT(*) FROM Utilisateur WHERE id = '1'");
    query.exec();
    query.next();

    if (query.value(0).toInt() == 0) {
        query.exec(
            "INSERT INTO Utilisateur (id, nom) "
            "VALUES ('1', 'Utilisateur principal')"
            );
        qDebug() << "Utilisateur par défaut créé";
    }
}
