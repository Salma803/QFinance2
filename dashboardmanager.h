#ifndef DASHBOARDMANAGER_H
#define DASHBOARDMANAGER_H

#include <QObject>
#include <QtCharts/QChartView>
#include <QList>

class Categorie;
class Compte;

class DashboardManager : public QObject
{
    Q_OBJECT

public:
    struct Statistiques {
        double totalRevenus;
        double totalDepenses;
        double totalTransferts;
        double solde;
        double depensesParJour;
        QMap<QString, double> depensesParCategorie;
        QMap<QString, double> evolutionMensuelle;
    };

    explicit DashboardManager(QObject *parent = nullptr);
    ~DashboardManager();

    void initialiserCharts(QChartView *chartDepenses,
                           QChartView *chartRevenusVsDepenses,
                           QChartView *chartEvolution,
                           QChartView *chartRepartition);

    void setCategories(const QList<Categorie*> &categories);
    void setComptes(const QList<Compte*> &comptes);

    void actualiserDashboard(int mois, int annee,
                             const QList<Categorie*> &categories,
                             const QList<Compte*> &comptes,
                             const QString &filtreCategorie,
                             const QString &filtreCompte);

    Statistiques getStatistiques() const;
    QString getRecommandations() const;

signals:
    void dashboardActualise();

private:
    void calculerStatistiques(int mois, int annee,
                              const QString &filtreCompte,
                              const QString &filtreCategorie);

    QString genererRecommandations(int mois, int annee, const QString &filtreCompte);

    void creerChartDepensesParCategorie(int mois, int annee,
                                        const QString &filtreCompte,
                                        const QString &filtreCategorie);

    void creerChartRevenusVsDepenses(int mois, int annee,
                                     const QString &filtreCompte);

    void creerChartEvolution(int annee,
                             const QString &filtreCategorie,
                             const QString &filtreCompte);

    void creerChartRepartition(int mois, int annee,
                               const QString &filtreCompte,
                               const QString &filtreCategorie);

    // Méthodes pour les transferts
    double getTotalTransferts(int mois, int annee);
    double getTotalTransfertsCompte(int mois, int annee, const QString &compteId);
    double getTransfertsSortants(int mois, int annee, const QString &compteId = QString());

private:
    QChartView *m_chartDepenses;
    QChartView *m_chartRevenusVsDepenses;
    QChartView *m_chartEvolution;
    QChartView *m_chartRepartition;

    QList<Categorie*> m_categories;
    QList<Compte*> m_comptes;

    Statistiques m_statistiques;
    QString m_recommandations;
};

#endif // DASHBOARDMANAGER_H
