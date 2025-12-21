/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabComptes;
    QVBoxLayout *vboxLayout;
    QGroupBox *groupCompte;
    QHBoxLayout *hboxLayout;
    QLineEdit *editNomCompte;
    QComboBox *comboTypeCompte;
    QPushButton *btnAjouterCompte;
    QListWidget *listComptes;
    QGroupBox *groupTransfert;
    QHBoxLayout *hboxLayout1;
    QComboBox *comboSource;
    QComboBox *comboDestination;
    QDoubleSpinBox *spinMontant;
    QPushButton *btnTransfert;
    QWidget *tabCategories;
    QVBoxLayout *vboxLayout1;
    QTreeWidget *treeCategories;
    QHBoxLayout *hboxLayout2;
    QPushButton *btnAjouterCategorie;
    QPushButton *btnModifierCategorie;
    QPushButton *btnSupprimerCategorie;
    QWidget *tabOperations;
    QVBoxLayout *vboxLayout2;
    QComboBox *comboTypeOperation;
    QLineEdit *editNomOperation;
    QDoubleSpinBox *spinMontantOperation;
    QDateEdit *dateOperation;
    QComboBox *comboCompteOperation;
    QComboBox *comboCategoriePrincipale;
    QComboBox *comboSousCategorie;
    QCheckBox *checkRecurrente;
    QComboBox *comboFrequence;
    QPushButton *btnAjouterOperation;
    QWidget *tabHistorique;
    QVBoxLayout *vboxLayout3;
    QComboBox *comboCompteHistorique;
    QTableWidget *tableOperations;
    QHBoxLayout *hboxLayout3;
    QPushButton *btnSupprimerOperation;
    QSpacerItem *spacerItem;
    QWidget *tabDashboard;
    QVBoxLayout *verticalLayoutDashboard;
    QGroupBox *groupFiltres;
    QHBoxLayout *horizontalLayoutFiltres;
    QLabel *labelMois;
    QComboBox *comboMois;
    QLabel *labelAnnee;
    QSpinBox *spinAnnee;
    QLabel *labelCompte;
    QComboBox *comboFiltreCompte;
    QLabel *labelCategorie;
    QComboBox *comboFiltreCategorie;
    QPushButton *btnActualiser;
    QGroupBox *groupExport;
    QHBoxLayout *horizontalLayoutExport;
    QPushButton *btnExportPDF;
    QPushButton *btnExportPDFTousComptes;
    QSpacerItem *spacerItem1;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QChartView *chartDepensesParCategorie;
    QChartView *chartRevenusVsDepenses;
    QHBoxLayout *horizontalLayout_3;
    QChartView *chartEvolution;
    QChartView *chartRepartition;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *labelRevenusTotal;
    QLabel *label_2;
    QLabel *labelDepensesTotal;
    QLabel *label_3;
    QLabel *labelSolde;
    QLabel *label_4;
    QLabel *labelDepensesMoyennes;
    QLabel *label_5;
    QLabel *labelTransfertsSortants;
    QLabel *label_6;
    QLabel *labelTransfertsEntrants;
    QGroupBox *groupRecommandations;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *textRecommandations;
    QSpacerItem *verticalSpacer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1400, 850);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabComptes = new QWidget();
        tabComptes->setObjectName("tabComptes");
        vboxLayout = new QVBoxLayout(tabComptes);
        vboxLayout->setObjectName("vboxLayout");
        groupCompte = new QGroupBox(tabComptes);
        groupCompte->setObjectName("groupCompte");
        hboxLayout = new QHBoxLayout(groupCompte);
        hboxLayout->setObjectName("hboxLayout");
        editNomCompte = new QLineEdit(groupCompte);
        editNomCompte->setObjectName("editNomCompte");

        hboxLayout->addWidget(editNomCompte);

        comboTypeCompte = new QComboBox(groupCompte);
        comboTypeCompte->addItem(QString());
        comboTypeCompte->addItem(QString());
        comboTypeCompte->setObjectName("comboTypeCompte");

        hboxLayout->addWidget(comboTypeCompte);

        btnAjouterCompte = new QPushButton(groupCompte);
        btnAjouterCompte->setObjectName("btnAjouterCompte");

        hboxLayout->addWidget(btnAjouterCompte);


        vboxLayout->addWidget(groupCompte);

        listComptes = new QListWidget(tabComptes);
        listComptes->setObjectName("listComptes");

        vboxLayout->addWidget(listComptes);

        groupTransfert = new QGroupBox(tabComptes);
        groupTransfert->setObjectName("groupTransfert");
        hboxLayout1 = new QHBoxLayout(groupTransfert);
        hboxLayout1->setObjectName("hboxLayout1");
        comboSource = new QComboBox(groupTransfert);
        comboSource->setObjectName("comboSource");

        hboxLayout1->addWidget(comboSource);

        comboDestination = new QComboBox(groupTransfert);
        comboDestination->setObjectName("comboDestination");

        hboxLayout1->addWidget(comboDestination);

        spinMontant = new QDoubleSpinBox(groupTransfert);
        spinMontant->setObjectName("spinMontant");
        spinMontant->setMaximum(1000000.000000000000000);

        hboxLayout1->addWidget(spinMontant);

        btnTransfert = new QPushButton(groupTransfert);
        btnTransfert->setObjectName("btnTransfert");

        hboxLayout1->addWidget(btnTransfert);


        vboxLayout->addWidget(groupTransfert);

        tabWidget->addTab(tabComptes, QString());
        tabCategories = new QWidget();
        tabCategories->setObjectName("tabCategories");
        vboxLayout1 = new QVBoxLayout(tabCategories);
        vboxLayout1->setObjectName("vboxLayout1");
        treeCategories = new QTreeWidget(tabCategories);
        treeCategories->setObjectName("treeCategories");

        vboxLayout1->addWidget(treeCategories);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName("hboxLayout2");
        btnAjouterCategorie = new QPushButton(tabCategories);
        btnAjouterCategorie->setObjectName("btnAjouterCategorie");

        hboxLayout2->addWidget(btnAjouterCategorie);

        btnModifierCategorie = new QPushButton(tabCategories);
        btnModifierCategorie->setObjectName("btnModifierCategorie");

        hboxLayout2->addWidget(btnModifierCategorie);

        btnSupprimerCategorie = new QPushButton(tabCategories);
        btnSupprimerCategorie->setObjectName("btnSupprimerCategorie");

        hboxLayout2->addWidget(btnSupprimerCategorie);


        vboxLayout1->addLayout(hboxLayout2);

        tabWidget->addTab(tabCategories, QString());
        tabOperations = new QWidget();
        tabOperations->setObjectName("tabOperations");
        vboxLayout2 = new QVBoxLayout(tabOperations);
        vboxLayout2->setObjectName("vboxLayout2");
        comboTypeOperation = new QComboBox(tabOperations);
        comboTypeOperation->addItem(QString());
        comboTypeOperation->addItem(QString());
        comboTypeOperation->setObjectName("comboTypeOperation");

        vboxLayout2->addWidget(comboTypeOperation);

        editNomOperation = new QLineEdit(tabOperations);
        editNomOperation->setObjectName("editNomOperation");

        vboxLayout2->addWidget(editNomOperation);

        spinMontantOperation = new QDoubleSpinBox(tabOperations);
        spinMontantOperation->setObjectName("spinMontantOperation");
        spinMontantOperation->setMaximum(1000000.000000000000000);

        vboxLayout2->addWidget(spinMontantOperation);

        dateOperation = new QDateEdit(tabOperations);
        dateOperation->setObjectName("dateOperation");
        dateOperation->setCalendarPopup(true);
        dateOperation->setDate(QDate(2000, 1, 1));

        vboxLayout2->addWidget(dateOperation);

        comboCompteOperation = new QComboBox(tabOperations);
        comboCompteOperation->setObjectName("comboCompteOperation");

        vboxLayout2->addWidget(comboCompteOperation);

        comboCategoriePrincipale = new QComboBox(tabOperations);
        comboCategoriePrincipale->setObjectName("comboCategoriePrincipale");

        vboxLayout2->addWidget(comboCategoriePrincipale);

        comboSousCategorie = new QComboBox(tabOperations);
        comboSousCategorie->setObjectName("comboSousCategorie");

        vboxLayout2->addWidget(comboSousCategorie);

        checkRecurrente = new QCheckBox(tabOperations);
        checkRecurrente->setObjectName("checkRecurrente");

        vboxLayout2->addWidget(checkRecurrente);

        comboFrequence = new QComboBox(tabOperations);
        comboFrequence->addItem(QString());
        comboFrequence->addItem(QString());
        comboFrequence->addItem(QString());
        comboFrequence->setObjectName("comboFrequence");

        vboxLayout2->addWidget(comboFrequence);

        btnAjouterOperation = new QPushButton(tabOperations);
        btnAjouterOperation->setObjectName("btnAjouterOperation");

        vboxLayout2->addWidget(btnAjouterOperation);

        tabWidget->addTab(tabOperations, QString());
        tabHistorique = new QWidget();
        tabHistorique->setObjectName("tabHistorique");
        vboxLayout3 = new QVBoxLayout(tabHistorique);
        vboxLayout3->setObjectName("vboxLayout3");
        comboCompteHistorique = new QComboBox(tabHistorique);
        comboCompteHistorique->setObjectName("comboCompteHistorique");

        vboxLayout3->addWidget(comboCompteHistorique);

        tableOperations = new QTableWidget(tabHistorique);
        if (tableOperations->columnCount() < 4)
            tableOperations->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableOperations->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableOperations->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableOperations->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableOperations->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        tableOperations->setObjectName("tableOperations");
        tableOperations->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked|QAbstractItemView::EditTrigger::EditKeyPressed);
        tableOperations->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        tableOperations->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        vboxLayout3->addWidget(tableOperations);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setObjectName("hboxLayout3");
        btnSupprimerOperation = new QPushButton(tabHistorique);
        btnSupprimerOperation->setObjectName("btnSupprimerOperation");
        btnSupprimerOperation->setEnabled(false);

        hboxLayout3->addWidget(btnSupprimerOperation);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout3->addItem(spacerItem);


        vboxLayout3->addLayout(hboxLayout3);

        tabWidget->addTab(tabHistorique, QString());
        tabDashboard = new QWidget();
        tabDashboard->setObjectName("tabDashboard");
        verticalLayoutDashboard = new QVBoxLayout(tabDashboard);
        verticalLayoutDashboard->setObjectName("verticalLayoutDashboard");
        groupFiltres = new QGroupBox(tabDashboard);
        groupFiltres->setObjectName("groupFiltres");
        horizontalLayoutFiltres = new QHBoxLayout(groupFiltres);
        horizontalLayoutFiltres->setObjectName("horizontalLayoutFiltres");
        labelMois = new QLabel(groupFiltres);
        labelMois->setObjectName("labelMois");

        horizontalLayoutFiltres->addWidget(labelMois);

        comboMois = new QComboBox(groupFiltres);
        comboMois->setObjectName("comboMois");

        horizontalLayoutFiltres->addWidget(comboMois);

        labelAnnee = new QLabel(groupFiltres);
        labelAnnee->setObjectName("labelAnnee");

        horizontalLayoutFiltres->addWidget(labelAnnee);

        spinAnnee = new QSpinBox(groupFiltres);
        spinAnnee->setObjectName("spinAnnee");
        spinAnnee->setMinimum(2000);
        spinAnnee->setMaximum(2100);

        horizontalLayoutFiltres->addWidget(spinAnnee);

        labelCompte = new QLabel(groupFiltres);
        labelCompte->setObjectName("labelCompte");

        horizontalLayoutFiltres->addWidget(labelCompte);

        comboFiltreCompte = new QComboBox(groupFiltres);
        comboFiltreCompte->setObjectName("comboFiltreCompte");

        horizontalLayoutFiltres->addWidget(comboFiltreCompte);

        labelCategorie = new QLabel(groupFiltres);
        labelCategorie->setObjectName("labelCategorie");

        horizontalLayoutFiltres->addWidget(labelCategorie);

        comboFiltreCategorie = new QComboBox(groupFiltres);
        comboFiltreCategorie->setObjectName("comboFiltreCategorie");

        horizontalLayoutFiltres->addWidget(comboFiltreCategorie);

        btnActualiser = new QPushButton(groupFiltres);
        btnActualiser->setObjectName("btnActualiser");

        horizontalLayoutFiltres->addWidget(btnActualiser);


        verticalLayoutDashboard->addWidget(groupFiltres);

        groupExport = new QGroupBox(tabDashboard);
        groupExport->setObjectName("groupExport");
        horizontalLayoutExport = new QHBoxLayout(groupExport);
        horizontalLayoutExport->setObjectName("horizontalLayoutExport");
        btnExportPDF = new QPushButton(groupExport);
        btnExportPDF->setObjectName("btnExportPDF");

        horizontalLayoutExport->addWidget(btnExportPDF);

        btnExportPDFTousComptes = new QPushButton(groupExport);
        btnExportPDFTousComptes->setObjectName("btnExportPDFTousComptes");

        horizontalLayoutExport->addWidget(btnExportPDFTousComptes);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutExport->addItem(spacerItem1);


        verticalLayoutDashboard->addWidget(groupExport);

        scrollArea = new QScrollArea(tabDashboard);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1362, 662));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        chartDepensesParCategorie = new QChartView(scrollAreaWidgetContents);
        chartDepensesParCategorie->setObjectName("chartDepensesParCategorie");
        chartDepensesParCategorie->setMinimumSize(QSize(650, 350));

        horizontalLayout_2->addWidget(chartDepensesParCategorie);

        chartRevenusVsDepenses = new QChartView(scrollAreaWidgetContents);
        chartRevenusVsDepenses->setObjectName("chartRevenusVsDepenses");
        chartRevenusVsDepenses->setMinimumSize(QSize(650, 350));

        horizontalLayout_2->addWidget(chartRevenusVsDepenses);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        chartEvolution = new QChartView(scrollAreaWidgetContents);
        chartEvolution->setObjectName("chartEvolution");
        chartEvolution->setMinimumSize(QSize(650, 350));

        horizontalLayout_3->addWidget(chartEvolution);

        chartRepartition = new QChartView(scrollAreaWidgetContents);
        chartRepartition->setObjectName("chartRepartition");
        chartRepartition->setMinimumSize(QSize(650, 350));

        horizontalLayout_3->addWidget(chartRepartition);


        verticalLayout_2->addLayout(horizontalLayout_3);

        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        labelRevenusTotal = new QLabel(groupBox);
        labelRevenusTotal->setObjectName("labelRevenusTotal");
        labelRevenusTotal->setAlignment(Qt::AlignRight);

        gridLayout->addWidget(labelRevenusTotal, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        labelDepensesTotal = new QLabel(groupBox);
        labelDepensesTotal->setObjectName("labelDepensesTotal");
        labelDepensesTotal->setAlignment(Qt::AlignRight);

        gridLayout->addWidget(labelDepensesTotal, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        labelSolde = new QLabel(groupBox);
        labelSolde->setObjectName("labelSolde");
        labelSolde->setAlignment(Qt::AlignRight);

        gridLayout->addWidget(labelSolde, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        labelDepensesMoyennes = new QLabel(groupBox);
        labelDepensesMoyennes->setObjectName("labelDepensesMoyennes");
        labelDepensesMoyennes->setAlignment(Qt::AlignRight);

        gridLayout->addWidget(labelDepensesMoyennes, 3, 1, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        labelTransfertsSortants = new QLabel(groupBox);
        labelTransfertsSortants->setObjectName("labelTransfertsSortants");
        labelTransfertsSortants->setAlignment(Qt::AlignRight);

        gridLayout->addWidget(labelTransfertsSortants, 4, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        labelTransfertsEntrants = new QLabel(groupBox);
        labelTransfertsEntrants->setObjectName("labelTransfertsEntrants");
        labelTransfertsEntrants->setAlignment(Qt::AlignRight);

        gridLayout->addWidget(labelTransfertsEntrants, 5, 1, 1, 1);


        verticalLayout_2->addWidget(groupBox);

        groupRecommandations = new QGroupBox(scrollAreaWidgetContents);
        groupRecommandations->setObjectName("groupRecommandations");
        verticalLayout_3 = new QVBoxLayout(groupRecommandations);
        verticalLayout_3->setObjectName("verticalLayout_3");
        textRecommandations = new QTextEdit(groupRecommandations);
        textRecommandations->setObjectName("textRecommandations");
        textRecommandations->setReadOnly(true);

        verticalLayout_3->addWidget(textRecommandations);


        verticalLayout_2->addWidget(groupRecommandations);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayoutDashboard->addWidget(scrollArea);

        tabWidget->addTab(tabDashboard, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Finance App", nullptr));
        groupCompte->setTitle(QCoreApplication::translate("MainWindow", "Ajouter un compte", nullptr));
        editNomCompte->setPlaceholderText(QCoreApplication::translate("MainWindow", "Nom du compte", nullptr));
        comboTypeCompte->setItemText(0, QCoreApplication::translate("MainWindow", "Courant", nullptr));
        comboTypeCompte->setItemText(1, QCoreApplication::translate("MainWindow", "\303\211pargne", nullptr));

        btnAjouterCompte->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        groupTransfert->setTitle(QCoreApplication::translate("MainWindow", "Transfert", nullptr));
        btnTransfert->setText(QCoreApplication::translate("MainWindow", "Transf\303\251rer", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabComptes), QCoreApplication::translate("MainWindow", "Comptes", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeCategories->headerItem();
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "Restant (\342\202\254)", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "D\303\251pens\303\251 (\342\202\254)", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Budget (\342\202\254)", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Cat\303\251gorie", nullptr));
        btnAjouterCategorie->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        btnModifierCategorie->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btnSupprimerCategorie->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabCategories), QCoreApplication::translate("MainWindow", "Cat\303\251gories", nullptr));
        comboTypeOperation->setItemText(0, QCoreApplication::translate("MainWindow", "Revenu", nullptr));
        comboTypeOperation->setItemText(1, QCoreApplication::translate("MainWindow", "D\303\251pense", nullptr));

        editNomOperation->setPlaceholderText(QCoreApplication::translate("MainWindow", "Nom de l'op\303\251ration", nullptr));
        comboCategoriePrincipale->setPlaceholderText(QCoreApplication::translate("MainWindow", "Cat\303\251gorie principale", nullptr));
        comboSousCategorie->setPlaceholderText(QCoreApplication::translate("MainWindow", "Sous-cat\303\251gorie", nullptr));
        checkRecurrente->setText(QCoreApplication::translate("MainWindow", "D\303\251pense r\303\251currente", nullptr));
        comboFrequence->setItemText(0, QCoreApplication::translate("MainWindow", "Mensuelle", nullptr));
        comboFrequence->setItemText(1, QCoreApplication::translate("MainWindow", "Trimestrielle", nullptr));
        comboFrequence->setItemText(2, QCoreApplication::translate("MainWindow", "Annuelle", nullptr));

        btnAjouterOperation->setText(QCoreApplication::translate("MainWindow", "Ajouter l'op\303\251ration", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabOperations), QCoreApplication::translate("MainWindow", "Op\303\251rations", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableOperations->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableOperations->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableOperations->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Cat\303\251gorie", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableOperations->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Montant", nullptr));
        btnSupprimerOperation->setText(QCoreApplication::translate("MainWindow", "Supprimer l'op\303\251ration", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabHistorique), QCoreApplication::translate("MainWindow", "Historique", nullptr));
        groupFiltres->setTitle(QCoreApplication::translate("MainWindow", "Filtres", nullptr));
        labelMois->setText(QCoreApplication::translate("MainWindow", "Mois:", nullptr));
        labelAnnee->setText(QCoreApplication::translate("MainWindow", "Ann\303\251e:", nullptr));
        labelCompte->setText(QCoreApplication::translate("MainWindow", "Compte:", nullptr));
        labelCategorie->setText(QCoreApplication::translate("MainWindow", "Cat\303\251gorie:", nullptr));
        btnActualiser->setText(QCoreApplication::translate("MainWindow", "Actualiser", nullptr));
        groupExport->setTitle(QCoreApplication::translate("MainWindow", "Export PDF", nullptr));
        btnExportPDF->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Exporter avec filtres actuels", nullptr));
#if QT_CONFIG(tooltip)
        btnExportPDF->setToolTip(QCoreApplication::translate("MainWindow", "Exporte les statistiques avec les filtres de mois, compte et cat\303\251gorie s\303\251lectionn\303\251s", nullptr));
#endif // QT_CONFIG(tooltip)
        btnExportPDFTousComptes->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Exporter rapport complet", nullptr));
#if QT_CONFIG(tooltip)
        btnExportPDFTousComptes->setToolTip(QCoreApplication::translate("MainWindow", "Exporte un rapport d\303\251taill\303\251 sur tous les comptes et cat\303\251gories", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Statistiques du mois", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Revenus totaux:", nullptr));
        labelRevenusTotal->setText(QCoreApplication::translate("MainWindow", "0 \342\202\254", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "D\303\251penses totales:", nullptr));
        labelDepensesTotal->setText(QCoreApplication::translate("MainWindow", "0 \342\202\254", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Solde net:", nullptr));
        labelSolde->setText(QCoreApplication::translate("MainWindow", "0 \342\202\254", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "D\303\251penses moyennes/jour:", nullptr));
        labelDepensesMoyennes->setText(QCoreApplication::translate("MainWindow", "0 \342\202\254", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Transferts sortants:", nullptr));
        labelTransfertsSortants->setText(QCoreApplication::translate("MainWindow", "0 \342\202\254", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Transferts entrants:", nullptr));
        labelTransfertsEntrants->setText(QCoreApplication::translate("MainWindow", "0 \342\202\254", nullptr));
        groupRecommandations->setTitle(QCoreApplication::translate("MainWindow", "Analyse & Recommandations", nullptr));
        textRecommandations->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Cliquez sur \"Actualiser\" pour voir les recommandations.</p></body></html>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDashboard), QCoreApplication::translate("MainWindow", "Dashboard", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
