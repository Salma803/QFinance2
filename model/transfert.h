#ifndef TRANSFERT_H
#define TRANSFERT_H

#include <QDate>

class Compte;


/**
 * @class Transfert
 * @brief Représente un transfert d'argent entre deux comptes.
 *
 * Un transfert correspond à un mouvement financier entre :
 * - un compte source
 * - un compte destination
 *
 * Les transferts sont utilisés pour :
 * - les virements manuels
 * - les transferts automatiques en cas de solde négatif
 *
 * Ils impactent directement le solde des comptes concernés.
 */
class Transfert
{
public:
    /**
     * @brief Constructeur de la classe Transfert.
     *
     * Lors de la création du transfert :
     * - le transfert est ajouté à la liste des transferts sortants du compte source
     * - le transfert est ajouté à la liste des transferts entrants du compte destination
     *
     * @param montant Montant du transfert
     * @param date Date du transfert
     * @param source Compte source (peut être nullptr)
     * @param destination Compte destination (peut être nullptr)
     */
    Transfert(double montant,
              const QDate& date,
              Compte* source,
              Compte* destination);

    // Getters
    double getMontant() const;
    QDate getDate() const;
    Compte* getSource() const;
    Compte* getDestination() const;

private:
    double montant;
    QDate date;

    Compte* source;
    Compte* destination;
};

#endif // TRANSFERT_H
