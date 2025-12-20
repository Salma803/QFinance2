#include "BudgetRepository.h"
#include "CategorieRepository.h"
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
        "INSERT INTO Budget (id, categorie_id, mois, annee, montant) "
        "VALUES (:id, :cat, :mois, :annee, :montant) "
        "ON CONFLICT(categorie_id, mois, annee) DO UPDATE SET "
        "montant = excluded.montant"
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

double BudgetRepository::calculerDepensesCategorie(
    const QString& categorieId,
    int mois,
    int annee)
{
    double total = 0.0;
    QSqlQuery query;

    // Dépenses directes
    query.prepare(
        "SELECT IFNULL(SUM(montant), 0) "
        "FROM Operation "
        "WHERE categorie_id = :cat "
        "AND type = 'DEPENSE' "
        "AND strftime('%m', date) = :mois "
        "AND strftime('%Y', date) = :annee"
        );
    query.bindValue(":cat", categorieId);
    query.bindValue(":mois", QString("%1").arg(mois, 2, 10, QChar('0')));
    query.bindValue(":annee", QString::number(annee));
    query.exec();
    query.next();
    total += query.value(0).toDouble();

    // Sous-catégories
    QList<QString> enfants =
        CategorieRepository::chargerIdsSousCategories(categorieId);

    for (const QString& enfantId : enfants) {
        total += calculerDepensesCategorie(enfantId, mois, annee);
    }

    return total;
}
double BudgetRepository::calculerBudgetRestant(
    const QString& categorieId,
    int mois,
    int annee)
{
    double budget = obtenirBudget(categorieId, mois, annee);
    if (budget < 0)
        return -1;

    double depenses =
        calculerDepensesCategorie(categorieId, mois, annee);

    return budget - depenses;
}

