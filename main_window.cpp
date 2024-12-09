﻿#include "main_window.h"
#include <QHBoxLayout>
#include <QMetaObject>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QRegExp>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentIndex(0), currentLineEdit(nullptr)
{
    setWindowTitle(tr("东北大学万兆以太网拆组包引擎"));

    QWidget *pCentralWidget = new QWidget;
    setCentralWidget(pCentralWidget);

    pCentralWidget->setObjectName("centerwidget");
    pCentralWidget->setStyleSheet("#centerwidget{background:#888;border:1px solid #000;}");



    // 初始化 UI 和布局
    pUserUi = new user_ui;
    QHBoxLayout *setUserLayout = new QHBoxLayout(pCentralWidget);
    setUserLayout->setMargin(1);
    setUserLayout->setSpacing(0);
    setUserLayout->addWidget(pUserUi);


    // 信号槽连接
    resize(1200, 500);
    QObject::connect(pUserUi->pButStart, &QPushButton::clicked, this, &MainWindow::app_error);

    // 初始化 DMA 程序
    // 新建xdma读写实例
    pXdma = new xdma_programe;
    if(pXdma->getDevice())
    //if(1)
    {
        pUserUi->pButStart->hide();
    }
    else
    {
        pUserUi->pButStart->setText(tr("设备未就绪"));
        pUserUi->ChartManager->hide();
        pUserUi->pPcieManage->hide();
        pUserUi->btnShowGroupRx->hide();
        pUserUi->btnShowUnpackRx->hide();
        pUserUi->btnShowGroupTx->hide();
        pUserUi->btnShowUnpackTx->hide();
    }

    dataBuffLen = 1024;
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    //pDataBuff = (char *)_aligned_malloc(dataBuffLen, sys_info.dwPageSize);
    //size_t dataBuffLen = 1024;  // 每个缓冲区的大小

    // 分配多个缓冲区
    for (int i = 0; i < 11; ++i) {
        pDataBuff[i] = (char*)_aligned_malloc(dataBuffLen, sys_info.dwPageSize);
    }
    for (int i = 0; i < 3; ++i) {
        pDataBuffw[i] = (char*)_aligned_malloc(dataBuffLen, sys_info.dwPageSize);
    }

    // 初始化定时器
    pTimer = new QTimer;
    QObject::connect(pTimer, &QTimer::timeout, this, &MainWindow::read_next_addresses);
    QObject::connect(pXdma, SIGNAL(opt_end(int)), this, SLOT(opt_end(int)));

    QObject::connect(pUserUi->btnShowUnpackRx, &QPushButton::clicked, pUserUi->ChartManager, &chartmanager::showUnpackRxChart);
    QObject::connect(pUserUi->btnShowGroupRx, &QPushButton::clicked, pUserUi->ChartManager, &chartmanager::showGroupRxChart);
    QObject::connect(pUserUi->btnShowUnpackTx, &QPushButton::clicked, pUserUi->ChartManager, &chartmanager::showUnpackTxChart);
    QObject::connect(pUserUi->btnShowGroupTx, &QPushButton::clicked, pUserUi->ChartManager, &chartmanager::showGroupTxChart);

    QObject::connect(pUserUi->pPcieManage->pButWrite, SIGNAL(clicked(bool)), this, SLOT(pcie_opt()));

    // 初始化地址和目标组件
    initializeAddressAndTargets();

    // 开启定时器和 DMA 线程
    pTimer->start();
    pXdma->start();


}

