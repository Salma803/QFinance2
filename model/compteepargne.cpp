#include "CompteEpargne.h"
#include "Operation.h"
#include "Transfert.h"

CompteEpargne::CompteEpargne(const QString& id, const QString& nom, double solde)
    : Compte(id, nom, solde)
{
}

void CompteEpargne::mettreAJourSolde()
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
