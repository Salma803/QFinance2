#ifndef BUDGET_H
#define BUDGET_H

#include <QString>

class Budget
{
public:
    Budget(const QString& id,
           const QString& categorieId,
           int mois,
           int annee,
           double montant);

    QString getCategorieId() const;
    int getMois() const;
    int getAnnee() const;
    double getMontant() const;

private:
    QString id;
    QString categorieId;
    int mois;
    int annee;
    double montant;
};

#endif
