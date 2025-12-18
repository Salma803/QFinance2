#include "Transfert.h"
#include "Compte.h"

Transfert::Transfert(double montant,
                     const QDate& date,
                     Compte* source,
                     Compte* destination)
    : montant(montant),
    date(date),
    source(source),
    destination(destination)
{
    if (source) {
        source->ajouterTransfertSource(this);
    }

    if (destination) {
        destination->ajouterTransfertDestination(this);
    }
}

// =======================
// Getters
// =======================

double Transfert::getMontant() const
{
    return montant;
}

QDate Transfert::getDate() const
{
    return date;
}

Compte* Transfert::getSource() const
{
    return source;
}

Compte* Transfert::getDestination() const
{
    return destination;
}
