#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ui_dialogAjouterCategorie.h"
#include "ui_dialogModifierCategorie.h"
#include "ui_dialogSupprimerCategorie.h"

#include "model/CompteCourant.h"
#include "model/CompteEpargne.h"
#include "model/Transfert.h"
#include "repository/compterepository.h"
#include "repository/TransfertRepository.h"
#include "repository/CategorieRepository.h"
#include "repository/BudgetRepository.h"
#include "repository/OperationRepository.h"


#include <QUuid>
#include <QPushButton>
#include <QDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    utilisateur("1", "Utilisateur principal")
{
    ui->setupUi(this);

    // Connexions
    connect(ui->btnAjouterCompte, &QPushButton::clicked,
            this, &MainWindow::ajouterCompte);
    connect(ui->btnTransfert, &QPushButton::clicked,
            this, &MainWindow::effectuerTransfert);
    connect(ui->btnAjouterCategorie, &QPushButton::clicked,
            this, &MainWindow::ajouterCategorie);
    connect(ui->btnModifierCategorie, &QPushButton::clicked,
            this, &MainWindow::modifierCategorie);
    connect(ui->btnSupprimerCategorie, &QPushButton::clicked,
            this, &MainWindow::supprimerCategorie);
    connect(ui->btnAjouterOperation, &QPushButton::clicked,
            this, &MainWindow::ajouterOperation);
    connect(ui->comboCompteHistorique,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::chargerHistoriqueCompte);


    // ✅ 1) Charger comptes depuis DB
    QList<Compte*> comptes = CompteRepository::chargerComptes("1");
    for (Compte* c : comptes) {
        utilisateur.ajouterCompte(c);
    }
    rafraichirUI();

    // ✅ 2) Charger catégories depuis DB
    categories = CategorieRepository::chargerCategories("1");
    chargerCategoriesUI();

    // ✅ 3) Remplir combos opérations
    remplirCombosOperation();
    remplirHistoriqueComptes();

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::ajouterCompte()
{
    QString nom = ui->editNomCompte->text();
    if (nom.isEmpty())
        return;

    QString id = QUuid::createUuid().toString();

    Compte* compte = nullptr;

    if (ui->comboTypeCompte->currentText() == "Courant") {
        compte = new CompteCourant(id, nom, 0);
    } else {
        compte = new CompteEpargne(id, nom, 0);
    }

    // 🔑 Persistance en base (utilisateur = "1")
    if (CompteRepository::ajouterCompte(compte, "1")) {
        utilisateur.ajouterCompte(compte);
        rafraichirUI();
        ui->editNomCompte->clear();
    } else {
        delete compte; // évite fuite mémoire
    }
}


void MainWindow::effectuerTransfert()
{
    int srcIndex = ui->comboSource->currentIndex();
    int dstIndex = ui->comboDestination->currentIndex();

    if (srcIndex == dstIndex || srcIndex < 0 || dstIndex < 0)
        return;

    double montant = ui->spinMontant->value();
    if (montant <= 0)
        return;

    Compte* source = utilisateur.getComptes().at(srcIndex);
    Compte* destination = utilisateur.getComptes().at(dstIndex);

    // 1️⃣ Création du transfert (met à jour les soldes en mémoire)
    Transfert* transfert = new Transfert(
        montant,
        QDate::currentDate(),
        source,
        destination
        );

    // 2️⃣ Persistance du transfert
    if (!TransfertRepository::ajouterTransfert(transfert)) {
        delete transfert;
        return;
    }

    // 3️⃣ Mise à jour des soldes en base
    CompteRepository::mettreAJourSolde(source);
    CompteRepository::mettreAJourSolde(destination);

    rafraichirUI();
}


void MainWindow::rafraichirUI()
{
    ui->listComptes->clear();
    ui->comboSource->clear();
    ui->comboDestination->clear();

    for (Compte* c : utilisateur.getComptes()) {
        QString text = c->getNom() + " | Solde : " + QString::number(c->getSolde());
        ui->listComptes->addItem(text);
        ui->comboSource->addItem(c->getNom());
        ui->comboDestination->addItem(c->getNom());
    }
}
void MainWindow::ajouterCategorie()
{
    QDialog dialog(this);
    Ui::DialogAjouterCategorie uiDialog;
    uiDialog.setupUi(&dialog);

    connect(uiDialog.btnOk, &QPushButton::clicked,
            &dialog, &QDialog::accept);
    connect(uiDialog.btnCancel, &QPushButton::clicked,
            &dialog, &QDialog::reject);

    uiDialog.comboParentCategorie->addItem("Aucune", "");

    for (Categorie* c : categories) {
        uiDialog.comboParentCategorie->addItem(c->getNom(), c->getId());
    }

    if (dialog.exec() == QDialog::Accepted) {
        QString nom = uiDialog.editNomCategorie->text();
        QString parentId = uiDialog.comboParentCategorie->currentData().toString();
        double budget = uiDialog.spinBudget->value();

        if (nom.isEmpty())
            return;

        // 1️⃣ Ajouter la catégorie
        QString categorieId =
            CategorieRepository::ajouterCategorie(nom, "1", parentId);

        // 2️⃣ Si un budget est défini, l’enregistrer
        if (budget > 0) {
            BudgetRepository::definirBudget(
                categorieId,
                QDate::currentDate().month(),
                QDate::currentDate().year(),
                budget
                );
        }

        // 3️⃣ Recharger et rafraîchir
        categories = CategorieRepository::chargerCategories("1");
        chargerCategoriesUI();
    }
}

void MainWindow::modifierCategorie()
{
    QTreeWidgetItem* item = ui->treeCategories->currentItem();
    if (!item)
        return;

    // Retrouver la catégorie associée
    Categorie* categorie = nullptr;
    for (Categorie* c : categories) {
        if (item->text(0).startsWith(c->getNom())) {
            categorie = c;
            break;
        }
    }

    if (!categorie)
        return;

    QDialog dialog(this);
    Ui::DialogModifierCategorie uiDialog;
    uiDialog.setupUi(&dialog);

    // Pré-remplir
    uiDialog.editNomCategorie->setText(categorie->getNom());

    int mois = QDate::currentDate().month();
    int annee = QDate::currentDate().year();

    double budgetActuel = BudgetRepository::obtenirBudget(
        categorie->getId(), mois, annee
        );

    if (budgetActuel >= 0)
        uiDialog.spinBudget->setValue(budgetActuel);

    connect(uiDialog.btnOk, &QPushButton::clicked,
            &dialog, &QDialog::accept);
    connect(uiDialog.btnCancel, &QPushButton::clicked,
            &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString nouveauNom = uiDialog.editNomCategorie->text();
        double nouveauBudget = uiDialog.spinBudget->value();

        if (nouveauNom.isEmpty())
            return;

        // 1️⃣ Modifier le nom
        CategorieRepository::modifierCategorie(
            categorie->getId(), nouveauNom
            );

        // 2️⃣ Modifier / définir le budget
        if (nouveauBudget > 0) {
            BudgetRepository::definirBudget(
                categorie->getId(), mois, annee, nouveauBudget
                );
        }

        // 3️⃣ Recharger
        categories = CategorieRepository::chargerCategories("1");
        chargerCategoriesUI();
    }
}

void MainWindow::supprimerCategorie()
{
    if (!ui->treeCategories->currentItem())
        return;

    QDialog dialog(this);
    Ui::DialogSupprimerCategorie uiDialog;
    uiDialog.setupUi(&dialog);

    dialog.exec();
}
void MainWindow::chargerCategoriesUI()
{
    ui->treeCategories->clear();

    QMap<QString, QTreeWidgetItem*> map;

    int mois = QDate::currentDate().month();
    int annee = QDate::currentDate().year();

    // 1️⃣ Création des items
    for (Categorie* c : categories) {

        QString label = c->getNom();

        // 🔑 Récupérer le budget (hérité si besoin)
        QString budgetSource = c->getCategorieBudgetSource();
        double budget = BudgetRepository::obtenirBudget(
            budgetSource, mois, annee
            );

        if (budget >= 0) {
            label += "  [Budget: " + QString::number(budget) + " €]";
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << label);
        map[c->getId()] = item;
    }

    // 2️⃣ Hiérarchie parent / enfant
    for (Categorie* c : categories) {
        QTreeWidgetItem* item = map[c->getId()];
        if (c->getParent()) {
            map[c->getParent()->getId()]->addChild(item);
        } else {
            ui->treeCategories->addTopLevelItem(item);
        }
    }

    ui->treeCategories->expandAll();
}


void MainWindow::ajouterOperation()
{
    QString type = ui->comboTypeOperation->currentText();
    QString nom = ui->editNomOperation->text();
    double montant = ui->spinMontantOperation->value();
    QDate date = ui->dateOperation->date();

    QString compteId = ui->comboCompteOperation->currentData().toString();
    QString categorieId = ui->comboCategorieOperation->currentData().toString();

    if (nom.isEmpty() || montant <= 0)
        return;

    if (type == "Revenu") {
        OperationRepository::ajouterRevenu(
            nom, date, montant,
            compteId, categorieId,
            "Non spécifiée"
            );
    } else {
        bool recurrente = ui->checkRecurrente->isChecked();
        QString frequence = ui->comboFrequence->currentText();

        OperationRepository::ajouterDepense(
            nom, date, montant,
            compteId, categorieId,
            recurrente, frequence
            );
    }

    // Mise à jour solde
    Compte* compte = utilisateur.getCompteById(compteId);
    compte->mettreAJourSolde();
    CompteRepository::mettreAJourSolde(compte);

    rafraichirUI();
}
void MainWindow::remplirCombosOperation()
{
    ui->comboCompteOperation->clear();
    ui->comboCategorieOperation->clear();

    for (Compte* c : utilisateur.getComptes()) {
        ui->comboCompteOperation->addItem(c->getNom(), c->getId());
    }

    for (Categorie* cat : categories) {
        ui->comboCategorieOperation->addItem(cat->getNom(), cat->getId());
    }
}
void MainWindow::remplirHistoriqueComptes()
{
    ui->comboCompteHistorique->clear();

    for (Compte* c : utilisateur.getComptes()) {
        ui->comboCompteHistorique->addItem(c->getNom(), c->getId());
    }
}

void MainWindow::chargerHistoriqueCompte()
{
    ui->tableOperations->setRowCount(0);

    QString compteId =
        ui->comboCompteHistorique->currentData().toString();

    if (compteId.isEmpty())
        return;

    QList<QVariantMap> ops =
        OperationRepository::chargerOperationsParCompte(compteId);

    ui->tableOperations->setRowCount(ops.size());

    for (int i = 0; i < ops.size(); ++i) {
        ui->tableOperations->setItem(i, 0,
                                     new QTableWidgetItem(ops[i]["date"].toString()));
        ui->tableOperations->setItem(i, 1,
                                     new QTableWidgetItem(ops[i]["nom"].toString()));
        ui->tableOperations->setItem(i, 2,
                                     new QTableWidgetItem(ops[i]["categorie"].toString()));
        ui->tableOperations->setItem(i, 3,
                                     new QTableWidgetItem(
                                         QString::number(ops[i]["montant"].toDouble())
                                         ));
    }
}

