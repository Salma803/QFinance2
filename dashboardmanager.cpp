#include "DashboardManager.h"

// Inclure les headers QtCharts et utiliser le namespace
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QDebug>
#include <QColor>

#include "repository/OperationRepository.h"
#include "repository/BudgetRepository.h"


DashboardManager::DashboardManager(QObject *parent)
    : QObject(parent)
    , m_chartDepenses(nullptr)
    , m_chartRevenusVsDepenses(nullptr)
{
}

void DashboardManager::initialiserCharts(QChartView *chartDepenses, QChartView *chartRevenusVsDepenses)
{
    m_chartDepenses = chartDepenses;
    m_chartRevenusVsDepenses = chartRevenusVsDepenses;
}

void DashboardManager::setCategories(const QList<Categorie*> &categories)
{
    m_categories = categories;
}

void DashboardManager::actualiserDashboard(int mois, int annee, const QList<Categorie*> &categories)
{
    if (categories.isEmpty() && !m_categories.isEmpty()) {
        // Utiliser les catégories existantes si aucune nouvelle n'est fournie
        creerChartDepensesParCategorie(mois, annee);
        creerChartRevenusVsDepenses(mois, annee);
    } else if (!categories.isEmpty()) {
        m_categories = categories;
        creerChartDepensesParCategorie(mois, annee);
        creerChartRevenusVsDepenses(mois, annee);
    }

    emit dashboardActualise();
}

void DashboardManager::creerChartDepensesParCategorie(int mois, int annee)
{
    if (!m_chartDepenses) return;

    // Créer une nouvelle série de barres
    QBarSeries *series = new QBarSeries();

    // Pour chaque catégorie, calculer le total des dépenses
    for (Categorie* categorie : m_categories) {
        // Ne prendre que les catégories sans parent (catégories principales)
        if (categorie->getParent()) continue;

        // Calculer le total des dépenses pour cette catégorie et ses enfants
        double totalDepenses = 0;

        // Fonction récursive pour sommer les dépenses d'une catégorie et ses enfants
        std::function<void(Categorie*, double&)> sommerDepensesCategorie =
            [&](Categorie* cat, double& total) {
                // Dépenses de la catégorie courante
                total += OperationRepository::getTotalDepensesCategorie(
                    cat->getId(), mois, annee);

                // Dépenses des catégories enfants
                for (Categorie* enfant : cat->getEnfants()) {
                    sommerDepensesCategorie(enfant, total);
                }
            };

        sommerDepensesCategorie(categorie, totalDepenses);

        if (totalDepenses > 0) {
            QBarSet *set = new QBarSet(categorie->getNom());
            *set << totalDepenses;
            series->append(set);
        }
    }

    if (series->count() == 0) {
        delete series;
        // Créer un chart vide avec un message
        QChart *chart = new QChart();
        chart->setTitle("Aucune dépense pour cette période");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        m_chartDepenses->setChart(chart);
        return;
    }

    // Créer le chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Dépenses par catégorie");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Créer les axes
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.2f €");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Configurer la légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartDepenses->setChart(chart);
}

void DashboardManager::creerChartRevenusVsDepenses(int mois, int annee)
{
    if (!m_chartRevenusVsDepenses) return;

    // Calculer les totaux
    double totalRevenus = OperationRepository::getTotalRevenus(mois, annee);
    double totalDepenses = OperationRepository::getTotalDepenses(mois, annee);

    // Créer une série de barres
    QBarSeries *series = new QBarSeries();

    // Barre des revenus
    QBarSet *setRevenus = new QBarSet("Revenus");
    *setRevenus << totalRevenus;
    setRevenus->setColor(QColor(46, 204, 113)); // Vert

    // Barre des dépenses
    QBarSet *setDepenses = new QBarSet("Dépenses");
    *setDepenses << totalDepenses;
    setDepenses->setColor(QColor(231, 76, 60)); // Rouge

    series->append(setRevenus);
    series->append(setDepenses);

    // Créer le chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Revenus vs Dépenses");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Créer les axes
    QStringList categories;
    categories << "Montants";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.2f €");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Configurer la légende
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartRevenusVsDepenses->setChart(chart);

    // Ajouter le solde comme annotation
    double solde = totalRevenus - totalDepenses;
    chart->setTitle(QString("Revenus vs Dépenses (Solde: %1 €)")
                        .arg(solde, 0, 'f', 2));
}
