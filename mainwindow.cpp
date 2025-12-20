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

#include <QDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QUuid>
#include <QPushButton>
#include <QDate>
#include <QDebug>
#include <QSqlQuery>
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    utilisateur("1", "Utilisateur principal"),
    dashboardManager(new DashboardManager(this))
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
    connect(ui->comboCategoriePrincipale,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::chargerSousCategories);

    connect(ui->btnSupprimerOperation, &QPushButton::clicked,
            this, &MainWindow::supprimerOperation);

    connect(ui->tableOperations, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onOperationSelectionChanged);

    // Connexions pour le dashboard
    connect(ui->btnActualiser, &QPushButton::clicked,
            this, &MainWindow::actualiserDashboard);

    // IMPORTANT: Ne pas connecter cellChanged ici - nous le ferons dans chargerHistoriqueCompte

    // ✅ 1) Charger comptes depuis DB
    QList<Compte*> comptes = CompteRepository::chargerComptes("1");
    for (Compte* c : comptes) {
        utilisateur.ajouterCompte(c);
    }
    rafraichirUI();

    // ✅ 2) Créer catégories par défaut et charger depuis DB
    categorieRepository.creerCategoriesParDefaut();
    categories = categorieRepository.chargerCategories("1");

    // ✅ IMPORTANT: initialiser dashboard AVANT d'utiliser comboMois/spinAnnee
    initialiserDashboard();

    // ✅ Maintenant seulement afficher
    chargerCategoriesUI();

    // ✅ 3) Remplir combos opérations
    remplirCombosOperation();
    remplirHistoriqueComptes();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialiserDashboard()
{
    // Initialiser le combo des mois
    initialiserComboMois();

    // Définir l'année courante
    int anneeCourante = QDate::currentDate().year();
    ui->spinAnnee->setValue(anneeCourante);

    // Initialiser les charts
    dashboardManager->initialiserCharts(
        ui->chartDepensesParCategorie,
        ui->chartRevenusVsDepenses
        );

    dashboardManager->setCategories(categories);

    // Actualiser le dashboard avec les valeurs par défaut
    actualiserDashboard();
}

void MainWindow::initialiserComboMois()
{
    ui->comboMois->clear();

    QStringList moisNoms = {
        "Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"
    };

    for (int i = 0; i < 12; i++) {
        ui->comboMois->addItem(moisNoms[i], i + 1);
    }

    // Sélectionner le mois courant
    int moisCourant = QDate::currentDate().month();
    ui->comboMois->setCurrentIndex(moisCourant - 1);
}

