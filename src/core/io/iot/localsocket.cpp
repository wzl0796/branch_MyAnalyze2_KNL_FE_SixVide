#include "localsocket.h"

LocalSocket::LocalSocket(QObject *parent) : QObject(parent)
{
    m_localSocket = new  QLocalSocket();
    HEAD.append(0xB5);
    HEAD.append(0x5B);
    connect(m_localSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(onErrorConnect(QLocalSocket::LocalSocketError)));
    connect(m_localSocket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
            this, SLOT(onStateChanged(QLocalSocket::LocalSocketState)));
    connect(m_localSocket, SIGNAL(readyRead()), this, SLOT(onReadingData()));

    connect(this, SIGNAL(connectToServer()), this, SLOT(onConnectToServer()));
    startTimer(INTERVAL_SOCKET_CHECK);
}

LocalSocket::~LocalSocket()
{
    m_localSocket->disconnectFromServer();
    delete m_localSocket;
}

void LocalSocket::directConnect()
{
    emit connectToServer();
}

bool LocalSocket::connectToServer(const QString &serverName)
{
    bool res = true;
    m_localSocket->connectToServer(serverName);
    if (m_localSocket->waitForConnected(INTERVAL_SOCKET_CONNECT)) {
        res = true;
//        qInfo().noquote() << QString("QLocalSocket connect successfully");
    } else {
        res = false;
//        qDebug().noquote() << QString("QLocalSocket connect failed! %1").arg(m_localSocket->errorString());
    }
    return res;
}

void LocalSocket::sendMessage(const QString &msg)
{
    QMutexLocker _lck(&m_mtx);
    m_localSocket->write(msg.toStdString().c_str());
    m_localSocket->waitForBytesWritten(100);
}

/*!
 * \brief LocalSocket::sendMessage
 * \param msg
 */
void LocalSocket::sendMessage(const QByteArray &msg)
{
    if (isConnected()) {
       QMutexLocker _lck(&m_mtx);

       QByteArray ba;
       ba.append(0xB5);
       ba.append(0x5B);
       int size = msg.size();
       ba.append((char)(size >> 24) & 0xff);
       ba.append((char)(size >> 16) & 0xff);
       ba.append((char)(size >> 8)  & 0xff);
       ba.append((char)size & 0xff);
       ba.append(msg);
       m_localSocket->write(ba);
       m_localSocket->flush();
    }
}

bool LocalSocket::isConnected() const
{
    return (m_localSocket->state() == QLocalSocket::ConnectedState);
}

void LocalSocket::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev)
    onConnectToServer();
}

void LocalSocket::onConnectToServer()
{

    if (m_localSocket != nullptr) {
        if (m_localSocket->state() == QLocalSocket::UnconnectedState) {
            if (!connectToServer(serverName)) {
#ifdef Q_OS_UNIX
                QProcess process;
                process.startDetached("./plugs/mystart.sh");
#endif
            } else {
                emit connectedSuccess();
            }
        }
    }
}

void LocalSocket::onErrorConnect(QLocalSocket::LocalSocketError error)
{
#ifdef Q_OS_UNIX
    qDebug().noquote() << QString("QLocalSocket Error changed(Error:%1)").arg(error);
#endif
}

void LocalSocket::onStateChanged(QLocalSocket::LocalSocketState state)
{
#ifdef Q_OS_UNIX
    qDebug().noquote() << QString("QLocalSocket state changed(state:%1)").arg(state);
#endif
}

void LocalSocket::onReadingData()
{
    m_rcvDatagram.append(m_localSocket->readAll());

    int headIndex = m_rcvDatagram.contains(HEAD) ?
                m_rcvDatagram.indexOf(HEAD) : -1;

    while (headIndex >= 0) {
        if (m_rcvDatagram.size() > headIndex + 6) {             //! 长度数据在包中
            int nSize = (_uchar(m_rcvDatagram, headIndex+2) << 24) |
                        (_uchar(m_rcvDatagram, headIndex+3) << 16) |
                        (_uchar(m_rcvDatagram, headIndex+4) << 8)  |
                        _uchar(m_rcvDatagram, headIndex+5);
            if (m_rcvDatagram.size() >= nSize + headIndex + 6) {   //! 缓存数据 ≥ 一帧数据量(包括包头2Byte和大小字节4Byte）
                QByteArray datagram = m_rcvDatagram.mid(headIndex+6, nSize);
                emit readLocalSocketData(datagram);
                m_rcvDatagram.remove(0, headIndex + 6 + nSize);
            } else {            //! 缓存数据 < 一帧数据量，包不完整，不处理，等下一帧数据
                break;
            }
        } else {
            break;
        }
        headIndex = m_rcvDatagram.contains(HEAD) ?
                    m_rcvDatagram.indexOf(HEAD) : -1;
    }
}
