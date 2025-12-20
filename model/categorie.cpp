#include "Categorie.h"

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
 * Héritage de budget :
 * - si la catégorie a un budget → elle-même
 * - sinon → remonte jusqu’au parent
 */
QString Categorie::getCategorieBudgetSource() const
{
    if (!parent) {
        return id;
    }
    return parent->getCategorieBudgetSource();
}

void Categorie::setParent(Categorie* p) { parent = p; }

