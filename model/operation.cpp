#include "Operation.h"

/**
 * @brief Constructeur de la classe Operation.
 */
Operation::Operation(const QString& id,
                     const QString& nom,
                     const QDate& date,
                     double montant,
                     const QString& compteId,
                     const QString& categorieId)
    : id(id),
    nom(nom),
    date(date),
    montant(montant),
    compteId(compteId),
    categorieId(categorieId)
{
}

QString Operation::getId() const { return id; }
QString Operation::getNom() const { return nom; }
QDate Operation::getDate() const { return date; }
double Operation::getMontant() const { return montant; }
QString Operation::getCompteId() const { return compteId; }
QString Operation::getCategorieId() const { return categorieId; }
