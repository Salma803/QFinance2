/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
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
    QComboBox *comboMois;
    QSpinBox *spinAnnee;
    QPushButton *btnActualiser;
    QSpacerItem *horizontalSpacerFiltres;
    QChartView *chartDepensesParCategorie;
    QChartView *chartRevenusVsDepenses;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(449, 449);
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
        tableOperations->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        tableOperations->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableOperations->setSelectionMode(QAbstractItemView::SingleSelection);

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
        comboMois = new QComboBox(groupFiltres);
        comboMois->setObjectName("comboMois");

        horizontalLayoutFiltres->addWidget(comboMois);

        spinAnnee = new QSpinBox(groupFiltres);
        spinAnnee->setObjectName("spinAnnee");
        spinAnnee->setMinimum(2000);
        spinAnnee->setMaximum(2100);

        horizontalLayoutFiltres->addWidget(spinAnnee);

        btnActualiser = new QPushButton(groupFiltres);
        btnActualiser->setObjectName("btnActualiser");

        horizontalLayoutFiltres->addWidget(btnActualiser);

        horizontalSpacerFiltres = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutFiltres->addItem(horizontalSpacerFiltres);


        verticalLayoutDashboard->addWidget(groupFiltres);

        chartDepensesParCategorie = new QChartView(tabDashboard);
        chartDepensesParCategorie->setObjectName("chartDepensesParCategorie");

        verticalLayoutDashboard->addWidget(chartDepensesParCategorie);

        chartRevenusVsDepenses = new QChartView(tabDashboard);
        chartRevenusVsDepenses->setObjectName("chartRevenusVsDepenses");

        verticalLayoutDashboard->addWidget(chartRevenusVsDepenses);

        tabWidget->addTab(tabDashboard, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(3);


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
        btnActualiser->setText(QCoreApplication::translate("MainWindow", "Actualiser", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDashboard), QCoreApplication::translate("MainWindow", "Dashboard", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
