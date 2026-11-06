#include "mysocketmgr.h"
#include "core/io/modbus/modbustcpclass.h"
#include "core/globalflow/globalflow.h"

MySocketMgr *g_socketMgr;

MySocketMgr::MySocketMgr(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_UNIX
    m_socketio = new MySocketio;
    qRegisterMetaType<string>("string");
    connect(m_socketio, SIGNAL(receive(string)), this, SLOT(onReceiveSlt(string)));
#endif
//    connect(this,             SIGNAL(sigControlData(int,int)), g_modbusTcpClass, SLOT(sendControlData(int,int)));
    m_AnalyzeIndex = 0;
    m_MachineIndex = 0;
}

MySocketMgr::~MySocketMgr()
{
    disConnectServer();
}

#ifdef Q_OS_UNIX
MySocketio* MySocketMgr::getSocketio()
{
    return m_socketio;
}
#endif

void MySocketMgr::connectServer()
{
#ifdef Q_OS_UNIX
    m_socketio->connect(URL_MSG_SERVER);
    QString code = struCnfg.analyzeInfo[0]->analyzeModel.sModelCode;
    m_socketio->sendLogin(code.toStdString());
    m_socketio->sendLineLogin(code.toStdString());
#endif
}

void MySocketMgr::disConnectServer()
{
#ifdef Q_OS_UNIX
    m_socketio->disconnect();
#endif
}

bool MySocketMgr::getConnectState()
{
#ifdef Q_OS_UNIX
    return m_socketio->getConnectState();
#else
    return true;
#endif
}

