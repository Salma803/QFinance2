#include "CompteRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QSqlDatabase>

#include "../model//CompteCourant.h"
#include "../model/CompteEpargne.h"

bool CompteRepository::ajouterCompte(const Compte* compte, const QString& utilisateurId)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Compte (id, utilisateur_id, nom, solde, type) "
        "VALUES (:id, :uid, :nom, :solde, :type)"
        );

    QString type = dynamic_cast<const CompteCourant*>(compte) ? "COURANT" : "EPARGNE";

    query.bindValue(":id", compte->getId());
    query.bindValue(":uid", utilisateurId);
    query.bindValue(":nom", compte->getNom());
    query.bindValue(":solde", compte->getSolde());
    query.bindValue(":type", type);

    if (!query.exec()) {
        qDebug() << "Erreur ajout compte:" << query.lastError();
        return false;
    }

    return true;
}

QList<Compte*> CompteRepository::chargerComptes(const QString& utilisateurId)
{
    QList<Compte*> comptes;

    QSqlQuery query;
    query.prepare(
        "SELECT id, nom, solde, type "
        "FROM Compte WHERE utilisateur_id = :uid"
        );
    query.bindValue(":uid", utilisateurId);

    if (!query.exec()) {
        qDebug() << "Erreur chargement comptes:" << query.lastError();
        return comptes;
    }

    while (query.next()) {
        QString id = query.value("id").toString();
        QString nom = query.value("nom").toString();
        double solde = query.value("solde").toDouble();
        QString type = query.value("type").toString();

        Compte* compte = nullptr;

        if (type == "COURANT") {
            compte = new CompteCourant(id, nom, solde);
        } else {
            compte = new CompteEpargne(id, nom, solde);
        }

        comptes.append(compte);
    }

    return comptes;
}
double CompteRepository::calculerSolde(const QString& compteId)
{
    double solde = 0.0;
    QSqlQuery query;

    // 1️⃣ Revenus
    query.prepare(
        "SELECT IFNULL(SUM(montant), 0) "
        "FROM Operation "
        "WHERE compte_id = :id AND type = 'REVENU'"
        );
    query.bindValue(":id", compteId);
    query.exec();
    query.next();
    solde += query.value(0).toDouble();

    // 2️⃣ Dépenses
    query.prepare(
        "SELECT IFNULL(SUM(montant), 0) "
        "FROM Operation "
        "WHERE compte_id = :id AND type = 'DEPENSE'"
        );
    query.bindValue(":id", compteId);
    query.exec();
    query.next();
    solde -= query.value(0).toDouble();

    // 3️⃣ Transferts entrants
    query.prepare(
        "SELECT IFNULL(SUM(montant), 0) "
        "FROM Transfert "
        "WHERE destination_id = :id"
        );
    query.bindValue(":id", compteId);
    query.exec();
    query.next();
    solde += query.value(0).toDouble();

    // 4️⃣ Transferts sortants
    query.prepare(
        "SELECT IFNULL(SUM(montant), 0) "
        "FROM Transfert "
        "WHERE source_id = :id"
        );
    query.bindValue(":id", compteId);
    query.exec();
    query.next();
    solde -= query.value(0).toDouble();

    return solde;
}
void CompteRepository::mettreAJourSolde(const QString& compteId)
{
    double solde = calculerSolde(compteId);

    QSqlQuery query;
    query.prepare(
        "UPDATE Compte SET solde = :s WHERE id = :id"
        );
    query.bindValue(":s", solde);
    query.bindValue(":id", compteId);
    query.exec();
}

QString CompteRepository::trouverCompteCourantAvecSoldeSuffisant(
    const QString& utilisateurId,
    const QString& compteExcluId,
    double montantMin)
{
    QSqlQuery query;
    query.prepare(
        "SELECT id FROM Compte "
        "WHERE utilisateur_id = :user "
        "AND id != :exclude "
        "AND type = 'COURANT'"
        );

    query.bindValue(":user", utilisateurId);
    query.bindValue(":exclude", compteExcluId);

    if (!query.exec()) {
        qDebug() << "Erreur recherche compte source:" << query.lastError();
        return "";
    }

    QString meilleurCompte;
    double meilleurSolde = 0;

    while (query.next()) {
        QString id = query.value(0).toString();
        double solde = calculerSolde(id);

        qDebug() << "Compte" << id << "solde =" << solde;

        if (solde >= montantMin && solde > meilleurSolde) {
            meilleurSolde = solde;
            meilleurCompte = id;
        }
    }

    return meilleurCompte;
}


bool CompteRepository::supprimerCompteEtDependances(const QString& compteId)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "DB non ouverte";
        return false;
    }

    if (!db.transaction()) {
        qDebug() << "Transaction impossible:" << db.lastError();
        return false;
    }

    auto execDelete = [&](const QString& sql) -> bool {
        QSqlQuery q(db);
        q.prepare(sql);
        q.addBindValue(compteId);
        if (!q.exec()) {
            qDebug() << "Erreur SQL:" << q.lastError().text();
            qDebug() << "SQL:" << sql;
            return false;
        }
        return true;
    };

    // ✅ 1) Supprimer opérations
    if (!execDelete("DELETE FROM Operation WHERE compte_id = ?")) {
        db.rollback();
        return false;
    }

    // ✅ 2) Supprimer transferts SORTANTS
    if (!execDelete("DELETE FROM Transfert WHERE source_id = ?")) {
        db.rollback();
        return false;
    }

    // ✅ 3) Supprimer transferts ENTRANTS
    if (!execDelete("DELETE FROM Transfert WHERE destination_id = ?")) {
        db.rollback();
        return false;
    }

    // ✅ 4) Supprimer le compte
    {
        QSqlQuery q(db);
        q.prepare("DELETE FROM Compte WHERE id = ?");
        q.addBindValue(compteId);
        if (!q.exec()) {
            qDebug() << "Erreur suppression compte:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        qDebug() << "Commit impossible:" << db.lastError();
        db.rollback();
        return false;
    }

    return true;
}
