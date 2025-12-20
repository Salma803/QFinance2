/********************************************************************************
** Form generated from reading UI file 'dialogAjouterCategorie.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGAJOUTERCATEGORIE_H
#define UI_DIALOGAJOUTERCATEGORIE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogAjouterCategorie
{
public:
    QVBoxLayout *vboxLayout;
    QLineEdit *editNomCategorie;
    QLabel *label;
    QComboBox *comboParentCategorie;
    QDoubleSpinBox *spinBudget;
    QHBoxLayout *hboxLayout;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *DialogAjouterCategorie)
    {
        if (DialogAjouterCategorie->objectName().isEmpty())
            DialogAjouterCategorie->setObjectName("DialogAjouterCategorie");
        vboxLayout = new QVBoxLayout(DialogAjouterCategorie);
        vboxLayout->setObjectName("vboxLayout");
        editNomCategorie = new QLineEdit(DialogAjouterCategorie);
        editNomCategorie->setObjectName("editNomCategorie");

        vboxLayout->addWidget(editNomCategorie);

        label = new QLabel(DialogAjouterCategorie);
        label->setObjectName("label");

        vboxLayout->addWidget(label);

        comboParentCategorie = new QComboBox(DialogAjouterCategorie);
        comboParentCategorie->setObjectName("comboParentCategorie");

        vboxLayout->addWidget(comboParentCategorie);

        spinBudget = new QDoubleSpinBox(DialogAjouterCategorie);
        spinBudget->setObjectName("spinBudget");
        spinBudget->setMinimum(0.000000000000000);
        spinBudget->setMaximum(1000000.000000000000000);

        vboxLayout->addWidget(spinBudget);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        btnOk = new QPushButton(DialogAjouterCategorie);
        btnOk->setObjectName("btnOk");

        hboxLayout->addWidget(btnOk);

        btnCancel = new QPushButton(DialogAjouterCategorie);
        btnCancel->setObjectName("btnCancel");

        hboxLayout->addWidget(btnCancel);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(DialogAjouterCategorie);

        QMetaObject::connectSlotsByName(DialogAjouterCategorie);
    } // setupUi

    void retranslateUi(QDialog *DialogAjouterCategorie)
    {
        DialogAjouterCategorie->setWindowTitle(QCoreApplication::translate("DialogAjouterCategorie", "Ajouter une cat\303\251gorie", nullptr));
        editNomCategorie->setPlaceholderText(QCoreApplication::translate("DialogAjouterCategorie", "Nom de la cat\303\251gorie", nullptr));
        label->setText(QCoreApplication::translate("DialogAjouterCategorie", "Cat\303\251gorie parente :", nullptr));
        spinBudget->setSuffix(QCoreApplication::translate("DialogAjouterCategorie", " \342\202\254", nullptr));
        spinBudget->setSpecialValueText(QCoreApplication::translate("DialogAjouterCategorie", "Aucun", nullptr));
        btnOk->setText(QCoreApplication::translate("DialogAjouterCategorie", "OK", nullptr));
        btnCancel->setText(QCoreApplication::translate("DialogAjouterCategorie", "Annuler", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogAjouterCategorie: public Ui_DialogAjouterCategorie {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGAJOUTERCATEGORIE_H
