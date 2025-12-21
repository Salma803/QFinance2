#ifndef DASHBOARDMANAGER_H
#define DASHBOARDMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QMap>
#include <QSize>

#include "model/Categorie.h"
#include "model/Compte.h"

class QChartView;
class QChart;

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
        double transfertsEntrants;
        double transfertsSortants;
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

    bool exporterPDF(const QString &cheminFichier,
                     int mois,
                     int annee,
                     const QString &filtreCompte,
                     const QString &filtreCategorie,
                     bool tousLesComptes);

    Statistiques getStatistiques() const;
    QString getRecommandations() const;

    // Méthodes publiques pour obtenir les statistiques calculées
    double getTransfertsEntrants(int mois, int annee, const QString &compteId);

signals:
    void dashboardActualise(const DashboardManager::Statistiques& stats,
                            const QString& recommandations);

private:
    void calculerStatistiques(int mois, int annee,
                              const QString &filtreCompte,
                              const QString &filtreCategorie);

    QString genererRecommandations(int mois, int annee, const QString &filtreCompte);

    void creerChartDepensesParCategorie(int mois, int annee,
                                        const QString &filtreCompte,
                                        const QString &filtreCategorie);

    void creerChartRevenusVsDepenses(int mois, int annee, const QString &filtreCompte);
    void creerChartEvolution(int annee, const QString &filtreCategorie, const QString &filtreCompte);
    void creerChartRepartition(int mois, int annee,
                               const QString &filtreCompte,
                               const QString &filtreCategorie);

    double getTotalTransferts(int mois, int annee);
    double getTotalTransfertsCompte(int mois, int annee, const QString &compteId);
    double getTransfertsSortants(int mois, int annee, const QString &compteId);
    double getTransfertsEntrantsPrive(int mois, int annee, const QString &compteId);

    QString captureDiagrammeEnImage(QChartView* chartView, const QString &nom, const QSize &taille = QSize(800, 500));

    QString genererHTMLStatistiques(int mois, int annee,
                                    const QString &filtreCompte,
                                    const QString &filtreCategorie,
                                    bool tousLesComptes);

    QString genererHTMLStatistiques(int mois, int annee,
                                    const QString &filtreCompte,
                                    const QString &filtreCategorie,
                                    bool tousLesComptes,
                                    const QString &imageDepensesPath,
                                    const QString &imageRevenusPath,
                                    const QString &imageEvolutionPath,
                                    const QString &imageRepartitionPath);

    QString genererHTMLTableauComptes();
    QString genererHTMLTableauCategories(int mois, int annee);

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
