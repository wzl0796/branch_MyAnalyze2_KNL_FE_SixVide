#ifndef MYUDPAI_H
#define MYUDPAI_H

#include <QObject>
#include <QTime>
#include <QUdpSocket>
#include <QByteArray>
#include "params/globalparams.h"
#include "params/define.h"
#include "core/io/udp/myudp.h"
#include "core/globalflow/globalflow.h"

class myUdpAI : public QObject
{
    Q_OBJECT
public:
    explicit myUdpAI(int n_port, QObject *parent = 0);

    ~myUdpAI();

    QByteArray read(int nWaitTime=5);   //! 读操作

    //! 指定IP发送数据
    void write(QByteArray ba); //! 写操作
    QByteArray writeRead(QByteArray ba, int nWaitTime=50); //! 写操作

signals:

public slots:


private:
     void _clearBuffer(void);    //! 清空读缓存

private:
    QUdpSocket   *m_udpSocket;
    QMutex m_mutex;
    int m_port;


};

#endif // MYUDPAI_H
