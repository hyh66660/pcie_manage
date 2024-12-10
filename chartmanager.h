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

    void addDataToUnpackRx(int time, unsigned int unpackRx, QString name);


    QChart *chart;
    QLineSeries *lineSeriesUnpackRx;

    QValueAxis *axisX;
    QValueAxis *axisY;
    QChartView *chartView;

    void setupChart();
};

#endif // CHARTMANAGER_H
