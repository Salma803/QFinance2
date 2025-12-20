#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model/Utilisateur.h"
#include "model/Categorie.h"
#include "dashboardmanager.h"
#include "repository/categorierepository.h"



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
    void actualiserDashboard();
    void initialiserDashboard();

    void supprimerOperation();
    void onTableOperationsCellChanged(int row, int column);


private:
    void rafraichirUI();
    void initialiserComboMois();

    Ui::MainWindow *ui;   // pointeur vers type incomplet (OK)
    CategorieRepository categorieRepository;
    QString currentOperationId;
    bool isModifyingTable = false;


    Utilisateur utilisateur;
    QList<Categorie*> categories;
    void remplirCombosOperation();
    void remplirHistoriqueComptes();
    void chargerHistoriqueCompte();
    void remplirCategoriesPrincipales();
    void chargerSousCategories();
    void onOperationSelectionChanged();

    // Méthodes
    QString findCategorieIdByName(const QString &categorieName);
    void restoreOriginalValue(int row, int column, const QVariantMap &originalData);




    DashboardManager *dashboardManager;

};

#endif // MAINWINDOW_H