void MySocketMgr::sendHeart()
{
    QString code = struCnfg.analyzeInfo[0]->analyzeModel.sModelCode;

    JsonObject json;
    json["from"] = code;
    json["to"] = "system";
    json["type"] = 1;
    json["data"] = "";
    json["msg"] = "are you online?";
    json["code"] = 0;
#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif
#ifdef Q_OS_UNIX
    m_socketio->sendHeart(QString(QJson::serialize(json)).toStdString());
#endif

    json["to"] = code;
    json["data"] = "1";
    json["msg"] = QString("i am online");
    json["code"] = CMD_HEART_APP;
#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif
#ifdef Q_OS_UNIX
    m_socketio->sendRoom(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::onReceiveSlt(string data)
{
    if(!getConnectState())
        return;

    QString str = QString::fromStdString(data);
#if 0
    //g_tool.printLog(str);
    printf(qPrintable(str));
    printf("\n");
    fflush(0);
#endif
    if(str == "")return;

    JsonObject json = QJson::parse(str).toMap();
    sAppID = json["from"].toString();
    int cmd = json["code"].toInt();
    int analyze_index = 0, sorter_index = 0;
    gFlow.getAnalyzeAndSorterIndex(sAppID, analyze_index, sorter_index);
    //qDebug() << "receive sorter analyze: " << analyze_index << " sorter_index: " << sorter_index;

    analyzeShare  *m_pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(analyze_index);
    analyzeGlobal *m_pAnalyze      = gFlow.getSpecifyAnalyzeGlobal(analyze_index);

//    QString code = struCnfg.analyzeInfo[0]->analyzeModel.sModelCode;
    QString code = struCnfg.master_device_code;

    if (json["to"].toString() ==  code)
    {
        switch (cmd) {
        case CMD_REQ_MAIN:
            sendMainUI();
            break;
        case CMD_REQ_IP:
            emit sigGetVPNIP();
            break;
        case CMD_CTRL_VPN:
//            closeVPN(json);
            break;
        case SDK_SORTER_OBTAIN_INFO:
            parseSDKInfoFromSorter(analyze_index, sorter_index, str, m_pAnalyzeShare->sorterFeedbackData);
            break;
        case SDK_SORTER_OBTAIN_OUTPUT:
            if(sAppID == m_curSorterCode){
                parseOutputInfoFromSorter(str, m_pAnalyzeShare->sorterFeedbackData);
            }
            break;
        case SDK_SORTER_OBTAIN_ALARM:
            if(sAppID == m_curSorterCode){
                parseAlarmInfoFromSorter(str, m_pAnalyzeShare->sorterFeedbackData);
            }
            break;
        }
    }
}

void MySocketMgr::sendMainUI()
{
    if(!getConnectState())
        return;

    JsonObject json_data;
    json_data["status"] = 1;
    json_data["ip"] = struGsh.sIp;
    json_data["switch"] = gFlow.getSpecifyAnalyzeShare(0)->bFeeder ? 1 : 0;

    QString code = struCnfg.analyzeInfo[0]->analyzeModel.sModelCode;

    JsonObject json;
    json["from"] = code;
    json["to"] = code;
    json["data"] = QJson::serialize(json_data);
    json["msg"]  = QString("send main info");
    json["code"] = CMD_REQ_MAIN;
#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif
#ifdef Q_OS_UNIX
    m_socketio->sendRoom(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::onReqVPNIPSlt()
{
    if(!getConnectState())
        return;

    JsonObject json_data;
    if(struGsh.sIp.isEmpty()){
        json_data["code"] = 2;
        json_data["msg"] = "获取IP地址失败";
    }else{
        json_data["code"] = 0;
        json_data["msg"] = "获取IP地址成功";
        json_data["data"] = struGsh.sIp;
    }

    QString code = struCnfg.analyzeInfo[0]->analyzeModel.sModelCode;

    JsonObject json;
    json["from"] = code;
    json["to"]   = sAppID;
    json["type"] = CMD_TYPE_RESPONSE;
    json["data"] = QString(QJson::serialize(json_data));
    json["msg"]  = QString("reponse vpn ip");
    json["code"] = CMD_REQ_IP;

#ifdef Q_OS_UNIX
    m_socketio->sendMsg(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::closeVPN(JsonObject json)
{
    emit sigCloseVpn();

    JsonObject jsonResponse;
    jsonResponse["data"] = json["data"];
    jsonResponse["type"] = CMD_TYPE_RESPONSE;
    jsonResponse["code"] = CMD_CTRL_VPN;
    jsonResponse["from"] = json["to"];
    jsonResponse["to"] = json["from"];
    jsonResponse["msg"] = json["msg"];

#ifdef Q_OS_UNIX
    m_socketio->sendMsg(QString(QJson::serialize(jsonResponse)).toStdString());
#endif
//    qDebug() << QJson::serialize(jsonResponse);
}

void MySocketMgr::requestSorterSchm(int analyzeIndex, QString sorterCode)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = sorterCode;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"] = sorterCode;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = "";
    json["msg"] = QString("request sorter scheme");
    json["code"] = CMD_REQ_SCHMBIND;

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::requestSorterOperatorInfo(int analyzeIndex, QString sorterCode, int machineIndex)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = sorterCode;
    m_MachineIndex = machineIndex;

    JsonObject json;
    json["from"] = struCnfg.analyzeInfo[0]->analyzeModel.sModelCode;
    json["to"]   = sorterCode;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = 0;
    json["msg"]  = QString("request status info from sorter");
    json["code"] = SDK_SORTER_OBTAIN_INFO;

    m_isRequest = true;

#if 0
    //g_tool.printLog(QString(QJson::serialize(json)));
    printf(qPrintable(QString(QJson::serialize(json))));
    printf("\n");
    fflush(0);
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::requestSorterOutputInfo(int analyzeIndex, QString sorterCode)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = sorterCode;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"]   = sorterCode;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = 0;
    json["msg"]  = QString("request status info from sorter");
    json["code"] = SDK_SORTER_OBTAIN_OUTPUT;

#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::requesrSorterAlarmStatusInfo(int analyzeIndex, QString sorterCode)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = sorterCode;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"]   = sorterCode;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = 0;
    json["msg"]  = QString("request status info from sorter");
    json["code"] = SDK_SORTER_OBTAIN_ALARM;

#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::sendFeederMsgToSorter(int analyzeIndex, QString curSorter, bool bFeeder)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = curSorter;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"]   = curSorter;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = bFeeder ? 1 : 2;
    json["msg"]  = QString("switch vibrator %1").arg(bFeeder ? "On" : "Off");
    json["code"] = SDK_SORTER_FEEDER;

#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::sendChgSchmInfo(int analyzeIndex, QString curSorter, int nSchm)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = curSorter;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"]   = curSorter;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = nSchm;
    json["msg"]  = QString("switch target scheme");
    json["code"] = SDK_SORTER_SCHM;

#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::sendFeederValueInfo2Sorter(int analyzeIndex, QString curSorter, int index, int nOutput)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = curSorter;

    JsonObject json_data;
    json_data["index"] = index;
    json_data["value"] = nOutput;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"]   = curSorter;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = QJson::serialize(json_data);
    json["msg"]  = QString("ctrl output");
    json["code"] = SDK_SORTER_OUTPUT;

#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

void MySocketMgr::sendOperSensInfo2Sorter(int analyzeIndex, QString curSorter, int index, int nSens)
{
    if(!getConnectState())
        return;

    analyzeGlobal *m_analyze = gFlow.getSpecifyAnalyzeGlobal(analyzeIndex);
    m_curSorterCode = curSorter;

    JsonObject json_data;
    json_data["index"] = index;
    json_data["value"] = nSens;

    JsonObject json;
    json["from"] = m_analyze->analyzeModel.sModelCode;
    json["to"]   = curSorter;
    json["type"] = CMD_TYPE_REQUEST;
    json["data"] = QJson::serialize(json_data);
    json["msg"]  = QString("ctrl sensitivity");
    json["code"] = OPER_SET_SENS;

#if 0
    g_tool.printLog(QString(QJson::serialize(json)));
#endif

#ifdef Q_OS_UNIX
    m_socketio->sendMsgToSorter(QString(QJson::serialize(json)).toStdString());
#endif
}

/*! 接收色选机状态信息 */
void MySocketMgr::parseSDKInfoFromSorter(int analyze_index, int sorter_index, QString str, struSorterData &sorterData)
{
    if(str.isEmpty())
        return;

    bool isChanged = false;
    JsonObject json0 = QJson::parse(str).toMap();
    JsonObject json = QJson::parse(json0["data"].toString()).toMap();
    JsonObject json_data;

    sorterData.nOnOff = true;
    int onOff = json["switch"].toInt();
    if(onOff == 1){
        sorterData.feedOnOff = 1;
    }else if(onOff == 2){
        sorterData.feedOnOff = 0;
    }else{//! 清灰
        sorterData.feedOnOff = 2;
    }

    int feederDir = json["feederDir"].toInt();
    if(0 == feederDir){
        sorterData.bTickPositive = true;
    }else{
        sorterData.bTickPositive = false;
    }

    analyzeShare *m_pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(analyze_index);

    if(m_pAnalyzeShare->analyzeOperInfo[sorter_index].feedOnOff != sorterData.feedOnOff){
        isChanged = true;
        m_pAnalyzeShare->analyzeOperInfo[sorter_index].feedOnOff = sorterData.feedOnOff;
    }
    sorterData.nCurSchem = json["scheme"].toInt();
    if(m_pAnalyzeShare->analyzeOperInfo[sorter_index].nCurSchem != sorterData.nCurSchem){
        isChanged = true;
        m_pAnalyzeShare->analyzeOperInfo[sorter_index].nCurSchem = sorterData.nCurSchem;
    }
    QString sorterOperSchm;
    sorterOperSchm.clear();
    JsonArray vjsonType = QJson::parse(QJson::serialize(json["schemeList"])).toList();
    for (int i = 0; i < vjsonType.size(); i++) {
        json_data = QJson::parse(QJson::serialize(vjsonType[i])).toMap();
        sorterOperSchm += json_data["name"].toString() + ";";
    }

    sorterOperSchm = sorterOperSchm.left(sorterOperSchm.length()-1);
    memcpy(m_pAnalyzeShare->sorterFeedbackData.sorterSchmList, sorterOperSchm.toStdString().data(), 500);

    if(!strcmp(m_pAnalyzeShare->analyzeOperInfo[sorter_index].sorterSchmList,sorterData.sorterSchmList)){
        isChanged = true;
        memcpy(m_pAnalyzeShare->analyzeOperInfo[sorter_index].sorterSchmList, sorterData.sorterSchmList,500);
    }
    vjsonType.clear();
    vjsonType = QJson::parse(QJson::serialize(json["feed"])).toList();
    sorterData.tickGroupTotal = vjsonType.size();
    if(m_pAnalyzeShare->analyzeOperInfo[sorter_index].tickGroupTotal != sorterData.tickGroupTotal){
        isChanged = true;
        m_pAnalyzeShare->analyzeOperInfo[sorter_index].tickGroupTotal = sorterData.tickGroupTotal;
    }
    for (int i = 0; i < sorterData.tickGroupTotal; i++) {
        json_data = QJson::parse(QJson::serialize(vjsonType[i])).toMap();
        if(i == json_data["index"]){
            sorterData.feederValue[i] = json_data["value"].toInt();
            if(m_pAnalyzeShare->analyzeOperInfo[sorter_index].feederValue[i] != sorterData.feederValue[i]){
                isChanged = true;
                m_pAnalyzeShare->analyzeOperInfo[sorter_index].feederValue[i] = sorterData.feederValue[i];
            }
        }
    }
    vjsonType.clear();
    for(int i=0; i<ARITHMETIC_TOTAL; i++){
//        memset(m_pAnalyzeShare->sorterFeedbackData.sensName, 0, sizeof(m_pAnalyzeShare->sorterFeedbackData.sensName));
        memset(m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensName[i], 0, 50*sizeof(char));
        m_pAnalyzeShare->sorterFeedbackData.sensValue[i] = 0;
        m_pAnalyzeShare->sorterFeedbackData.sensEna[i] = false;
        m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensEna[i] = false;
        m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensValue[i] = 0;
    }
    vjsonType = QJson::parse(QJson::serialize(json["sensitivity"])).toList();

    for(int i=0; i < vjsonType.size(); i++){
        json_data = QJson::parse(QJson::serialize(vjsonType[i])).toMap();
        QString sensName = json_data["name"].toString();
    int arith_index = json_data["code"].toInt();
    int arith_value = json_data["value"].toInt();
        memcpy(sorterData.sensName[arith_index], sensName.toStdString().data(), 50*sizeof(char));
        sorterData.sensValue[arith_index] = arith_value;
        sorterData.sensEna[arith_index] = true;

        if (strcmp(m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensName[arith_index], sorterData.sensName[arith_index])) {
            isChanged = true;
            memcpy(m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensName[arith_index], sorterData.sensName[arith_index], 50);
        }

        if (m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensValue[arith_index] != sorterData.sensValue[arith_index]) {
            isChanged = true;
            m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensValue[arith_index] = sorterData.sensValue[arith_index];
        }

        if (m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensEna[arith_index] != sorterData.sensEna[arith_index]) {
            isChanged = true;
            m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensEna[arith_index] = sorterData.sensEna[arith_index];
        }
    }

    m_pAnalyzeShare->sorterFeedbackData.bSorterReqOtherFlag = true;

    if (isChanged) {
        int machine_index = gFlow.getMachineIndexInServer(analyze_index, sorter_index, 0)-1;
        //! 更新界面
        emit sigRefreshDisplayPage(analyze_index, sorter_index);

        //! 更新PLC中控数据
        //! 震动量
        int nValueCmd = (WRITE_MODE_OFFSET+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
        int nControlCmd = (WRITE_MODE_ONOFF_OFFSET+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
        emit sigControlData(nControlCmd, 1);
        gFlow.msleep(15);

        int tmp = m_pAnalyzeShare->analyzeOperInfo[sorter_index].tickGroupTotal-1;
        if(tmp >= 0 && tmp < MAX_GROUP_TICK)
            emit sigControlData(nValueCmd, m_pAnalyzeShare->analyzeOperInfo[sorter_index].feederValue[tmp]);
        for (int i = 0; i < ARITHMETIC_TOTAL; i++) {
            int arith_state = 0;
            if (m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensEna[i]) {
                arith_state = 1;
            } else {
                arith_state = 0;
            }
            if (i == ARITH_GREY_A || i == ARITH_GREY_B || i == ARITH_DISCOLOR_A) { //! 病斑 整白
                nValueCmd = (i+1+WRITE_MODE_OFFSET+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
                nControlCmd = (i+1+WRITE_MODE_ONOFF_OFFSET+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
                emit sigControlData(nControlCmd, arith_state);
                gFlow.msleep(15);
                emit sigControlData(nValueCmd, m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensValue[i]);
                gFlow.msleep(5);
            } else if (i == ARITH_INTEL_A) {
                nValueCmd = (i+WRITE_MODE_OFFSET+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
                nControlCmd = (i+WRITE_MODE_ONOFF_OFFSET+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
                emit sigControlData(nControlCmd, arith_state);
                gFlow.msleep(15);
                emit sigControlData(nValueCmd, m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensValue[i]);
                gFlow.msleep(5);
            } else if (i == ARITH_SCALE) {
                nValueCmd = (i+8+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
                nControlCmd = (i+18+((struCnfg.productId - 1)*5 + struCnfg.server_Id-1)*MAX_MACHINE_MODE_NUM)*MACHINE_BASE_ADDR+machine_index+1;
                emit sigControlData(nControlCmd, arith_state);
                gFlow.msleep(15);
                emit sigControlData(nValueCmd, m_pAnalyzeShare->analyzeOperInfo[sorter_index].sensValue[i]);
                gFlow.msleep(5);
            }
        }
    }

    if(!m_isRequest) {
        if(isChanged) {
            emit sigRefreshSorterOperPage();
        }
    }else{
        m_isRequest = false;
    }
}

/*! 接收色选机产量状态信息 */
void MySocketMgr::parseOutputInfoFromSorter(QString str, struSorterData &sorterData)
{
    if(str.isEmpty())
        return;
    JsonObject json = QJson::parse(str).toMap();
    QString out = json["value"].toString();
    if(out != QString("--.--")){
        sorterData.nOutPut = out.toDouble();
    }
}

/*! 接收色选机报警状态信息 */
void MySocketMgr::parseAlarmInfoFromSorter(QString str, struSorterData &sorterData)
{
    if(str.isEmpty())
        return;

    analyzeShare  *m_pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(0);
    struSorterData &tmp = m_pAnalyzeShare->sorterFeedbackData;

    JsonObject jsondetail = QJson::parse(str).toMap();
    JsonObject json_data;
    JsonArray vjsonType = QJson::parse(QJson::serialize(jsondetail["alarmValue"])).toList();
    for(int i=0; i<vjsonType.size(); i++){
        json_data = QJson::parse(QJson::serialize(vjsonType[i])).toMap();
        if(i == json_data["code"]){
            tmp.sorterStatus[i] = json_data["value"].toInt();
        }
    }
}

/**
 * @brief MySocketMgr::获取色选机信息
 */
void MySocketMgr::getSorterInfo()
{
    if (!struGsh.nHasSorter) return;

    if(struGsh.nNetwortStat != NETWORK_NORMAL) return;

    QString machine_code;
    //! 挨个分析仪获取色选机信息
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        int nMachineNum = struCnfg.analyzeInfo[i]->machineNum;
        for (int j = 0; j < nMachineNum; j++) {
            int type = struCnfg.analyzeInfo[i]->machineList[j]->modelMachine.model_type;
            if (type == MACHINE_SORTER) {
                machine_code = QString(struCnfg.analyzeInfo[i]->machineList[j]->modelMachine.sModelCode);
                struGsh.analyzeInfo[i]->machineShareList[j]->isSample = false;
                requestSorterOperatorInfo(i, machine_code, j);
                gFlow.msleep(50);
            }
        }

    }
}
