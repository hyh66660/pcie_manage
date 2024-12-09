#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLineEdit>
#include <QVector>
#include "user_ui.h"
#include "xdma_programe.h"
#include <QtCharts>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    void app_error(void);
    void pcie_opt();
    void opt_end(int rw);

private:
    // 初始化读取的地址和目标组件
    void initializeAddressAndTargets();

    // 每次读取 5 个地址的数据
    void read_next_addresses();

    // 数据处理函数
    void processDataForCurrentAddress();
    void processMtuData();
    void processSrcMacData();
    void processDestMacData();
    void processUnpackRxData();
    void processGroupRxData();
    void processUnpackTxData();
    void processGroupTxData();
    void processUnpackMacSrcData();
    void processUnpackMacDstData();
    void processGroupMacSrcData();
    void processGroupMacDstData();
    void processMacAddress(const QString& macStr, unsigned char* macBytes);


    int timeCounter=0;                  // 时间计数器，用于横坐标（秒）

private:
    user_ui *pUserUi;  // 主界面UI对象
    xdma_programe *pXdma;  // DMA 程序实例
    QTimer *pTimer;  // 定时器

    // 存储DMA读取数据的缓冲区
    //char *pDataBuff;
    char* pDataBuff[11];
    char* pDataBuffw[3];
    int dataBuffLen;

    // 存储需要读取的地址
    QList<quint32> addresses;
    QList<QLineEdit*> targetLineEdits;  // 存储目标组件（LineEdit）

    // 当前处理的索引
    int currentIndex;

    // 当前读取的地址和LineEdit
    quint32 currentAddr;
    QLineEdit *currentLineEdit;

    // 读取长度和操作模式
    int opt_len;
    int opt_mode;

    QString strBuff[2];
};

#endif // MAINWINDOW_H
