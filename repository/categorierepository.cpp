#include "CategorieRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>

#include "../model/Categorie.h"

void CategorieRepository::creerCategoriesParDefaut()
{
    QSqlQuery query;

    if (!query.exec(
            "SELECT COUNT(*) FROM Categorie WHERE utilisateur_id IS NULL")) {
        qDebug() << query.lastError();
        return;
    }

    if (!query.next())
        return;

    if (query.value(0).toInt() > 0)
        return;

    query.prepare(
        "INSERT INTO Categorie (id, nom, parent_id, utilisateur_id) "
        "VALUES (:id, :nom, :parent, NULL)");

    struct Cat { QString id, nom, parent; };

    QList<Cat> cats = {
        {"c1","Salaire & Revenus",""},
        {"c2","Logement",""},
        {"c3","Alimentation",""},
        {"c4","Transport",""},
        {"c5","Santé",""},
        {"c6","Loisirs",""},
        {"c7","Abonnements",""},
        {"c8","Épargne",""},
        {"c9","Autres",""},

        {"c10","Salaire","c1"},
        {"c11","Prime","c1"},
        {"c12","Aides / Bourses","c1"},
        {"c13","Revenus divers","c1"},

        {"c20","Loyer","c2"},
        {"c21","Charges","c2"},
        {"c22","Électricité","c2"},
        {"c23","Internet","c2"},

        {"c30","Courses","c3"},
        {"c31","Restaurants","c3"}
        // etc.
    };


    for (const auto& c : cats) {
        query.bindValue(":id", c.id);
        query.bindValue(":nom", c.nom);
        query.bindValue(":parent",
                        c.parent.isEmpty() ? QVariant() : c.parent);

        if (!query.exec())
            qDebug() << query.lastError();
    }

    qDebug() << "Catégories par défaut créées";
}


QList<Categorie*> CategorieRepository::chargerCategories(const QString& utilisateurId)
{
    QList<Categorie*> categories;
    QMap<QString, Categorie*> map;

    QSqlQuery query;
    query.prepare(
        "SELECT id, nom, parent_id "
        "FROM Categorie "
        "WHERE utilisateur_id IS NULL OR utilisateur_id = :uid"
        );
    query.bindValue(":uid", utilisateurId);
    query.exec();

    // 1ère passe : créer objets
    while (query.next()) {
        QString id = query.value("id").toString();
        QString nom = query.value("nom").toString();
        map[id] = new Categorie(id, nom);
    }

    query.first();
    query.previous();

    // 2ème passe : relier parents / enfants
    while (query.next()) {
        QString id = query.value("id").toString();
        QString parentId = query.value("parent_id").toString();

        if (!parentId.isEmpty()) {
            map[id]->~Categorie();
            new (map[id]) Categorie(id, map[id]->getNom(), map[parentId]);
        }
        categories.append(map[id]);
    }

    return categories;
}

QString CategorieRepository::ajouterCategorie(const QString& nom,
                                              const QString& utilisateurId,
                                              const QString& parentId)
{
    QString id = QUuid::createUuid().toString();
    QSqlQuery query;

    query.prepare(
        "INSERT INTO Categorie (id, nom, parent_id, utilisateur_id) "
        "VALUES (:id, :nom, :parent, :user)"
        );

    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":parent",
                    parentId.isEmpty() ? QVariant() : parentId);
    query.bindValue(":user", utilisateurId);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return "";
    }

    return id;
}

bool CategorieRepository::modifierCategorie(const QString& categorieId,
                                            const QString& nouveauNom)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE Categorie "
        "SET nom = :nom "
        "WHERE id = :id"
        );

    query.bindValue(":nom", nouveauNom);
    query.bindValue(":id", categorieId);

    if (!query.exec()) {
        qDebug() << "Erreur modification catégorie:"
                 << query.lastError().text();
        return false;
    }

    return true;
}
