#ifndef BUDGETREPOSITORY_H
#define BUDGETREPOSITORY_H

#include <QString>

class Budget;

class BudgetRepository
{
public:
    static bool definirBudget(const QString& categorieId,
                              int mois,
                              int annee,
                              double montant);

    static double obtenirBudget(const QString& categorieId,
                                int mois,
                                int annee);
};

#endif
