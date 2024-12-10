#include "main_window.h"
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
    resize(1200, 1000);
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
        pUserUi->ChartManagerUnpackRx->hide();
        pUserUi->ChartManagerUnpackTx->hide();
        pUserUi->ChartManagerGroupRx->hide();
        pUserUi->ChartManagerGroupTx->hide();
        pUserUi->pPcieManage->hide();
    }

    dataBuffLen = 1024;
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    //pDataBuff = (char *)_aligned_malloc(dataBuffLen, sys_info.dwPageSize);
    //size_t dataBuffLen = 1024;  // 每个缓冲区的大小

    // 分配多个缓冲区
    for (int i = 0; i < 13; ++i) {
        pDataBuff[i] = (char*)_aligned_malloc(dataBuffLen, sys_info.dwPageSize);
    }
    for (int i = 0; i < 5; ++i) {
        pDataBuffw[i] = (char*)_aligned_malloc(dataBuffLen, sys_info.dwPageSize);
    }

    // 初始化定时器
    pTimer = new QTimer;
    QObject::connect(pTimer, &QTimer::timeout, this, &MainWindow::read_next_addresses);
    QObject::connect(pXdma, SIGNAL(opt_end(int)), this, SLOT(opt_end(int)));

    QObject::connect(pUserUi->pPcieManage->pButWrite, SIGNAL(clicked(bool)), this, SLOT(pcie_opt()));

    // 初始化地址和目标组件
    initializeAddressAndTargets();

    // 开启定时器和 DMA 线程
    pTimer->start();
    pXdma->start();


}

