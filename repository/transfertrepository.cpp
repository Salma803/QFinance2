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
