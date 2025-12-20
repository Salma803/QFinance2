#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>

// Forward declarations
namespace Ui {
class MainWindow;
}

// Model includes
#include "model/Utilisateur.h"
#include "model/Categorie.h"
#include "DashboardManager.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QTableWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructeur/Destructeur
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // === SLOTS POUR LA GESTION DES COMPTES ===
    void ajouterCompte();
    void effectuerTransfert();

    // === SLOTS POUR LA GESTION DES CATÉGORIES ===
    void ajouterCategorie();
    void modifierCategorie();
    void supprimerCategorie();
    void chargerCategoriesUI();

    // === SLOTS POUR LA GESTION DES OPÉRATIONS ===
    void ajouterOperation();
    void remplirCombosOperation();
    void remplirHistoriqueComptes();
    void chargerHistoriqueCompte();
    void chargerSousCategories();
    void onOperationSelectionChanged();
    void supprimerOperation();
    void onTableOperationsCellChanged(int row, int column);

    // === SLOTS POUR LE DASHBOARD ===
    void initialiserDashboard();
    void actualiserDashboard();
    void onFiltreCompteChanged(int index);
    void onFiltreCategorieChanged(int index);
    void mettreAJourRecommandations();

private:
    // === MÉTHODES PRIVÉES D'INITIALISATION ===
    void rafraichirUI();
    void initialiserComboMois();
    void initialiserFiltresDashboard();

    // === MÉTHODES PRIVÉES UTILITAIRES ===
    void restoreOriginalValue(int row, int column, const QVariantMap &originalData);
    QString findCategorieIdByName(const QString &categorieName);

    // === VARIABLES MEMBRES ===
    Ui::MainWindow *ui;                   // Interface utilisateur
    Utilisateur utilisateur;              // Utilisateur courant
    QList<Categorie*> categories;         // Liste des catégories
    DashboardManager *dashboardManager;   // Gestionnaire du dashboard

    // Variables pour la gestion des opérations
    bool isModifyingTable;                // Flag pour éviter les boucles infinies
    QString currentOperationId;           // ID de l'opération sélectionnée
};

#endif // MAINWINDOW_H
