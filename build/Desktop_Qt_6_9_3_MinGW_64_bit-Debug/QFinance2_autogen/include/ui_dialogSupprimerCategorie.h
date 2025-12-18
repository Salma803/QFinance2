/********************************************************************************
** Form generated from reading UI file 'dialogSupprimerCategorie.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGSUPPRIMERCATEGORIE_H
#define UI_DIALOGSUPPRIMERCATEGORIE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogSupprimerCategorie
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QHBoxLayout *hboxLayout;
    QPushButton *btnOui;
    QPushButton *btnNon;

    void setupUi(QDialog *DialogSupprimerCategorie)
    {
        if (DialogSupprimerCategorie->objectName().isEmpty())
            DialogSupprimerCategorie->setObjectName("DialogSupprimerCategorie");
        vboxLayout = new QVBoxLayout(DialogSupprimerCategorie);
        vboxLayout->setObjectName("vboxLayout");
        label = new QLabel(DialogSupprimerCategorie);
        label->setObjectName("label");

        vboxLayout->addWidget(label);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        btnOui = new QPushButton(DialogSupprimerCategorie);
        btnOui->setObjectName("btnOui");

        hboxLayout->addWidget(btnOui);

        btnNon = new QPushButton(DialogSupprimerCategorie);
        btnNon->setObjectName("btnNon");

        hboxLayout->addWidget(btnNon);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(DialogSupprimerCategorie);

        QMetaObject::connectSlotsByName(DialogSupprimerCategorie);
    } // setupUi

    void retranslateUi(QDialog *DialogSupprimerCategorie)
    {
        DialogSupprimerCategorie->setWindowTitle(QCoreApplication::translate("DialogSupprimerCategorie", "Supprimer la cat\303\251gorie", nullptr));
        label->setText(QCoreApplication::translate("DialogSupprimerCategorie", "Voulez-vous vraiment supprimer cette cat\303\251gorie et ses sous-cat\303\251gories ?", nullptr));
        btnOui->setText(QCoreApplication::translate("DialogSupprimerCategorie", "Oui", nullptr));
        btnNon->setText(QCoreApplication::translate("DialogSupprimerCategorie", "Non", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogSupprimerCategorie: public Ui_DialogSupprimerCategorie {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGSUPPRIMERCATEGORIE_H
