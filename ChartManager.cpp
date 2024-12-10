#include "chartmanager.h"
#include <QDebug>

chartmanager::chartmanager(QWidget *parent)
    : QWidget(parent),
      chart(new QChart()),
      lineSeriesUnpackRx(new QLineSeries()),
      axisX(new QValueAxis()),
      axisY(new QValueAxis()),
      chartView(new QChartView(chart))
{
    // Set up chart layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);

    setupChart();
}

chartmanager::~chartmanager()
{
    delete chart;
    delete lineSeriesUnpackRx;
    delete axisX;
    delete axisY;
    delete chartView;
}

void chartmanager::setupChart()
{
    // Set up chart properties
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(lineSeriesUnpackRx); // Default to UnpackRx
    chart->setTitle("UnpackRx Speed Over Time");

    // Set X axis (time)
    axisX->setRange(0, 10);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Time (s)");

    // Set Y axis (speed)
    axisY->setRange(0, 1000);
    axisX->setLabelFormat("%u");
    axisY->setTitleText("Speed (Kbps)");

    // Set axes for both series
    chart->setAxisX(axisX, lineSeriesUnpackRx);
    chart->setAxisY(axisY, lineSeriesUnpackRx);
}

void chartmanager::addDataToUnpackRx(int time, unsigned int UnpackRx, QString name)
{
    chart->setTitle(name + " Speed Over Time");
    if(UnpackRx < 1000){
        axisY->setTitleText("Speed (Kbps)");
        axisY->setRange(0, 1000);
        lineSeriesUnpackRx->append(time, UnpackRx);
    }else if(UnpackRx >= 1000 && UnpackRx < 1000000){
        UnpackRx = UnpackRx / 1000;
        axisY->setTitleText("Speed (Mbps)");
        axisY->setRange(0, 1000);
        lineSeriesUnpackRx->append(time, UnpackRx);
    }else {
        UnpackRx = UnpackRx / 1000000;
        axisY->setTitleText("Speed (Gbps)");
        axisY->setRange(0, 10);
        lineSeriesUnpackRx->append(time, UnpackRx);
    }
    // If more than 10 seconds, adjust the axis
    if (time > 10) {
        axisX->setRange(time - 10, time);
    }
}

