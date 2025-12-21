#include "DashboardManager.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QDebug>
#include <QColor>
#include <QDate>
#include <QMap>
#include <QScrollArea>
#include <QSqlQuery>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QDateTime>
#include <QImage>

#include "repository/OperationRepository.h"
#include "repository/BudgetRepository.h"
#include "repository/CompteRepository.h"
#include "repository/TransfertRepository.h"
#include "model/Categorie.h"
#include "model/Compte.h"

DashboardManager::DashboardManager(QObject *parent)
    : QObject(parent)
    , m_chartDepenses(nullptr)
    , m_chartRevenusVsDepenses(nullptr)
    , m_chartEvolution(nullptr)
    , m_chartRepartition(nullptr)
{
    m_statistiques.totalRevenus = 0;
    m_statistiques.totalDepenses = 0;
    m_statistiques.totalTransferts = 0;
    m_statistiques.solde = 0;
    m_statistiques.depensesParJour = 0;
}

DashboardManager::~DashboardManager()
{
}

void DashboardManager::initialiserCharts(QChartView *chartDepenses,
                                         QChartView *chartRevenusVsDepenses,
                                         QChartView *chartEvolution,
                                         QChartView *chartRepartition)
{
    m_chartDepenses = chartDepenses;
    m_chartRevenusVsDepenses = chartRevenusVsDepenses;
    m_chartEvolution = chartEvolution;
    m_chartRepartition = chartRepartition;
}

void DashboardManager::setCategories(const QList<Categorie*> &categories)
{
    m_categories = categories;
}

void DashboardManager::setComptes(const QList<Compte*> &comptes)
{
    m_comptes = comptes;
}

void DashboardManager::actualiserDashboard(int mois, int annee,
                                           const QList<Categorie*> &categories,
                                           const QList<Compte*> &comptes,
                                           const QString &filtreCategorie,
                                           const QString &filtreCompte)
{
    if (!categories.isEmpty()) {
        m_categories = categories;
    }

    if (!comptes.isEmpty()) {
        m_comptes = comptes;
    }

    qDebug() << "=== DASHBOARD DEBUG ===";
    qDebug() << "Mois:" << mois << "Année:" << annee;
    qDebug() << "Filtre compte:" << filtreCompte;
    qDebug() << "Filtre catégorie:" << filtreCategorie;
    qDebug() << "Nombre de catégories:" << m_categories.size();
    qDebug() << "Nombre de comptes:" << m_comptes.size();

    // Calculer les statistiques AVEC filtres
    calculerStatistiques(mois, annee, filtreCompte, filtreCategorie);

    // Générer les recommandations AVEC filtres
    m_recommandations = genererRecommandations(mois, annee, filtreCompte);

    // Créer les graphiques AVEC filtres
    creerChartDepensesParCategorie(mois, annee, filtreCompte, filtreCategorie);
    creerChartRevenusVsDepenses(mois, annee, filtreCompte);
    creerChartEvolution(annee, filtreCategorie, filtreCompte);
    creerChartRepartition(mois, annee, filtreCompte, filtreCategorie);

    emit dashboardActualise();
}

void DashboardManager::calculerStatistiques(int mois, int annee,
                                            const QString &filtreCompte,
                                            const QString &filtreCategorie)
{
    // Réinitialiser
    m_statistiques.totalRevenus = 0;
    m_statistiques.totalDepenses = 0;
    m_statistiques.totalTransferts = 0;
    m_statistiques.depensesParCategorie.clear();
    m_statistiques.evolutionMensuelle.clear();

    qDebug() << "=== CALCUL STATISTIQUES ===";
    qDebug() << "Filtre compte:" << filtreCompte;
    qDebug() << "Filtre catégorie:" << filtreCategorie;

    // Calculer les totaux AVEC filtres
    if (filtreCompte.isEmpty()) {
        m_statistiques.totalRevenus = OperationRepository::getTotalRevenus(mois, annee);
        m_statistiques.totalDepenses = OperationRepository::getTotalDepenses(mois, annee);
        m_statistiques.totalTransferts = getTotalTransferts(mois, annee);
    } else {
        m_statistiques.totalRevenus = OperationRepository::getTotalRevenusCompte(mois, annee, filtreCompte);
        m_statistiques.totalDepenses = OperationRepository::getTotalDepensesCompte(mois, annee, filtreCompte);
        m_statistiques.totalTransferts = getTotalTransfertsCompte(mois, annee, filtreCompte);
    }

    // Calculer le solde net (revenus - dépenses - transferts sortants)
    double transfertsSortants = getTransfertsSortants(mois, annee, filtreCompte);
    m_statistiques.solde = m_statistiques.totalRevenus - m_statistiques.totalDepenses - transfertsSortants;

    // Calculer les dépenses par jour (moyenne) - inclut les transferts
    QDate dateDebut(annee, mois, 1);
    int joursDansMois = dateDebut.daysInMonth();
    m_statistiques.depensesParJour = (m_statistiques.totalDepenses + transfertsSortants) / joursDansMois;

    // Calculer les dépenses par catégorie AVEC filtres
    for (Categorie* categorie : m_categories) {
        if (categorie->getParent()) continue; // Ignorer les sous-catégories

        double totalCategorie = 0;

        if (filtreCompte.isEmpty() && filtreCategorie.isEmpty()) {
            totalCategorie = OperationRepository::getTotalDepensesCategorie(
                categorie->getId(), mois, annee);
        } else if (!filtreCompte.isEmpty() && filtreCategorie.isEmpty()) {
            totalCategorie = OperationRepository::getTotalDepensesCategorieCompte(
                categorie->getId(), mois, annee, filtreCompte);
        } else if (filtreCategorie == categorie->getId()) {
            // Si on filtre par cette catégorie principale, on inclut toutes ses sous-catégories
            totalCategorie = OperationRepository::getTotalDepensesCategorieCompte(
                categorie->getId(), mois, annee, filtreCompte);

            // Ajouter les sous-catégories si elles existent
            for (Categorie* enfant : categorie->getEnfants()) {
                double totalEnfant = OperationRepository::getTotalDepensesCategorieCompte(
                    enfant->getId(), mois, annee, filtreCompte);
                totalCategorie += totalEnfant;
            }
        }

        if (totalCategorie > 0) {
            m_statistiques.depensesParCategorie[categorie->getNom()] = totalCategorie;
            qDebug() << "  Catégorie" << categorie->getNom() << ":" << totalCategorie << "€";
        }
    }

    // Calculer l'évolution mensuelle pour l'année AVEC filtres
    for (int m = 1; m <= 12; m++) {
        if (m > mois && annee == QDate::currentDate().year()) break;

        double depensesMois = 0;
        double revenusMois = 0;

        if (filtreCompte.isEmpty()) {
            depensesMois = OperationRepository::getTotalDepenses(m, annee);
            revenusMois = OperationRepository::getTotalRevenus(m, annee);
        } else {
            depensesMois = OperationRepository::getTotalDepensesCompte(m, annee, filtreCompte);
            revenusMois = OperationRepository::getTotalRevenusCompte(m, annee, filtreCompte);
        }

        // Stocker le solde (revenus - dépenses) pour l'évolution
        m_statistiques.evolutionMensuelle[QString::number(m)] = revenusMois - depensesMois;
    }

    qDebug() << "Revenus totaux:" << m_statistiques.totalRevenus;
    qDebug() << "Dépenses totales:" << m_statistiques.totalDepenses;
    qDebug() << "Transferts totaux:" << m_statistiques.totalTransferts;
    qDebug() << "Transferts sortants:" << transfertsSortants;
    qDebug() << "Solde:" << m_statistiques.solde;
    qDebug() << "=== FIN STATISTIQUES ===";
}

