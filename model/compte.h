#ifndef COMPTE_H
#define COMPTE_H

#include <QString>
#include <QList>

class Operation;
class Transfert;

class Compte
{
public:
    virtual ~Compte() = default;

    // Getters
    QString getId() const;
    QString getNom() const;
    double getSolde() const;

    // Méthodes métier (diagramme UML)
    void ajouterOperation(Operation* operation);
    void supprimerOperation(Operation* operation);

    void ajouterTransfertSource(Transfert* transfert);
    void ajouterTransfertDestination(Transfert* transfert);

    virtual void mettreAJourSolde() = 0;

protected:
    // Constructeur protégé (classe abstraite)
    Compte(const QString& id, const QString& nom, double solde);

    QString id;                         // UUID
    QString nom;
    double solde;

    QList<Operation*> operations;
    QList<Transfert*> transfertsSource;
    QList<Transfert*> transfertsDestination;
};

#endif // COMPTE_H
