#include "Transfert.h"
#include "Compte.h"

/**
 * @brief Constructeur de la classe Transfert.
 *
 * Ajoute automatiquement le transfert :
 * - comme transfert sortant au compte source
 * - comme transfert entrant au compte destination
 */
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
