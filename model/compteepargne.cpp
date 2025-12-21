#include "CompteEpargne.h"
#include "Operation.h"
#include "Transfert.h"

/**
 * @brief Constructeur de la classe CompteEpargne.
 */
CompteEpargne::CompteEpargne(const QString& id, const QString& nom, double solde)
    : Compte(id, nom, solde)
{
}

/**
 * @brief Recalcule le solde du compte épargne.
 *
 * Le solde est recalculé dynamiquement en additionnant :
 * - les montants des opérations
 * - les transferts entrants
 *
 * Et en soustrayant :
 * - les transferts sortants
 */
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
