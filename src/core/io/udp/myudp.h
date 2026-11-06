#ifndef MYUDP_H
#define MYUDP_H

#include <QObject>
#include <QUdpSocket>
#include "myudpcontrol.h"
#include "myudpai.h"


class myUdp : public QObject
{
    Q_OBJECT
public:
    explicit myUdp(int port_num, QObject *parent = 0);

    ~myUdp();

    void writeDatagram(QByteArray data, int nCount = 3, quint16 port = DEST_PORT);

    int readDatagram(char *buff, int rcvLength, int timeout);

    void setAnalyzeIndex(int analyzeIndex);

    void resetSimBuffer(void);

    void clearudpbuff();

private:
    bool checkHeadAndTail(const char *recvbuf, int dataLen);

signals:
    void sigImageDataReady(QByteArray,int,int); /*! 信号: 波形刷新 */
    void sigSocinferFinished(std::vector<BBox> vbox, int nCamid, uint nRow);
    void sigSocinferBottleFinished(QByteArray, int , uint);
    void sigIM01Tempure(int);
    void sigemitdata(QByteArray data, int nCount, quint16 port);
public slots:
    void sltInitSocket();

    void rebindSocket();

    void onSenddataSlt(QByteArray data, int nCount, quint16 port);

private:
    myUdpControl *my_udp_control;
//    myUdpAI      *my_udp_ai;
    int n_Port;
    int n_Port_1808;
    int m_analyzeIndex;

};

//extern myUdp *g_myUdp;

extern QList<myUdp*>       g_udpSocket;
extern QList<QThread*>     g_udpthread;

#endif // MYUDP_H
