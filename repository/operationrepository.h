#ifndef OPERATIONREPOSITORY_H
#define OPERATIONREPOSITORY_H

#include <QString>
#include <QDate>

class OperationRepository
{
public:
    static bool ajouterRevenu(const QString& nom,
                              const QDate& date,
                              double montant,
                              const QString& compteId,
                              const QString& categorieId,
                              const QString& provenance);

    static bool ajouterDepense(const QString& nom,
                               const QDate& date,
                               double montant,
                               const QString& compteId,
                               const QString& categorieId,
                               bool estRecurrente,
                               const QString& frequence);
    static QList<QVariantMap> chargerOperationsParCompte(const QString& compteId);

};

#endif // OPERATIONREPOSITORY_H
