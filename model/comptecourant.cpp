#include "CompteCourant.h"
#include "Operation.h"
#include "Transfert.h"

CompteCourant::CompteCourant(const QString& id, const QString& nom, double solde)
    : Compte(id, nom, solde)
{
}

void CompteCourant::mettreAJourSolde()
{
    solde = 0;

    for (Operation* op : operations) {
        solde += op->getMontant();
    }

    for (Transfert* t : transfertsSource) {
        solde -= t->getMontant();
    }

    for (Transfert* t : transfertsDestination) {
        solde += t->getMontant();
    }
}


