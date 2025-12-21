#ifndef TRANSFERTREPOSITORY_H
#define TRANSFERTREPOSITORY_H
#include <QVariantMap>
class Transfert;

class TransfertRepository
{
public:
    static bool ajouterTransfert(const Transfert* transfert);
    static QList<QVariantMap> chargerTransfertsParCompte(
        const QString& compteId
        );
    static bool ajouterTransfertAuto(
        const QString& sourceId,
        const QString& destinationId,
        double montant,
        const QDate& date
        );


};

#endif // TRANSFERTREPOSITORY_H
