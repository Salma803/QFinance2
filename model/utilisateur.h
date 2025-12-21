#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <QString>
#include <QList>

class Compte;

/**
 * @class Utilisateur
 * @brief Représente un utilisateur de l'application.
 *
 * Un utilisateur possède :
 * - un identifiant unique
 * - un nom
 * - une liste de comptes bancaires
 *
 * Cette classe correspond au modèle "Utilisateur" dans l'architecture MVC.
 */
class Utilisateur
{
public:
    /**
     * @brief Constructeur par défaut.
     */
    Utilisateur();

    /**
     * @brief Constructeur avec identifiant et nom.
     * @param id Identifiant unique de l'utilisateur
     * @param nom Nom de l'utilisateur
     */
    Utilisateur(const QString& id, const QString& nom);

    // =======================
    // Getters
    // =======================

    /**
     * @brief Retourne l'identifiant de l'utilisateur.
     * @return Identifiant de l'utilisateur
     */
    QString getId() const;

    /**
     * @brief Retourne le nom de l'utilisateur.
     * @return Nom de l'utilisateur
     */
    QString getNom() const;

    /**
     * @brief Retourne la liste des comptes de l'utilisateur.
     * @return Liste de pointeurs vers les comptes
     */
    QList<Compte*> getComptes() const;

    // =======================
    // Setters
    // =======================

    /**
     * @brief Modifie le nom de l'utilisateur.
     * @param nom Nouveau nom
     */
    void setNom(const QString& nom);

    // =======================
    // Méthodes métier
    // =======================

    /**
     * @brief Ajoute un compte à l'utilisateur.
     * @param compte Pointeur vers le compte à ajouter
     */
    void ajouterCompte(Compte* compte);

    /**
     * @brief Supprime un compte de l'utilisateur.
     * @param compte Pointeur vers le compte à supprimer
     */
    void supprimerCompte(const QString& compteId);

    /**
     * @brief Recherche un compte par son identifiant.
     * @param id Identifiant du compte recherché
     * @return Pointeur vers le compte s'il existe, nullptr sinon
     */
    Compte* getCompteById(const QString& id) const;

private:
    QString id;                 /**< Identifiant de l'utilisateur */
    QString nom;                /**< Nom de l'utilisateur */
    QList<Compte*> comptes;     /**< Liste des comptes associés */
};

#endif // UTILISATEUR_H
