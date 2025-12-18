#ifndef COMPTECOURANT_H
#define COMPTECOURANT_H

#include "Compte.h"

class CompteCourant : public Compte
{
public:
    CompteCourant(const QString& id, const QString& nom, double solde);

    void mettreAJourSolde() override;

    bool estCourant() const override { return true; }

};

#endif // COMPTECOURANT_H
