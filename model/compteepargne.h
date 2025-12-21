#ifndef COMPTEEPARGNE_H
#define COMPTEEPARGNE_H

#include "Compte.h"
/**
 * @class CompteEpargne
 * @brief Représente un compte épargne.
 *
 * Un compte épargne est un type de compte bancaire destiné
 * principalement à la conservation de fonds.
 *
 * Il hérite de la classe Compte et partage le même mécanisme
 * de calcul de solde que le compte courant.
 */
class CompteEpargne : public Compte
{
public:
    /**
     * @brief Constructeur de la classe CompteEpargne.
     *
     * @param id Identifiant unique du compte (UUID)
     * @param nom Nom du compte
     * @param solde Solde initial du compte
     */
    CompteEpargne(const QString& id, const QString& nom, double solde);
    /**
     * @brief Recalcule le solde du compte épargne.
     *
     * Le solde est calculé à partir :
     * - des opérations associées au compte
     * - des transferts sortants
     * - des transferts entrants
     */
    void mettreAJourSolde() override;

    bool estCourant() const override { return false; }

};

#endif // COMPTEEPARGNE_H