MainWindow::~MainWindow()
{
    for (int i = 0; i < 13; ++i) {
        if (pDataBuff[i]) {
            _aligned_free(pDataBuff[i]);
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (pDataBuffw[i]) {
            _aligned_free(pDataBuffw[i]);
        }
    }
}

void MainWindow::initializeAddressAndTargets()
{
    // 设置要读取的地址
    addresses = {0x0004, 0x0008, 0x0010, 0x0018, 0x0020, 0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C};

    // 设置与之对应的目标 LineEdit
    targetLineEdits = {
        pUserUi->pPcieManage->pLineEditMtu,
        pUserUi->pPcieManage->pLineEditSrcMac,
        pUserUi->pPcieManage->pLineEditDestMac,
        pUserUi->pPcieManage->pLineEditGroupSrcMac,
        pUserUi->pPcieManage->pLineEditGroupDestMac,
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
    }else if(currentIndex==1 || currentIndex==2 || currentIndex==3 || currentIndex==4){
        opt_mode = 1;
        opt_len = 6;
    }else {
        opt_mode = 6;
        opt_len = 4;
    }
    pXdma->opt_pack(opt_mode, pDataBuff[currentIndex], opt_len, currentAddr);


}

void MainWindow::pcie_opt()
{
    pTimer->stop();
    unsigned int addr[5]={0x002C, 0x0030, 0x0038, 0x0040, 0x0048};
    QPushButton *but = qobject_cast<QPushButton *>(sender());

    if(but == pUserUi->pPcieManage->pButWrite)
    {
        opt_mode = 4;

        QString mtuw = pUserUi->pPcieManage->pLineEditMtuW->text();
        QString mtur = pUserUi->pPcieManage->pLineEditMtu->text();
        if(mtuw != "" && mtuw != mtur){
            bool ok = false;
            int value = mtuw.toInt(&ok, 10);
            QString textHex = QString::number(value, 16).rightJustified(4, '0').toUpper();
            QByteArray ba0;
            ba0 = textHex.toLocal8Bit();
            getHexFromText(ba0.data(), ba0.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 0);
            std::swap(pDataBuffw[0][0], pDataBuffw[0][1]);
            //if(opt_len > 0){
            //    pXdma->opt_pack(opt_mode, pDataBuffw[0], opt_len, addr[0]);
            //    opt_len = 0;
            //}
        }else {
            bool ok = false;
            int value = mtur.toInt(&ok, 10);
            QString textHex = QString::number(value, 16).rightJustified(4, '0').toUpper();
            QByteArray ba0;
            ba0 = textHex.toLocal8Bit();
            getHexFromText(ba0.data(), ba0.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 0);
            std::swap(pDataBuffw[0][0], pDataBuffw[0][1]);
        }

        QString macSrcw = pUserUi->pPcieManage->pLineEditSrcMacW->text();
        QString macSrcr = pUserUi->pPcieManage->pLineEditSrcMac->text();
        if(macSrcw != "" && macSrcw != macSrcr){
            QByteArray ba1;
            ba1 = macSrcw.toLocal8Bit();
            getHexFromText(ba1.data(), ba1.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 4);
            //if(opt_len > 0){
            //    pXdma->opt_pack(opt_mode, pDataBuffw[1], opt_len, addr[1]);
            //    opt_len = 0;
            //}
        }else{
            QByteArray ba1;
            ba1 = macSrcr.toLocal8Bit();
            getHexFromText(ba1.data(), ba1.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 4);
        }

        QString macDstw = pUserUi->pPcieManage->pLineEditDestMacW->text();
        QString macDstr = pUserUi->pPcieManage->pLineEditDestMac->text();
        if(macDstw != "" && macDstw != macDstr){
            QByteArray ba2;
            ba2 = macDstw.toLocal8Bit();
            getHexFromText(ba2.data(), ba2.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 12);
            //if(opt_len > 0){
            //    pXdma->opt_pack(opt_mode, pDataBuffw[2], opt_len, addr[2]);
            //    opt_len = 0;
            //}
        }else{
            QByteArray ba2;
            ba2 = macDstr.toLocal8Bit();
            getHexFromText(ba2.data(), ba2.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 12);
        }

        QString groupMacSrcw = pUserUi->pPcieManage->pLineEditGroupSrcMacW->text();
        QString groupMacSrcr = pUserUi->pPcieManage->pLineEditGroupSrcMac->text();
        if(groupMacSrcw != "" && groupMacSrcw != groupMacSrcr){
            QByteArray ba3;
            ba3 = groupMacSrcw.toLocal8Bit();
            getHexFromText(ba3.data(), ba3.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 20);
            //if(opt_len > 0){
            //    pXdma->opt_pack(opt_mode, pDataBuffw[3], opt_len, addr[3]);
            //    opt_len = 0;
            //}
        }else{
            QByteArray ba3;
            ba3 = groupMacSrcr.toLocal8Bit();
            getHexFromText(ba3.data(), ba3.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 20);
        }

        QString GroupMacDstw = pUserUi->pPcieManage->pLineEditGroupDestMacW->text();
        QString GroupMacDstr = pUserUi->pPcieManage->pLineEditGroupDestMac->text();
        if(GroupMacDstw != "" && GroupMacDstw != GroupMacDstr){
            QByteArray ba4;
            ba4 = GroupMacDstw.toLocal8Bit();
            getHexFromText(ba4.data(), ba4.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 28);
            //if(opt_len > 0){
            //    pXdma->opt_pack(opt_mode, pDataBuffw[4], opt_len, addr[4]);
            //    opt_len = 0;
            //}
        }else{
            QByteArray ba4;
            ba4 = GroupMacDstr.toLocal8Bit();
            getHexFromText(ba4.data(), ba4.length(), (unsigned char *)pDataBuffw[0], dataBuffLen, 28);
        }
        pXdma->opt_pack(opt_mode, pDataBuffw[0], 34, addr[0]);

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
            case 3: // GroupSrcMac
                processGroupSrcMacData();
                break;
            case 4: // GroupDestMac
                processGroupDestMacData();
                break;
            case 5: // UnpackRx
                processUnpackRxData();
                break;
            case 6: // UnpackTxGroupRx
                processGroupRxData();
                break;
            case 7: //
                processUnpackTxData();
                break;
            case 8: // GroupTx
                processGroupTxData();
                break;
            case 9: // UnpackMacSrc
                processUnpackMacSrcData();
                break;
            case 10: // UnpackMacDst
                processUnpackMacDstData();
                break;
            case 11: // GroupMacSrc
                processGroupMacSrcData();
                break;
            case 12: // GroupMacDst
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
        pTimer->start(75);
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

void MainWindow::processGroupSrcMacData()
{
    getTextFromHex((unsigned char *)pDataBuff[3], 6, strBuff[2]);
    currentLineEdit->setText(strBuff[2]);
}

void MainWindow::processGroupDestMacData()
{
    getTextFromHex((unsigned char *)pDataBuff[4], 6, strBuff[3]);
    currentLineEdit->setText(strBuff[3]);
}

void MainWindow::processUnpackRxData()
{
    // 假设 UnpackRx 是一个整数值
    unsigned int UnpackRx = *(unsigned int*)pDataBuff[5];
    UnpackRx = UnpackRx / 10 * 8 / 100;
    unsigned int UnpackRxchart = UnpackRx;
    if(UnpackRx < 1000){
        double speed = static_cast<double>(UnpackRx);
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Kbps");
    }else if(UnpackRx >= 1000 && UnpackRx < 1000000){
        double speed = static_cast<double>(UnpackRx) / 1000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Mbps");
    }else {
        double speed = static_cast<double>(UnpackRx) / 1000000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Gbps");
    }
    pUserUi->ChartManagerUnpackRx->addDataToUnpackRx(timeCounter, UnpackRxchart);
    //++timeCounter;
}

void MainWindow::processGroupRxData()
{
    // 假设 GroupRx 是一个整数值
    unsigned int GroupRx = *(unsigned int*)pDataBuff[6];
    GroupRx = GroupRx / 10 * 8 / 100;
    unsigned int GroupRxchart = GroupRx;
    if(GroupRx < 1000){
        double speed = static_cast<double>(GroupRx);
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Kbps");
    }else if(GroupRx >= 1000 && GroupRx < 1000000){
        double speed = static_cast<double>(GroupRx) / 1000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Mbps");
    }else {
        double speed = static_cast<double>(GroupRx) / 1000000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Gbps");
    }
    pUserUi->ChartManagerGroupRx->addDataToUnpackRx(timeCounter, GroupRxchart);
    //++timeCounter;
}

void MainWindow::processUnpackTxData()
{
    // 假设 UnpackTx 是一个整数值
    unsigned int UnpackTx = *(unsigned int*)pDataBuff[7];
    UnpackTx = UnpackTx / 10 * 8 / 100;
    unsigned int UnpackTxchart = UnpackTx;
    if(UnpackTx < 1000){
        double speed = static_cast<double>(UnpackTx);
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Kbps");
    }else if(UnpackTx >= 1000 && UnpackTx < 1000000){
        double speed = static_cast<double>(UnpackTx) / 1000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Mbps");
    }else {
        double speed = static_cast<double>(UnpackTx) / 1000000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Gbps");
    }
    pUserUi->ChartManagerUnpackTx->addDataToUnpackRx(timeCounter, UnpackTxchart);
    //++timeCounter;
}

void MainWindow::processGroupTxData()
{
    // 假设 GroupTx 是一个整数值
    unsigned int GroupTx = *(unsigned int*)pDataBuff[8];
    GroupTx = GroupTx / 10 * 8 / 100;
    unsigned int GroupTxchart = GroupTx;
    if(GroupTx < 1000){
        double speed = static_cast<double>(GroupTx);
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Kbps");
    }else if(GroupTx >= 1000 && GroupTx < 1000000){
        double speed = static_cast<double>(GroupTx) / 1000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Mbps");
    }else {
        double speed = static_cast<double>(GroupTx) / 1000000;
        QString speedString = QString::number(speed, 'f', 1);
        currentLineEdit->setText(speedString+"Gbps");
    }
    pUserUi->ChartManagerGroupTx->addDataToUnpackRx(timeCounter, GroupTxchart);
    ++timeCounter;
}

void MainWindow::processUnpackMacSrcData()
{
    // 假设 UnpackMacSrc 是一个整数值
    unsigned int UnpackMacSrc = *(unsigned int*)pDataBuff[9];
    QString mac = QString::number(UnpackMacSrc);
    currentLineEdit->setText(mac);
}

void MainWindow::processUnpackMacDstData()
{
    // 假设 UnpackMacDst 是一个整数值
    unsigned int UnpackMacDst = *(unsigned int*)pDataBuff[10];
    QString mac = QString::number(UnpackMacDst);
    currentLineEdit->setText(mac);
}

void MainWindow::processGroupMacSrcData()
{
    // 假设 GroupMacSrc 是一个整数值
    unsigned int GroupMacSrc = *(unsigned int*)pDataBuff[11];
    QString mac = QString::number(GroupMacSrc);
    currentLineEdit->setText(mac);
}

void MainWindow::processGroupMacDstData()
{
    // 假设 GroupMacDst 是一个整数值
    unsigned int GroupMacDst = *(unsigned int*)pDataBuff[12];
    QString mac = QString::number(GroupMacDst);
    currentLineEdit->setText(mac);
}
