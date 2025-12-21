#include "Compte.h"
#include "Operation.h"
#include "Transfert.h"

/**
 * @brief Constructeur de Compte.
 */
Compte::Compte(const QString& id, const QString& nom, double solde)
    : id(id), nom(nom), solde(solde)
{
}

// =======================
// Getters
// =======================

QString Compte::getId() const
{
    return id;
}

QString Compte::getNom() const
{
    return nom;
}

double Compte::getSolde() const
{
    return solde;
}

// =======================
// Méthodes métier
// =======================

/**
 * @brief Ajoute une opération au compte.
 */
void Compte::ajouterOperation(Operation* operation)
{
    if (!operations.contains(operation)) {
        operations.append(operation);
        mettreAJourSolde();
    }
}

/**
 * @brief Supprime une opération du compte.
 */
void Compte::supprimerOperation(Operation* operation)
{
    operations.removeAll(operation);
    mettreAJourSolde();
}

/**
 * @brief Ajoute un transfert sortant (source).
 */
void Compte::ajouterTransfertSource(Transfert* transfert)
{
    transfertsSource.append(transfert);
    mettreAJourSolde();
}

/**
 * @brief Ajoute un transfert entrant (destination).
 */
void Compte::ajouterTransfertDestination(Transfert* transfert)
{
    transfertsDestination.append(transfert);
    mettreAJourSolde();
}
