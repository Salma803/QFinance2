#include "TransfertRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "../model/Transfert.h"
#include "../model/Compte.h"

bool TransfertRepository::ajouterTransfert(const Transfert* transfert)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Transfert (id, montant, date, source_id, destination_id) "
        "VALUES (:id, :montant, :date, :src, :dst)"
        );

    query.bindValue(":id", QString::number(reinterpret_cast<quintptr>(transfert)));
    query.bindValue(":montant", transfert->getMontant());
    query.bindValue(":date", transfert->getDate().toString(Qt::ISODate));
    query.bindValue(":src", transfert->getSource()->getId());
    query.bindValue(":dst", transfert->getDestination()->getId());

    if (!query.exec()) {
        qDebug() << "Erreur ajout transfert:" << query.lastError();
        return false;
    }

    return true;
}
QList<QVariantMap> TransfertRepository::chargerTransfertsParCompte(
    const QString& compteId)
{
    QList<QVariantMap> transferts;
    QSqlQuery query;

    query.prepare(
        "SELECT date, montant, source_id, destination_id "
        "FROM Transfert "
        "WHERE source_id = :id OR destination_id = :id "
        "ORDER BY date DESC"
        );
    query.bindValue(":id", compteId);

    if (!query.exec()) {
        qDebug() << "Erreur chargement transferts:" << query.lastError();
        return transferts;
    }

    while (query.next()) {
        QVariantMap t;

        bool sortant = query.value("source_id").toString() == compteId;

        t["date"] = query.value("date").toDate();
        t["montant"] = sortant
                           ? -query.value("montant").toDouble()
                           : query.value("montant").toDouble();
        t["type"] = sortant ? "Transfert sortant" : "Transfert entrant";
        t["nom"] = sortant ? "Vers autre compte" : "Depuis autre compte";
        t["categorie"] = "-";

        transferts.append(t);
    }

    return transferts;
}
bool TransfertRepository::ajouterTransfertAuto(
    const QString& sourceId,
    const QString& destinationId,
    double montant,
    const QDate& date
    ) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Transfert (source_id, destination_id, montant, date, type) "
        "VALUES (:src, :dst, :montant, :date, 'AUTO')"
        );

    query.bindValue(":src", sourceId);
    query.bindValue(":dst", destinationId);
    query.bindValue(":montant", montant);
    query.bindValue(":date", date.toString(Qt::ISODate));

    if (!query.exec()) {
        qDebug() << "❌ ajouterTransfertAuto FAILED:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ Transfert AUTO OK:" << sourceId << "->" << destinationId << montant;
    return true;
}


