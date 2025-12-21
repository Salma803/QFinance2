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
    QMap<QString, QString> parentMap;

    QSqlQuery query;
    query.prepare(
        "SELECT id, nom, parent_id "
        "FROM Categorie "
        "WHERE utilisateur_id IS NULL OR utilisateur_id = :uid"
        );
    query.bindValue(":uid", utilisateurId);

    if (!query.exec()) {
        qDebug() << "Erreur chargerCategories:" << query.lastError();
        return categories;
    }

    // 1) Créer tous les objets + mémoriser leur parent_id
    while (query.next()) {
        QString id = query.value("id").toString();
        QString nom = query.value("nom").toString();
        QString parentId = query.value("parent_id").toString();

        map[id] = new Categorie(id, nom, nullptr);
        parentMap[id] = parentId;
    }

    // 2) Relier parent/enfants
    for (auto it = map.begin(); it != map.end(); ++it) {
        QString id = it.key();
        Categorie* cat = it.value();

        QString parentId = parentMap.value(id);
        if (!parentId.isEmpty() && map.contains(parentId)) {
            Categorie* parent = map[parentId];
            // on ne "reconstruit" pas l'objet : on relie proprement
            // (donc il faut que Categorie ait un setter OU qu'on passe par ajouterEnfant)
            cat->setParent(parent);        // <- à ajouter si pas présent
            parent->ajouterEnfant(cat);
        }
    }

    categories = map.values();
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

bool CategorieRepository::supprimerCategorieAvecEnfants(const QString& categorieId)
{
    qDebug() << "Suppression de la catégorie:" << categorieId;

    QSqlQuery query;

    // Vérifier si la catégorie existe
    query.prepare("SELECT COUNT(*) FROM Categorie WHERE id = :id");
    query.bindValue(":id", categorieId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Erreur vérification existence:" << query.lastError().text();
        return false;
    }

    if (query.value(0).toInt() == 0) {
        qDebug() << "Catégorie non trouvée";
        return false;
    }

    // Récupérer tous les IDs à supprimer (parent + enfants)
    QList<QString> idsASupprimer;
    idsASupprimer.append(categorieId);

    // Trouver les enfants récursivement
    std::function<void(const QString&)> trouverEnfants = [&](const QString& parentId) {
        query.prepare("SELECT id FROM Categorie WHERE parent_id = :parentId");
        query.bindValue(":parentId", parentId);

        if (query.exec()) {
            while (query.next()) {
                QString enfantId = query.value("id").toString();
                idsASupprimer.append(enfantId);
                trouverEnfants(enfantId);
            }
        }
    };

    trouverEnfants(categorieId);

    qDebug() << "Catégories à supprimer:" << idsASupprimer.size();

    // Supprimer (en commençant par les enfants)
    bool succes = true;
    std::reverse(idsASupprimer.begin(), idsASupprimer.end());

    for (const QString& id : idsASupprimer) {
        query.prepare("DELETE FROM Categorie WHERE id = :id");
        query.bindValue(":id", id);

        if (!query.exec()) {
            qDebug() << "Erreur suppression" << id << ":" << query.lastError().text();
            succes = false;
        }
    }

    qDebug() << "Suppression" << (succes ? "réussie" : "échouée");
    return succes;
}

QList<QString> CategorieRepository::chargerIdsSousCategories(
    const QString& parentId)
{
    QList<QString> enfants;
    QSqlQuery query;

    query.prepare(
        "SELECT id FROM Categorie WHERE parent_id = :pid"
        );
    query.bindValue(":pid", parentId);

    if (!query.exec()) {
        qDebug() << "Erreur chargement sous-categories:" << query.lastError();
        return enfants;
    }

    while (query.next()) {
        enfants.append(query.value(0).toString());
    }

    return enfants;
}

QString CategorieRepository::getIdParNom(const QString& nom)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Categorie WHERE nom = :nom LIMIT 1");
    query.bindValue(":nom", nom);

    if (!query.exec()) {
        qDebug() << "Erreur getIdParNom Categorie:" << query.lastError();
        return "";
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return "";
}

