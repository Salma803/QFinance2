#ifndef CATEGORIE_H
#define CATEGORIE_H

#include <QString>
#include <QList>

class Categorie
{
public:
    Categorie(const QString& id,
              const QString& nom,
              Categorie* parent = nullptr);

    // Getters
    QString getId() const;
    QString getNom() const;
    Categorie* getParent() const;
    QList<Categorie*> getEnfants() const;

    // Hiérarchie
    void ajouterEnfant(Categorie* enfant);
    void setParent(Categorie* p);


    // Règle métier : héritage de budget
    QString getCategorieBudgetSource() const;

private:
    QString id;
    QString nom;

    Categorie* parent;
    QList<Categorie*> enfants;
};

#endif // CATEGORIE_H