void MainWindow::actualiserDashboard()
{
    int mois = ui->comboMois->currentData().toInt();
    int annee = ui->spinAnnee->value();

    qDebug() << "Actualisation du dashboard - Mois:" << mois << "Année:" << annee;

    dashboardManager->actualiserDashboard(mois, annee, categories);
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

    if (CompteRepository::ajouterCompte(compte, "1")) {
        CompteRepository::mettreAJourSolde(compte->getId());
        utilisateur.ajouterCompte(compte);
        rafraichirUI();
        remplirCombosOperation();
        remplirHistoriqueComptes();
    } else {
        delete compte;
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

    Transfert* transfert = new Transfert(
        montant,
        QDate::currentDate(),
        source,
        destination
        );

    if (!TransfertRepository::ajouterTransfert(transfert)) {
        delete transfert;
        return;
    }

    CompteRepository::mettreAJourSolde(source->getId());
    CompteRepository::mettreAJourSolde(destination->getId());

    rafraichirUI();
    chargerHistoriqueCompte();
    actualiserDashboard();
}

void MainWindow::rafraichirUI()
{
    ui->listComptes->clear();
    ui->comboSource->clear();
    ui->comboDestination->clear();

    for (Compte* c : utilisateur.getComptes()) {

        // 🔑 Solde recalculé depuis la DB
        double solde = CompteRepository::calculerSolde(c->getId());

        QString text = c->getNom()
                       + " | Solde : "
                       + QString::number(solde, 'f', 2);

        ui->listComptes->addItem(text);
        ui->comboSource->addItem(c->getNom(), c->getId());
        ui->comboDestination->addItem(c->getNom(), c->getId());
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

        // Utilise l'instance categorieRepository
        QString categorieId = categorieRepository.ajouterCategorie(nom, "1", parentId);

        if (budget > 0) {
            BudgetRepository::definirBudget(
                categorieId,
                QDate::currentDate().month(),
                QDate::currentDate().year(),
                budget
                );
        }

        // Recharger les catégories
        categories = categorieRepository.chargerCategories("1");
        chargerCategoriesUI();

        dashboardManager->setCategories(categories);
        actualiserDashboard();
    }
}

void MainWindow::modifierCategorie()
{
    QTreeWidgetItem* item = ui->treeCategories->currentItem();
    if (!item)
        return;

    // Récupérer l'ID depuis l'item
    QString categorieId = item->data(0, Qt::UserRole).toString();
    if (categorieId.isEmpty())
        return;

    // Trouver la catégorie
    Categorie* categorie = nullptr;
    for (Categorie* c : categories) {
        if (c->getId() == categorieId) {
            categorie = c;
            break;
        }
    }

    if (!categorie)
        return;

    QDialog dialog(this);
    Ui::DialogModifierCategorie uiDialog;
    uiDialog.setupUi(&dialog);

    uiDialog.editNomCategorie->setText(categorie->getNom());

    int mois = QDate::currentDate().month();
    int annee = QDate::currentDate().year();

    double budgetActuel = BudgetRepository::obtenirBudget(
        categorieId, mois, annee
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

        // Utilise l'instance categorieRepository
        categorieRepository.modifierCategorie(categorieId, nouveauNom);

        if (nouveauBudget > 0) {
            BudgetRepository::definirBudget(
                categorieId, mois, annee, nouveauBudget
                );
        }

        // Recharger les catégories
        categories = categorieRepository.chargerCategories("1");
        chargerCategoriesUI();

        dashboardManager->setCategories(categories);
        actualiserDashboard();
    }
}

void MainWindow::chargerCategoriesUI()
{
    ui->treeCategories->clear();

    int mois = ui->comboMois->currentData().toInt();
    if (mois <= 0)
        mois = QDate::currentDate().month();

    int annee = ui->spinAnnee->value();
    if (annee <= 0)
        annee = QDate::currentDate().year();

    QMap<QString, QTreeWidgetItem*> map;

    for (Categorie* c : categories) {

        double budget = BudgetRepository::obtenirBudget(
            c->getId(), mois, annee
            );


        double depenses = BudgetRepository::calculerDepensesCategorie(
            c->getId(), mois, annee
            );

        double restant = budget - depenses;

        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, c->getNom());

        if (budget >= 0) {
            item->setText(1, QString::number(budget, 'f', 2));
            item->setText(2, QString::number(depenses, 'f', 2));
            item->setText(3, QString::number(restant, 'f', 2));

            item->setForeground(3, restant < 0 ? Qt::red : Qt::darkGreen);
        } else {
            item->setText(1, "-");
            item->setText(2, "-");
            item->setText(3, "-");
        }

        item->setData(0, Qt::UserRole, c->getId());
        map[c->getId()] = item;
    }

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


void MainWindow::supprimerCategorie()
{
    QTreeWidgetItem* item = ui->treeCategories->currentItem();
    if (!item)
        return;

    QString categorieId = item->data(0, Qt::UserRole).toString();
    QString nomCategorie = item->text(0);

    if (categorieId.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Impossible de récupérer l'ID de la catégorie.");
        return;
    }

    // Vérifier si la catégorie existe dans la base
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Categorie WHERE id = :id");
    checkQuery.bindValue(":id", categorieId);

    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Erreur",
                             QString("Catégorie \"%1\" non trouvée dans la base de données.").arg(nomCategorie));
        return;
    }

    // Dialogue de confirmation
    QDialog dialog(this);
    Ui::DialogSupprimerCategorie uiDialog;
    uiDialog.setupUi(&dialog);

    uiDialog.label->setText(QString("Supprimer \"%1\" et ses sous-catégories ?").arg(nomCategorie));

    connect(uiDialog.btnOui, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(uiDialog.btnNon, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Tentative de suppression de la catégorie:" << nomCategorie << "ID:" << categorieId;

        bool succes = categorieRepository.supprimerCategorieAvecEnfants(categorieId);

        if (succes) {
            QMessageBox::information(this, "Succès", "Catégorie supprimée avec succès");

            // Recharger les catégories
            categories = categorieRepository.chargerCategories("1");
            chargerCategoriesUI();
            remplirCombosOperation();

            // Actualiser le dashboard
            dashboardManager->setCategories(categories);
            actualiserDashboard();
        } else {
            QMessageBox::warning(this, "Erreur",
                                 "Impossible de supprimer la catégorie.");
        }
    }
}

