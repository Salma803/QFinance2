#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ui_dialogAjouterCategorie.h"
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
#include <QRegularExpressionValidator>
#include <functional>

// AJOUTER CES INCLUSIONS POUR L'EXPORT PDF
#include <QFileDialog>
#include <QDir>
#include <QProgressDialog>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <exception>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    utilisateur("1", "Utilisateur principal"),
    dashboardManager(new DashboardManager(this))
{
    ui->setupUi(this);
    ui->comboTypeOperation->clear();
    ui->comboTypeOperation->addItem("Revenu", "REVENU");
    ui->comboTypeOperation->addItem("Dépense", "DEPENSE");


    // Connexions
    connect(ui->btnAjouterCompte, &QPushButton::clicked,
            this, &MainWindow::ajouterCompte);
    connect(ui->btnTransfert, &QPushButton::clicked,
            this, &MainWindow::effectuerTransfert);
    connect(ui->btnAjouterCategorie, &QPushButton::clicked,
            this, &MainWindow::ajouterCategorie);
    connect(ui->treeCategories,
            &QTreeWidget::itemChanged,
            this,
            &MainWindow::onCategorieItemChanged);
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
    connect(ui->comboFiltreCompte, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFiltreCompteChanged);
    connect(ui->comboFiltreCategorie, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFiltreCategorieChanged);

    // AJOUTER CES CONNEXIONS POUR L'EXPORT PDF
    connect(ui->btnExportPDF, &QPushButton::clicked,
            this, &MainWindow::exporterPDFStatistiques);
    connect(ui->btnExportPDFTousComptes, &QPushButton::clicked,
            this, &MainWindow::exporterPDFTousComptes);

    connect(ui->comboFiltreCategorieOp,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::appliquerFiltreOperations);

    connect(ui->editFiltreTexteOp,
            &QLineEdit::textChanged,
            this,
            &MainWindow::appliquerFiltreOperations);

    QRegularExpression regex("^[a-zA-Z0-9 éèêàùçîôû_-]*$");
    QValidator* filtreValidator =
        new QRegularExpressionValidator(regex, this);

    ui->editFiltreTexteOp->setValidator(filtreValidator);



    // IMPORTANT: Ne pas connecter cellChanged ici - nous le ferons dans chargerHistoriqueCompte

    // ✅ 1) Charger comptes depuis DB
    QList<Compte*> comptes = CompteRepository::chargerComptes("1");
    for (Compte* c : comptes) {
        utilisateur.ajouterCompte(c);
    }
    rafraichirUI();

    // ✅ 2) Créer catégories par défaut et charger depuis DB
    // Note: Assurez-vous d'avoir une instance de CategorieRepository
    // Si vous avez une variable categorieRepository, utilisez-la
    // categorieRepository.creerCategoriesParDefaut();
    categories = CategorieRepository::chargerCategories("1");

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
        initialiserFiltresDashboard(); // Mettre à jour les filtres du dashboard
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

        // Utilise la méthode statique de CategorieRepository
        QString categorieId = CategorieRepository::ajouterCategorie(nom, "1", parentId);

        if (budget > 0) {
            BudgetRepository::definirBudget(
                categorieId,
                QDate::currentDate().month(),
                QDate::currentDate().year(),
                budget
                );
        }

        // Recharger les catégories
        categories = CategorieRepository::chargerCategories("1");
        chargerCategoriesUI();
        remplirCombosOperation();
        initialiserFiltresDashboard(); // Mettre à jour les filtres

        dashboardManager->setCategories(categories);
        actualiserDashboard();
    }
}


