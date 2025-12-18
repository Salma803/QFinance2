#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <QString>
#include <QList>

class Compte; // forward declaration

class Utilisateur
{
public:
    // Constructeurs
    Utilisateur();
    Utilisateur(const QString& id, const QString& nom);

    // Getters
    QString getId() const;
    QString getNom() const;
    QList<Compte*> getComptes() const;

    // Setters
    void setNom(const QString& nom);

    // Méthodes métier (diagramme UML)
    void ajouterCompte(Compte* compte);
    void supprimerCompte(Compte* compte);

private:
    QString id;                 // UUID
    QString nom;
    QList<Compte*> comptes;     // ensemble des comptes
};

#endif // UTILISATEUR_H
