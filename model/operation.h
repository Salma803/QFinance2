#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include <QDate>

/**
 * @class Operation
 * @brief Représente une opération financière.
 *
 * Une opération correspond à un mouvement financier associé
 * à un compte et à une catégorie.
 *
 * Elle peut représenter :
 * - une dépense
 * - un revenu
 *
 * Les opérations sont stockées en base de données et utilisées
 * pour le calcul des soldes, des statistiques et des budgets.
 */
class Operation
{
public:
    /**
     * @brief Constructeur de la classe Operation.
     *
     * @param id Identifiant unique de l'opération (UUID)
     * @param nom Libellé de l'opération
     * @param date Date de l'opération
     * @param montant Montant de l'opération
     * @param compteId Identifiant du compte associé
     * @param categorieId Identifiant de la catégorie associée
     */
    Operation(const QString& id,
              const QString& nom,
              const QDate& date,
              double montant,
              const QString& compteId,
              const QString& categorieId);

    /**
     * @brief Retourne l'identifiant de l'opération.
     * @return UUID de l'opération
     */
    QString getId() const;

    /**
     * @brief Retourne le nom (libellé) de l'opération.
     * @return Nom de l'opération
     */
    QString getNom() const;

    /**
     * @brief Retourne la date de l'opération.
     * @return Date de l'opération
     */
    QDate getDate() const;

    /**
     * @brief Retourne le montant de l'opération.
     * @return Montant de l'opération
     */
    double getMontant() const;

    /**
     * @brief Retourne l'identifiant du compte associé.
     * @return ID du compte
     */
    QString getCompteId() const;

    /**
     * @brief Retourne l'identifiant de la catégorie associée.
     * @return ID de la catégorie
     */
    QString getCategorieId() const;

protected:
    QString id;          /**< Identifiant unique de l'opération */
    QString nom;         /**< Nom ou libellé de l'opération */
    QDate date;          /**< Date de l'opération */
    double montant;      /**< Montant de l'opération */
    QString compteId;    /**< Identifiant du compte lié */
    QString categorieId; /**< Identifiant de la catégorie liée */
};

#endif // OPERATION_H
