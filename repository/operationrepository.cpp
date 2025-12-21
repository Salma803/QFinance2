#include "OperationRepository.h"
#include "CompteRepository.h"
#include "TransfertRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QDebug>
#include <QMessageBox>

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
    qDebug() << ">>> ajouterDepense APPELEE <<<";

    // =========================
    // 1️⃣ Solde AVANT dépense
    // =========================
    double soldeAvant = CompteRepository::calculerSolde(compteId);
    qDebug() << "Solde AVANT depense =" << soldeAvant;

    // =========================
    // 2️⃣ Insérer la dépense
    // =========================
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Operation "
        "(id, nom, date, montant, type, compte_id, categorie_id, est_recurrente, frequence) "
        "VALUES (:id, :nom, :date, :montant, 'DEPENSE', :compte, :categorie, :rec, :freq)"
        );

    QString operationId = QUuid::createUuid().toString();

    query.bindValue(":id", operationId);
    query.bindValue(":nom", nom);
    query.bindValue(":date", date.toString(Qt::ISODate));
    query.bindValue(":montant", montant);
    query.bindValue(":compte", compteId);
    query.bindValue(":categorie", categorieId);
    query.bindValue(":rec", estRecurrente ? 1 : 0);
    query.bindValue(":freq", frequence);

    if (!query.exec()) {
        qDebug() << "❌ Erreur ajout dépense:" << query.lastError().text();
        return false;
    }

    // =========================
    // 3️⃣ Solde APRÈS dépense
    // =========================
    double soldeApres = soldeAvant - montant;
    qDebug() << "Solde APRES depense =" << soldeApres;

    // Mettre à jour le solde du compte déficitaire
    CompteRepository::mettreAJourSolde(compteId);

    // =========================
    // 4️⃣ Déclencher transfert automatique si négatif
    // =========================
    if (soldeApres < 0) {
        qDebug() << ">>> SOLDE NEGATIF DETECTE <<<";

        double montantManquant = -soldeApres;

        QString compteSourceId =
            CompteRepository::trouverCompteCourantAvecSoldeSuffisant(
                "1",        // utilisateurId
                compteId,   // compte déficitaire
                montantManquant
                );

        qDebug() << "Compte source trouvé =" << compteSourceId;

        if (!compteSourceId.isEmpty()) {
            qDebug() << "⚠️ Transfert automatique de"
                     << montantManquant
                     << "depuis"
                     << compteSourceId;

            bool ok = TransfertRepository::ajouterTransfertAuto(
                compteSourceId,
                compteId,
                montantManquant,
                QDate::currentDate()
                );

            if (ok) {
                CompteRepository::mettreAJourSolde(compteSourceId);
                CompteRepository::mettreAJourSolde(compteId);

                QMessageBox::information(
                    nullptr,
                    "Transfert automatique",
                    QString("Le compte était négatif.\n"
                            "Un transfert automatique de %1 € a été effectué.")
                        .arg(montantManquant, 0, 'f', 2)
                    );
            }
        } else {
            qDebug() << "❗ Aucun compte avec solde suffisant";
        }
    }

    return true;
}


QList<QVariantMap> OperationRepository::chargerOperationsParCompte(
    const QString& compteId)
{
    QList<QVariantMap> ops;
    QSqlQuery query;

    query.prepare(
        "SELECT o.id, o.date, o.nom, "
        "COALESCE(c.nom, 'Sans catégorie') AS categorie, "
        "o.montant, o.type, o.categorie_id "
        "FROM Operation o "
        "LEFT JOIN Categorie c ON o.categorie_id = c.id "
        "WHERE o.compte_id = :cid "
        "ORDER BY o.date DESC"
        );
    query.bindValue(":cid", compteId);

    if (!query.exec()) {
        qDebug() << "Erreur chargerOperationsParCompte:" << query.lastError().text();
        return ops;
    }

    while (query.next()) {
        QVariantMap row;
        row["id"] = query.value("id");
        row["date"] = QDate::fromString(query.value("date").toString(), Qt::ISODate);
        row["nom"] = query.value("nom");
        row["categorie"] = query.value("categorie");
        row["montant"] = query.value("montant").toDouble();
        row["type"] = query.value("type").toString();
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

    // CORRECTION : Chercher 'REVENU' en majuscules
    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'REVENU'"  // MAJUSCULES
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);

    if (query.exec() && query.next()) {
        double result = query.value(0).toDouble();
        qDebug() << "Total revenus pour" << moisStr << "/" << anneeStr << ":" << result;
        return result;
    } else {
        qDebug() << "Erreur getTotalRevenus:" << query.lastError().text();
    }

    return 0.0;
}

double OperationRepository::getTotalDepenses(int mois, int annee)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    // CORRECTION : Chercher 'DEPENSE' en majuscules
    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'DEPENSE'"  // MAJUSCULES
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);

    if (query.exec() && query.next()) {
        double result = query.value(0).toDouble();
        qDebug() << "Total dépenses pour" << moisStr << "/" << anneeStr << ":" << result;
        return result;
    } else {
        qDebug() << "Erreur getTotalDepenses:" << query.lastError().text();
    }

    return 0.0;
}

double OperationRepository::getTotalDepensesCategorie(const QString &categorieId, int mois, int annee)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    // CORRECTION : Chercher 'DEPENSE' en majuscules
    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'DEPENSE' "  // MAJUSCULES
        "AND categorie_id = :categorie_id"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);
    query.bindValue(":categorie_id", categorieId);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

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

// Ajoutez ces méthodes dans OperationRepository.cpp

double OperationRepository::getTotalRevenusCompte(int mois, int annee, const QString &compteId)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'REVENU' "
        "AND compte_id = :compte_id"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);
    query.bindValue(":compte_id", compteId);

    if (query.exec() && query.next()) {
        double result = query.value(0).toDouble();
        qDebug() << "Total revenus compte" << compteId << "pour" << moisStr << "/" << anneeStr << ":" << result;
        return result;
    } else {
        qDebug() << "Erreur getTotalRevenusCompte:" << query.lastError().text();
    }

    return 0.0;
}

double OperationRepository::getTotalDepensesCompte(int mois, int annee, const QString &compteId)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'DEPENSE' "
        "AND compte_id = :compte_id"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);
    query.bindValue(":compte_id", compteId);

    if (query.exec() && query.next()) {
        double result = query.value(0).toDouble();
        qDebug() << "Total dépenses compte" << compteId << "pour" << moisStr << "/" << anneeStr << ":" << result;
        return result;
    } else {
        qDebug() << "Erreur getTotalDepensesCompte:" << query.lastError().text();
    }

    return 0.0;
}

double OperationRepository::getTotalDepensesCategorieCompte(const QString &categorieId,
                                                            int mois, int annee,
                                                            const QString &compteId)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    query.prepare(
        "SELECT SUM(montant) FROM Operation "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND type = 'DEPENSE' "
        "AND categorie_id = :categorie_id "
        "AND compte_id = :compte_id"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);
    query.bindValue(":categorie_id", categorieId);
    query.bindValue(":compte_id", compteId);

    if (query.exec() && query.next()) {
        double result = query.value(0).toDouble();
        qDebug() << "Dépenses catégorie" << categorieId << "compte" << compteId << ":" << result;
        return result;
    } else {
        qDebug() << "Erreur getTotalDepensesCategorieCompte:" << query.lastError().text();
    }

    return 0.0;
}
