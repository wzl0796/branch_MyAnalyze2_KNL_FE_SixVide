#include "myudpai.h"

myUdpAI::myUdpAI(int n_port, QObject *parent) : QObject(parent)
{
    m_port = n_port;

    m_udpSocket = new QUdpSocket();

    if (!m_udpSocket->bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress)){
        qCritical("Failed to bind %d.",m_port);
    }

    m_udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);

}

/**
 * @brief myUdpAI::析构函数
 */
myUdpAI::~myUdpAI()
{
    m_udpSocket->deleteLater();
}

QByteArray myUdpAI::read(int nWaitTime)
{
    //! 检查网口状态
    if (!m_udpSocket->isValid()) {
        qDebug("m_udpSocket is invalid");
        return QByteArray();
    }

    QTime time;
    time.start();
    QByteArray ba;
    do {
        if (m_udpSocket->hasPendingDatagrams()) {
            int nSize = m_udpSocket->pendingDatagramSize();
            ba.resize(nSize);

            QHostAddress address;
            quint16 port;
            m_udpSocket->readDatagram(ba.data(), ba.size(), &address, &port);
            break;
        }
        gFlow.msleep(1);
    }while (time.elapsed() < nWaitTime);

    return ba;
}

QByteArray myUdpAI::writeRead(QByteArray ba, int nWaitTime)
{
//    if (!m_mutex.tryLock(200)) {
//        return QByteArray();
//    }

    _clearBuffer();


    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);

    QByteArray data = read(nWaitTime);
//    m_mutex.unlock();
    return data;
}

void myUdpAI::write(QByteArray ba)
{
      g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
}


void myUdpAI::_clearBuffer()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        char *p = (char *)malloc(m_udpSocket->pendingDatagramSize());
        if (p == NULL) {
            qWarning("Failed to malloc, %s,%s,%d", __FILE__, __FUNCTION__, __LINE__);
            return;
        }

        m_udpSocket->readDatagram(p, sizeof(p));
        free(p);
    }
}

