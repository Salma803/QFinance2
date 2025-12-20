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
    static void mettreAJourSolde(const QString& compteId);
    static double calculerSolde(const QString& compteId);

};

#endif // COMPTEREPOSITORY_H
