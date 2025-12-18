#include "OperationRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QDebug>

bool OperationRepository::ajouterRevenu(const QString& nom,
                                        const QDate& date,
                                        double montant,
                                        const QString& compteId,
                                        const QString& categorieId,
                                        const QString& provenance)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Operation "
        "(id, nom, date, montant, type, compte_id, categorie_id, provenance) "
        "VALUES (:id, :nom, :date, :montant, 'REVENU', :compte, :categorie, :prov)"
        );

    query.bindValue(":id", QUuid::createUuid().toString());
    query.bindValue(":nom", nom);
    query.bindValue(":date", date.toString(Qt::ISODate));
    query.bindValue(":montant", montant);
    query.bindValue(":compte", compteId);
    query.bindValue(":categorie", categorieId);
    query.bindValue(":prov", provenance);

    if (!query.exec()) {
        qDebug() << "Erreur ajout revenu:" << query.lastError().text();
        return false;
    }

    return true;
}

bool OperationRepository::ajouterDepense(const QString& nom,
                                         const QDate& date,
                                         double montant,
                                         const QString& compteId,
                                         const QString& categorieId,
                                         bool estRecurrente,
                                         const QString& frequence)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Operation "
        "(id, nom, date, montant, type, compte_id, categorie_id, est_recurrente, frequence) "
        "VALUES (:id, :nom, :date, :montant, 'DEPENSE', :compte, :categorie, :rec, :freq)"
        );

    query.bindValue(":id", QUuid::createUuid().toString());
    query.bindValue(":nom", nom);
    query.bindValue(":date", date.toString(Qt::ISODate));
    query.bindValue(":montant", montant);
    query.bindValue(":compte", compteId);
    query.bindValue(":categorie", categorieId);
    query.bindValue(":rec", estRecurrente ? 1 : 0);
    query.bindValue(":freq", frequence);

    if (!query.exec()) {
        qDebug() << "Erreur ajout dépense:" << query.lastError().text();
        return false;
    }

    return true;
}
