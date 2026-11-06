#include "mymodbus_server.h"
#include "common/mymessagebox.h"

MyModbus *m_modbusS;

/*!
 * \brief MyModbus::MyModbus
 * \param parent
 */
MyModbus::MyModbus(int nType, QObject *parent) : QObject(parent)
{
    m_nType = nType;

    if(m_nType == TYPE_TCP) {
        m_server = new QModbusTcpServer;
    }
    else {
        m_server = new QModbusRtuSerialSlave;
    }

    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, struCnfg.iModbusDataLenth });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, struCnfg.iModbusDataLenth });
    m_server->setMap(reg);
    connect(m_server, &QModbusServer::dataWritten,  this, &MyModbus::dataWritten);
    connect(m_server, &QModbusServer::stateChanged, this, &MyModbus::onStateChanged);
    connect(m_server, &QModbusServer::errorOccurred,this, &MyModbus::handleDeviceError);
    m_server->setValue(QModbusServer::DeviceBusy, 0x0000);
    m_server->setValue(QModbusServer::ListenOnlyMode, false);

    sDetectdata.resize(40);
    sDetectdata.fill(0);

    m_pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(0);
    m_analyze = gFlow.getSpecifyAnalyzeGlobal(0);
}

/*!
 * \brief 设置服务器IP及端口
 * \param ip：IP地址
 * \param nPort：端口号
 */
void MyModbus::setServer(QString ip, int nPort)
{
    m_sIP = ip;
    m_nPort = nPort;
}

/*!
 * \brief 设置服务器串口
 * \param portName：串口名称
 */
void MyModbus::setServer(QString portName)
{
    m_portName = portName;
}

/*!
 * \brief 连接MODBUS服务器
 * \return
 */
bool MyModbus::connectToModbus(void)
{
    if (!m_server) {
        return false;
    }

    //! 若设备已连接则断开连接
    if (m_server->state() == QModbusDevice::ConnectedState) {
        m_server->disconnectDevice();
    }

    //! 连接Modbus设备
    if(m_nType == TYPE_TCP) {
        m_server->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_sIP);
        m_server->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_nPort);
    }
    else {
        m_server->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
        m_server->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 9600);
        m_server->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
        m_server->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);
        m_server->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_portName);
    }
    m_server->setServerAddress(m_nId);
    if (!m_server->connectDevice()) {
        return false;
    }

    return true;
}

void MyModbus::disConnect()
{
    if (m_server) {
        m_server->disconnectDevice();
        m_server->disconnect();
        delete m_server;
        m_server = nullptr;
    }
}

/*!
 * \brief 读取modbus数据
 */
void MyModbus::dataWritten(QModbusDataUnit::RegisterType table, int address, int size)
{
    for (int i = 0; i < size; ++i) {
        quint16 value;
        bool ok = false;
        switch (table) {
        case QModbusDataUnit::Coils:
            m_server->data(QModbusDataUnit::Coils, address + i, &value);
            break;
        case QModbusDataUnit::HoldingRegisters:
            m_server->data(QModbusDataUnit::HoldingRegisters, address + i, &value);
            break;
        default:
            break;
        }
//        g_tool.printLog(QString("----------address %1 = %2-------").arg(address+i).arg(QString::number(value).toInt(&ok,10)));

//        if(strAlg.nMaterialType == MATERIAL_RICE)
        _analyzeRiceData(address+i, value);
    }    
}

void MyModbus::_analyzeRiceData(int address, quint16 value)
{
    switch (address) {
    case MD_FEED_STATUS:
//        if((value == 1 && !m_pAnalyzeShare->bFeeder && !m_pAnalyzeShare->bWipe) || (value == 0 && m_pAnalyzeShare->bFeeder)) {
//            g_processData.sendStartType(2);
//            emit sigFeederCtrl();
//        }
        break;
    case MD_RICE_SCHM:
//        if(!m_pAnalyzeShare->bFeeder && value != m_analyze->curTargetSchm) {
//            m_analyze->curTargetSchm = value;
//            emit sigChgSchm();
//        }
        break;
    case MD_RICE_PEOPLE:
//        if(value != gs_consoleGlobal.curPeopleNo) {
//            gs_consoleGlobal.curPeopleNo = value;
//            emit sigChgPeople();
//        }
        break;
    case MD_RICE_ORIGINAL:
//        if(value != gs_consoleGlobal.curOriginNo) {
//            gs_consoleGlobal.curOriginNo = value;
//            emit sigChgOriginal();
//        }
        break;
    case MD_FEED_AMOUNT:
    {
//        int mergeFeeder = g_tool.calcFeedMount(m_analyze->nFeederSeg*100+m_analyze->nFeeder);
//        if(value > 0 && mergeFeeder != value) {
//            mergeFeeder = value;
//            g_udpSocket[0]->sendAccCmd(m_pAnalyzeShare->bFeeder, mergeFeeder/256, mergeFeeder%256);
//        }
    }
        break;
//    case MD_CLEAR:
//        emit sigClear();
//        break;
    case MD_LK2360_SAMPLE:
    if(!m_pAnalyzeShare->bFeeder && value >=1 && value <=4) {
//        QString str;
//        m_pAnalyzeShare->curSampleIndex = value-1;
//        QStringList strList;
//        if(gs_consoleGlobal.sampleList.contains(",")) {
//            strList = gs_consoleGlobal.sampleList.split(",");
//        }
//        else if(gs_consoleGlobal.sampleList.compare("") != 0) {
//            strList << gs_consoleGlobal.sampleList;
//        }
//        memcpy(m_pAnalyzeShare->curOrigin, strList[m_pAnalyzeShare->curSampleIndex].toStdString().data(), 80);
//        emit sigRefreshStatisHeader(0,strList[m_pAnalyzeShare->curSampleIndex]);
//        g_udpSocket[0]->onSendSVCmdSlt(0,m_pAnalyzeShare->curSampleIndex);
    }
        break;
    case MD_DETECTNO_ENA:
        if(value == 1) {
//            QString sDetectSN;
//            for(int i = 0; i < 40; i++) {
//                if(sDetectdata[i] == 0)
//                    break;
//                sDetectSN += QChar(sDetectdata[i]);
//            }
//            if(sDetectSN.compare(g_consoleShare.sDetectSN) != 0) {
//                g_consoleShare.sDetectSN = sDetectSN;
//                emit sigRefreshDetectSN();
//            }
        }
        else {
//            for(int i = 0; i < 40; i++) {
//                sDetectdata[i] = 0;
//            }
        }
        break;
    default:
        break;
    }

    if(address >= MD_DETECTNO1 && address < MD_DETECTNO1+19) {
        sDetectdata[(address-MD_DETECTNO1)*2]   = value%256;
        sDetectdata[(address-MD_DETECTNO1)*2+1] = value/256;
//        g_tool.printLog(QString("----------address %1 = %2-------").arg(address).arg(value%256).arg(value/256)
//                       +QString(QChar(sDetectdata[(address-MD_DETECTNO1)*2]))
//                       +QString(QChar(sDetectdata[(address-MD_DETECTNO1)*2+1])));
    }
}

/*!
 * \brief 发送modbus数据
 */
void MyModbus::setRegister(QList<int> lData, QList<int> lAddress)
{
    if (!m_server || m_server->state() == QModbusDevice::UnconnectedState)
        return;

    int id = 0;
    foreach (int iData, lData) {
        //if(lAddress[id] >= MD_DETECTNO1)
        //    g_flow.printLog(QString("---------setData %1 %2 %3----------").arg(lAddress[id]).arg(iData%256).arg(iData/256));
        bool ok = true;
        ok = m_server->setData(QModbusDataUnit::HoldingRegisters, lAddress[id], iData);
        id++;
    }
}

/*!
 * \brief 判断Modbus连接状态
 * \return
 */
bool MyModbus::isActive()
{
    if (m_server->state() == QModbusDevice::ConnectedState) {
        return true;
    }
//    g_tool.printLog("modbus connect fail");
    return false;
}

/*!
 * \brief 槽函数: modbus通讯异常
 */
void MyModbus::handleDeviceError(QModbusDevice::Error newError)
{
    if (newError == QModbusDevice::NoError || !m_server)
        return;

    infoWidget->raise();
    infoWidget->setLabelText(m_server->errorString());
    infoWidget->showSecs();
}

/*!
 * \brief 定时器事件
 * \param event
 */
void MyModbus::timerEvent(QTimerEvent *event)
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
void MyModbus::onStateChanged(QModbusDevice::State state)
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
void MyModbus::msleep(int msecs)
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
bool MyModbus::_confirmConnected()
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
