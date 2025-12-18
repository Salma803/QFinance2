#include "Utilisateur.h"
#include "Compte.h"

Utilisateur::Utilisateur()
{
}

Utilisateur::Utilisateur(const QString& id, const QString& nom)
    : id(id), nom(nom)
{
}

// =======================
// Getters
// =======================

QString Utilisateur::getId() const
{
    return id;
}

QString Utilisateur::getNom() const
{
    return nom;
}

QList<Compte*> Utilisateur::getComptes() const
{
    return comptes;
}

// =======================
// Setters
// =======================

void Utilisateur::setNom(const QString& nom)
{
    this->nom = nom;
}

// =======================
// Méthodes métier
// =======================

void Utilisateur::ajouterCompte(Compte* compte)
{
    if (!comptes.contains(compte)) {
        comptes.append(compte);
    }
}

void Utilisateur::supprimerCompte(Compte* compte)
{
    comptes.removeAll(compte);
}