void MainWindow::ajouterOperation()
{
    int typeIndex = ui->comboTypeOperation->currentIndex();
    QString nom = ui->editNomOperation->text();
    double montant = ui->spinMontantOperation->value();
    QDate date = ui->dateOperation->date();

    QString compteId = ui->comboCompteOperation->currentData().toString();
    QString categorieId = ui->comboSousCategorie->currentData().toString();

    if (categorieId.isEmpty()) {
        categorieId = ui->comboCategoriePrincipale->currentData().toString();
    }

    if (nom.isEmpty() || montant <= 0 || compteId.isEmpty() || categorieId.isEmpty())
        return;

    if (typeIndex == 0) { // Revenu
        OperationRepository::ajouterRevenu(
            nom, date, montant,
            compteId, categorieId,
            "Non spécifiée"
            );
    } else { // Dépense
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
    if (compte) {
        CompteRepository::mettreAJourSolde(compteId);

        // ✅ Rafraîchissements
        rafraichirUI();
        chargerHistoriqueCompte();
        chargerCategoriesUI();
        actualiserDashboard();
    }
}

void MainWindow::remplirCombosOperation()
{
    // Comptes
    ui->comboCompteOperation->clear();
    for (Compte* c : utilisateur.getComptes()) {
        if (c->estCourant()) {
            ui->comboCompteOperation->addItem(c->getNom(), c->getId());
        }
    }

    // Catégories principales
    ui->comboCategoriePrincipale->clear();
    ui->comboCategoriePrincipale->addItem("Sélectionnez une catégorie", "");
    for (Categorie* c : categories) {
        if (!c->getParent()) {
            ui->comboCategoriePrincipale->addItem(c->getNom(), c->getId());
        }
    }

    // Sous-catégories
    chargerSousCategories();
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
    // Déconnecter d'abord pour éviter les signaux pendant le chargement
    disconnect(ui->tableOperations, &QTableWidget::cellChanged,
               this, &MainWindow::onTableOperationsCellChanged);

    ui->tableOperations->setRowCount(0);
    ui->tableOperations->clearContents();

    QString compteId = ui->comboCompteHistorique->currentData().toString();

    if (compteId.isEmpty()) {
        // Reconnecter
        connect(ui->tableOperations, &QTableWidget::cellChanged,
                this, &MainWindow::onTableOperationsCellChanged);
        return;
    }

    // Charger opérations
    QList<QVariantMap> ops = OperationRepository::chargerOperationsParCompte(compteId);

    // Charger transferts
    QList<QVariantMap> transferts = TransfertRepository::chargerTransfertsParCompte(compteId);

    // Fusionner
    QList<QVariantMap> all = ops;
    all.append(transferts);

    // Trier par date décroissante
    std::sort(all.begin(), all.end(),
              [](const QVariantMap& a, const QVariantMap& b) {
                  return a["date"].toDate() > b["date"].toDate();
              });

    // Afficher
    ui->tableOperations->setRowCount(all.size());

    for (int i = 0; i < all.size(); ++i) {
        // Stocker toutes les données dans un QVariantMap
        QVariantMap operationData;

        // Copier toutes les données originales
        operationData = all[i];

        // Ajouter l'ID si présent
        if (all[i].contains("id")) {
            operationData["id"] = all[i]["id"];
        }

        // Date
        QDate date = all[i]["date"].toDate();
        QTableWidgetItem *dateItem = new QTableWidgetItem(date.toString("dd/MM/yyyy"));
        dateItem->setData(Qt::UserRole, operationData);
        dateItem->setFlags(dateItem->flags() | Qt::ItemIsEditable);
        ui->tableOperations->setItem(i, 0, dateItem);

        // Nom
        QString nom = all[i]["nom"].toString();
        QTableWidgetItem *nomItem = new QTableWidgetItem(nom);
        nomItem->setData(Qt::UserRole, operationData);
        nomItem->setFlags(nomItem->flags() | Qt::ItemIsEditable);
        ui->tableOperations->setItem(i, 1, nomItem);

        // Catégorie
        QString categorie = all[i]["categorie"].toString();
        QString categorieId = all[i].value("categorie_id").toString();
        QTableWidgetItem *categorieItem = new QTableWidgetItem(categorie);
        categorieItem->setData(Qt::UserRole, operationData);
        categorieItem->setFlags(categorieItem->flags() | Qt::ItemIsEditable);

        // Stocker l'ID de catégorie dans UserRole+1
        if (!categorieId.isEmpty()) {
            categorieItem->setData(Qt::UserRole + 1, categorieId);
        }

        ui->tableOperations->setItem(i, 2, categorieItem);

        // Montant avec signe métier
        double montant = all[i]["montant"].toDouble();
        QString type = all[i].value("type").toString();

        if (type == "DEPENSE" || type == "TRANSFERT_SORTANT") {
            montant = -montant;
        }

        QTableWidgetItem *montantItem = new QTableWidgetItem(QString::number(montant, 'f', 2));
        montantItem->setData(Qt::UserRole, operationData);
        montantItem->setFlags(montantItem->flags() | Qt::ItemIsEditable);

        if (montant < 0) {
            montantItem->setForeground(Qt::red);
        } else {
            montantItem->setForeground(Qt::darkGreen);
        }

        ui->tableOperations->setItem(i, 3, montantItem);
    }

    // Reconnecter après le chargement
    connect(ui->tableOperations, &QTableWidget::cellChanged,
            this, &MainWindow::onTableOperationsCellChanged);

    // Désactiver le bouton de suppression par défaut
    ui->btnSupprimerOperation->setEnabled(false);
}

void MainWindow::chargerSousCategories()
{
    ui->comboSousCategorie->clear();

    QString parentId = ui->comboCategoriePrincipale->currentData().toString();

    if (parentId.isEmpty()) {
        ui->comboSousCategorie->addItem("Sélectionnez d'abord une catégorie", "");
        ui->comboSousCategorie->setEnabled(false);
        return;
    }

    bool hasChildren = false;

    for (Categorie* c : categories) {
        if (c->getParent() && c->getParent()->getId() == parentId) {
            ui->comboSousCategorie->addItem(c->getNom(), c->getId());
            hasChildren = true;
        }
    }

    ui->comboSousCategorie->setEnabled(hasChildren);

    if (!hasChildren) {
        ui->comboSousCategorie->addItem("Aucune sous-catégorie", "");
    }
}

// Méthode appelée quand une opération est sélectionnée dans le tableau
void MainWindow::onOperationSelectionChanged()
{
    QList<QTableWidgetItem*> selectedItems = ui->tableOperations->selectedItems();
    if (selectedItems.isEmpty()) {
        currentOperationId = "";
        ui->btnSupprimerOperation->setEnabled(false);
        return;
    }

    int row = selectedItems.first()->row();

    // Récupérer les données de la ligne sélectionnée
    QString nom = ui->tableOperations->item(row, 1)->text();
    QString dateStr = ui->tableOperations->item(row, 0)->text();
    QString montantStr = ui->tableOperations->item(row, 3)->text();

    // Convertir la date
    QDate date = QDate::fromString(dateStr, "dd/MM/yyyy");

    // Nettoyer le montant (enlever le signe négatif si présent)
    double montant = qAbs(montantStr.toDouble());

    // Récupérer l'ID du compte courant
    QString compteId = ui->comboCompteHistorique->currentData().toString();

    // Récupérer l'ID de l'opération
    currentOperationId = OperationRepository::getOperationId(compteId, nom, date, montant);

    if (currentOperationId.isEmpty()) {
        qDebug() << "Impossible de trouver l'ID de l'opération";
        ui->btnSupprimerOperation->setEnabled(false);
    } else {
        qDebug() << "Opération sélectionnée - ID:" << currentOperationId;
        ui->btnSupprimerOperation->setEnabled(true);
    }
}

// Méthode pour supprimer l'opération sélectionnée
void MainWindow::supprimerOperation()
{
    if (currentOperationId.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune opération sélectionnée.");
        return;
    }

    QString compteId = ui->comboCompteHistorique->currentData().toString();
    if (compteId.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucun compte sélectionné.");
        return;
    }

    // Demander confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  "Êtes-vous sûr de vouloir supprimer cette opération ?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // Supprimer l'opération
    if (OperationRepository::supprimerOperation(currentOperationId, compteId)) {
        QMessageBox::information(this, "Succès", "Opération supprimée avec succès.");

        // Recharger l'historique
        chargerHistoriqueCompte();

        // Rafraîchir l'interface
        rafraichirUI();

        // Actualiser le dashboard
        actualiserDashboard();

        // Réinitialiser l'ID
        currentOperationId = "";
        ui->btnSupprimerOperation->setEnabled(false);
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de supprimer l'opération.");
    }
}

// Méthode appelée quand une cellule du tableau est modifiée
void MainWindow::onTableOperationsCellChanged(int row, int column)
{
    // Éviter les boucles infinies
    if (isModifyingTable) return;

    isModifyingTable = true;

    // Récupérer l'item modifié
    QTableWidgetItem *item = ui->tableOperations->item(row, column);
    if (!item) {
        isModifyingTable = false;
        return;
    }

    // Récupérer les données originales
    QVariantMap originalData = item->data(Qt::UserRole).toMap();
    if (originalData.isEmpty()) {
        isModifyingTable = false;
        return;
    }

    // Ignorer les transferts (pas d'ID d'opération)
    if (!originalData.contains("id") || originalData["id"].toString().isEmpty()) {
        isModifyingTable = false;
        return;
    }

    QString operationId = originalData["id"].toString();
    QString compteId = ui->comboCompteHistorique->currentData().toString();

    // Si pas d'ID de compte, annuler
    if (compteId.isEmpty()) {
        restoreOriginalValue(row, column, originalData);
        isModifyingTable = false;
        return;
    }

    // Récupérer les nouvelles valeurs de TOUTE la ligne
    QString newNom = ui->tableOperations->item(row, 1)->text();
    QString newDateStr = ui->tableOperations->item(row, 0)->text();
    QString newCategorie = ui->tableOperations->item(row, 2)->text();
    QString newMontantStr = ui->tableOperations->item(row, 3)->text();

    // Valider les nouvelles valeurs
    if (newNom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom ne peut pas être vide.");
        restoreOriginalValue(row, column, originalData);
        isModifyingTable = false;
        return;
    }

    QDate newDate = QDate::fromString(newDateStr, "dd/MM/yyyy");
    if (!newDate.isValid()) {
        QMessageBox::warning(this, "Erreur", "Date invalide. Format: JJ/MM/AAAA");
        restoreOriginalValue(row, column, originalData);
        isModifyingTable = false;
        return;
    }

    // Valider le montant
    bool ok;
    double newMontant = newMontantStr.toDouble(&ok);
    if (!ok || newMontant == 0) {
        QMessageBox::warning(this, "Erreur", "Montant invalide.");
        restoreOriginalValue(row, column, originalData);
        isModifyingTable = false;
        return;
    }

    // Déterminer si c'est une dépense ou un revenu basé sur le signe
    bool isDepense = newMontant < 0;
    double montantPourBD = qAbs(newMontant);

    // Trouver l'ID de catégorie
    QString newCategorieId = findCategorieIdByName(newCategorie);
    if (newCategorieId.isEmpty()) {
        // Essayer de récupérer l'ID stocké depuis l'item catégorie
        QTableWidgetItem *categorieItem = ui->tableOperations->item(row, 2);
        if (categorieItem) {
            newCategorieId = categorieItem->data(Qt::UserRole + 1).toString();
        }

        if (newCategorieId.isEmpty()) {
            QMessageBox::warning(this, "Erreur",
                                 QString("Catégorie \"%1\" non trouvée.").arg(newCategorie));
            restoreOriginalValue(row, column, originalData);
            isModifyingTable = false;
            return;
        }
    }

    // Vérifier les changements
    QString originalNom = originalData["nom"].toString();
    QDate originalDate = originalData["date"].toDate();
    double originalMontant = originalData["montant"].toDouble();
    QString originalCategorie = originalData["categorie"].toString();

    bool hasChanges = (originalNom != newNom) ||
                      (originalDate != newDate) ||
                      (originalMontant != montantPourBD) ||
                      (originalCategorie != newCategorie);

    if (!hasChanges) {
        // Pas de changement réel, sortir
        isModifyingTable = false;
        return;
    }

    // Demander confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  QString("Voulez-vous vraiment modifier cette opération ?\n\n"
                                          "Nom: %1 → %2\n"
                                          "Date: %3 → %4\n"
                                          "Montant: %5 € → %6 €\n"
                                          "Catégorie: %7 → %8")
                                      .arg(originalNom)
                                      .arg(newNom)
                                      .arg(originalDate.toString("dd/MM/yyyy"))
                                      .arg(newDate.toString("dd/MM/yyyy"))
                                      .arg(originalMontant)
                                      .arg(montantPourBD)
                                      .arg(originalCategorie)
                                      .arg(newCategorie),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Mettre à jour la base de données
        bool success = OperationRepository::modifierOperation(operationId,
                                                              newNom,
                                                              newDate,
                                                              montantPourBD,
                                                              newCategorieId);

        if (success) {
            QMessageBox::information(this, "Succès", "Opération modifiée avec succès.");

            // Mettre à jour les données stockées dans l'item
            QVariantMap updatedData = originalData;
            updatedData["nom"] = newNom;
            updatedData["date"] = newDate;
            updatedData["montant"] = montantPourBD;
            updatedData["categorie"] = newCategorie;
            updatedData["categorie_id"] = newCategorieId;

            // Mettre à jour toutes les cellules de la ligne
            for (int col = 0; col < 4; col++) {
                QTableWidgetItem *rowItem = ui->tableOperations->item(row, col);
                if (rowItem) {
                    rowItem->setData(Qt::UserRole, updatedData);
                }
            }

            // Mettre à jour aussi la catégorie dans UserRole+1
            QTableWidgetItem *categorieItem = ui->tableOperations->item(row, 2);
            if (categorieItem) {
                categorieItem->setData(Qt::UserRole + 1, newCategorieId);
            }

            // Rafraîchir l'interface
            rafraichirUI();
            actualiserDashboard();
        } else {
            QMessageBox::warning(this, "Erreur", "Échec de la modification.");
            restoreOriginalValue(row, column, originalData);
        }
    } else {
        // Annuler : restaurer la valeur originale
        restoreOriginalValue(row, column, originalData);
    }

    isModifyingTable = false;
}

