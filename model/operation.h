#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include <QDate>

class Operation
{
public:
    Operation(const QString& id,
              const QString& nom,
              const QDate& date,
              double montant,
              const QString& compteId,
              const QString& categorieId);

    virtual ~Operation() = default;

    QString getId() const;
    QString getNom() const;
    QDate getDate() const;
    double getMontant() const;
    QString getCompteId() const;
    QString getCategorieId() const;

    virtual bool estDepense() const = 0;

protected:
    QString id;
    QString nom;
    QDate date;
    double montant;
    QString compteId;
    QString categorieId;
};

#endif // OPERATION_H
