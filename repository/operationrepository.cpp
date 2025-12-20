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

QList<QVariantMap> OperationRepository::chargerOperationsParCompte(
    const QString& compteId)
{
    QList<QVariantMap> ops;
    QSqlQuery query;

    query.prepare(
        "SELECT o.id, o.date, o.nom, c.nom AS categorie, o.montant, o.type, o.categorie_id "
        "FROM Operation o "
        "JOIN Categorie c ON o.categorie_id = c.id "
        "WHERE o.compte_id = :cid "
        "ORDER BY o.date DESC"
        );
    query.bindValue(":cid", compteId);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return ops;
    }

    while (query.next()) {
        QVariantMap row;
        row["id"] = query.value("id");
        row["date"] = query.value("date");
        row["nom"] = query.value("nom");
        row["categorie"] = query.value("categorie");
        row["montant"] = query.value("montant");
        row["type"] = query.value("type");
        row["categorie_id"] = query.value("categorie_id");
        ops.append(row);
    }

    return ops;
}

double OperationRepository::getTotalRevenus(int mois, int annee)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    qDebug() << "=== DEBUG getTotalRevenus ===";
    qDebug() << "Mois:" << moisStr << "Année:" << anneeStr;

    // Version 1 : Utiliser SUBSTR
    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'revenu'"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);

    if (!query.exec()) {
        qDebug() << "Erreur SQL (SUBSTR):" << query.lastError().text();

        // Version 2 : Alternative avec strftime
        query.prepare(
            "SELECT SUM(montant) FROM Operation "
            "WHERE strftime('%m', date) = :mois "
            "AND strftime('%Y', date) = :annee "
            "AND type = 'revenu'"
            );
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);

        if (!query.exec()) {
            qDebug() << "Erreur SQL (strftime):" << query.lastError().text();
            return 0.0;
        }
    }

    if (query.next()) {
        double total = query.value(0).toDouble();
        qDebug() << "Total revenus trouvé:" << total;
        return total;
    }

    qDebug() << "Aucun revenu pour cette période";
    return 0.0;
}

double OperationRepository::getTotalDepenses(int mois, int annee)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    qDebug() << "=== DEBUG getTotalDepenses ===";
    qDebug() << "Mois:" << moisStr << "Année:" << anneeStr;

    // Version SUBSTR (plus fiable)
    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'depense'"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);

    if (!query.exec()) {
        qDebug() << "Erreur SQL (SUBSTR):" << query.lastError().text();

        // Version strftime
        query.prepare(
            "SELECT SUM(montant) FROM Operation "
            "WHERE strftime('%m', date) = :mois "
            "AND strftime('%Y', date) = :annee "
            "AND type = 'depense'"
            );
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);

        if (!query.exec()) {
            qDebug() << "Erreur SQL (strftime):" << query.lastError().text();
            return 0.0;
        }
    }

    if (query.next()) {
        double total = query.value(0).toDouble();
        qDebug() << "Total dépenses trouvé:" << total;
        return total;
    }

    qDebug() << "Aucune dépense pour cette période";
    return 0.0;
}

double OperationRepository::getTotalDepensesCategorie(const QString &categorieId, int mois, int annee)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    qDebug() << "=== DEBUG getTotalDepensesCategorie ===";
    qDebug() << "Catégorie:" << categorieId << "Mois:" << moisStr << "Année:" << anneeStr;

    // Version SUBSTR
    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'depense' "
        "AND categorie_id = :categorie_id"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);
    query.bindValue(":categorie_id", categorieId);

    if (!query.exec()) {
        qDebug() << "Erreur SQL (SUBSTR):" << query.lastError().text();

        // Version strftime
        query.prepare(
            "SELECT SUM(montant) FROM Operation "
            "WHERE strftime('%m', date) = :mois "
            "AND strftime('%Y', date) = :annee "
            "AND type = 'depense' "
            "AND categorie_id = :categorie_id"
            );
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);
        query.bindValue(":categorie_id", categorieId);

        if (!query.exec()) {
            qDebug() << "Erreur SQL (strftime):" << query.lastError().text();
            return 0.0;
        }
    }

    if (query.next()) {
        double total = query.value(0).toDouble();
        qDebug() << "Total pour catégorie" << categorieId << ":" << total;
        return total;
    }

    qDebug() << "Aucune dépense pour cette catégorie et période";
    return 0.0;
}

