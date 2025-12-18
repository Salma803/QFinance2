#ifndef COMPTEREPOSITORY_H
#define COMPTEREPOSITORY_H

#include <QList>
#include <QString>

class Compte;
class Utilisateur;

class CompteRepository
{
public:
    static bool ajouterCompte(const Compte* compte, const QString& utilisateurId);
    static QList<Compte*> chargerComptes(const QString& utilisateurId);
    static bool mettreAJourSolde(const Compte* compte);

};

#endif // COMPTEREPOSITORY_H
