#include "modbusctlclass.h"
#include "core/globalflow/globalflow.h"
#include "params/globalparams.h"

modbusCtlClass::modbusCtlClass(QObject *parent)
    : QObject(parent)
{
    m_myModbus = new MyModbusC(TYPE_SERIAL);
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    QString portName;
    bool bExist = false;
    for(int i = 0; i < ports.size(); i++){
        portName = ports[i].portName();
        if("ttyS0" == portName)
            bExist = true;
    }
    //! ttyS0 422
    if(bExist)
        m_myModbus->setServer("ttyS0");
    else if(ports.size() >= 2)
        m_myModbus->setServer(ports[1].portName());
    m_myModbus->connectToModbus();
    gFlow.msleep(10);
    set_state = false;
    get_state = false;
}

/**
 * @brief modbusCtlClass::获取砻谷机数据
 */
void modbusCtlClass::getMillData()
{

}

void modbusCtlClass::powerAnaCmd(int index, bool bOn)
{
    QVector<int> vData;
    if(bOn) {
        vData.push_back(0x1000);
    }else {
        vData.push_back(0);
    }
    m_myModbus->writeHoldingRegisters(vData, PLC_ANAPOWER_ADDR+PLC_ANAPOWER_OFFSET*index);
    gFlow.msleep(10);
}

bool modbusCtlClass::getAnaPower(int index)
{
    bool bNewPlc = false;
    QString plcVerStr = struGsh.sPLCVersion;
    if("NULL" != plcVerStr){
        if(plcVerStr.mid(8,4).toInt() > 100)
            bNewPlc = true;
    }

    if(bNewPlc){
        m_myModbus->readHoldingRegisters(PLC_ANA_POWER + PLC_ANAPOWER_OFFSET*index, 1);
        gFlow.msleep(30);
        int ret = -1;
        QVector<quint16> verDatas = m_myModbus->values();
        if(verDatas.count() > 0){
            for(int i=0; i<verDatas.count(); i++){
                ret = verDatas[i];
                break;
            }
        }
        return (1==ret);
    } else {
        auto pAnalyze = gFlow.getSpecifyAnalyzeShare(index);
        if(pAnalyze != nullptr){
            for(int i = 0; i < MAX_CAM_NUM; ++i)
            {
                strcpy(pAnalyze->sCamVersion[i], "NULL");
            }
            strcpy(pAnalyze->sCtrlVersion, "NULL");
            strcpy(pAnalyze->sPulseVersion, "NULL");
            strcpy(pAnalyze->sSamVersion[0], "NULL");
            strcpy(pAnalyze->sSamVersion[1], "NULL");
//            gFlow.checkVersionIsOk(index);
//            if("NULL" == QString(pAnalyze->sCamVersion[CAM_1])
//                    && "NULL" == QString(pAnalyze->sCamVersion[CAM_2])
//                    && "NULL" == QString(pAnalyze->sCamVersion[CAM_3])
//                    && "NULL" == QString(pAnalyze->sCtrlVersion)
//                    && "NULL" == QString(pAnalyze->sPulseVersion)) {
//                return false;
//            }else{
//                return true;
//            }
        } else
            return false;
    }
}

void modbusCtlClass::powerMachineCmd(int anaIndex, int machineIndex, bool bOn)
{
    QVector<int> vData;
    if(bOn) {
        vData.push_back(0x1000);
    }else {
        vData.push_back(0);
    }
    m_myModbus->writeHoldingRegisters(vData, PLC_ANAPOWER_ADDR+PLC_ANAPOWER_OFFSET*anaIndex+(PLC_MACHINE_OFFSET*(machineIndex+1)));
    gFlow.msleep(10);
}

void modbusCtlClass::getPlcVersion()
{
    m_myModbus->readHoldingRegisters(PLC_VER, 3);
    gFlow.msleep(30);
    QVector<quint16> verDatas = m_myModbus->values();
    if(verDatas.count() > 0){
        QString ver;
        ver.clear();
        for(int i=0; i<verDatas.count(); i++){
            QString datas = QString::number(verDatas[i],16);
            ver += datas.rightJustified(4, '0');
        }
        struGsh.sPLCVersion = ver;
    }
    emit sigGetPlcVer();
}

//软关机
void modbusCtlClass::softShutDown()
{
    QVector<int> vData;
    vData.push_back(0x1000);

    m_myModbus->writeHoldingRegisters(vData, PLC_SERVER_SOFT_SHUTDOWN);
    gFlow.msleep(10);
}

void modbusCtlClass::alarmLightSet(bool bRed, bool bYellow, bool bGreen)
{
    QVector<int> vDataRed, vDataYellow, vDataGreen;
    vDataRed.push_back(bRed ? 0x1000: 0x00);
    vDataYellow.push_back(bYellow ? 0x1000: 0x00);
    vDataGreen.push_back(bGreen ? 0x1000: 0x00);

    m_myModbus->writeHoldingRegisters(vDataRed, PLC_RED_LIGHT_ALARM);
    m_myModbus->writeHoldingRegisters(vDataYellow, PLC_YELLOW_LIGHT_ALARM);
    m_myModbus->writeHoldingRegisters(vDataGreen, PLC_GREEN_LIGHT_ALARM);

    gFlow.msleep(10);
}
/**
 * @brief modbusCtlClass::设置碾米机数据
 * @param index
 * @param nCmd
 */
int modbusCtlClass::setMillData(QVector<int> vData, int nCmd, int nTimeOut)
{
    int count = 0, ret = -1;
#ifdef Q_OS_UNIX
//    while (get_state && count < 3) {
//        count++;
//        gFlow.msleep(50);
//    }
//    if (count == 3) {
//        return -1;
//    }
    set_state = true;
    m_myModbus->writeHoldingRegisters(vData, nCmd);
    gFlow.msleep(nTimeOut);
    set_state = false;
    ret = 0;
#endif
    return ret;
}

/**
 * @brief modbusCtlClass::控制砻谷机
 * @param data
 * @param nCmd
 */
void modbusCtlClass::onCtrMillByPlcSlt(int data, int nCmd)
{
    QVector<int> vData;
    vData.push_back(data);
    vData.push_back(0x1000);
    set_state = true;
    m_myModbus->writeHoldingRegisters(vData, nCmd);
    gFlow.msleep(20);
    set_state = false;
}


modbusCtlClass* g_modbusCtlClass;
