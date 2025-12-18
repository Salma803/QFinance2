#ifndef REVENU_H
#define REVENU_H

#include "Operation.h"

class Revenu : public Operation
{
public:
    Revenu(const QString& id,
           const QString& nom,
           const QDate& date,
           double montant,
           const QString& compteId,
           const QString& categorieId,
           const QString& provenance);

    QString getProvenance() const;
    bool estDepense() const override { return false; }

private:
    QString provenance;
};

#endif
