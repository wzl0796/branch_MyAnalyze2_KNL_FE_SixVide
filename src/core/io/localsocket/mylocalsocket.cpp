#include "mylocalsocket.h"

MyLocalSocket* g_localSocket;

MyLocalSocket::MyLocalSocket(QObject *parent) : QObject(parent)
{
    m_localsoket = nullptr;
    m_localthread = new QThread();
    this->moveToThread(m_localthread);
    m_localthread->start();
}

MyLocalSocket::~MyLocalSocket()
{
    m_localsoket->disconnectFromServer();
    delete m_localsoket;
}

/*!
 * \brief 发送数据包
 * \param Obj
 * \return
 */
void MyLocalSocket::writedata(QJsonObject jobj)
{
   emit sigsenddata(jobj);
}

/*!
 * \brief 槽: 连接服务器
 * \return
 */
bool MyLocalSocket::connectToServer(const QString& serverName)
{
    bool res = true;
    m_localsoket->connectToServer(serverName);
    if (m_localsoket->waitForConnected(3000)) {
        res = true;
    } else {
        res = false;
    }

    return res;
}

/*!
 * \brief 槽：socket错误提示
 * \param stat
 */
void MyLocalSocket::onSocketErrors(QLocalSocket::LocalSocketError stat)
{
    qDebug()<<QString("错误码%1").arg(stat);
}

/*!
 * \brief 槽: 接收数据
 */
void MyLocalSocket::onRsvDataSlt()
{
    QByteArray array;
    do{
        array.append(m_localsoket->readAll());
    }while(m_localsoket->bytesAvailable());

    auto vByteary = analyzersvData(array);
    for(QByteArray ary:vByteary){
        struGsh.bSorterstalst.clear();
        QJsonDocument jsondoc = QJsonDocument::fromJson(ary);
        QJsonObject obj = jsondoc.object();
        if(obj.contains("code")&&obj["code"].toInt()==10){
            int res = obj["data"].toObject().value("adjust_result").toInt();
            emit siglocalsocketfeedback(res);
        }else if(obj.contains("code")&&obj["code"].toInt()==3){
            bool res = obj["data"].toObject().value("sorter").toBool();
            struGsh.bSorterstalst.push_back(res);
        }
    }
}

/*!
 * \brief 初始化localsocket
 */
void MyLocalSocket::onInitLocalSocket()
{
    if(m_localsoket !=nullptr){
        return;
    }

    m_localsoket = new QLocalSocket();
    connect(m_localsoket,   SIGNAL(readyRead()),    this,   SLOT(onRsvDataSlt()));
    connect(this,           SIGNAL(sigsenddata(QJsonObject)), this, SLOT(onlocalSocketsend(QJsonObject)));
    connect(m_localsoket,   SIGNAL(error(QLocalSocket::LocalSocketError)),  this,
            SLOT(onSocketErrors(QLocalSocket::LocalSocketError)));

    onConnectServer();

    startTimer(30000);
}

/*!
 * \brief 槽：发送数据
 * \param djson
 */
void MyLocalSocket::onlocalSocketsend(QJsonObject djson)
{
    if(m_localsoket != nullptr && m_localsoket->isValid()){
        if(isConnected()){

            QJsonDocument doc;
            doc.setObject(djson);
            QByteArray array =  doc.toJson();


            QByteArray ba;
            ba.append(0xB5);
            ba.append(0x5B);
            int size = array.size();
            ba.append((char)(size >> 24) & 0xff);
            ba.append((char)(size >> 16) & 0xff);
            ba.append((char)(size >> 8)  & 0xff);
            ba.append((char)size & 0xff);
            ba.append(array);

            m_localsoket->write(ba);
            m_localsoket->flush();
        }else{
            qDebug()<<"local socket is not connect";
        }
    }
}

/*!
 * \brief 连接服务
 * \return
 */
void MyLocalSocket::onConnectServer()
{
    if (m_localsoket != nullptr) {
        if (m_localsoket->state() == QLocalSocket::UnconnectedState) {
            if (!connectToServer(sorterserverName)) {
#ifdef Q_OS_UNIX
                QProcess process;
                process.startDetached("./plugins/MyAdjust");
#endif
               emit siglocalsocketfeedback(-1);
            } else {
                emit sigsendadjparams(true);
                emit siglocalsocketfeedback(-2);
            }
        }
    }

    //! 定时查询状态
    if(m_localsoket != nullptr && m_localsoket->isValid()) {
        if (m_localsoket->state() == QLocalSocket::ConnectedState &&
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass) {
            //! 发送与色选机通讯状态查询
            QJsonObject obj;
            obj["code"] = 3;
            obj["data"] = QJsonObject();
            onlocalSocketsend(obj);
        }
    }
}

/*!
 * \brief 解析数据
 * \param array
 * \return
 */
QList<QByteArray> MyLocalSocket::analyzersvData(QByteArray ba)
{
    QList<QByteArray> list;
    QVector<int> vHead;
    QVector<int> vLength;
    for (int i = 0; i < ba.size()-HEAD_LENGTH; i++) {
        if ((uchar)ba[i] == 0xB5 && (uchar)ba[i+1] == 0x5B) {
            vHead.push_back(i);
            int nLen = ((uchar)ba[i+2]<<24) + ((uchar)ba[i+3]<<16)
                       + ((uchar)ba[i+4]<<8) + ((uchar)ba[i+5]);
            vLength.push_back(nLen);
        }
    }

    for (int i = 0; i < vHead.size(); i++) {
        if ((vHead[i] + vLength[i]) <= ba.size()) {
            list.push_back(ba.mid(vHead[i]+HEAD_LENGTH, vLength[i]));
        }
    }

    return list;
}

/*!
 * \brief 是否连接
 * \return
 */
bool MyLocalSocket::isConnected() const
{
    if(m_localsoket != nullptr){
        return (m_localsoket->state() == QLocalSocket::ConnectedState);
    }else{
        return false;
    }
}

/*!
 * \brief 定时刷新loaclsocket状态
 * \param ev
 */
void MyLocalSocket::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev);
    onConnectServer();
}
