#include "pcie_manage.h"

pcie_manage::pcie_manage(QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet(""
                  "*{font-size:20px;color:#fff;font-family:华文新魏;}"
                  "QComboBox{"
                  "padding:6px;"
                  "padding-top:2px;"
                  "padding-bottom:2px;"
                  "border:1px solid #000;"
                  "background-color:#555;"
                  "color:#fff;}"
                  "QComboBox QAbstractItemView"
                  "{"
                  "border:1px solid black;"
                  "background-color:#555;"
                  "selection-color:#fff;"
                  "selection-background-color:#777;"
                  "outline:0px;"
                  "}"
                  "QComboBox QAbstractItemView::item"
                  "{"
                  "min-height: 20px;"
                  "}"
                  "QLineEdit"
                  "{"
                  "border:1px solid #000;"
                  "padding:2px;"
                  "background-color:#555;"
                  "max-width:250px;min-width:100px;"
                  "}"
                  "QPushButton{"
                  "background-color:rgba(0, 0, 0, 50);"
                  "border-style: outset;"
                  "border-width: 2px;"
                  "border-color: #8B7355;"
                  "min-width:80px;max-width:80px;"
                  "padding-top:2px;padding-bottom:2px;"
                  "}"
                  "QPushButton:hover{"
                  "background-color:rgba(0, 0, 0, 100);"
                  "}"
                  "QPushButton:pressed{"
                  "background-color:rgba(0, 0, 0, 100);"
                  "border-style: inset;"
                  "padding-top:1px;padding-bottom:-1px;"
                  "padding-left:1px;padding-right:-1px;"
                  "}"
                  "QTextEdit"
                  "{"
                  "background-color:#555;"
                  "border:1px solid #000;"
                  "}"
                  "");
    QGridLayout *pLayout = new QGridLayout(this);

    pLabOpt = new QLabel;
    pLabAddr = new QLabel;
    pLabLength = new QLabel;
    pComboBox = new QComboBox;
    pLineEditAddr = new QLineEdit;
    pLineEditLength = new QLineEdit;
    pButRead = new QPushButton;
    pButWrite = new QPushButton;
    pTextEdit = new QTextEdit;
    pLabMtu = new QLabel;
    pLabSrcMac = new QLabel;
    pLabDestMac = new QLabel;
    pLabGroupSrcMac = new QLabel;
    pLabGroupDestMac = new QLabel;
    pLabUnpackRxSpeed = new QLabel;
    pLabGroupRxSpeed = new QLabel;
    pLabUnpackTxSpeed = new QLabel;
    pLabGroupTxSpeed = new QLabel;
    pLabUnpackMacSrc = new QLabel;
    pLabUnpackMacDst = new QLabel;
    pLabGroupMacSrc = new QLabel;
    pLabGroupMacDst = new QLabel;
    pLineEditMtu = new QLineEdit;
    pLineEditMtuW = new QLineEdit;
    pLineEditSrcMac = new QLineEdit;
    pLineEditSrcMacW = new QLineEdit;
    pLineEditGroupSrcMac = new QLineEdit;
    pLineEditGroupSrcMacW = new QLineEdit;
    pLineEditDestMac = new QLineEdit;
    pLineEditDestMacW = new QLineEdit;
    pLineEditGroupDestMac = new QLineEdit;
    pLineEditGroupDestMacW = new QLineEdit;
    pLineEditUnpackRxSpeed = new QLineEdit;
    pLineEditGroupRxSpeed = new QLineEdit;
    pLineEditUnpackTxSpeed = new QLineEdit;
    pLineEditGroupTxSpeed = new QLineEdit;
    pLineEditUnpackMacSrc = new QLineEdit;
    pLineEditUnpackMacDst = new QLineEdit;
    pLineEditGroupMacSrc = new QLineEdit;
    pLineEditGroupMacDst = new QLineEdit;

    pLabOpt->setText(tr("操作模式"));
    pLabAddr->setText(tr("操作地址"));
    pLabLength->setText(tr("读取长度"));
    pComboBox->setItemDelegate(new QStyledItemDelegate());
    pComboBox->addItem(tr("控制模式"));
    pComboBox->addItem(tr("用户模式"));
    pComboBox->addItem(tr("内存模式"));
    pButRead->setText(tr("读"));
    pButWrite->setText(tr("写"));
    pLabMtu->setText(tr("最大传输单元(MTU)"));
    pLabSrcMac->setText(tr("拆包源MAC地址(UNPACK_SRC_MAC)"));
    pLabDestMac->setText(tr("拆包目标MAC地址(UNPACK_DEST_MAC)"));
    pLabGroupSrcMac->setText(tr("组包源MAC地址(GROUP_SRC_MAC)"));
    pLabGroupDestMac->setText(tr("组包目标MAC地址(GROUP_DEST_MAC)"));
    pLabUnpackRxSpeed->setText(tr("拆包接收端速率(UNPACK_RX)"));
    pLabGroupRxSpeed->setText(tr("组包接收端速率(GROUP_RX)"));
    pLabUnpackTxSpeed->setText(tr("拆包发送端速率(UNPACK_TX)"));
    pLabGroupTxSpeed->setText(tr("组包发送端速率(GROUP_TX)"));
    pLabUnpackMacSrc->setText(tr("拆包源MAC地址个数(UNPACK_MAC_SRC)"));
    pLabUnpackMacDst->setText(tr("拆包目标MAC地址个数(UNPACK_MAC_DST)"));
    pLabGroupMacSrc->setText(tr("组包源MAC地址个数(GROUP_MAC_SRC)"));
    pLabGroupMacDst->setText(tr("组包目标MAC地址个数(GROUP_MAC_DST)"));
    pLineEditMtu->setText("1500");



    pLayout->addWidget(pLabMtu,         0, 0);
    pLayout->addWidget(pLabSrcMac,      1, 0);
    pLayout->addWidget(pLabDestMac,     2, 0);
    pLayout->addWidget(pLabGroupSrcMac,      3, 0);
    pLayout->addWidget(pLabGroupDestMac,     4, 0);
    pLayout->addWidget(pLabUnpackRxSpeed, 5, 0);
    pLayout->addWidget(pLabGroupRxSpeed,  5, 2);
    pLayout->addWidget(pLabUnpackTxSpeed, 6, 0);
    pLayout->addWidget(pLabGroupTxSpeed,  6, 2);
    pLayout->addWidget(pLabUnpackMacSrc, 7, 0);
    pLayout->addWidget(pLabUnpackMacDst,  8, 0);
    pLayout->addWidget(pLabGroupMacSrc, 7, 2);
    pLayout->addWidget(pLabGroupMacDst,  8, 2);
    pLayout->addWidget(pLineEditMtu,    0, 1);
    pLayout->addWidget(pLineEditMtuW,   0, 2);
    pLayout->addWidget(pButWrite,       0, 3);
    pLayout->addWidget(pLineEditSrcMac, 1, 1);
    pLayout->addWidget(pLineEditSrcMacW, 1, 2);
    pLayout->addWidget(pLineEditGroupSrcMac, 3, 1);
    pLayout->addWidget(pLineEditGroupSrcMacW, 3, 2);
    pLayout->addWidget(pLineEditDestMac,2, 1);
    pLayout->addWidget(pLineEditDestMacW,2, 2);
    pLayout->addWidget(pLineEditGroupDestMac,4, 1);
    pLayout->addWidget(pLineEditGroupDestMacW,4, 2);
    pLayout->addWidget(pLineEditUnpackRxSpeed,5, 1);
    pLayout->addWidget(pLineEditGroupRxSpeed, 5, 3);
    pLayout->addWidget(pLineEditUnpackTxSpeed,6, 1);
    pLayout->addWidget(pLineEditGroupTxSpeed, 6, 3);
    pLayout->addWidget(pLineEditUnpackMacSrc,7, 1);
    pLayout->addWidget(pLineEditUnpackMacDst, 8, 1);
    pLayout->addWidget(pLineEditGroupMacSrc,7, 3);
    pLayout->addWidget(pLineEditGroupMacDst, 8, 3);
}