MainWindow::~MainWindow()
{
    for (int i = 0; i < 11; ++i) {
        if (pDataBuff[i]) {
            _aligned_free(pDataBuff[i]);
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (pDataBuffw[i]) {
            _aligned_free(pDataBuffw[i]);
        }
    }
}

void MainWindow::initializeAddressAndTargets()
{
    // 设置要读取的地址
    addresses = {0x0004, 0x0008, 0x0010, 0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C};

    // 设置与之对应的目标 LineEdit
    targetLineEdits = {
        pUserUi->pPcieManage->pLineEditMtu,
        pUserUi->pPcieManage->pLineEditSrcMac,
        pUserUi->pPcieManage->pLineEditDestMac,
        pUserUi->pPcieManage->pLineEditUnpackRxSpeed,
        pUserUi->pPcieManage->pLineEditUnpackTxSpeed,
        pUserUi->pPcieManage->pLineEditGroupRxSpeed,
        pUserUi->pPcieManage->pLineEditGroupTxSpeed,
        pUserUi->pPcieManage->pLineEditUnpackMacSrc,
        pUserUi->pPcieManage->pLineEditUnpackMacDst,
        pUserUi->pPcieManage->pLineEditGroupMacSrc,
        pUserUi->pPcieManage->pLineEditGroupMacDst
    };

    // 确保地址和目标组件数量一致
    Q_ASSERT(targetLineEdits.size() == addresses.size());
}

void MainWindow::app_error()
{
    // 错误处理代码，暂时不做任何操作
}

void MainWindow::read_next_addresses()
{
    if (addresses.isEmpty() || targetLineEdits.isEmpty())
        return;

    // 停止定时器，避免干扰当前操作
    pTimer->stop();

    // 设置当前操作的地址和目标组件
    currentAddr = addresses[currentIndex];
    currentLineEdit = targetLineEdits[currentIndex];

    // 配置 DMA 读取操作
    if(currentIndex==0){
        opt_mode = 1;
        opt_len = 2;
    }else if(currentIndex==1 || currentIndex==2){
        opt_mode = 1;
        opt_len = 6;
    }else {
        opt_mode = 6;
        opt_len = 4;
    }
    pXdma->opt_pack(opt_mode, pDataBuff[currentIndex], opt_len, currentAddr);


}

void MainWindow::processMacAddress(const QString& macStr, unsigned char* macBytes)
{
    // 去除冒号和空格等非十六进制字符
    QString cleanMac = macStr;
    cleanMac.remove(QRegExp("[:\\s]"));

    // 确保输入的 MAC 地址长度为 12（6 字节 * 2 个字符）
    if (cleanMac.length() != 12) {
        qWarning() << "Invalid MAC address length!";
        return;
    }

    // 将每对十六进制字符转换为字节
    for (int i = 0; i < 6; ++i) {
        bool ok;
        macBytes[i] = cleanMac.mid(2 * i, 2).toUInt(&ok, 16);  // 每两位字符转为一个字节
        if (!ok) {
            qWarning() << "Invalid MAC address format!";
            return;
        }
        //qDebug() << QString::number(macBytes[i], 16).rightJustified(2, '0').toUpper();  // 打印十六进制格式
    }
}

void MainWindow::pcie_opt()
{
    pTimer->stop();
    unsigned int addr[3]={0x002C, 0x0030, 0x0038};
    QPushButton *but = qobject_cast<QPushButton *>(sender());

    if(but == pUserUi->pPcieManage->pButWrite)
    {
        opt_mode = 4;
        opt_len = 0;

        QString textDec = pUserUi->pPcieManage->pLineEditMtuW->text();
        if(textDec != ""){
            bool ok = false;
            int value = textDec.toInt(&ok, 10);
            QString textHex = QString::number(value, 16).rightJustified(4, '0').toUpper();
            QByteArray ba0;
            ba0 = textHex.toLocal8Bit();
            opt_len = getHexFromText(ba0.data(), ba0.length(), (unsigned char *)pDataBuffw[0], dataBuffLen);
            if (opt_len == 2) {
                std::swap(pDataBuffw[0][0], pDataBuffw[0][1]);
            }
            if(opt_len > 0){
                qDebug() << "DMA写入: " << QByteArray::fromRawData(pDataBuffw[0], opt_len).toHex();
                pXdma->opt_pack(opt_mode, pDataBuffw[0], opt_len, addr[0]);
                opt_len = 0;
            }
        }

        QString macSrc = pUserUi->pPcieManage->pLineEditSrcMacW->text();
        if(macSrc != ""){
            QByteArray ba1;
            ba1 = pUserUi->pPcieManage->pLineEditSrcMacW->text().toLocal8Bit();
            opt_len = getHexFromText(ba1.data(), ba1.length(), (unsigned char *)pDataBuffw[1], dataBuffLen);
            if(opt_len > 0){
                pXdma->opt_pack(opt_mode, pDataBuffw[1], opt_len, addr[1]);
                opt_len = 0;
            }
        }

        QString macDst = pUserUi->pPcieManage->pLineEditDestMacW->text();
        if(macDst != ""){
            QByteArray ba2;
            ba2 = pUserUi->pPcieManage->pLineEditDestMacW->text().toLocal8Bit();
            opt_len = getHexFromText(ba2.data(), ba2.length(), (unsigned char *)pDataBuffw[2], dataBuffLen);
            if(opt_len > 0){
                pXdma->opt_pack(opt_mode, pDataBuffw[2], opt_len, addr[2]);
                opt_len = 0;
            }
        }
    }
}

void MainWindow::opt_end(int rw)
{
    // 使用 QTimer::singleShot 确保 UI 更新在主线程中
    QTimer::singleShot(0, this, [=] {
        if (rw == 0 && currentLineEdit)
        {
            // 根据 currentIndex 选择不同的处理函数
            switch (currentIndex)
            {
            case 0: // MTU
                processMtuData();
                break;
            case 1: // SrcMac
                processSrcMacData();
                break;
            case 2: // DestMac
                processDestMacData();
                break;
            case 3: // UnpackRx
                processUnpackRxData();
                break;
            case 4: // GroupRx
                processGroupRxData();
                break;
            case 5: // UnpackTx
                processUnpackTxData();
                break;
            case 6: // GroupTx
                processGroupTxData();
                break;
            case 7: // UnpackMacSrc
                processUnpackMacSrcData();
                break;
            case 8: // UnpackMacDst
                processUnpackMacDstData();
                break;
            case 9: // GroupMacSrc
                processGroupMacSrcData();
                break;
            case 10: // GroupMacDst
                processGroupMacDstData();
                break;
            default:
                break;
            }
            // 更新索引
            currentIndex = (currentIndex + 1) % addresses.size();
        }
        else if (currentLineEdit)
        {
            //currentLineEdit->setText(tr("读取失败"));
        }

        // 重启定时器
        pTimer->start(90);
    });
}


void MainWindow::processMtuData()
{
    // 假设 MTU 数据是一个整数值
    //getTextFromHex((unsigned char *)pDataBuff[0], 2, strBuff);
    //qDebug() << "DMA读取: " << QByteArray::fromRawData(pDataBuff[0], 2).toHex();
    unsigned int mtu = *(unsigned int*)pDataBuff[0];
    QString mtuData = QString::number(mtu);
    currentLineEdit->setText(mtuData);
}

void MainWindow::processSrcMacData()
{
    getTextFromHex((unsigned char *)pDataBuff[1], 6, strBuff[0]);
    currentLineEdit->setText(strBuff[0]);
}

void MainWindow::processDestMacData()
{
    getTextFromHex((unsigned char *)pDataBuff[2], 6, strBuff[1]);
    currentLineEdit->setText(strBuff[1]);
}

void MainWindow::processUnpackRxData()
{
    // 假设 UnpackRx 是一个整数值
    unsigned int UnpackRx = *(unsigned int*)pDataBuff[3];
    UnpackRx = UnpackRx / 10 * 8 / 100;
    unsigned int UnpackRxchart = UnpackRx;
    if(UnpackRx < 1000){
        QString speed = QString::number(UnpackRx);
        currentLineEdit->setText(speed+"Kbps");
    }else if(UnpackRx >= 1000 && UnpackRx < 1000000){
        UnpackRx = UnpackRx / 1000;
        QString speed = QString::number(UnpackRx);
        currentLineEdit->setText(speed+"Mbps");
    }else {
        UnpackRx = UnpackRx / 1000000;
        QString speed = QString::number(UnpackRx);
        currentLineEdit->setText(speed+"Gbps");
    }

    if (pUserUi->ChartManager->chart->series().contains(pUserUi->ChartManager->lineSeriesUnpackRx)) {
        pUserUi->ChartManager->addDataToUnpackRx(timeCounter, UnpackRxchart);
        ++timeCounter;
    }
}

void MainWindow::processGroupRxData()
{
    // 假设 GroupRx 是一个整数值
    unsigned int GroupRx = *(unsigned int*)pDataBuff[4];
    GroupRx = GroupRx / 10 * 8 / 100;
    unsigned int GroupRxchart = GroupRx;
    if(GroupRx < 1000){
        QString speed = QString::number(GroupRx);
        currentLineEdit->setText(speed+"Kbps");
    }else if(GroupRx >= 1000 && GroupRx < 1000000){
        GroupRx = GroupRx / 1000;
        QString speed = QString::number(GroupRx);
        currentLineEdit->setText(speed+"Mbps");
    }else {
        GroupRx = GroupRx / 1000000;
        QString speed = QString::number(GroupRx);
        currentLineEdit->setText(speed+"Gbps");
    }
    if (pUserUi->ChartManager->chart->series().contains(pUserUi->ChartManager->lineSeriesGroupRx)) {
        pUserUi->ChartManager->addDataToGroupRx(timeCounter, GroupRxchart);
        ++timeCounter;
    }

}

void MainWindow::processUnpackTxData()
{
    // 假设 UnpackTx 是一个整数值
    unsigned int UnpackTx = *(unsigned int*)pDataBuff[5];
    UnpackTx = UnpackTx / 10 * 8 / 100;
    unsigned int UnpackTxchart = UnpackTx;
    if(UnpackTx < 1000){
        QString speed = QString::number(UnpackTx);
        currentLineEdit->setText(speed+"Kbps");
    }else if(UnpackTx >= 1000 && UnpackTx < 1000000){
        UnpackTx = UnpackTx / 1000;
        QString speed = QString::number(UnpackTx);
        currentLineEdit->setText(speed+"Mbps");
    }else {
        UnpackTx = UnpackTx / 1000000;
        QString speed = QString::number(UnpackTx);
        currentLineEdit->setText(speed+"Gbps");
    }
    if (pUserUi->ChartManager->chart->series().contains(pUserUi->ChartManager->lineSeriesUnpackTx)) {
        pUserUi->ChartManager->addDataToUnpackTx(timeCounter, UnpackTxchart);
        ++timeCounter;
    }

}

void MainWindow::processGroupTxData()
{
    // 假设 GroupTx 是一个整数值
    unsigned int GroupTx = *(unsigned int*)pDataBuff[6];
    GroupTx = GroupTx / 10 * 8 / 100;
    unsigned int GroupTxchart = GroupTx;
    if(GroupTx < 1000){
        QString speed = QString::number(GroupTx);
        currentLineEdit->setText(speed+"Kbps");
    }else if(GroupTx >= 1000 && GroupTx < 1000000){
        GroupTx = GroupTx / 1000;
        QString speed = QString::number(GroupTx);
        currentLineEdit->setText(speed+"Mbps");
    }else {
        GroupTx = GroupTx / 1000000;
        QString speed = QString::number(GroupTx);
        currentLineEdit->setText(speed+"Gbps");
    }

    if (pUserUi->ChartManager->chart->series().contains(pUserUi->ChartManager->lineSeriesGroupTx)) {
        pUserUi->ChartManager->addDataToGroupTx(timeCounter, GroupTxchart);
        ++timeCounter;
    }

}

void MainWindow::processUnpackMacSrcData()
{
    // 假设 UnpackMacSrc 是一个整数值
    int UnpackMacSrc = *(int*)pDataBuff[7];
    QString mac = QString::number(UnpackMacSrc);
    currentLineEdit->setText(mac);
}

void MainWindow::processUnpackMacDstData()
{
    // 假设 UnpackMacDst 是一个整数值
    int UnpackMacDst = *(int*)pDataBuff[8];
    QString mac = QString::number(UnpackMacDst);
    currentLineEdit->setText(mac);
}

void MainWindow::processGroupMacSrcData()
{
    // 假设 GroupMacSrc 是一个整数值
    int GroupMacSrc = *(int*)pDataBuff[9];
    QString mac = QString::number(GroupMacSrc);
    currentLineEdit->setText(mac);
}

void MainWindow::processGroupMacDstData()
{
    // 假设 GroupMacDst 是一个整数值
    int GroupMacDst = *(int*)pDataBuff[10];
    QString mac = QString::number(GroupMacDst);
    currentLineEdit->setText(mac);
}