QString OperationRepository::getOperationId(const QString& compteId, const QString& nomOperation,
                                            const QDate& date, double montant)
{
    QSqlQuery query;
    query.prepare(
        "SELECT id FROM Operation "
        "WHERE compte_id = :compte "
        "AND nom = :nom "
        "AND date = :date "
        "AND montant = :montant "
        "LIMIT 1"
        );
    query.bindValue(":compte", compteId);
    query.bindValue(":nom", nomOperation);
    query.bindValue(":date", date.toString(Qt::ISODate));
    query.bindValue(":montant", montant);

    if (!query.exec()) {
        qDebug() << "Erreur récupération ID opération:" << query.lastError().text();
        return "";
    }

    if (query.next()) {
        return query.value("id").toString();
    }

    return "";
}

bool OperationRepository::supprimerOperation(const QString& operationId, const QString& compteId)
{
    qDebug() << "=== SUPPRESSION OPÉRATION ===";
    qDebug() << "ID opération:" << operationId;
    qDebug() << "ID compte:" << compteId;

    // D'abord, vérifier si l'opération existe et appartient bien au compte
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT montant, type FROM Operation WHERE id = :id AND compte_id = :compte");
    checkQuery.bindValue(":id", operationId);
    checkQuery.bindValue(":compte", compteId);

    if (!checkQuery.exec()) {
        qDebug() << "Erreur vérification opération:" << checkQuery.lastError().text();
        return false;
    }

    if (!checkQuery.next()) {
        qDebug() << "Opération non trouvée ou n'appartient pas au compte";
        return false;
    }

    double montant = checkQuery.value("montant").toDouble();
    QString type = checkQuery.value("type").toString();
    qDebug() << "Montant:" << montant << "Type:" << type;

    // Désactiver temporairement les contraintes de clé étrangère
    QSqlQuery pragmaQuery;
    pragmaQuery.exec("PRAGMA foreign_keys = OFF");

    // Commencer une transaction
    QSqlDatabase::database().transaction();

    bool succes = true;

    // Supprimer l'opération
    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM Operation WHERE id = :id");
    deleteQuery.bindValue(":id", operationId);

    if (!deleteQuery.exec()) {
        qDebug() << "Erreur suppression:" << deleteQuery.lastError().text();
        succes = false;
    }

    // Mettre à jour le solde si suppression réussie
    if (succes) {
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE Compte SET solde = solde + :ajustement WHERE id = :compte");

        double ajustement = (type.toUpper() == "DEPENSE") ? montant : -montant;
        updateQuery.bindValue(":ajustement", ajustement);
        updateQuery.bindValue(":compte", compteId);

        if (!updateQuery.exec()) {
            qDebug() << "Erreur mise à jour solde:" << updateQuery.lastError().text();
            succes = false;
        }
    }

    // Finaliser transaction
    if (succes) {
        QSqlDatabase::database().commit();
    } else {
        QSqlDatabase::database().rollback();
    }

    // Réactiver les contraintes
    pragmaQuery.exec("PRAGMA foreign_keys = ON");

    return succes;
}

// NOUVELLES MÉTHODES POUR LA MODIFICATION

QVariantMap OperationRepository::getOperationDetails(const QString& operationId)
{
    QVariantMap details;
    QSqlQuery query;

    query.prepare(
        "SELECT o.id, o.nom, o.date, o.montant, o.type, o.compte_id, o.categorie_id, c.nom AS categorie_nom "
        "FROM Operation o "
        "LEFT JOIN Categorie c ON o.categorie_id = c.id "
        "WHERE o.id = :id"
        );
    query.bindValue(":id", operationId);

    if (query.exec() && query.next()) {
        details["id"] = query.value("id");
        details["nom"] = query.value("nom");
        details["date"] = query.value("date");
        details["montant"] = query.value("montant");
        details["type"] = query.value("type");
        details["compte_id"] = query.value("compte_id");
        details["categorie_id"] = query.value("categorie_id");
        details["categorie_nom"] = query.value("categorie_nom");
    } else {
        qDebug() << "Erreur récupération détails opération:" << query.lastError().text();
    }

    return details;
}

