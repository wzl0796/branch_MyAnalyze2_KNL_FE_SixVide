#include "mymodbus_client.h"

/*!
 * \brief MyModbus::MyModbus
 * \param parent
 */
MyModbusC::MyModbusC(int nType, QObject *parent) : QObject(parent)
{
    m_nType = nType;

    if(m_nType == TYPE_TCP) {
        m_client = new QModbusTcpClient;
    }
    else {
        m_client = new QModbusRtuSerialMaster;
    }
//    connect(m_client, SIGNAL(stateChanged(QModbusDevice::State)), this, SLOT(onStateChanged(QModbusDevice::State)));
//    startTimer(3*1000);     //! 间隔发送测试数据，保证Modbus连接性
}

/*!
 * \brief 连接MODBUS服务器
 * \return
 */
bool MyModbusC::connectToModbus(void)
{
    if (!m_client) {
        return false;
    }

    //! 若设备已连接则断开连接
    if (m_client->state() == QModbusDevice::ConnectedState) {
        m_client->disconnectDevice();
    }

    //! 连接Modbus设备
    if(m_nType == TYPE_TCP) {
        m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_sIP);
        m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_nPort);
    }else {
        m_client->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::EvenParity);
        m_client->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        m_client->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
        m_client->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);
        m_client->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_portName);
    }

    if (!m_client->connectDevice()) {
        return false;
    }
    return true;
}

/*!
 * \brief 读线圈数据
 * \param addr
 * \param num
 * \param serverId
 * \return
 */
bool MyModbusC::readCoils(int addr, quint16 num, int serverId)
{
    return true;
}

/*!
 * \brief 读保持寄存器
 * \param addr
 * \param num
 * \param serverId
 * \return
 */
bool MyModbusC::readHoldingRegisters(int addr, quint16 num, int serverId)
{
    //! 确认连接状态
    if (!_confirmConnected()) {
        return false;
    }

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, addr, num);
    if (auto *reply = m_client->sendReadRequest(readUnit, serverId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &MyModbusC::readReady);
        } else {
            delete reply;
        }
    } else {
        qDebug() << m_client->errorString();
        return false;
    }

    return true;
}

void MyModbusC::readReady()
{
    m_values.clear();

    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
//            g_tool.printLog(QString("Address: %1, Value: %2").arg(unit.startAddress())
//                            .arg(QString::number(unit.value(i))));
        }
        m_values = unit.values();
    } else {
//        qDebug() << (QString("Read response error: %1 (code: 0x%2)")
//                     .arg(reply->errorString()).arg(reply->error(), -1, 16));
    }

    reply->deleteLater();
}

/*!
 * \brief 写线圈数据
 * \param str：写入的数据
 * \param addr：起始地址
 * \param num：写入个数
 * \return
 */
bool MyModbusC::writeCoils(QString str, int addr, int num)
{
    return true;
}

/*!
 * \brief 写保持寄存器
 * \param vData：写入的数据
 * \param addr：起始地址
 * \param serverId：服务器ID
 * \return
 */
bool MyModbusC::writeHoldingRegisters(QVector<int> vData, int addr, int serverId)
{
    //! 确认连接状态
    if (!_confirmConnected()) {
        return false;
    }

    quint16 numWrite = static_cast<quint16>(vData.size());
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, addr, numWrite);
    for (int i = 0; i < vData.size(); i++) {
        writeUnit.setValue(i, vData.at(i));
//        qDebug() << i << " " << vData.at(i);
    }

    if (auto *reply = m_client->sendWriteRequest(writeUnit, serverId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << (QString("Write response error: %1 (Mobus exception: 0x%2)")
                                 .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16));
                } else if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << (QString("Write response error: %1 (code: 0x%2)").
                                arg(reply->errorString()).arg(reply->error(), -1, 16));
                } else {
  //                  qDebug() << "Write reply ok: " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
                }
                reply->deleteLater();
            });
        } else {
            reply->deleteLater();
        }
    } else {
        qDebug() << m_client->errorString();
        return false;
    }

    return true;
}

/*!
 * \brief 判断Modbus连接状态
 * \return
 */
bool MyModbusC::isActive()
{
    if (m_client->state() == QModbusDevice::ConnectedState) {
        return true;
    }
//    g_tool.printLog(QString("modbus connect fail"));
    return false;
}

/*!
 * \brief 设置服务器IP及端口
 * \param ip：IP地址
 * \param nPort：端口号
 */
void MyModbusC::setServer(QString ip, int nPort)
{
    m_sIP = ip;
    m_nPort = nPort;
}

/*!
 * \brief 设置服务器串口
 * \param portName：串口名称
 */
void MyModbusC::setServer(QString portName)
{
    m_portName = portName;
}

/*!
 * \brief 定时器事件
 * \param event
 */
void MyModbusC::timerEvent(QTimerEvent *event)
{
//    Q_UNUSED(event);
//    if (isActive()) {
//        QVector<int > vData;
//        vData.push_back(0);
//        writeHoldingRegisters(vData, MB_CONNECT);
//    }
}

/*!
 * \brief 槽：Modbus状态变化
 * \param state：Modbus设备状态
 */
void MyModbusC::onStateChanged(QModbusDevice::State state)
{
    switch (state) {
    case QModbusDevice::ConnectedState:
        qDebug("[INFO]: Modbus device %d connected.", m_nId);
        break;
    case QModbusDevice::ConnectingState:
        qDebug("[INFO]: Modbus device %d connecting.", m_nId);
        break;
    case QModbusDevice::UnconnectedState:
        qDebug("[INFO]: Modbus device %d unconnected.", m_nId);
        break;
    case QModbusDevice::ClosingState:
        qDebug("[INFO]: Modbus device %d closing.", m_nId);
        break;
    default:
        break;
    }
}

/*!
 * \brief 休眠函数
 * \param msecs：休眠时间
 */
void MyModbusC::msleep(int msecs)
{
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

/*!
 * \brief 确认连接状态
 * \return 连接状态
 */
bool MyModbusC::_confirmConnected()
{
    if (!isActive()) {
        connectToModbus();
        msleep(50);
        if (!isActive()) {  //! 若再次连接失败则退出
            return false;
        }
    }
    return true;
}
