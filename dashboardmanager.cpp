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

#include "repository/OperationRepository.h"
#include "repository/BudgetRepository.h"
#include "repository/CompteRepository.h"
#include "repository/TransfertRepository.h"
#include "model/Categorie.h"

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
