#ifndef USER_UI_H
#define USER_UI_H

#include <unistd.h>
#include <QtWidgets>
#include "xdma_programe.h"
#include "pcie_manage.h"
#include "chartmanager.h"

class user_ui : public QFrame
{
    Q_OBJECT
public:
    explicit user_ui(QWidget *parent = 0);

    QPushButton *pButStart;
    pcie_manage *pPcieManage;
    chartmanager *ChartManagerUnpackRx;
    chartmanager *ChartManagerUnpackTx;
    chartmanager *ChartManagerGroupRx;
    chartmanager *ChartManagerGroupTx;

signals:

public slots:
};

#endif // USER_UI_H
