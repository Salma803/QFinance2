#ifndef COMPTECOURANT_H
#define COMPTECOURANT_H

#include "Compte.h"

/**
 * @class CompteCourant
 * @brief Représente un compte courant.
 *
 * Un compte courant permet :
 * - d'ajouter des revenus
 * - d'ajouter des dépenses
 * - d'effectuer des transferts entrants et sortants
 *
 * Le solde est recalculé dynamiquement à partir :
 * - des opérations
 * - des transferts
 */
class CompteCourant : public Compte
{
public:
    /**
     * @brief Constructeur de CompteCourant.
     *
     * @param id Identifiant unique du compte (UUID)
     * @param nom Nom du compte
     * @param solde Solde initial
     */
    CompteCourant(const QString& id, const QString& nom, double solde);
    /**
     * @brief Recalcule le solde du compte courant.
     *
     * Règles de calcul :
     * - Somme des montants des opérations
     * - Moins les transferts sortants
     * - Plus les transferts entrants
     */
    void mettreAJourSolde() override;

    bool estCourant() const override { return true; }

};

#endif // COMPTECOURANT_H
