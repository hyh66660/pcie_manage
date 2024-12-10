#include "chartmanager.h"
#include <QDebug>

chartmanager::chartmanager(QWidget *parent)
    : QWidget(parent),
      chart(new QChart()),
      lineSeriesUnpackRx(new QLineSeries()),
      lineSeriesGroupRx(new QLineSeries()),
      lineSeriesUnpackTx(new QLineSeries()),
      lineSeriesGroupTx(new QLineSeries()),
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
    delete lineSeriesGroupRx;
    delete lineSeriesUnpackTx;
    delete lineSeriesGroupTx;
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

void chartmanager::addDataToUnpackRx(int time, unsigned int UnpackRx)
{
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

void chartmanager::addDataToGroupRx(int time, unsigned int GroupRx)
{
    if(GroupRx < 1000){
        axisY->setTitleText("Speed (Kbps)");
        axisY->setRange(0, 1000);
        lineSeriesGroupRx->append(time, GroupRx);
    }else if(GroupRx >= 1000 && GroupRx < 1000000){
        GroupRx = GroupRx / 1000;
        axisY->setTitleText("Speed (Mbps)");
        axisY->setRange(0, 1000);
        lineSeriesGroupRx->append(time, GroupRx);
    }else {
        GroupRx = GroupRx / 1000000;
        axisY->setTitleText("Speed (Gbps)");
        axisY->setRange(0, 10);
        lineSeriesGroupRx->append(time, GroupRx);
    }
    // If more than 10 seconds, adjust the axis
    if (time > 10) {
        axisX->setRange(time - 10, time);
    }
}

void chartmanager::addDataToUnpackTx(int time, unsigned int UnpackTx)
{
    if(UnpackTx < 1000){
        axisY->setTitleText("Speed (Kbps)");
        axisY->setRange(0, 1000);
        lineSeriesUnpackTx->append(time, UnpackTx);
    }else if(UnpackTx >= 1000 && UnpackTx < 1000000){
        UnpackTx = UnpackTx / 1000;
        axisY->setTitleText("Speed (Mbps)");
        axisY->setRange(0, 1000);
        lineSeriesUnpackTx->append(time, UnpackTx);
    }else {
        UnpackTx = UnpackTx / 1000000;
        axisY->setTitleText("Speed (Gbps)");
        axisY->setRange(0, 10);
        lineSeriesUnpackTx->append(time, UnpackTx);
    }
    // If more than 10 seconds, adjust the axis
    if (time > 10) {
        axisX->setRange(time - 10, time);
    }
}

void chartmanager::addDataToGroupTx(int time, unsigned int GroupTx)
{
    if(GroupTx < 1000){
        axisY->setTitleText("Speed (Kbps)");
        axisY->setRange(0, 1000);
        lineSeriesGroupTx->append(time, GroupTx);
    }else if(GroupTx >= 1000 && GroupTx < 1000000){
        GroupTx = GroupTx / 1000;
        axisY->setTitleText("Speed (Mbps)");
        axisY->setRange(0, 1000);
        lineSeriesGroupTx->append(time, GroupTx);
    }else {
        GroupTx = GroupTx / 1000000;
        axisY->setTitleText("Speed (Gbps)");
        axisY->setRange(0, 10);
        lineSeriesGroupTx->append(time, GroupTx);
    }
    // If more than 10 seconds, adjust the axis
    if (time > 10) {
        axisX->setRange(time - 10, time);
    }
}

void chartmanager::showUnpackRxChart()
{
    if (!chart->series().contains(lineSeriesUnpackRx)) {
        chart->removeAllSeries();
        lineSeriesUnpackRx = new QLineSeries();
        chart->addSeries(lineSeriesUnpackRx);     // 添加 UnpackRx 系列
        chart->setTitle("UnpackRx Speed Over Time");

        chart->setAxisX(axisX, lineSeriesUnpackRx);
        chart->setAxisY(axisY, lineSeriesUnpackRx);
    }

    chartView->update();  // 更新图表视图
}

void chartmanager::showGroupRxChart()
{
    if (!chart->series().contains(lineSeriesGroupRx)) {
        chart->removeAllSeries();
        lineSeriesGroupRx = new QLineSeries();
        chart->addSeries(lineSeriesGroupRx);      // 添加 GroupRx 系列
        chart->setTitle("UnpackTx Speed Over Time");

        chart->setAxisX(axisX, lineSeriesGroupRx);
        chart->setAxisY(axisY, lineSeriesGroupRx);
    }

    chartView->update();  // 更新图表视图
}

void chartmanager::showUnpackTxChart()
{
    if (!chart->series().contains(lineSeriesUnpackTx)) {
        chart->removeAllSeries();
        lineSeriesUnpackTx = new QLineSeries();
        chart->addSeries(lineSeriesUnpackTx);     // 添加 UnpackRx 系列
        chart->setTitle("GroupRx Speed Over Time");

        chart->setAxisX(axisX, lineSeriesUnpackTx);
        chart->setAxisY(axisY, lineSeriesUnpackTx);
    }

    chartView->update();  // 更新图表视图
}

void chartmanager::showGroupTxChart()
{
    if (!chart->series().contains(lineSeriesGroupTx)) {
        chart->removeAllSeries();
        lineSeriesGroupTx = new QLineSeries();
        chart->addSeries(lineSeriesGroupTx);      // 添加 GroupRx 系列
        chart->setTitle("GroupTx Speed Over Time");

        chart->setAxisX(axisX, lineSeriesGroupTx);
        chart->setAxisY(axisY, lineSeriesGroupTx);
    }

    chartView->update();  // 更新图表视图
}
