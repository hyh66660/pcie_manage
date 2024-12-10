#include "user_ui.h"

user_ui::user_ui(QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet("user_ui{"
                  "background:#444;border:1px solid #777;}"
                  "#mainui{"
                  "border:1px solid #222;}"
                  "QLabel{color:white;font-size:12px;}");

    // 创建主布局
    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->setMargin(0);

    // 创建 UI 组件容器
    QFrame *pUI = new QFrame;
    pUI->setObjectName("mainui");
    pLayout->addWidget(pUI);

    // 创建 QVBoxLayout，纵向排列 pButStart
    QVBoxLayout *pUserLayout = new QVBoxLayout(pUI);

    pButStart = new QPushButton;
    pButStart->setStyleSheet("QPushButton{"
                             "background-color:rgba(0, 0, 0, 50);"
                             "border-style: outset;"
                             "border-width: 2px;"
                             "border-radius: 5px;"
                             "border-color: #8B7355;"
                             "font: bold 28px;"
                             "min-width:200px;max-width:200px;"
                             "min-height:40px;max-height:40px;"
                             "color:white;"
                             "font-family:华文新魏;}"
                             "QPushButton:hover{"
                             "background-color:rgba(0, 0, 0, 100);"
                             "} "
                             "QPushButton:pressed{"
                             "background-color:rgba(0, 0, 0, 100);"
                             "border-style: inset;"
                             "padding-top:1px;padding-bottom:-1px;"
                             "padding-left:1px;padding-right:-1px;}");
    pUserLayout->addWidget(pButStart);

    // 创建 pcie_manage 实例
    pPcieManage = new pcie_manage;

    // 创建并添加 ChartManager
    ChartManagerUnpackRx = new chartmanager(this);
    ChartManagerUnpackTx = new chartmanager(this);
    ChartManagerGroupRx = new chartmanager(this);
    ChartManagerGroupTx = new chartmanager(this);
    ChartManagerUnpackRx->setFixedSize(600, 300);
    ChartManagerUnpackTx->setFixedSize(600, 300);
    ChartManagerGroupRx->setFixedSize(600, 300);
    ChartManagerGroupTx->setFixedSize(600, 300);

    QHBoxLayout *pChartManager1 = new QHBoxLayout();
    pChartManager1->addWidget(ChartManagerUnpackRx);
    pChartManager1->addWidget(ChartManagerUnpackTx);

    QHBoxLayout *pChartManager2 = new QHBoxLayout();
    pChartManager2->addWidget(ChartManagerGroupRx);
    pChartManager2->addWidget(ChartManagerGroupTx);


    QVBoxLayout *pManageLayout = new QVBoxLayout();
    pManageLayout->addWidget(pPcieManage);  // 添加 pPcieManage
    pManageLayout->addLayout(pChartManager1);
    pManageLayout->addLayout(pChartManager2);

    pUserLayout->addLayout(pManageLayout);  // 使用 Layout 而不是直接添加 Widget
}
