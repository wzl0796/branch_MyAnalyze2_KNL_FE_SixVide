#ifndef LOCALSOCKET_H
#define LOCALSOCKET_H

#include <QObject>
#include <QLocalSocket>
#include <QMutex>
#include <QMutexLocker>
#include <QProcess>

#include "localsocketdefines.h"

const QString serverName = "Meyer";

class LocalSocket : public QObject
{
    Q_OBJECT
public:
    explicit LocalSocket(QObject *parent = nullptr);
    ~LocalSocket();

    void directConnect();
    bool connectToServer(const QString &serverName);

    void sendMessage(const QString &msg);
    void sendMessage(const QByteArray &msg);
    bool isConnected() const;
    inline uchar _uchar(QByteArray ba, int index){
        return (index >= ba.size()) ? 0x00 : (ba.at(index) & 0xff);
    }

protected:
    void timerEvent(QTimerEvent *ev);

signals:
    void connectToServer();
    void readLocalSocketData(QByteArray);
    void connectedSuccess();

public slots:
    void onConnectToServer();
    void onErrorConnect(QLocalSocket::LocalSocketError error);
    void onStateChanged(QLocalSocket::LocalSocketState state);
    void onReadingData();

private:
    QLocalSocket *m_localSocket = nullptr;
    QMutex		m_mtx;

    QByteArray m_rcvDatagram;

    QByteArray HEAD;
};

#endif // LOCALSOCKET_H
