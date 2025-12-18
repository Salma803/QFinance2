#include "Operation.h"

Operation::Operation(const QString& id,
                     const QString& nom,
                     const QDate& date,
                     double montant)
    : id(id), nom(nom), date(date), montant(montant)
{
}

QString Operation::getId() const
{
    return id;
}

QString Operation::getNom() const
{
    return nom;
}

QDate Operation::getDate() const
{
    return date;
}

double Operation::getMontant() const
{
    return montant;
}