void MainWindow::chargerCategoriesUI()
{
    ui->treeCategories->blockSignals(true);
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

        // 🔹 Rendre l'item éditable UNE SEULE FOIS
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        // Colonnes
        item->setText(0, c->getNom()); // Nom
        item->setText(1, budget >= 0 ? QString::number(budget, 'f', 2) : "-"); // Budget
        item->setText(2, budget >= 0 ? QString::number(depenses, 'f', 2) : "-"); // Dépenses
        item->setText(3, budget >= 0 ? QString::number(restant, 'f', 2) : "-"); // Restant

        item->setForeground(3, restant < 0 ? Qt::red : Qt::darkGreen);

        // ID catégorie
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
    ui->treeCategories->blockSignals(false);
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

        bool succes = CategorieRepository::supprimerCategorieAvecEnfants(categorieId);

        if (succes) {
            QMessageBox::information(this, "Succès", "Catégorie supprimée avec succès");

            // Recharger les catégories
            categories = CategorieRepository::chargerCategories("1");
            chargerCategoriesUI();
            remplirCombosOperation();
            initialiserFiltresDashboard();

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
    qDebug() << "=== ajouterOperation() APPELEE ===";

    QString type = ui->comboTypeOperation->currentData().toString();
    qDebug() << "Type DATA =" << type;

    QString nom = ui->editNomOperation->text().trimmed();
    double montant = ui->spinMontantOperation->value();
    QDate date = ui->dateOperation->date();

    QString compteId = ui->comboCompteOperation->currentData().toString();
    QString categorieId = ui->comboSousCategorie->currentData().toString();
    if (categorieId.isEmpty())
        categorieId = ui->comboCategoriePrincipale->currentData().toString();

    if (nom.isEmpty() || montant <= 0 || compteId.isEmpty() || categorieId.isEmpty()) {
        qDebug() << "STOP: champs invalides";
        return;
    }

    bool ok = false;

    if (type == "REVENU") {
        qDebug() << "=> ajouterRevenu()";
        ok = OperationRepository::ajouterRevenu(
            nom, date, montant,
            compteId, categorieId,
            "Non spécifiée"
            );
    }
    else if (type == "DEPENSE") {
        qDebug() << "=> ajouterDepense()";
        ok = OperationRepository::ajouterDepense(
            nom, date, montant,
            compteId, categorieId,
            ui->checkRecurrente->isChecked(),
            ui->comboFrequence->currentText()
            );
    }
    else {
        qDebug() << "ERREUR: type inconnu";
        return;
    }

    qDebug() << "Repository retour =" << ok;

    if (!ok) return;

    rafraichirUI();
    chargerHistoriqueCompte();
    chargerCategoriesUI();
    actualiserDashboard();

    qDebug() << "=== ajouterOperation() FIN ===";
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
    // 🔒 Éviter les signaux pendant le chargement
    disconnect(ui->tableOperations, &QTableWidget::cellChanged,
               this, &MainWindow::onTableOperationsCellChanged);

    ui->tableOperations->setRowCount(0);
    ui->tableOperations->clearContents();

    QString compteId = ui->comboCompteHistorique->currentData().toString();
    if (compteId.isEmpty()) {
        connect(ui->tableOperations, &QTableWidget::cellChanged,
                this, &MainWindow::onTableOperationsCellChanged);
        return;
    }

    // ===============================
    // 1️⃣ Charger les données depuis la DB
    // ===============================

    QList<QVariantMap> ops =
        OperationRepository::chargerOperationsParCompte(compteId);

    QList<QVariantMap> transferts =
        TransfertRepository::chargerTransfertsParCompte(compteId);

    // ===============================
    // 2️⃣ Stocker TOUTES les opérations
    // ===============================

    allOperations.clear();
    allOperations = ops;
    allOperations.append(transferts);

    // ===============================
    // 3️⃣ Trier par date décroissante
    // ===============================

    std::sort(allOperations.begin(), allOperations.end(),
              [](const QVariantMap& a, const QVariantMap& b) {
                  return a["date"].toDate() > b["date"].toDate();
              });

    // ===============================
    // 4️⃣ Remplir le combo filtre catégories
    // ===============================

    ui->comboFiltreCategorieOp->blockSignals(true);
    ui->comboFiltreCategorieOp->clear();
    ui->comboFiltreCategorieOp->addItem("Toutes les catégories", "");

    QSet<QString> categoriesUniques;
    for (const QVariantMap& op : allOperations) {
        categoriesUniques.insert(op["categorie"].toString());
    }

    for (const QString& cat : categoriesUniques) {
        ui->comboFiltreCategorieOp->addItem(cat, cat);
    }

    ui->comboFiltreCategorieOp->blockSignals(false);

    // ===============================
    // 5️⃣ Appliquer le filtre (affichage)
    // ===============================

    appliquerFiltreOperations();

    // ===============================
    // 6️⃣ Réactiver les signaux
    // ===============================

    connect(ui->tableOperations, &QTableWidget::cellChanged,
            this, &MainWindow::onTableOperationsCellChanged);

    // Bouton supprimer désactivé par défaut
    ui->btnSupprimerOperation->setEnabled(false);
}


void MainWindow::appliquerFiltreOperations()
{
    if (allOperations.isEmpty())
        return;

    QString filtreCategorie =
        ui->comboFiltreCategorieOp->currentData().toString();

    QString filtreTexte =
        ui->editFiltreTexteOp->text().trimmed().toLower();

    QList<QVariantMap> filtered;

    for (const QVariantMap& op : allOperations) {

        // 🔹 Filtre catégorie
        if (!filtreCategorie.isEmpty() &&
            op["categorie"].toString() != filtreCategorie)
            continue;

        // 🔹 Filtre texte (nom)
        if (!filtreTexte.isEmpty() &&
            !op["nom"].toString().toLower().contains(filtreTexte))
            continue;

        filtered.append(op);
    }

    afficherOperations(filtered);
}

void MainWindow::afficherOperations(const QList<QVariantMap>& operations)
{
    disconnect(ui->tableOperations, &QTableWidget::cellChanged,
               this, &MainWindow::onTableOperationsCellChanged);

    ui->tableOperations->clearContents();
    ui->tableOperations->setRowCount(operations.size());

    for (int i = 0; i < operations.size(); ++i) {
        const QVariantMap& op = operations[i];

        // ======================
        // Date
        // ======================
        QDate date = op["date"].toDate();
        QTableWidgetItem* dateItem =
            new QTableWidgetItem(date.toString("dd/MM/yyyy"));
        dateItem->setData(Qt::UserRole, op);
        dateItem->setFlags(dateItem->flags() | Qt::ItemIsEditable);
        ui->tableOperations->setItem(i, 0, dateItem);

        // ======================
        // Nom
        // ======================
        QTableWidgetItem* nomItem =
            new QTableWidgetItem(op["nom"].toString());
        nomItem->setData(Qt::UserRole, op);
        nomItem->setFlags(nomItem->flags() | Qt::ItemIsEditable);
        ui->tableOperations->setItem(i, 1, nomItem);

        // ======================
        // Catégorie
        // ======================
        QTableWidgetItem* catItem =
            new QTableWidgetItem(op["categorie"].toString());
        catItem->setData(Qt::UserRole, op);
        catItem->setFlags(catItem->flags() | Qt::ItemIsEditable);

        if (op.contains("categorie_id")) {
            catItem->setData(Qt::UserRole + 1, op["categorie_id"]);
        }

        ui->tableOperations->setItem(i, 2, catItem);

        // ======================
        // Montant
        // ======================
        double montant = op["montant"].toDouble();
        QString type = op.value("type").toString();

        if (type == "DEPENSE" || type == "TRANSFERT_SORTANT") {
            montant = -montant;
        }

        QTableWidgetItem* montantItem =
            new QTableWidgetItem(QString::number(montant, 'f', 2));
        montantItem->setData(Qt::UserRole, op);
        montantItem->setFlags(montantItem->flags() | Qt::ItemIsEditable);
        montantItem->setForeground(montant < 0 ? Qt::red : Qt::darkGreen);

        ui->tableOperations->setItem(i, 3, montantItem);
    }

    connect(ui->tableOperations, &QTableWidget::cellChanged,
            this, &MainWindow::onTableOperationsCellChanged);
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

// ==================== MÉTHODES DU DASHBOARD ====================

void MainWindow::initialiserDashboard()
{
    // Initialiser le combo des mois
    initialiserComboMois();

    // Définir l'année courante
    int anneeCourante = QDate::currentDate().year();
    ui->spinAnnee->setValue(anneeCourante);

    // Initialiser les filtres
    initialiserFiltresDashboard();

    // Initialiser les charts
    dashboardManager->initialiserCharts(
        ui->chartDepensesParCategorie,
        ui->chartRevenusVsDepenses,
        ui->chartEvolution,
        ui->chartRepartition
        );

    dashboardManager->setCategories(categories);
    dashboardManager->setComptes(utilisateur.getComptes());

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

void MainWindow::initialiserFiltresDashboard()
{
    // Filtre par compte
    ui->comboFiltreCompte->clear();
    ui->comboFiltreCompte->addItem("Tous les comptes", "");

    for (Compte* compte : utilisateur.getComptes()) {
        ui->comboFiltreCompte->addItem(compte->getNom(), compte->getId());
    }

    // Filtre par catégorie
    ui->comboFiltreCategorie->clear();
    ui->comboFiltreCategorie->addItem("Toutes les catégories", "");

    for (Categorie* categorie : categories) {
        if (!categorie->getParent()) {
            ui->comboFiltreCategorie->addItem(categorie->getNom(), categorie->getId());
        }
    }
}

void MainWindow::actualiserDashboard()
{
    int mois = ui->comboMois->currentData().toInt();
    int annee = ui->spinAnnee->value();

    QString filtreCompte = ui->comboFiltreCompte->currentData().toString();
    QString filtreCategorie = ui->comboFiltreCategorie->currentData().toString();

    qDebug() << "Actualisation dashboard: Mois" << mois << "Année" << annee
             << "Compte:" << filtreCompte << "Catégorie:" << filtreCategorie;

    dashboardManager->actualiserDashboard(mois, annee, categories,
                                          utilisateur.getComptes(),
                                          filtreCategorie, filtreCompte);

    // Mettre à jour les recommandations
    mettreAJourRecommandations();
}

void MainWindow::onFiltreCompteChanged(int index)
{
    Q_UNUSED(index);
    actualiserDashboard();
}

void MainWindow::onFiltreCategorieChanged(int index)
{
    Q_UNUSED(index);
    actualiserDashboard();
}

void MainWindow::mettreAJourRecommandations()
{
    QString recommandations = dashboardManager->getRecommandations();
    ui->textRecommandations->setPlainText(recommandations);

    // Mettre à jour les statistiques affichées
    DashboardManager::Statistiques stats = dashboardManager->getStatistiques();

    // Calculer les transferts pour l'affichage
    int mois = ui->comboMois->currentData().toInt();
    int annee = ui->spinAnnee->value();
    QString filtreCompte = ui->comboFiltreCompte->currentData().toString();

    // Calculer les transferts sortants et entrants
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    double transfertsSortants = 0;
    double transfertsEntrants = 0;

    if (filtreCompte.isEmpty()) {
        // Transferts sortants totaux
        query.prepare("SELECT SUM(montant) FROM Transfert WHERE substr(date, 6, 2) = :mois AND substr(date, 1, 4) = :annee");
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);
        if (query.exec() && query.next()) {
            transfertsSortants = query.value(0).toDouble();
        }
    } else {
        // Transferts sortants pour le compte
        query.prepare("SELECT SUM(montant) FROM Transfert WHERE substr(date, 6, 2) = :mois AND substr(date, 1, 4) = :annee AND source_id = :compte");
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);
        query.bindValue(":compte", filtreCompte);
        if (query.exec() && query.next()) {
            transfertsSortants = query.value(0).toDouble();
        }

        // Transferts entrants pour le compte
        query.prepare("SELECT SUM(montant) FROM Transfert WHERE substr(date, 6, 2) = :mois AND substr(date, 1, 4) = :annee AND destination_id = :compte");
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);
        query.bindValue(":compte", filtreCompte);
        if (query.exec() && query.next()) {
            transfertsEntrants = query.value(0).toDouble();
        }
    }

    // Mettre à jour les labels des statistiques
    ui->labelRevenusTotal->setText(QString("%1 €").arg(stats.totalRevenus, 0, 'f', 2));
    ui->labelDepensesTotal->setText(QString("%1 €").arg(stats.totalDepenses, 0, 'f', 2));

    // Ajouter les transferts si les labels existent
    QLabel* labelTransfertsSortants = findChild<QLabel*>("labelTransfertsSortants");
    QLabel* labelTransfertsEntrants = findChild<QLabel*>("labelTransfertsEntrants");

    if (labelTransfertsSortants) {
        labelTransfertsSortants->setText(QString("%1 €").arg(transfertsSortants, 0, 'f', 2));
    }

    if (labelTransfertsEntrants) {
        labelTransfertsEntrants->setText(QString("%1 €").arg(transfertsEntrants, 0, 'f', 2));
    }

    // Solde net (revenus + transferts entrants - dépenses - transferts sortants)
    double soldeNet = stats.totalRevenus + transfertsEntrants - stats.totalDepenses - transfertsSortants;
    ui->labelSolde->setText(QString("%1 €").arg(soldeNet, 0, 'f', 2));

    ui->labelDepensesMoyennes->setText(QString("%1 €").arg(stats.depensesParJour, 0, 'f', 2));

    // Mettre en couleur le solde
    if (soldeNet < 0) {
        ui->labelSolde->setStyleSheet("color: red; font-weight: bold;");
    } else {
        ui->labelSolde->setStyleSheet("color: green; font-weight: bold;");
    }

    // Mettre en couleur les transferts sortants si élevés
    if (labelTransfertsSortants) {
        if (transfertsSortants > stats.totalRevenus * 0.3 && stats.totalRevenus > 0) {
            labelTransfertsSortants->setStyleSheet("color: orange; font-weight: bold;");
        } else {
            labelTransfertsSortants->setStyleSheet("");
        }
    }
}


