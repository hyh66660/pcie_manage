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

    // 创建按钮并添加到垂直布局中
    btnShowUnpackRx = new QPushButton("UnpackRx Speed", this);
    btnShowGroupRx = new QPushButton("UnpackTx Speed", this);
    btnShowUnpackTx = new QPushButton("GroupRx Speed", this);
    btnShowGroupTx = new QPushButton("GroupTx Speed", this);
    btnShowUnpackRx->setStyleSheet("QPushButton{"
                                         "background-color:rgba(0, 0, 0, 50);"
                                         "border-style: outset;"
                                         "border-width: 2px;"
                                         "border-radius: 5px;"
                                         "border-color: #8B7355;"
                                         "font: bold 20px;"
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
    btnShowGroupRx->setStyleSheet("QPushButton{"
                                        "background-color:rgba(0, 0, 0, 50);"
                                        "border-style: outset;"
                                        "border-width: 2px;"
                                        "border-radius: 5px;"
                                        "border-color: #8B7355;"
                                        "font: bold 20px;"
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
    btnShowUnpackTx->setStyleSheet("QPushButton{"
                                         "background-color:rgba(0, 0, 0, 50);"
                                         "border-style: outset;"
                                         "border-width: 2px;"
                                         "border-radius: 5px;"
                                         "border-color: #8B7355;"
                                         "font: bold 20px;"
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
    btnShowGroupTx->setStyleSheet("QPushButton{"
                                        "background-color:rgba(0, 0, 0, 50);"
                                        "border-style: outset;"
                                        "border-width: 2px;"
                                        "border-radius: 5px;"
                                        "border-color: #8B7355;"
                                        "font: bold 20px;"
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
    QVBoxLayout *pButtonLayout = new QVBoxLayout();
    pButtonLayout->addWidget(btnShowUnpackRx);
    pButtonLayout->addWidget(btnShowGroupRx);
    pButtonLayout->addWidget(btnShowUnpackTx);
    pButtonLayout->addWidget(btnShowGroupTx);

    // 创建 pcie_manage 实例
    pPcieManage = new pcie_manage;

    // 创建并添加 ChartManager
    ChartManager = new chartmanager(this);
    ChartManager->setFixedSize(600, 400);

    QHBoxLayout *pChartManager = new QHBoxLayout();
    pChartManager->addWidget(ChartManager);
    pChartManager->addLayout(pButtonLayout);


    QVBoxLayout *pManageLayout = new QVBoxLayout();
    pManageLayout->addWidget(pPcieManage);  // 添加 pPcieManage
    pManageLayout->addLayout(pChartManager);  // 添加按钮布局

    pUserLayout->addLayout(pManageLayout);  // 使用 Layout 而不是直接添加 Widget
}
