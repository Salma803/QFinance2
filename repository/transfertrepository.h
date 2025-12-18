#ifndef TRANSFERTREPOSITORY_H
#define TRANSFERTREPOSITORY_H

class Transfert;

class TransfertRepository
{
public:
    static bool ajouterTransfert(const Transfert* transfert);
};

#endif // TRANSFERTREPOSITORY_H
