#ifndef TRANSFERT_H
#define TRANSFERT_H

#include <QDate>

class Compte;

class Transfert
{
public:
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
