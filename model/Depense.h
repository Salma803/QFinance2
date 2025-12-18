#ifndef DEPENSE_H
#define DEPENSE_H

#include "Operation.h"

enum class Frequence {
    Mensuelle,
    Trimestrielle,
    Annuelle
};

class Depense : public Operation
{
public:
    Depense(const QString& id,
            const QString& nom,
            const QDate& date,
            double montant,
            const QString& compteId,
            const QString& categorieId,
            bool estRecurrente,
            Frequence frequence);

    bool estDepense() const override { return true; }

    bool isRecurrente() const;
    Frequence getFrequence() const;

private:
    bool recurrente;
    Frequence frequence;
};

#endif