QString DashboardManager::genererRecommandations(int mois, int annee, const QString &filtreCompte)
{
    QStringList recommandations;

    // Analyse des transferts
    double transfertsSortants = getTransfertsSortants(mois, annee, filtreCompte);

    if (transfertsSortants > m_statistiques.totalRevenus * 0.3) {
        recommandations << "💸 **Transferts élevés** : "
                        << QString::number(transfertsSortants, 'f', 2)
                        << " € de transferts ce mois (plus de 30% de vos revenus).";
        recommandations << "→ **Conseil** : Vérifiez si ces transferts sont nécessaires.";
    }

    // Analyse du solde pour inclure les transferts
    if (m_statistiques.solde < -100) {
        recommandations << "⚠️ **Déficit important** : Votre solde est négatif de "
                        << QString::number(-m_statistiques.solde, 'f', 2) << " € (inclut transferts).";
        if (transfertsSortants > 0) {
            recommandations << "   (dont " << QString::number(transfertsSortants, 'f', 2)
            << " € de transferts sortants)";
        }
        recommandations << "→ **Action immédiate** : Réduisez dépenses et transferts non essentiels.";
    } else if (m_statistiques.solde < 0) {
        recommandations << "⚠️ **Petit déficit** : Vous avez un solde négatif de "
                        << QString::number(-m_statistiques.solde, 'f', 2) << " €.";
    } else if (m_statistiques.solde > 1000) {
        recommandations << "✅ **Excédent important** : Vous avez économisé "
                        << QString::number(m_statistiques.solde, 'f', 2) << " € ce mois.";
        recommandations << "→ **Conseil** : Placez 70% sur épargne, gardez 30% pour projets.";
    } else if (m_statistiques.solde > 0) {
        recommandations << "👍 **Solde positif** : Excédent de "
                        << QString::number(m_statistiques.solde, 'f', 2) << " €.";
    }

    // Analyse des dépenses quotidiennes pour inclure transferts
    if (m_statistiques.depensesParJour > 50) {
        recommandations << "💰 **Dépenses quotidiennes élevées** : "
                        << QString::number(m_statistiques.depensesParJour, 'f', 2)
                        << " €/jour (inclut transferts).";
        recommandations << "→ **Objectif** : Essayez de rester sous 35 €/jour.";
    } else if (m_statistiques.depensesParJour > 0) {
        recommandations << "📊 **Dépenses quotidiennes** : "
                        << QString::number(m_statistiques.depensesParJour, 'f', 2)
                        << " €/jour (inclut transferts).";
    }

    // Recommandation spécifique sur les transferts
    if (transfertsSortants > 0) {
        double pourcentageTransferts = (transfertsSortants / (m_statistiques.totalRevenus + 0.01)) * 100;
        if (pourcentageTransferts > 20) {
            recommandations << "🔁 **Transferts importants** : "
                            << QString::number(pourcentageTransferts, 'f', 1)
                            << "% de vos revenus partent en transferts.";
            recommandations << "→ **Vérification** : Ces transferts sont-ils optimisés ?";
        }
    }

    // Analyse par catégorie
    if (!m_statistiques.depensesParCategorie.isEmpty()) {
        QString plusGrosseCategorie;
        double maxDepenses = 0;
        double totalToutesDepenses = m_statistiques.totalDepenses;

        for (auto it = m_statistiques.depensesParCategorie.begin();
             it != m_statistiques.depensesParCategorie.end(); ++it) {
            if (it.value() > maxDepenses) {
                maxDepenses = it.value();
                plusGrosseCategorie = it.key();
            }
        }

        if (totalToutesDepenses > 0) {
            double pourcentage = (maxDepenses / totalToutesDepenses) * 100;

            if (pourcentage > 40) {
                recommandations << "🎯 **Catégorie dominante** : " << plusGrosseCategorie
                                << " représente " << QString::number(pourcentage, 'f', 1) << "% de vos dépenses.";
                recommandations << "→ **Vérification** : Ces dépenses sont-elles toutes nécessaires ?";
            }
        }
    }

    // Analyse saisonnière
    int moisCourant = QDate::currentDate().month();
    if (moisCourant == 12) {
        recommandations << "🎄 **Période de Noël** : Surveillez vos dépenses cadeaux et festives.";
    } else if (moisCourant >= 6 && moisCourant <= 8) {
        recommandations << "☀️ **Été** : Pensez à budgéter vos vacances et activités estivales.";
    }

    // Épargne
    if (m_statistiques.totalRevenus > 0) {
        double pourcentageEpargne = (m_statistiques.solde / m_statistiques.totalRevenus) * 100;

        if (pourcentageEpargne >= 15) {
            recommandations << "🌟 **Excellente épargne** : " << QString::number(pourcentageEpargne, 'f', 1)
                            << "% d'épargne. Parfait !";
        } else if (pourcentageEpargne >= 10) {
            recommandations << "📈 **Bonne épargne** : " << QString::number(pourcentageEpargne, 'f', 1)
                            << "% d'épargne. Continuez !";
        } else if (pourcentageEpargne > 0) {
            recommandations << "💡 **Épargne modeste** : " << QString::number(pourcentageEpargne, 'f', 1)
                            << "% d'épargne. Objectif : 15%.";
        }
    }

    // Recommandation générale si peu de données
    if (m_statistiques.totalDepenses < 50 && m_statistiques.totalRevenus < 50) {
        recommandations << "📋 **Peu de données** : Ajoutez plus d'opérations pour des analyses précises.";
    }

    // Recommandation finale
    if (recommandations.isEmpty()) {
        recommandations << "📊 **Situation stable** : Vos finances semblent bien gérées.";
        recommandations << "→ **Conseil** : Continuez à suivre régulièrement vos dépenses.";
    }

    return recommandations.join("\n\n");
}

