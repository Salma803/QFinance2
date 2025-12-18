#include "BudgetRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QDebug>

bool BudgetRepository::definirBudget(const QString& categorieId,
                                     int mois,
                                     int annee,
                                     double montant)
{
    QSqlQuery query;
    query.prepare(
        "INSERT OR REPLACE INTO Budget "
        "(id, categorie_id, mois, annee, montant) "
        "VALUES (:id, :cat, :mois, :annee, :montant)"
        );

    query.bindValue(":id", QUuid::createUuid().toString());
    query.bindValue(":cat", categorieId);
    query.bindValue(":mois", mois);
    query.bindValue(":annee", annee);
    query.bindValue(":montant", montant);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

double BudgetRepository::obtenirBudget(const QString& categorieId,
                                       int mois,
                                       int annee)
{
    QSqlQuery query;
    query.prepare(
        "SELECT montant FROM Budget "
        "WHERE categorie_id = :cat AND mois = :mois AND annee = :annee"
        );

    query.bindValue(":cat", categorieId);
    query.bindValue(":mois", mois);
    query.bindValue(":annee", annee);

    if (query.exec() && query.next())
        return query.value(0).toDouble();

    return -1.0; // pas de budget défini
}
