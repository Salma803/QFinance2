#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model/Utilisateur.h"
#include "model/Categorie.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ajouterCompte();
    void effectuerTransfert();
    void ajouterCategorie();
    void modifierCategorie();
    void supprimerCategorie();
    void chargerCategoriesUI();
    void ajouterOperation();

private:
    void rafraichirUI();

    Ui::MainWindow *ui;   // pointeur vers type incomplet (OK)

    Utilisateur utilisateur;
    QList<Categorie*> categories;
    void remplirCombosOperation();
    void remplirHistoriqueComptes();
    void chargerHistoriqueCompte();
    void remplirCategoriesPrincipales();
    void chargerSousCategories();

};

#endif // MAINWINDOW_H
