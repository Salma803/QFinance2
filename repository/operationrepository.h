#ifndef OPERATIONREPOSITORY_H
#define OPERATIONREPOSITORY_H

#include <QString>
#include <QDate>

class OperationRepository
{
public:
    static bool ajouterRevenu(const QString& nom,
                              const QDate& date,
                              double montant,
                              const QString& compteId,
                              const QString& categorieId,
                              const QString& provenance);

    static bool ajouterDepense(const QString& nom,
                               const QDate& date,
                               double montant,
                               const QString& compteId,
                               const QString& categorieId,
                               bool estRecurrente,
                               const QString& frequence);
    static QList<QVariantMap> chargerOperationsParCompte(const QString& compteId);

    static double getTotalRevenus(int mois, int annee);
    static double getTotalDepenses(int mois, int annee);
    static double getTotalDepensesCategorie(const QString &categorieId, int mois, int annee);
    static QList<QVariantMap> getDepensesParCategorie(int mois, int annee);

    // Méthodes pour la suppression et modification
    static QString getOperationId(const QString& compteId, const QString& nomOperation,
                                  const QDate& date, double montant);

    static bool supprimerOperation(const QString& operationId, const QString& compteId);

    // NOUVELLES MÉTHODES POUR LA MODIFICATION
    static QVariantMap getOperationDetails(const QString& operationId);
    static bool modifierOperation(const QString& operationId,
                                  const QString& nouveauNom,
                                  const QDate& nouvelleDate,
                                  double nouveauMontant,
                                  const QString& nouvelleCategorieId);
};

#endif // OPERATIONREPOSITORY_H
