#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include <QDate>

class Categorie;

class Operation
{
public:
    virtual ~Operation() = default;

    QString getId() const;
    QString getNom() const;
    QDate getDate() const;
    double getMontant() const;

protected:
    Operation(const QString& id,
              const QString& nom,
              const QDate& date,
              double montant);

    QString id;      // UUID
    QString nom;
    QDate date;
    double montant;
};

#endif // OPERATION_H