void DashboardManager::creerChartDepensesParCategorie(int mois, int annee,
                                                      const QString &filtreCompte,
                                                      const QString &filtreCategorie)
{
    if (!m_chartDepenses) return;

    qDebug() << "Création chart dépenses par catégorie...";
    qDebug() << "Filtre compte:" << filtreCompte;
    qDebug() << "Filtre catégorie:" << filtreCategorie;

    QBarSeries *series = new QBarSeries();
    QMap<QString, double> categoriesAvecDonnees;

    // Calculer les totaux par catégorie AVEC filtres
    for (Categorie* categorie : m_categories) {
        if (categorie->getParent()) continue; // Ignorer les sous-catégories au niveau principal

        double totalDepenses = 0;

        if (filtreCategorie.isEmpty()) {
            // Afficher toutes les catégories principales
            if (filtreCompte.isEmpty()) {
                totalDepenses = OperationRepository::getTotalDepensesCategorie(
                    categorie->getId(), mois, annee);
            } else {
                totalDepenses = OperationRepository::getTotalDepensesCategorieCompte(
                    categorie->getId(), mois, annee, filtreCompte);
            }
        } else if (filtreCategorie == categorie->getId()) {
            // Si on filtre par cette catégorie, on montre ses sous-catégories
            // On ne traite pas ici, on traitera dans la boucle suivante pour les enfants
            continue;
        }

        qDebug() << "  Catégorie" << categorie->getNom() << ":" << totalDepenses;

        if (totalDepenses > 0) {
            categoriesAvecDonnees[categorie->getNom()] = totalDepenses;
        }
    }

    // Si on filtre par une catégorie spécifique, montrer ses sous-catégories
    if (!filtreCategorie.isEmpty()) {
        for (Categorie* categorie : m_categories) {
            if (categorie->getParent() && categorie->getParent()->getId() == filtreCategorie) {
                double totalDepenses = 0;

                if (filtreCompte.isEmpty()) {
                    totalDepenses = OperationRepository::getTotalDepensesCategorie(
                        categorie->getId(), mois, annee);
                } else {
                    totalDepenses = OperationRepository::getTotalDepensesCategorieCompte(
                        categorie->getId(), mois, annee, filtreCompte);
                }

                qDebug() << "  Sous-catégorie" << categorie->getNom() << ":" << totalDepenses;

                if (totalDepenses > 0) {
                    categoriesAvecDonnees[categorie->getNom()] = totalDepenses;
                }
            }
        }
    }

    // Trier par montant décroissant
    QList<QString> categoriesTriees = categoriesAvecDonnees.keys();
    std::sort(categoriesTriees.begin(), categoriesTriees.end(),
              [&](const QString &a, const QString &b) {
                  return categoriesAvecDonnees[a] > categoriesAvecDonnees[b];
              });

    // Garder seulement les 8 plus grosses catégories
    if (categoriesTriees.size() > 8) {
        categoriesTriees = categoriesTriees.mid(0, 8);
    }

    // Créer les barres
    for (const QString &nomCategorie : categoriesTriees) {
        QBarSet *set = new QBarSet(nomCategorie);
        *set << categoriesAvecDonnees[nomCategorie];

        // Couleurs selon le montant
        double montant = categoriesAvecDonnees[nomCategorie];
        if (montant > 500) {
            set->setColor(QColor(231, 76, 60)); // Rouge
        } else if (montant > 200) {
            set->setColor(QColor(230, 126, 34)); // Orange
        } else {
            set->setColor(QColor(52, 152, 219)); // Bleu
        }

        series->append(set);
    }

    if (series->count() == 0) {
        delete series;
        QChart *chart = new QChart();

        if (!filtreCategorie.isEmpty()) {
            chart->setTitle("Aucune dépense pour cette catégorie et cette période");
        } else {
            chart->setTitle("Aucune dépense pour cette période");
        }

        m_chartDepenses->setChart(chart);
        return;
    }

    QChart *chart = new QChart();
    chart->addSeries(series);

    // Titre adapté au filtre
    QString titre = "Dépenses par catégorie - " + QString::number(mois) + "/" + QString::number(annee);
    if (!filtreCompte.isEmpty()) {
        titre += " (Compte filtré)";
    }
    if (!filtreCategorie.isEmpty()) {
        titre += " (Catégorie filtrée)";
    }

    chart->setTitle(titre);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Axes
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.0f €");
    axisY->setTitleText("Montant (€)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    m_chartDepenses->setChart(chart);
    qDebug() << "Chart dépenses par catégorie créé avec" << series->count() << "catégories";
}

void DashboardManager::creerChartRevenusVsDepenses(int mois, int annee, const QString &filtreCompte)
{
    if (!m_chartRevenusVsDepenses) return;

    qDebug() << "Création chart revenus vs dépenses...";
    qDebug() << "Filtre compte:" << filtreCompte;

    // Créer un graphique à barres groupées
    QBarSeries *series = new QBarSeries();

    double totalRevenus = 0;
    double totalDepenses = 0;
    double totalTransferts = 0;

    if (filtreCompte.isEmpty()) {
        totalRevenus = OperationRepository::getTotalRevenus(mois, annee);
        totalDepenses = OperationRepository::getTotalDepenses(mois, annee);
        totalTransferts = getTotalTransferts(mois, annee);
    } else {
        totalRevenus = OperationRepository::getTotalRevenusCompte(mois, annee, filtreCompte);
        totalDepenses = OperationRepository::getTotalDepensesCompte(mois, annee, filtreCompte);
        totalTransferts = getTotalTransfertsCompte(mois, annee, filtreCompte);
    }

    // Séparer transferts entrants et sortants
    double transfertsSortants = getTransfertsSortants(mois, annee, filtreCompte);
    double transfertsEntrants = totalTransferts - transfertsSortants;

    QBarSet *setRevenus = new QBarSet("Revenus");
    *setRevenus << totalRevenus;
    setRevenus->setColor(QColor(46, 204, 113)); // Vert

    QBarSet *setDepenses = new QBarSet("Dépenses");
    *setDepenses << totalDepenses;
    setDepenses->setColor(QColor(231, 76, 60)); // Rouge

    QBarSet *setTransfertsEntrants = new QBarSet("Transferts entrants");
    *setTransfertsEntrants << transfertsEntrants;
    setTransfertsEntrants->setColor(QColor(155, 89, 182)); // Violet

    QBarSet *setTransfertsSortants = new QBarSet("Transferts sortants");
    *setTransfertsSortants << transfertsSortants;
    setTransfertsSortants->setColor(QColor(241, 196, 15)); // Jaune

    series->append(setRevenus);
    series->append(setDepenses);
    series->append(setTransfertsEntrants);
    series->append(setTransfertsSortants);

    QChart *chart = new QChart();
    chart->addSeries(series);

    // Titre avec le solde
    double solde = totalRevenus + transfertsEntrants - totalDepenses - transfertsSortants;
    QString titre = QString("Revenus vs Dépenses vs Transferts - Solde: %1 €")
                        .arg(solde, 0, 'f', 2);

    if (!filtreCompte.isEmpty()) {
        titre += " (Compte filtré)";
    }

    if (solde < 0) {
        titre = "⚠️ " + titre;
    }

    chart->setTitle(titre);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Axes
    QStringList categories;
    categories << "Comparaison";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.0f €");
    axisY->setTitleText("Montant (€)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartRevenusVsDepenses->setChart(chart);
    qDebug() << "Chart revenus vs dépenses vs transferts créé";
}

void DashboardManager::creerChartEvolution(int annee, const QString &filtreCategorie, const QString &filtreCompte)
{
    if (!m_chartEvolution) return;

    qDebug() << "Création chart évolution...";
    qDebug() << "Filtre compte:" << filtreCompte;
    qDebug() << "Filtre catégorie:" << filtreCategorie;

    QLineSeries *seriesSolde = new QLineSeries();
    seriesSolde->setName("Solde mensuel");
    seriesSolde->setColor(QColor(52, 152, 219));

    // Récupérer les données pour chaque mois AVEC filtres
    QStringList moisLabels;
    double maxValue = 0;
    double minValue = 0;

    for (int mois = 1; mois <= 12; mois++) {
        if (mois > QDate::currentDate().month() && annee == QDate::currentDate().year()) {
            break;
        }

        double depenses = 0;
        double revenus = 0;

        if (filtreCompte.isEmpty()) {
            depenses = OperationRepository::getTotalDepenses(mois, annee);
            revenus = OperationRepository::getTotalRevenus(mois, annee);
        } else {
            depenses = OperationRepository::getTotalDepensesCompte(mois, annee, filtreCompte);
            revenus = OperationRepository::getTotalRevenusCompte(mois, annee, filtreCompte);
        }

        // Ajouter les transferts pour le solde
        double transfertsSortants = getTransfertsSortants(mois, annee, filtreCompte);
        double solde = revenus - depenses - transfertsSortants;

        seriesSolde->append(mois, solde);

        // Mettre à jour min/max pour l'axe Y
        if (solde > maxValue) maxValue = solde;
        if (solde < minValue) minValue = solde;

        // Labels des mois
        moisLabels << QDate(annee, mois, 1).toString("MMM");
    }

    if (seriesSolde->count() == 0) {
        delete seriesSolde;
        QChart *chart = new QChart();
        chart->setTitle("Évolution mensuelle (pas de données)");
        m_chartEvolution->setChart(chart);
        return;
    }

    QChart *chart = new QChart();
    chart->addSeries(seriesSolde);

    QString titre = "Évolution du solde " + QString::number(annee);
    if (!filtreCompte.isEmpty()) {
        titre += " (Compte filtré)";
    }
    if (!filtreCategorie.isEmpty()) {
        titre += " (Catégorie filtrée)";
    }

    chart->setTitle(titre);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Axe X
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(moisLabels);
    axisX->setTitleText("Mois");
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesSolde->attachAxis(axisX);

    // Axe Y
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.0f €");
    axisY->setTitleText("Solde (€)");

    // Ajuster l'échelle de l'axe Y
    double range = maxValue - minValue;
    double padding = range * 0.1; // 10% de padding
    axisY->setRange(minValue - padding, maxValue + padding);

    chart->addAxis(axisY, Qt::AlignLeft);
    seriesSolde->attachAxis(axisY);

    // Légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartEvolution->setChart(chart);
    qDebug() << "Chart évolution créé avec" << seriesSolde->count() << "points";
}

void DashboardManager::creerChartRepartition(int mois, int annee,
                                             const QString &filtreCompte,
                                             const QString &filtreCategorie)
{
    if (!m_chartRepartition) return;

    qDebug() << "Création chart répartition...";
    qDebug() << "Filtre compte:" << filtreCompte;
    qDebug() << "Filtre catégorie:" << filtreCategorie;

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.35);

    // Couleurs pour le camembert
    QList<QColor> couleurs = {
        QColor(52, 152, 219),   // Bleu
        QColor(46, 204, 113),   // Vert
        QColor(155, 89, 182),   // Violet
        QColor(241, 196, 15),   // Jaune
        QColor(230, 126, 34),   // Orange
        QColor(231, 76, 60),    // Rouge
        QColor(149, 165, 166),  // Gris
        QColor(22, 160, 133)    // Turquoise
    };

    // Ajouter les catégories avec données AVEC filtres
    int couleurIndex = 0;
    double totalToutesDepenses = 0;
    QMap<QString, double> depensesParCategorie;

    // Si on filtre par une catégorie, on montre ses sous-catégories
    if (!filtreCategorie.isEmpty()) {
        for (Categorie* categorie : m_categories) {
            if (categorie->getParent() && categorie->getParent()->getId() == filtreCategorie) {
                double depenses = 0;

                if (filtreCompte.isEmpty()) {
                    depenses = OperationRepository::getTotalDepensesCategorie(
                        categorie->getId(), mois, annee);
                } else {
                    depenses = OperationRepository::getTotalDepensesCategorieCompte(
                        categorie->getId(), mois, annee, filtreCompte);
                }

                if (depenses > 0) {
                    depensesParCategorie[categorie->getNom()] = depenses;
                    totalToutesDepenses += depenses;
                }
            }
        }
    } else {
        // Sinon, on montre les catégories principales
        for (Categorie* categorie : m_categories) {
            if (categorie->getParent()) continue; // Ignorer les sous-catégories

            double depenses = 0;

            if (filtreCompte.isEmpty()) {
                depenses = OperationRepository::getTotalDepensesCategorie(
                    categorie->getId(), mois, annee);
            } else {
                depenses = OperationRepository::getTotalDepensesCategorieCompte(
                    categorie->getId(), mois, annee, filtreCompte);
            }

            if (depenses > 0) {
                depensesParCategorie[categorie->getNom()] = depenses;
                totalToutesDepenses += depenses;
            }
        }
    }

    // Trier par montant décroissant
    QList<QString> categoriesTriees = depensesParCategorie.keys();
    std::sort(categoriesTriees.begin(), categoriesTriees.end(),
              [&](const QString &a, const QString &b) {
                  return depensesParCategorie[a] > depensesParCategorie[b];
              });

    // Ajouter au camembert (max 6 catégories)
    int maxCategories = qMin(6, categoriesTriees.size());
    double autresDepenses = 0;

    for (int i = 0; i < categoriesTriees.size(); i++) {
        QString nomCategorie = categoriesTriees[i];
        double depenses = depensesParCategorie[nomCategorie];

        if (i < maxCategories) {
            double pourcentage = (depenses / totalToutesDepenses) * 100;
            QString label = QString("%1\n%2€ (%3%)")
                                .arg(nomCategorie)
                                .arg(depenses, 0, 'f', 0)
                                .arg(pourcentage, 0, 'f', 1);

            QPieSlice *slice = series->append(label, depenses);
            slice->setColor(couleurs[couleurIndex % couleurs.size()]);
            slice->setLabelVisible();

            couleurIndex++;
        } else {
            autresDepenses += depenses;
        }
    }

    // Ajouter "Autres" si nécessaire
    if (autresDepenses > 0) {
        double pourcentageAutres = (autresDepenses / totalToutesDepenses) * 100;
        QString label = QString("Autres\n%1€ (%2%)")
                            .arg(autresDepenses, 0, 'f', 0)
                            .arg(pourcentageAutres, 0, 'f', 1);

        QPieSlice *slice = series->append(label, autresDepenses);
        slice->setColor(QColor(149, 165, 166)); // Gris
        slice->setLabelVisible();
    }

    if (series->count() == 0) {
        delete series;
        QChart *chart = new QChart();

        if (!filtreCategorie.isEmpty()) {
            chart->setTitle("Répartition des dépenses (pas de données pour cette catégorie)");
        } else {
            chart->setTitle("Répartition des dépenses (pas de données)");
        }

        m_chartRepartition->setChart(chart);
        return;
    }

    QChart *chart = new QChart();
    chart->addSeries(series);

    QString titre = "Répartition des dépenses - " + QString::number(mois) + "/" + QString::number(annee);
    if (!filtreCompte.isEmpty()) {
        titre += " (Compte filtré)";
    }
    if (!filtreCategorie.isEmpty()) {
        titre += " (Catégorie filtrée)";
    }

    chart->setTitle(titre);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    m_chartRepartition->setChart(chart);
    qDebug() << "Chart répartition créé avec" << series->count() << "sections";
}

// Méthodes pour les transferts
double DashboardManager::getTotalTransferts(int mois, int annee)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    query.prepare(
        "SELECT SUM(montant) FROM Transfert "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

double DashboardManager::getTotalTransfertsCompte(int mois, int annee, const QString &compteId)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    query.prepare(
        "SELECT SUM(montant) FROM Transfert "
        "WHERE substr(date, 6, 2) = :mois "
        "AND substr(date, 1, 4) = :annee "
        "AND (source_id = :compte OR destination_id = :compte)"
        );
    query.bindValue(":mois", moisStr);
    query.bindValue(":annee", anneeStr);
    query.bindValue(":compte", compteId);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

double DashboardManager::getTransfertsSortants(int mois, int annee, const QString &compteId)
{
    QSqlQuery query;
    QString moisStr = QString::number(mois).rightJustified(2, '0');
    QString anneeStr = QString::number(annee);

    if (compteId.isEmpty()) {
        // Tous les transferts sortants (tous comptes)
        query.prepare(
            "SELECT SUM(montant) FROM Transfert "
            "WHERE substr(date, 6, 2) = :mois "
            "AND substr(date, 1, 4) = :annee"
            );
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);
    } else {
        // Transferts sortants d'un compte spécifique
        query.prepare(
            "SELECT SUM(montant) FROM Transfert "
            "WHERE substr(date, 6, 2) = :mois "
            "AND substr(date, 1, 4) = :annee "
            "AND source_id = :compte"
            );
        query.bindValue(":mois", moisStr);
        query.bindValue(":annee", anneeStr);
        query.bindValue(":compte", compteId);
    }

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

DashboardManager::Statistiques DashboardManager::getStatistiques() const
{
    return m_statistiques;
}

QString DashboardManager::getRecommandations() const
{
    return m_recommandations;
}

QString DashboardManager::captureDiagrammeEnImage(QChartView* chartView, const QString &nom)
{
    if (!chartView || !chartView->chart()) {
        return "";
    }

    // Créer un nom de fichier unique
    QString tempDir = QDir::tempPath();
    QString imagePath = tempDir + "/" + nom + "_" +
                        QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";

    // Capturer le graphique en image haute résolution
    QPixmap pixmap = chartView->grab();

    // Redimensionner pour une meilleure qualité dans le PDF
    QPixmap scaledPixmap = pixmap.scaled(1200, 700, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Sauvegarder l'image
    scaledPixmap.save(imagePath, "PNG", 100);

    return imagePath;
}

// Version simple de générerHTMLStatistiques sans les chemins d'images
QString DashboardManager::genererHTMLStatistiques(int mois, int annee,
                                                  const QString &filtreCompte,
                                                  const QString &filtreCategorie,
                                                  bool tousLesComptes)
{
    // Appeler la version complète avec des chemins d'images vides
    return genererHTMLStatistiques(mois, annee, filtreCompte, filtreCategorie,
                                   tousLesComptes, "", "", "", "");
}

// Version complète avec chemins d'images
QString DashboardManager::genererHTMLStatistiques(int mois, int annee,
                                                  const QString &filtreCompte,
                                                  const QString &filtreCategorie,
                                                  bool tousLesComptes,
                                                  const QString &imageDepensesPath,
                                                  const QString &imageRevenusPath,
                                                  const QString &imageEvolutionPath,
                                                  const QString &imageRepartitionPath)
{
    QStringList moisNoms = {
        "Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"
    };

    QString moisNom = moisNoms.value(mois - 1, "Mois inconnu");

    QString html;
    html += "<!DOCTYPE html>";
    html += "<html>";
    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>Rapport Financier</title>";
    html += "<style>";
    html += "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; padding: 20px; background-color: #ffffff; color: #000000; font-size: 12pt; }";
    html += "h1 { color: #2c3e50; border-bottom: 3px solid #3498db; padding-bottom: 15px; margin-bottom: 30px; font-size: 24pt; }";
    html += "h2 { color: #34495e; margin-top: 40px; padding-bottom: 10px; border-bottom: 1px solid #ecf0f1; font-size: 18pt; }";
    html += "h3 { color: #7f8c8d; margin-top: 25px; font-size: 14pt; }";
    html += "table { width: 100%; border-collapse: collapse; margin: 20px 0; font-size: 11pt; }";
    html += "th { background-color: #3498db; color: white; padding: 12px; text-align: left; font-weight: bold; }";
    html += "td { padding: 10px; border-bottom: 1px solid #ddd; }";
    html += "tr:nth-child(even) { background-color: #f8f9fa; }";
    html += ".stat-box { background-color: #f8f9fa; padding: 20px; border-radius: 8px; margin: 20px 0; border: 1px solid #e9ecef; }";
    html += ".stat-value { font-size: 20pt; font-weight: bold; color: #2c3e50; }";
    html += ".stat-label { color: #6c757d; font-size: 11pt; font-weight: bold; }";
    html += ".positive { color: #28a745; }";
    html += ".negative { color: #dc3545; }";
    html += ".warning { color: #ffc107; }";
    html += ".section { page-break-inside: avoid; margin-bottom: 30px; }";
    html += ".header { text-align: center; margin-bottom: 40px; background-color: #f8f9fa; padding: 20px; border-radius: 8px; }";
    html += ".date { color: #6c757d; font-size: 11pt; margin-top: 5px; }";
    html += ".diagram-container { text-align: center; margin: 25px 0; page-break-inside: avoid; }";
    html += ".diagram-title { font-weight: bold; margin-bottom: 15px; color: #495057; font-size: 14pt; }";
    html += ".diagram-img { max-width: 100%; height: auto; border: 1px solid #dee2e6; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
    html += ".diagram-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; margin: 20px 0; }";
    html += ".footer { margin-top: 50px; padding-top: 20px; border-top: 2px solid #dee2e6; color: #6c757d; font-size: 10pt; text-align: center; }";
    html += ".info-box { background-color: #e8f4fd; border-left: 4px solid #3498db; padding: 15px; margin: 15px 0; border-radius: 4px; }";
    html += "@media print {";
    html += "  body { font-size: 10pt; }";
    html += "  .no-print { display: none; }";
    html += "  .page-break { page-break-before: always; }";
    html += "}";
    html += "</style>";
    html += "</head>";
    html += "<body>";

    // En-tête
    html += "<div class='header'>";
    html += "<h1>📊 Rapport Financier Personnel</h1>";
    html += "<div style='font-size: 14pt; color: #495057; margin: 10px 0;'>";
    html += moisNom + " " + QString::number(annee);

    if (!filtreCompte.isEmpty() && !tousLesComptes) {
        QString nomCompte = "Compte inconnu";
        for (Compte* compte : m_comptes) {
            if (compte->getId() == filtreCompte) {
                nomCompte = compte->getNom();
                break;
            }
        }
        html += " | Compte: " + nomCompte;
    } else if (tousLesComptes) {
        html += " | Vue globale (tous les comptes)";
    }

    if (!filtreCategorie.isEmpty()) {
        QString nomCategorie = "Catégorie inconnue";
        for (Categorie* categorie : m_categories) {
            if (categorie->getId() == filtreCategorie) {
                nomCategorie = categorie->getNom();
                break;
            }
        }
        html += " | Catégorie: " + nomCategorie;
    }
    html += "</div>";

    html += "<p class='date'>Généré le " + QDate::currentDate().toString("dd/MM/yyyy") +
            " à " + QTime::currentTime().toString("HH:mm") + "</p>";
    html += "</div>";

    // Statistiques principales
    html += "<div class='section'>";
    html += "<h2>📈 Vue d'ensemble financière</h2>";
    html += "<div class='stat-box'>";
    html += "<div style='display: grid; grid-template-columns: repeat(3, 1fr); gap: 20px;'>";

    // Revenus
    html += "<div style='text-align: center; padding: 15px; background-color: #d4edda; border-radius: 6px;'>";
    html += "<div class='stat-label'>Revenus totaux</div>";
    html += "<div class='stat-value positive'>" + QString::number(m_statistiques.totalRevenus, 'f', 2) + " €</div>";
    html += "</div>";

    // Dépenses
    html += "<div style='text-align: center; padding: 15px; background-color: #f8d7da; border-radius: 6px;'>";
    html += "<div class='stat-label'>Dépenses totales</div>";
    html += "<div class='stat-value negative'>" + QString::number(m_statistiques.totalDepenses, 'f', 2) + " €</div>";
    html += "</div>";

    // Solde
    QString soldeClass = m_statistiques.solde >= 0 ? "positive" : "negative";
    html += "<div style='text-align: center; padding: 15px; background-color: " +
            QString(m_statistiques.solde >= 0 ? "#d1ecf1" : "#f5c6cb") +
            "; border-radius: 6px;'>";
    html += "<div class='stat-label'>Solde net</div>";
    html += "<div class='stat-value " + soldeClass + "'>" +
            QString::number(m_statistiques.solde, 'f', 2) + " €</div>";
    html += "</div>";

    html += "</div>";

    // Statistiques secondaires
    html += "<div style='display: grid; grid-template-columns: repeat(3, 1fr); gap: 15px; margin-top: 20px;'>";

    html += "<div style='text-align: center;'>";
    html += "<div class='stat-label'>Dépenses/jour</div>";
    html += "<div style='font-size: 14pt;'>" + QString::number(m_statistiques.depensesParJour, 'f', 2) + " €</div>";
    html += "</div>";

    double transfertsSortants = getTransfertsSortants(mois, annee, filtreCompte);
    html += "<div style='text-align: center;'>";
    html += "<div class='stat-label'>Transferts sortants</div>";
    html += "<div style='font-size: 14pt;'>" + QString::number(transfertsSortants, 'f', 2) + " €</div>";
    html += "</div>";

    double totalTransferts = getTotalTransferts(mois, annee);
    html += "<div style='text-align: center;'>";
    html += "<div class='stat-label'>Transferts totaux</div>";
    html += "<div style='font-size: 14pt;'>" + QString::number(totalTransferts, 'f', 2) + " €</div>";
    html += "</div>";

    html += "</div>";
    html += "</div>";
    html += "</div>";

    // Diagrammes - seulement si on a des images
    bool hasImages = (!imageDepensesPath.isEmpty() && QFile::exists(imageDepensesPath)) ||
                     (!imageRevenusPath.isEmpty() && QFile::exists(imageRevenusPath)) ||
                     (!imageEvolutionPath.isEmpty() && QFile::exists(imageEvolutionPath)) ||
                     (!imageRepartitionPath.isEmpty() && QFile::exists(imageRepartitionPath));

    if (hasImages) {
        html += "<div class='section'>";
        html += "<h2>📊 Diagrammes et Graphiques</h2>";
        html += "<div class='info-box'>";
        html += "Les graphiques ci-dessous montrent la répartition de vos finances pour la période sélectionnée.";
        html += "</div>";

        html += "<div class='diagram-grid'>";

        // Diagramme 1: Dépenses par catégorie
        if (!imageDepensesPath.isEmpty() && QFile::exists(imageDepensesPath)) {
            QImage image(imageDepensesPath);
            if (!image.isNull()) {
                html += "<div class='diagram-container'>";
                html += "<div class='diagram-title'>Dépenses par Catégorie</div>";
                html += "<img src='file:///" + imageDepensesPath + "' class='diagram-img' />";
                html += "</div>";
            }
        }

        // Diagramme 2: Revenus vs Dépenses
        if (!imageRevenusPath.isEmpty() && QFile::exists(imageRevenusPath)) {
            QImage image(imageRevenusPath);
            if (!image.isNull()) {
                html += "<div class='diagram-container'>";
                html += "<div class='diagram-title'>Revenus vs Dépenses vs Transferts</div>";
                html += "<img src='file:///" + imageRevenusPath + "' class='diagram-img' />";
                html += "</div>";
            }
        }

        // Diagramme 3: Évolution
        if (!imageEvolutionPath.isEmpty() && QFile::exists(imageEvolutionPath)) {
            QImage image(imageEvolutionPath);
            if (!image.isNull()) {
                html += "<div class='diagram-container'>";
                html += "<div class='diagram-title'>Évolution du Solde Mensuel</div>";
                html += "<img src='file:///" + imageEvolutionPath + "' class='diagram-img' />";
                html += "</div>";
            }
        }

        // Diagramme 4: Répartition
        if (!imageRepartitionPath.isEmpty() && QFile::exists(imageRepartitionPath)) {
            QImage image(imageRepartitionPath);
            if (!image.isNull()) {
                html += "<div class='diagram-container'>";
                html += "<div class='diagram-title'>Répartition des Dépenses</div>";
                html += "<img src='file:///" + imageRepartitionPath + "' class='diagram-img' />";
                html += "</div>";
            }
        }

        html += "</div>"; // Fin diagram-grid
        html += "</div>"; // Fin section diagrammes
    }

    // Tableau des dépenses par catégorie
    if (!m_statistiques.depensesParCategorie.isEmpty()) {
        html += "<div class='section'>";
        html += "<h2>🏷️ Analyse Détailée par Catégorie</h2>";
        html += "<table>";
        html += "<thead>";
        html += "<tr><th style='width: 40%;'>Catégorie</th><th style='width: 20%; text-align: right;'>Montant</th><th style='width: 20%; text-align: right;'>% du total</th><th style='width: 20%; text-align: center;'>Tendance</th></tr>";
        html += "</thead>";
        html += "<tbody>";

        double totalDepenses = m_statistiques.totalDepenses;
        if (totalDepenses > 0) {
            for (auto it = m_statistiques.depensesParCategorie.begin(); it != m_statistiques.depensesParCategorie.end(); ++it) {
                double pourcentage = (it.value() / totalDepenses) * 100;
                QString tendanceIcon = "";
                QString tendanceClass = "";

                if (pourcentage > 30) {
                    tendanceIcon = "🔴";
                    tendanceClass = "negative";
                } else if (pourcentage > 15) {
                    tendanceIcon = "🟡";
                    tendanceClass = "warning";
                } else {
                    tendanceIcon = "🟢";
                    tendanceClass = "positive";
                }

                html += "<tr>";
                html += "<td>" + it.key() + "</td>";
                html += "<td style='text-align: right;'>" + QString::number(it.value(), 'f', 2) + " €</td>";
                html += "<td style='text-align: right;'>" + QString::number(pourcentage, 'f', 1) + "%</td>";
                html += "<td style='text-align: center;' class='" + tendanceClass + "'>" + tendanceIcon + "</td>";
                html += "</tr>";
            }
        }
        html += "</tbody>";
        html += "</table>";
        html += "</div>";
    }

    // Si on veut tous les comptes, ajouter le tableau des comptes
    if (tousLesComptes) {
        html += "<div class='section page-break'>";
        html += genererHTMLTableauComptes();
        html += "</div>";

        html += "<div class='section'>";
        html += genererHTMLTableauCategories(mois, annee);
        html += "</div>";
    }

    // Recommandations
    html += "<div class='section'>";
    html += "<h2>💡 Analyse Intelligente & Recommandations</h2>";
    html += "<div class='stat-box'>";

    // Convertir les recommandations en HTML
    QString recommandationsHTML = m_recommandations;
    recommandationsHTML.replace("\n\n", "</p><p style='margin-top: 10px;'>");
    recommandationsHTML.replace("\n", "<br>");

    // Remplacer les emojis par des versions HTML-safe
    recommandationsHTML.replace("⚠️", "⚠");
    recommandationsHTML.replace("✅", "✓");
    recommandationsHTML.replace("💰", "💰");
    recommandationsHTML.replace("📊", "📊");
    recommandationsHTML.replace("🎯", "🎯");
    recommandationsHTML.replace("🎄", "🎄");
    recommandationsHTML.replace("☀️", "☀");
    recommandationsHTML.replace("🌟", "★");
    recommandationsHTML.replace("📈", "↗");
    recommandationsHTML.replace("💡", "💡");
    recommandationsHTML.replace("📋", "📋");
    recommandationsHTML.replace("🔁", "↻");
    recommandationsHTML.replace("💸", "💸");

    html += "<div style='line-height: 1.6;'>";
    html += "<p style='margin-bottom: 15px;'>" + recommandationsHTML + "</p>";
    html += "</div>";

    html += "</div>";
    html += "</div>";

    // Résumé final
    html += "<div class='section'>";
    html += "<h2>📋 Résumé Exécutif</h2>";
    html += "<div class='info-box'>";
    html += "<p><strong>Situation financière :</strong> ";
    if (m_statistiques.solde >= 0) {
        html += "<span class='positive'>POSITIVE</span> - Votre situation financière est saine.";
    } else {
        html += "<span class='negative'>NÉGATIVE</span> - Des ajustements sont nécessaires.";
    }
    html += "</p>";

    html += "<p><strong>Recommandation principale :</strong> ";
    if (m_statistiques.solde > m_statistiques.totalRevenus * 0.15) {
        html += "Continuez votre excellente gestion des dépenses.";
    } else if (m_statistiques.solde >= 0) {
        html += "Maintenez votre équilibre financier actuel.";
    } else {
        html += "Réduisez vos dépenses non essentielles de 15% minimum.";
    }
    html += "</p>";

    html += "<p><strong>Objectif pour le prochain mois :</strong> ";
    double objectif = m_statistiques.solde >= 0 ? m_statistiques.solde * 1.1 : 0;
    html += "Atteindre un solde de <strong>" + QString::number(objectif, 'f', 2) + " €</strong>.";
    html += "</p>";
    html += "</div>";
    html += "</div>";

    // Pied de page
    html += "<div class='footer'>";
    html += "<p>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</p>";
    html += "<p><strong>Rapport généré par QFinance</strong> - Application de Gestion Financière Personnelle</p>";
    html += "<p>© " + QString::number(QDate::currentDate().year()) + " - Pour usage personnel uniquement</p>";
    html += "</div>";

    html += "</body>";
    html += "</html>";

    return html;
}

QString DashboardManager::genererHTMLTableauComptes()
{
    QString html;
    html += "<h2>🏦 État Global des Comptes</h2>";
    html += "<table>";
    html += "<thead>";
    html += "<tr><th>Nom du compte</th><th>Type</th><th style='text-align: right;'>Solde actuel</th><th style='text-align: center;'>Nbr opérations</th><th style='text-align: center;'>Statut</th></tr>";
    html += "</thead>";
    html += "<tbody>";

    double totalSolde = 0;
    int totalOperations = 0;

    for (Compte* compte : m_comptes) {
        // Calculer le solde du compte
        double solde = CompteRepository::calculerSolde(compte->getId());
        totalSolde += solde;

        // Compter le nombre d'opérations
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM Operation WHERE compte_id = :compte_id");
        query.bindValue(":compte_id", compte->getId());
        int nombreOperations = 0;
        if (query.exec() && query.next()) {
            nombreOperations = query.value(0).toInt();
        }

        // Ajouter les transferts
        query.prepare("SELECT COUNT(*) FROM Transfert WHERE source_id = :compte_id OR destination_id = :compte_id");
        query.bindValue(":compte_id", compte->getId());
        if (query.exec() && query.next()) {
            nombreOperations += query.value(0).toInt();
        }

        totalOperations += nombreOperations;

        // Déterminer le statut
        QString statut = "";
        QString statutClass = "";
        if (solde >= 0) {
            statut = "✓ Actif";
            statutClass = "positive";
        } else {
            statut = "⚠ Déficitaire";
            statutClass = "negative";
        }

        html += "<tr>";
        html += "<td><strong>" + compte->getNom() + "</strong></td>";
        html += "<td>" + QString(compte->estCourant() ? "Courant" : "Épargne") + "</td>";
        QString classe = (solde >= 0 ? "positive" : "negative");
        html += "<td style='text-align: right;' class='" + classe + "'><strong>" +
                QString::number(solde, 'f', 2) + " €</strong></td>";
        html += "<td style='text-align: center;'>" + QString::number(nombreOperations) + "</td>";
        html += "<td style='text-align: center;' class='" + statutClass + "'>" + statut + "</td>";
        html += "</tr>";
    }

    // Ligne de total
    html += "<tr style='background-color: #e9ecef; font-weight: bold;'>";
    html += "<td colspan='2'><strong>TOTAL</strong></td>";
    html += "<td style='text-align: right;' class='" +
            QString(totalSolde >= 0 ? "positive" : "negative") + "'>" +
            QString::number(totalSolde, 'f', 2) + " €</td>";
    html += "<td style='text-align: center;'>" + QString::number(totalOperations) + "</td>";
    html += "<td style='text-align: center;'>-</td>";
    html += "</tr>";

    html += "</tbody>";
    html += "</table>";

    return html;
}

QString DashboardManager::genererHTMLTableauCategories(int mois, int annee)
{
    QString html;
    html += "<h2>📋 Suivi des Budgets par Catégorie</h2>";
    html += "<table>";
    html += "<thead>";
    html += "<tr><th>Catégorie</th><th style='text-align: right;'>Budget</th><th style='text-align: right;'>Dépensé</th><th style='text-align: right;'>Restant</th><th style='text-align: center;'>État</th><th style='text-align: center;'>% Utilisé</th></tr>";
    html += "</thead>";
    html += "<tbody>";

    int categoriesAvecBudget = 0;
    int categoriesDepassees = 0;
    double totalBudget = 0;
    double totalDepense = 0;

    for (Categorie* categorie : m_categories) {
        if (categorie->getParent()) continue; // Ignorer les sous-catégories

        double budget = BudgetRepository::obtenirBudget(categorie->getId(), mois, annee);
        double depenses = BudgetRepository::calculerDepensesCategorie(categorie->getId(), mois, annee);
        double restant = budget - depenses;

        if (budget >= 0) {
            categoriesAvecBudget++;
            totalBudget += budget;
            totalDepense += depenses;

            if (depenses > budget) {
                categoriesDepassees++;
            }
        }

        html += "<tr>";
        html += "<td>" + categorie->getNom() + "</td>";

        if (budget >= 0) {
            html += "<td style='text-align: right;'>" + QString::number(budget, 'f', 2) + " €</td>";
            html += "<td style='text-align: right;'>" + QString::number(depenses, 'f', 2) + " €</td>";

            QString classeRestant = restant >= 0 ? "positive" : "negative";
            html += "<td style='text-align: right;' class='" + classeRestant + "'>" +
                    QString::number(restant, 'f', 2) + " €</td>";

            // État et pourcentage
            if (budget > 0) {
                double pourcentageUtilise = (depenses / budget) * 100;

                QString etat = "";
                QString etatClass = "";
                QString barreProgression = "";

                if (pourcentageUtilise > 100) {
                    etat = "Dépassé";
                    etatClass = "negative";
                } else if (pourcentageUtilise > 80) {
                    etat = "Risque";
                    etatClass = "warning";
                } else {
                    etat = "OK";
                    etatClass = "positive";
                }

                // Barre de progression simple
                int barreWidth = qMin(100, (int)pourcentageUtilise);
                QString barreColor = pourcentageUtilise > 100 ? "#dc3545" :
                                         (pourcentageUtilise > 80 ? "#ffc107" : "#28a745");

                barreProgression = QString("<div style='width: 60px; height: 8px; background-color: #e9ecef; border-radius: 4px; margin: 0 auto;'>"
                                           "<div style='width: %1%; height: 100%; background-color: %2; border-radius: 4px;'></div></div>")
                                       .arg(barreWidth).arg(barreColor);

                html += "<td style='text-align: center;' class='" + etatClass + "'>" + etat + "</td>";
                html += "<td style='text-align: center;'><div style='font-size: 10pt;'>" +
                        QString::number(pourcentageUtilise, 'f', 0) + "%</div>" +
                        barreProgression + "</td>";
            } else {
                html += "<td style='text-align: center;'>-</td>";
                html += "<td style='text-align: center;'>-</td>";
            }
        } else {
            html += "<td style='text-align: center;'>-</td>";
            html += "<td style='text-align: center;'>-</td>";
            html += "<td style='text-align: center;'>-</td>";
            html += "<td style='text-align: center;'>Sans budget</td>";
            html += "<td style='text-align: center;'>-</td>";
        }

        html += "</tr>";
    }

    // Résumé des budgets
    if (categoriesAvecBudget > 0) {
        html += "<tr style='background-color: #e9ecef; font-weight: bold;'>";
        html += "<td><strong>RÉSUMÉ BUDGETS</strong></td>";
        html += "<td style='text-align: right;'>" + QString::number(totalBudget, 'f', 2) + " €</td>";
        html += "<td style='text-align: right;'>" + QString::number(totalDepense, 'f', 2) + " €</td>";

        double totalRestant = totalBudget - totalDepense;
        QString classeTotalRestant = totalRestant >= 0 ? "positive" : "negative";
        html += "<td style='text-align: right;' class='" + classeTotalRestant + "'>" +
                QString::number(totalRestant, 'f', 2) + " €</td>";

        double pourcentageTotalUtilise = totalBudget > 0 ? (totalDepense / totalBudget) * 100 : 0;
        html += "<td style='text-align: center;'>" +
                QString::number(categoriesAvecBudget) + " budgets</td>";
        html += "<td style='text-align: center;'><strong>" +
                QString::number(pourcentageTotalUtilise, 'f', 0) + "%</strong></td>";
        html += "</tr>";
    }

    html += "</tbody>";
    html += "</table>";

    // Note sur les budgets
    if (categoriesAvecBudget > 0) {
        html += "<div class='info-box' style='margin-top: 20px;'>";
        html += "<p><strong>Analyse budgets :</strong> " +
                QString::number(categoriesAvecBudget) + " catégories budgétisées sur " +
                QString::number(m_categories.size()) + " (" +
                QString::number(categoriesDepassees) + " budgets dépassés)</p>";
        html += "</div>";
    }

    return html;
}

bool DashboardManager::exporterPDF(const QString &cheminFichier,
                                   int mois,
                                   int annee,
                                   const QString &filtreCompte,
                                   const QString &filtreCategorie,
                                   bool tousLesComptes)
{
    // Calculer les statistiques avant l'export
    calculerStatistiques(mois, annee, filtreCompte, filtreCategorie);

    // Générer les recommandations
    m_recommandations = genererRecommandations(mois, annee, filtreCompte);

    // Capturer les diagrammes actuels en images
    QString imageDepensesPath = captureDiagrammeEnImage(m_chartDepenses, "depenses");
    QString imageRevenusPath = captureDiagrammeEnImage(m_chartRevenusVsDepenses, "revenus");
    QString imageEvolutionPath = captureDiagrammeEnImage(m_chartEvolution, "evolution");
    QString imageRepartitionPath = captureDiagrammeEnImage(m_chartRepartition, "repartition");

    // Générer le contenu HTML avec les images
    QString htmlContent = genererHTMLStatistiques(mois, annee, filtreCompte, filtreCategorie,
                                                  tousLesComptes,
                                                  imageDepensesPath,
                                                  imageRevenusPath,
                                                  imageEvolutionPath,
                                                  imageRepartitionPath);

    // Créer un document HTML
    QTextDocument document;
    document.setHtml(htmlContent);

    // Configurer l'imprimante pour PDF
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);

    // Augmenter la résolution pour une meilleure qualité
    printer.setResolution(150);

    // Configurer les marges
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);

    // Exporter en PDF avec peinture manuelle pour contrôler la mise en page
    QPainter painter(&printer);

    // Rendre le document pour le PDF
    document.drawContents(&painter);

    painter.end();

    // Nettoyer les images temporaires
    if (!imageDepensesPath.isEmpty()) QFile::remove(imageDepensesPath);
    if (!imageRevenusPath.isEmpty()) QFile::remove(imageRevenusPath);
    if (!imageEvolutionPath.isEmpty()) QFile::remove(imageEvolutionPath);
    if (!imageRepartitionPath.isEmpty()) QFile::remove(imageRepartitionPath);

    qDebug() << "PDF exporté avec succès vers:" << cheminFichier;
    return true;
}