void MainWindow::exporterPDFStatistiques()
{
    exporterPDF(false);  // Export avec filtres actuels
}

void MainWindow::exporterPDFTousComptes()
{
    exporterPDF(true);   // Export tous les comptes
}

void MainWindow::exporterPDF(bool tousLesComptes)
{
    // Demander où sauvegarder le PDF
    QString defaultFileName;

    if (tousLesComptes) {
        defaultFileName = QString("rapport_complet_%1_%2.pdf")
        .arg(ui->comboMois->currentData().toInt())
            .arg(ui->spinAnnee->value());
    } else {
        QString filtreCompte = ui->comboFiltreCompte->currentText();
        QString filtreCategorie = ui->comboFiltreCategorie->currentText();

        defaultFileName = QString("rapport_%1_%2")
                              .arg(ui->comboMois->currentData().toInt())
                              .arg(ui->spinAnnee->value());

        if (!filtreCompte.isEmpty() && filtreCompte != "Tous les comptes") {
            defaultFileName += "_" + filtreCompte;
        }
        if (!filtreCategorie.isEmpty() && filtreCategorie != "Toutes les catégories") {
            defaultFileName += "_" + filtreCategorie;
        }
        defaultFileName += ".pdf";
    }

    // Nettoyer le nom de fichier
    defaultFileName = defaultFileName.replace(" ", "_").replace("/", "-");

    QString cheminFichier = QFileDialog::getSaveFileName(this,
                                                         "Exporter le rapport PDF",
                                                         QDir::homePath() + "/" + defaultFileName,
                                                         "Fichiers PDF (*.pdf)");

    if (cheminFichier.isEmpty()) {
        return;  // L'utilisateur a annulé
    }

    // S'assurer que l'extension est .pdf
    if (!cheminFichier.endsWith(".pdf", Qt::CaseInsensitive)) {
        cheminFichier += ".pdf";
    }

    // Récupérer les paramètres actuels
    int mois = ui->comboMois->currentData().toInt();
    int annee = ui->spinAnnee->value();
    QString filtreCompte = ui->comboFiltreCompte->currentData().toString();
    QString filtreCategorie = ui->comboFiltreCategorie->currentData().toString();

    // Afficher une boîte de dialogue de progression
    QProgressDialog progress("Génération du rapport PDF...", "Annuler", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue(10);
    QApplication::processEvents();

    try {
        // Exporter le PDF
        progress.setValue(30);

        bool succes = dashboardManager->exporterPDF(cheminFichier,
                                                    mois,
                                                    annee,
                                                    filtreCompte,
                                                    filtreCategorie,
                                                    tousLesComptes);

        progress.setValue(100);

        if (succes) {
            QMessageBox::information(this,
                                     "Export réussi",
                                     QString("Le rapport PDF a été généré avec succès :\n%1").arg(cheminFichier));

            // Optionnel : ouvrir le PDF
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this,
                                          "Ouvrir le PDF",
                                          "Voulez-vous ouvrir le fichier PDF maintenant ?",
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(cheminFichier));
            }
        } else {
            QMessageBox::warning(this,
                                 "Erreur d'export",
                                 "Une erreur est survenue lors de la génération du PDF.");
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this,
                              "Erreur",
                              QString("Erreur lors de l'export : %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this,
                              "Erreur",
                              "Une erreur inconnue est survenue lors de l'export.");
    }
}
void MainWindow::onCategorieItemChanged(QTreeWidgetItem *item, int column)
{
    if (isModifyingCategorie) return;

    // 🔒 Colonnes NON modifiables
    if (column == 2 || column == 3) {
        chargerCategoriesUI();   // rollback visuel
        return;
    }

    isModifyingCategorie = true;

    QString categorieId = item->data(0, Qt::UserRole).toString();
    if (categorieId.isEmpty()) {
        isModifyingCategorie = false;
        return;
    }

    int mois = ui->comboMois->currentData().toInt();
    int annee = ui->spinAnnee->value();

    if (column == 0) {
        QString nouveauNom = item->text(0).trimmed();
        if (nouveauNom.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Nom invalide");
            chargerCategoriesUI();
            isModifyingCategorie = false;
            return;
        }
        CategorieRepository::modifierCategorie(categorieId, nouveauNom);
    }

    else if (column == 1) {
        bool ok;
        double budget = item->text(1).toDouble(&ok);
        if (!ok || budget < 0) {
            QMessageBox::warning(this, "Erreur", "Budget invalide");
            chargerCategoriesUI();
            isModifyingCategorie = false;
            return;
        }
        BudgetRepository::definirBudget(categorieId, mois, annee, budget);
    }

    categories = CategorieRepository::chargerCategories("1");
    chargerCategoriesUI();
    actualiserDashboard();

    isModifyingCategorie = false;
}
