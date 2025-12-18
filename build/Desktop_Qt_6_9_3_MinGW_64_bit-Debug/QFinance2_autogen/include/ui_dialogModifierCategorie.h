/********************************************************************************
** Form generated from reading UI file 'dialogModifierCategorie.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGMODIFIERCATEGORIE_H
#define UI_DIALOGMODIFIERCATEGORIE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogModifierCategorie
{
public:
    QVBoxLayout *vboxLayout;
    QLineEdit *editNomCategorie;
    QDoubleSpinBox *spinBudget;
    QHBoxLayout *hboxLayout;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *DialogModifierCategorie)
    {
        if (DialogModifierCategorie->objectName().isEmpty())
            DialogModifierCategorie->setObjectName("DialogModifierCategorie");
        vboxLayout = new QVBoxLayout(DialogModifierCategorie);
        vboxLayout->setObjectName("vboxLayout");
        editNomCategorie = new QLineEdit(DialogModifierCategorie);
        editNomCategorie->setObjectName("editNomCategorie");

        vboxLayout->addWidget(editNomCategorie);

        spinBudget = new QDoubleSpinBox(DialogModifierCategorie);
        spinBudget->setObjectName("spinBudget");
        spinBudget->setMinimum(0.000000000000000);
        spinBudget->setMaximum(1000000.000000000000000);

        vboxLayout->addWidget(spinBudget);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        btnOk = new QPushButton(DialogModifierCategorie);
        btnOk->setObjectName("btnOk");

        hboxLayout->addWidget(btnOk);

        btnCancel = new QPushButton(DialogModifierCategorie);
        btnCancel->setObjectName("btnCancel");

        hboxLayout->addWidget(btnCancel);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(DialogModifierCategorie);

        QMetaObject::connectSlotsByName(DialogModifierCategorie);
    } // setupUi

    void retranslateUi(QDialog *DialogModifierCategorie)
    {
        DialogModifierCategorie->setWindowTitle(QCoreApplication::translate("DialogModifierCategorie", "Modifier la cat\303\251gorie", nullptr));
        spinBudget->setSuffix(QCoreApplication::translate("DialogModifierCategorie", " \342\202\254", nullptr));
        spinBudget->setSpecialValueText(QCoreApplication::translate("DialogModifierCategorie", "Aucun", nullptr));
        btnOk->setText(QCoreApplication::translate("DialogModifierCategorie", "Enregistrer", nullptr));
        btnCancel->setText(QCoreApplication::translate("DialogModifierCategorie", "Annuler", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogModifierCategorie: public Ui_DialogModifierCategorie {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGMODIFIERCATEGORIE_H
