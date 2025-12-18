#include "CompteRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

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
bool CompteRepository::mettreAJourSolde(const Compte* compte)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE Compte SET solde = :solde WHERE id = :id"
        );
    query.bindValue(":solde", compte->getSolde());
    query.bindValue(":id", compte->getId());

    if (!query.exec()) {
        qDebug() << "Erreur mise à jour solde:" << query.lastError();
        return false;
    }
    return true;
}
