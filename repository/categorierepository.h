#ifndef CATEGORIEREPOSITORY_H
#define CATEGORIEREPOSITORY_H

#include <QList>
#include <QString>

class Categorie;

class CategorieRepository
{
public:
    static void creerCategoriesParDefaut();
    static QList<Categorie*> chargerCategories(const QString& utilisateurId);
    static QString ajouterCategorie(const QString& nom,
                                    const QString& utilisateurId,
                                    const QString& parentId);

    static bool modifierCategorie(const QString& categorieId,
                                  const QString& nouveauNom);

    bool supprimerCategorie(const QString& categorieId);

    // Suppression d'une catégorie et de ses sous-catégories
    bool supprimerCategorieAvecEnfants(const QString& categorieId);
    static QList<QString> chargerIdsSousCategories(const QString& parentId);

};

#endif // CATEGORIEREPOSITORY_H
