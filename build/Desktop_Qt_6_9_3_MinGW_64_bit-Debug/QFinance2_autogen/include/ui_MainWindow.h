/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
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

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


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
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Nom de la cat\303\251gorie", nullptr));
        btnAjouterCategorie->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        btnModifierCategorie->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btnSupprimerCategorie->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabCategories), QCoreApplication::translate("MainWindow", "Cat\303\251gories", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
