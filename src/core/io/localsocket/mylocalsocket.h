#ifndef MYLOCALSOCKET_H
#define MYLOCALSOCKET_H

#include <QObject>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QProcess>
#include <params/globalparams.h>

#define HEAD_LENGTH     6   //! 定义包头长度

const QString sorterserverName = "MeyerAdjust";

class MyLocalSocket : public QObject
{
    Q_OBJECT
public:
    explicit MyLocalSocket(QObject *parent = nullptr);
    ~MyLocalSocket();

    void writedata(QJsonObject jobj);

signals:
    void sigsendadjparams(bool bInit);
    void sigsenddata(QJsonObject obj);
    void siglocalsocketfeedback(int ncode);

public slots:
    void onSocketErrors(QLocalSocket::LocalSocketError stat);
    void onRsvDataSlt();
    void onInitLocalSocket();
    void onlocalSocketsend(QJsonObject djson);
    void onConnectServer();

private:
    QList<QByteArray> analyzersvData(QByteArray ba);
    bool isConnected() const;
    void timerEvent(QTimerEvent *ev);
    bool connectToServer(const QString &serverName);

private:
    QLocalSocket *m_localsoket = nullptr;
    QThread      *m_localthread;
};

extern MyLocalSocket* g_localSocket;
#endif // MYLOCALSOCKET_H
