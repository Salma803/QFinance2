#ifndef CATEGORIE_H
#define CATEGORIE_H

#include <QString>
#include <QList>

/**
 * @class Categorie
 * @brief Représente une catégorie de dépenses ou de revenus.
 *
 * Une catégorie peut être hiérarchique :
 * - elle peut avoir une catégorie parente
 * - elle peut contenir plusieurs sous-catégories
 *
 * Les catégories sont utilisées pour :
 * - classer les opérations financières
 * - définir et hériter des budgets
 */
class Categorie
{
public:
    /**
     * @brief Constructeur de la classe Categorie.
     *
     * @param id Identifiant unique (UUID) de la catégorie
     * @param nom Nom de la catégorie
     * @param parent Catégorie parente (nullptr si catégorie principale)
     */
    Categorie(const QString& id,
              const QString& nom,
              Categorie* parent = nullptr);

    /**
     * @brief Retourne l'identifiant unique de la catégorie.
     * @return Identifiant (UUID)
     */
    QString getId() const;

    /**
     * @brief Retourne le nom de la catégorie.
     * @return Nom de la catégorie
     */
    QString getNom() const;

    /**
     * @brief Retourne la catégorie parente.
     * @return Pointeur vers la catégorie parente ou nullptr
     */
    Categorie* getParent() const;

    /**
     * @brief Retourne la liste des sous-catégories.
     * @return Liste de pointeurs vers les catégories enfants
     */
    QList<Categorie*> getEnfants() const;

    /**
     * @brief Ajoute une sous-catégorie à la catégorie courante.
     *
     * @param enfant Pointeur vers la catégorie enfant à ajouter
     */
    void ajouterEnfant(Categorie* enfant);

    /**
     * @brief Détermine la catégorie source du budget.
     *
     * Si la catégorie n'a pas de budget défini, la recherche
     * remonte récursivement vers la catégorie parente.
     *
     * @return Identifiant de la catégorie source du budget
     */
    QString getCategorieBudgetSource() const;

    /**
     * @brief Modifie la catégorie parente.
     *
     * @param p Nouvelle catégorie parente
     */
    void setParent(Categorie* p);

private:
    QString id;                     ///< Identifiant unique de la catégorie
    QString nom;                    ///< Nom de la catégorie
    Categorie* parent;              ///< Catégorie parente (nullptr si racine)
    QList<Categorie*> enfants;      ///< Liste des sous-catégories
};

#endif // CATEGORIE_H
