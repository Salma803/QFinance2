#include "Utilisateur.h"
#include "Compte.h"

/**
 * @brief Constructeur par défaut.
 */
Utilisateur::Utilisateur()
{
}

/**
 * @brief Constructeur avec identifiant et nom.
 */
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

void Utilisateur::supprimerCompte(const QString& compteId)
{
    for (int i = 0; i < comptes.size(); ++i) {
        if (comptes[i]->getId() == compteId) {
            delete comptes[i];
            comptes.removeAt(i);
            return;
        }
    }
}


Compte* Utilisateur::getCompteById(const QString& id) const
{
    for (Compte* compte : comptes) {
        if (compte->getId() == id)
            return compte;
    }
    return nullptr;
}