bool OperationRepository::modifierOperation(const QString& operationId,
                                            const QString& nouveauNom,
                                            const QDate& nouvelleDate,
                                            double nouveauMontant,
                                            const QString& nouvelleCategorieId)
{
    qDebug() << "=== MODIFICATION OPÉRATION ===";
    qDebug() << "ID:" << operationId;
    qDebug() << "Nouveau nom:" << nouveauNom;
    qDebug() << "Nouvelle date:" << nouvelleDate.toString("dd/MM/yyyy");
    qDebug() << "Nouveau montant:" << nouveauMontant;
    qDebug() << "Nouvelle catégorie ID:" << nouvelleCategorieId;

    // 1. Récupérer l'ancienne opération pour ajuster le solde
    QSqlQuery selectQuery;
    selectQuery.prepare(
        "SELECT montant, type, compte_id FROM Operation WHERE id = :id"
        );
    selectQuery.bindValue(":id", operationId);

    if (!selectQuery.exec() || !selectQuery.next()) {
        qDebug() << "Opération non trouvée";
        return false;
    }

    double ancienMontant = selectQuery.value("montant").toDouble();
    QString ancienType = selectQuery.value("type").toString();
    QString compteId = selectQuery.value("compte_id").toString();

    qDebug() << "Ancien montant:" << ancienMontant;
    qDebug() << "Ancien type:" << ancienType;
    qDebug() << "Compte ID:" << compteId;

    // Commencer une transaction
    QSqlDatabase::database().transaction();

    bool succes = true;

    // 2. Mettre à jour l'opération
    QSqlQuery updateQuery;
    updateQuery.prepare(
        "UPDATE Operation SET "
        "nom = :nom, "
        "date = :date, "
        "montant = :montant, "
        "categorie_id = :categorie "
        "WHERE id = :id"
        );

    updateQuery.bindValue(":nom", nouveauNom);
    updateQuery.bindValue(":date", nouvelleDate.toString(Qt::ISODate));
    updateQuery.bindValue(":montant", nouveauMontant);
    updateQuery.bindValue(":categorie", nouvelleCategorieId);
    updateQuery.bindValue(":id", operationId);

    if (!updateQuery.exec()) {
        qDebug() << "Erreur modification:" << updateQuery.lastError().text();
        succes = false;
    }

    // 3. Ajuster le solde du compte si le montant a changé
    if (succes) {
        // Calculer la différence
        double difference = 0;

        if (ancienType.toUpper() == "DEPENSE") {
            // Pour une dépense: ancien montant était soustrait, nouveau montant sera soustrait
            // Donc ajustement = (ancien - nouveau)
            difference = ancienMontant - nouveauMontant;
        } else if (ancienType.toUpper() == "REVENU") {
            // Pour un revenu: ancien montant était ajouté, nouveau montant sera ajouté
            // Donc ajustement = (nouveau - ancien)
            difference = nouveauMontant - ancienMontant;
        }

        qDebug() << "Différence à ajuster:" << difference;

        if (qAbs(difference) > 0.001) { // Si différence significative
            QSqlQuery soldeQuery;
            soldeQuery.prepare(
                "UPDATE Compte SET solde = solde + :difference WHERE id = :compte"
                );
            soldeQuery.bindValue(":difference", difference);
            soldeQuery.bindValue(":compte", compteId);

            if (!soldeQuery.exec()) {
                qDebug() << "Erreur ajustement solde:" << soldeQuery.lastError().text();
                succes = false;
            } else {
                qDebug() << "Solde ajusté de:" << difference;
            }
        }
    }

    // 4. Finaliser la transaction
    if (succes) {
        QSqlDatabase::database().commit();
        qDebug() << "Opération modifiée avec succès";
        return true;
    } else {
        QSqlDatabase::database().rollback();
        qDebug() << "Échec de la modification";
        return false;
    }
}
