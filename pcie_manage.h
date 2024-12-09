#ifndef PCIE_MANAGE_H
#define PCIE_MANAGE_H

#include <QtWidgets>

class pcie_manage : public QFrame
{
    Q_OBJECT
public:
    explicit pcie_manage(QWidget *parent = 0);


    void reciveData(unsigned char *pData, int *len);

signals:

public slots:

public:
    QLabel *pLabOpt;
    QLabel *pLabAddr;
    QLabel *pLabLength;
    QLabel *pLabMtu;
    QLabel *pLabSrcMac;
    QLabel *pLabDestMac;
    QLabel *pLabGroupSrcMac;
    QLabel *pLabGroupDestMac;
    QLabel *pLabUnpackRxSpeed;
    QLabel *pLabGroupRxSpeed;
    QLabel *pLabUnpackTxSpeed;
    QLabel *pLabGroupTxSpeed;
    QLabel *pLabUnpackMacSrc;
    QLabel *pLabUnpackMacDst;
    QLabel *pLabGroupMacSrc;
    QLabel *pLabGroupMacDst;
    QComboBox *pComboBox;
    QLineEdit *pLineEditAddr;
    QLineEdit *pLineEditLength;
    QLineEdit *pLineEditMtu;
    QLineEdit *pLineEditMtuW;
    QLineEdit *pLineEditSrcMac;
    QLineEdit *pLineEditSrcMacW;
    QLineEdit *pLineEditGroupSrcMac;
    QLineEdit *pLineEditGroupSrcMacW;
    QLineEdit *pLineEditDestMac;
    QLineEdit *pLineEditDestMacW;
    QLineEdit *pLineEditGroupDestMac;
    QLineEdit *pLineEditGroupDestMacW;
    QLineEdit *pLineEditUnpackRxSpeed;
    QLineEdit *pLineEditGroupRxSpeed;
    QLineEdit *pLineEditUnpackTxSpeed;
    QLineEdit *pLineEditGroupTxSpeed;
    QLineEdit *pLineEditUnpackMacSrc;
    QLineEdit *pLineEditUnpackMacDst;
    QLineEdit *pLineEditGroupMacSrc;
    QLineEdit *pLineEditGroupMacDst;
    QPushButton *pButRead;
    QPushButton *pButWrite;
    QTextEdit *pTextEdit;
};

int getHexFromText(char *pParseData, int parseLen, unsigned char *pBackData, int maxlen);
void getTextFromHex(unsigned char *pData, int len, QString &str);

#endif // PCIE_MANAGE_H
