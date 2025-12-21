#include "CompteCourant.h"
#include "Operation.h"
#include "Transfert.h"

/**
 * @brief Constructeur de la classe CompteCourant.
 */
CompteCourant::CompteCourant(const QString& id, const QString& nom, double solde)
    : Compte(id, nom, solde)
{
}
/**
 * @brief Recalcule le solde du compte courant.
 *
 * Le solde est recalculé à partir de :
 * - toutes les opérations associées
 * - tous les transferts sortants
 * - tous les transferts entrants
 */
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


