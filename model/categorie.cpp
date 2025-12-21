#include "Categorie.h"

/**
 * @brief Constructeur de Categorie.
 *
 * Si une catégorie parente est fournie,
 * la catégorie est automatiquement ajoutée
 * à la liste des enfants du parent.
 */
Categorie::Categorie(const QString& id,
                     const QString& nom,
                     Categorie* parent)
    : id(id), nom(nom), parent(parent)
{
    if (parent) {
        parent->ajouterEnfant(this);
    }
}

QString Categorie::getId() const
{
    return id;
}

QString Categorie::getNom() const
{
    return nom;
}

Categorie* Categorie::getParent() const
{
    return parent;
}

QList<Categorie*> Categorie::getEnfants() const
{
    return enfants;
}

void Categorie::ajouterEnfant(Categorie* enfant)
{
    if (!enfants.contains(enfant)) {
        enfants.append(enfant);
    }
}

/**
 * @brief Détermine la catégorie source du budget.
 *
 * Héritage de budget :
 * - Si la catégorie a un budget défini → elle-même
 * - Sinon → recherche récursive dans la hiérarchie parente
 */
QString Categorie::getCategorieBudgetSource() const
{
    if (!parent) {
        return id;
    }
    return parent->getCategorieBudgetSource();
}

void Categorie::setParent(Categorie* p)
{
    parent = p;
}