// Méthode utilitaire pour restaurer la valeur originale
void MainWindow::restoreOriginalValue(int row, int column, const QVariantMap &originalData)
{
    QString originalValue;

    switch (column) {
    case 0: // Date
        originalValue = originalData["date"].toDate().toString("dd/MM/yyyy");
        break;
    case 1: // Nom
        originalValue = originalData["nom"].toString();
        break;
    case 2: // Catégorie
        originalValue = originalData["categorie"].toString();
        break;
    case 3: // Montant
        double montant = originalData["montant"].toDouble();
        QString type = originalData.value("type").toString();
        if (type == "DEPENSE") {
            originalValue = QString::number(-montant, 'f', 2);
        } else {
            originalValue = QString::number(montant, 'f', 2);
        }
        break;
    }

    // Restaurer la valeur sans déclencher à nouveau cellChanged
    QTableWidgetItem *item = ui->tableOperations->item(row, column);
    if (item) {
        disconnect(ui->tableOperations, &QTableWidget::cellChanged,
                   this, &MainWindow::onTableOperationsCellChanged);

        item->setText(originalValue);

        connect(ui->tableOperations, &QTableWidget::cellChanged,
                this, &MainWindow::onTableOperationsCellChanged);
    }
}

// Méthode pour trouver l'ID d'une catégorie par son nom
QString MainWindow::findCategorieIdByName(const QString &categorieName)
{
    for (Categorie* cat : categories) {
        if (cat->getNom() == categorieName) {
            return cat->getId();
        }

        // Vérifier aussi les sous-catégories
        for (Categorie* enfant : cat->getEnfants()) {
            if (enfant->getNom() == categorieName) {
                return enfant->getId();
            }
        }
    }

    return "";
}
