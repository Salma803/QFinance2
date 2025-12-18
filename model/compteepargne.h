#ifndef COMPTEEPARGNE_H
#define COMPTEEPARGNE_H

#include "Compte.h"

class CompteEpargne : public Compte
{
public:
    CompteEpargne(const QString& id, const QString& nom, double solde);

    void mettreAJourSolde() override;

    bool estCourant() const override { return false; }

};

#endif // COMPTEEPARGNE_H
