#ifndef DASHBOARDMANAGER_H
#define DASHBOARDMANAGER_H

#include <QObject>
#include <QList>
#include "model/Categorie.h"

// Déclaration anticipée des classes QtCharts
QT_BEGIN_NAMESPACE
class QChart;
class QChartView;
class QBarSet;
class QBarSeries;
class QPieSeries;
class QBarCategoryAxis;
class QValueAxis;
QT_END_NAMESPACE

class DashboardManager : public QObject
{
    Q_OBJECT

public:
    explicit DashboardManager(QObject *parent = nullptr);

    void initialiserCharts(QChartView *chartDepenses, QChartView *chartRevenusVsDepenses);
    void actualiserDashboard(int mois, int annee, const QList<Categorie*> &categories);

    void setCategories(const QList<Categorie*> &categories);

signals:
    void dashboardActualise();

private:
    void creerChartDepensesParCategorie(int mois, int annee);
    void creerChartRevenusVsDepenses(int mois, int annee);

    QChartView *m_chartDepenses;
    QChartView *m_chartRevenusVsDepenses;
    QList<Categorie*> m_categories;
};

#endif // DASHBOARDMANAGER_H
