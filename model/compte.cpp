#include "Compte.h"
#include "Operation.h"
#include "Transfert.h"

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

void Compte::ajouterOperation(Operation* operation)
{
    if (!operations.contains(operation)) {
        operations.append(operation);
        mettreAJourSolde();
    }
}

void Compte::supprimerOperation(Operation* operation)
{
    operations.removeAll(operation);
    mettreAJourSolde();
}

void Compte::ajouterTransfertSource(Transfert* transfert)
{
    transfertsSource.append(transfert);
    mettreAJourSolde();
}

void Compte::ajouterTransfertDestination(Transfert* transfert)
{
    transfertsDestination.append(transfert);
    mettreAJourSolde();
}