int getHexFromText(char *pParseData, int parseLen, unsigned char *pBackData, int maxlen)
{
    int i;
    int cot=0;
    int sub=0;
    unsigned char d0;
    unsigned char d1;

    for(i=0;i<parseLen;i++)
    {
        if(pParseData[i] == ' ')
        {
            sub=0;
        }
        else
        {
            d1 = pParseData[i];
            if( (d1>='0') && (d1<='9') )
            {
                d1 -= '0';
            }
            else if( (d1>='a') && (d1<='f') )
            {
                d1 = d1-'a'+10;
            }
            else if( (d1>='A') && (d1<='F') )
            {
                d1 = d1-'A'+10;
            }
            else
            {
                continue;
            }
            if(sub)
            {
                d0 |= d1;
                pBackData[cot++] = d0;
                if(cot >= maxlen)
                {
                    break;
                }
                sub = 0;
            }
            else
            {
                sub = 1;
                d0 = d1<<4;
            }
        }
    }
    return cot;
}


void getTextFromHex(unsigned char *pData, int len, QString &str)
{
    char buff[10];

    str.resize(len*3);
    str = "";
    for(int i=0;i<len;i++)
    {
        sprintf(buff, "%02X", pData[i]);
        if(i)
        {
            str += " ";
        }
        str += buff;
    }
}
