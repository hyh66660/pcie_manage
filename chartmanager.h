#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

QT_CHARTS_USE_NAMESPACE

class chartmanager : public QWidget
{
    Q_OBJECT

public:
    explicit chartmanager(QWidget *parent = nullptr);
    ~chartmanager();

    void addDataToUnpackRx(int time, unsigned int unpackRx);
    void addDataToGroupRx(int time, unsigned int groupRx);
    void addDataToUnpackTx(int time, unsigned int unpackTx);
    void addDataToGroupTx(int time, unsigned int groupTx);

    void showUnpackRxChart();
    void showGroupRxChart();
    void showUnpackTxChart();
    void showGroupTxChart();

    QChart *chart;
    QLineSeries *lineSeriesUnpackRx;
    QLineSeries *lineSeriesGroupRx;
    QLineSeries *lineSeriesUnpackTx;
    QLineSeries *lineSeriesGroupTx;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QChartView *chartView;

    void setupChart();
};

#endif // CHARTMANAGER_H
