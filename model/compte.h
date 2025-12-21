#ifndef COMPTE_H
#define COMPTE_H

#include <QString>
#include <QList>

class Operation;
class Transfert;

/**
 * @class Compte
 * @brief Classe abstraite représentant un compte bancaire.
 *
 * Un compte possède :
 * - un identifiant unique
 * - un nom
 * - un solde
 *
 * Il peut contenir :
 * - des opérations (revenus / dépenses)
 * - des transferts sortants et entrants
 *
 * Cette classe est destinée à être héritée
 * (ex : CompteCourant, CompteEpargne).
 */
class Compte
{
public:
    /**
     * @brief Constructeur de la classe Compte.
     *
     * @param id Identifiant unique du compte (UUID)
     * @param nom Nom du compte
     * @param solde Solde initial du compte
     */
    Compte(const QString& id, const QString& nom, double solde);

    /**
     * @brief Destructeur virtuel.
     */
    virtual ~Compte() = default;


    /**
     * @brief boolean pour savoir si un copte est courant
     */
    virtual bool estCourant() const = 0;


    // =======================
    // Getters
    // =======================

    /**
     * @brief Retourne l'identifiant du compte.
     * @return Identifiant (UUID)
     */
    QString getId() const;

    /**
     * @brief Retourne le nom du compte.
     * @return Nom du compte
     */
    QString getNom() const;

    /**
     * @brief Retourne le solde actuel du compte.
     * @return Solde du compte
     */
    double getSolde() const;

    // =======================
    // Méthodes métier
    // =======================

    /**
     * @brief Ajoute une opération au compte.
     *
     * Met automatiquement à jour le solde du compte.
     *
     * @param operation Pointeur vers l'opération à ajouter
     */
    void ajouterOperation(Operation* operation);

    /**
     * @brief Supprime une opération du compte.
     *
     * Met automatiquement à jour le solde du compte.
     *
     * @param operation Pointeur vers l'opération à supprimer
     */
    void supprimerOperation(Operation* operation);

    /**
     * @brief Ajoute un transfert sortant (source).
     *
     * @param transfert Pointeur vers le transfert sortant
     */
    void ajouterTransfertSource(Transfert* transfert);

    /**
     * @brief Ajoute un transfert entrant (destination).
     *
     * @param transfert Pointeur vers le transfert entrant
     */
    void ajouterTransfertDestination(Transfert* transfert);

protected:
    /**
     * @brief Recalcule le solde du compte.
     *
     * Méthode appelée automatiquement après chaque
     * ajout ou suppression d'opération ou transfert.
     */
    virtual void mettreAJourSolde() = 0;

protected:
    QString id;                               ///< Identifiant unique du compte
    QString nom;                              ///< Nom du compte
    double solde;                             ///< Solde actuel du compte

    QList<Operation*> operations;             ///< Liste des opérations liées au compte
    QList<Transfert*> transfertsSource;       ///< Transferts sortants
    QList<Transfert*> transfertsDestination;  ///< Transferts entrants
};

#endif // COMPTE_H
