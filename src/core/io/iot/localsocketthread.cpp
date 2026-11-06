#include <QJsonDocument>
#include <QDebug>
#include <QtMath>
#include "localsocketthread.h"

LocalSocketThread* g_Lotsocket;

LocalSocketThread::LocalSocketThread(QObject *parent)
    : QObject(parent)
{

    m_localSocket = nullptr;

    m_localSocketthread = new QThread();
    this->moveToThread(m_localSocketthread);
    m_localSocketthread->start();

    initMapPacketFunctions();
//    initMapControlFunctions();
}



/*!
 * \brief 启用外挂程序
 */
void LocalSocketThread::onInitLocalSocket()
{
    if (m_localSocket != nullptr) {
        return;
//        delete m_localSocket;
//        m_localSocket = nullptr;
    }
    m_localSocket = new LocalSocket();
    connect(m_localSocket, SIGNAL(readLocalSocketData(QByteArray)),
            this, SLOT(onRecieveDatas(QByteArray)));
    connect(m_localSocket, SIGNAL(connectedSuccess()),
            this, SLOT(onSendMachineInfo()));
    connect(this, SIGNAL(sigHandleMsg(QJsonObject)), this, SLOT(onHandleMsg(QJsonObject)));
}

/*!
 * \brief 初始化打包函数map
 */
void LocalSocketThread::initMapPacketFunctions()
{
    m_mapPacketFunction.insert("attributes", &LocalSocketThread::packetProperty);
    m_mapPacketFunction.insert("version",    &LocalSocketThread::packetversion);
    m_mapPacketFunction.insert("scheme",     &LocalSocketThread::packetScheme);
//    m_mapPacketFunction.insert("component",  &LocalSocketThread::packetKeyComponents);
    m_mapPacketFunction.insert("telemetry",  &LocalSocketThread::packetTelemetry);

}

/*!
 * \brief 初始化服务器回调函数
 */
void LocalSocketThread::initMapControlFunctions()
{
//    m_mapCtrlHandleFunction.insert("shutdown", &LocalSocketThread::handleCtrlShutdown);
//    m_mapCtrlHandleFunction.insert("feeder", &LocalSocketThread::handleCtrlFeeder);
//    m_mapCtrlHandleFunction.insert("current_scheme_id", &LocalSocketThread::handleCtrlScheme);
//    m_mapCtrlHandleFunction.insert("wipe", &LocalSocketThread::handleCtrlWipe);
//    m_mapCtrlHandleFunction.insert("vibrator", &LocalSocketThread::handleCtrlVibrator);
//    m_mapCtrlHandleFunction.insert("arith", &LocalSocketThread::handleCtrlArith);
}

/*!
 * \brief 槽：处理服务器数据
 * \param ba
 */
void LocalSocketThread::onRecieveDatas(QByteArray ba)
{
    QJsonObject JsonObject = QJsonDocument::fromJson(ba).object();

    handleMessage(JsonObject);
}

/*!
 * \brief 槽: 发送数据
 */
void LocalSocketThread::onSendData()
{
    //! 检查LocalSocket状态
    if (m_localSocket == nullptr || !m_localSocket->isConnected()) {
        return;
    }

    //! 上传数据
    foreach (auto key, m_mapPacketFunction.keys()) {
        auto func = m_mapPacketFunction[key];
        QJsonObject jsonObject;
        jsonObject["key"] = key;
        jsonObject["data"] = func(this);
        sendJson(jsonObject);
    }
}

/*!
 * \brief 打包属性
 * \return
 */
QJsonObject LocalSocketThread::packetProperty()
{
    QJsonObject jsondata;
    jsondata["lifespan"] = g_delagCode.getExpireDate().toString("yyyyMMddhhmmss");
    jsondata["device_type"] = struCnfg.nMachineType == nMachine_Bottle ? "KB1" : "KA1";

    return jsondata;
}

/*!
 * \brief 打包版本
 * \return
 */
QJsonObject LocalSocketThread::packetversion()
{
    QJsonObject jsonData;
    jsonData["arm_version"] = packProVersionArm();
    jsonData["fpga_version"] = packProVersionFPGA();

    return jsonData;
}

/*!
 * \brief 打包关键部件
 * \return
 */
QJsonObject LocalSocketThread::packetKeyComponents()
{
    QJsonObject json;
    json["vibrator"] = packVibrator();
    json["servo"] = packElectric();
    json["lamp"] = packLampstatus();
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        json["halogen"] = packHalogen();
    }
    json["channel_major"] = packChannelMajor();
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist) {
        json["channel_assist"] = packChannelAssist();
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV){
        QJsonArray jsonArray;
        QJsonObject jsonObj;
        jsonObj["id"] = 0;
        jsonObj["power"] =0;
        jsonObj["content"] = "";
        jsonArray.append(jsonObj);
        json["ejector_power"] = jsonArray;
    }

    QVector<int> vInfId;
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        if(gFlow.bIsIMSerialCam(i)){
            vInfId.push_back(i);
        }
    }
    if (vInfId.size() > 0) {
        json["camera_temperature"] = packCameraTemperature();
    }

    return json;
}


/*!
 * \brief 打包方案数据
 * \return
 */
QJsonObject LocalSocketThread::packetScheme()
{
    QJsonArray schmeary;
    for(int i=0; i<struCnfg.nSchemTotal; i++){
        QJsonObject schmeobj;
        schmeobj["id"] = i;
        schmeobj["name"] = struCnfg.schemeList[i].name;
        schmeary.append(schmeobj);
    }


    QJsonObject json;
    json["current_scheme_id"] = struCnfg.nCntScheme;
    json["schemes"] = schmeary;

    return json;
}


/*!
 * \brief 上传信息
 */
void LocalSocketThread::uploadInfo(QJsonObject json)
{
    emit sigHandleMsg(json);
}



/*! 上传日志
 * \brief MyLocalSocketThread::uploadLogs
 * \param mode 1-自动；2-手动
 */
void LocalSocketThread::uploadLogs(int mode)
{
    QJsonObject jsonObj;
    jsonObj["key"] = KEY_UPLOADFILE;
    QJsonObject jsonData;
    jsonData["categoryId"] = "3";       //! 协议规定
    jsonData["mode"] = mode;
    jsonData["appCode"] = getAppCode();
    jsonData["dir"] = 1;
    jsonObj["data"] = jsonData;

    sendJson(jsonObj);
}

/*! 上传参数
 * \brief MyLocalSocketThread::uploadParams
 * \param mode 1-自动；2-手动
 */
void LocalSocketThread::uploadParams(QString dir, int mode)
{
    QJsonObject jsonObj;
    jsonObj["key"] = KEY_UPLOADFILE;
    QJsonObject jsonData;
    jsonData["categoryId"] = "5";       //! 协议规定
    jsonData["mode"] = mode;
    jsonData["appCode"] = getAppCode();
    jsonData["dir"] = dir;
    jsonObj["data"] = jsonData;

    sendJson(jsonObj);
}

void LocalSocketThread::uploadSorterAdjParams(QString dir, int mode)
{
    QString tmpdir = "/userdata/SorterAdjcnf";;
    if(gFlow.copyDir(dir,tmpdir)){
        QJsonObject jsonObj;
        jsonObj["key"] = KEY_UPLOADFILE;
        QJsonObject jsonData;
        jsonData["categoryId"] = "5";       //! 协议规定
        jsonData["mode"] = mode;
        jsonData["appCode"] = getAppCode();
        jsonData["dir"] = tmpdir;
        jsonObj["data"] = jsonData;

        sendJson(jsonObj);
    }
}

/*!
 * \brief 槽: 处理上传文件
 * \param obj
 */
void LocalSocketThread::onHandleMsg(QJsonObject obj)
{
    QJsonObject jsondata;
    bool bflag = false;

    if(obj.contains("Property")&&obj["Property"]==true){
        jsondata["key"] = "attributes";
        jsondata["data"] = packetProperty();
        bflag = true;
    }else if(obj.contains("Scheme")&&obj["Scheme"]==true){
        jsondata["key"] = "scheme";
        jsondata["data"] = packetScheme();
        bflag = true;
    }else if(obj.contains("Statics")&&obj["Statics"]==true){
        jsondata["key"] = "statistics";
        jsondata["data"] = packetStatistics();
        bflag = true;
    }else if(obj.contains("Version")&&obj["Version"]==true){
        jsondata["key"] = "version";
        jsondata["data"] = packetversion();
        bflag = true;
    }else if(obj.contains("Components")&&obj["Components"]==true){
        jsondata["key"] = "component";
        jsondata["data"] = packetKeyComponents();
        bflag = true;
    }else if(obj.contains("telemetry") &&obj["telemetry"] == true){
        jsondata["key"] = "telemetry";
        jsondata["data"] = packetTelemetry();
        bflag = true;
    }

    if(bflag){
        sendJson(jsondata);
    }
}

/*!
 * \brief 上传统计数据
 */
void LocalSocketThread::onuploadstaticRes()
{
    QJsonObject jsondata;
    jsondata["key"] = "statistics";
    jsondata["data"] = packetStatistics();
    sendJson(jsondata);
}

/*!
 * \brief 发送机编和产品ID
 */
void LocalSocketThread::onSendMachineInfo()
{
    QJsonObject jsonObj;
    jsonObj["key"] = "general";
    QJsonObject jsonData;
    jsonData["deviceNO"] = struCnfg.master_device_code;
    jsonData["productID"] = "MeyerZaAnalyser";   //! 
    jsonData["language"] = 0;
    jsonData["region"] = 0;
    jsonObj["data"] = jsonData;
    sendJson(jsonObj);

    QTimer::singleShot(30000, this, SLOT(onSendData()));
    QTimer::singleShot(40000, this, [this](){uploadParams("/ssd/cnf",1);});
    QTimer::singleShot(50000, this, [this](){uploadSorterAdjParams("/userdata/cnf",1);});

}

/*!
 * \brief LocalSocketThread::onSendLanguage
 * \param isEnglish
 */
void LocalSocketThread::onSendLanguage(int isEnglish)
{
    QJsonObject jsonObj;
    jsonObj["key"] = "general";
    QJsonObject jsonData;
    jsonData["language"] = isEnglish;
    jsonObj["data"] = jsonData;

    sendJson(jsonObj);
}


/*!
 * \brief 打包统计数据
 * \return
 */
QJsonObject LocalSocketThread::packetStatistics()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const bool isBottleType = (struCnfg.nMachineType == nMachine_Bottle);
    const bool  bPercentType = pProfile->nStaisticType == 1 ? true : false;
    const bool bSorterAdj = pProfile->bEnSorterClass;

    QJsonObject topjson;
    if(isBottleType){
        if(bPercentType){
            topjson["unit"] = "Ratio";
        }else{
            topjson["unit"] = "PPM";
        }
    }else{
        topjson["unit"] = "Number";
    }
    topjson["batch"] = struGsh.batchname;

    QJsonArray materarray;
    for(int i=0; i<pProfile->nStaticGroup; i++){
        QJsonArray labarry;
        QJsonObject jsongp;
        jsongp["channel_id"] = i;

        if(i<struGsh.vsMatinfo.size()){//! 上传统计信息
            QString keyname;
            const auto &matInfo = struGsh.vsMatinfo[i];
            for(const QString &title : matInfo.staInfo.keys()) {
                QString cattypestr = matInfo.staInfo[title];
                if(cattypestr.contains("M0/A1") && matInfo.staRes.contains(title)){
                    keyname = title;
                    break;
                }
            }
            if(!keyname.isEmpty()){
                jsongp["material_num"] = (int) struGsh.vsMatinfo[i].staRes[keyname][0];

            }
        }

        //! 瓶片增加预遍历
        float nDiff = 0;
        QString strmaxvalchild;
        if(isBottleType) {
            float maxRes = 0.0;
            float totalValue = 0.0;
            auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
            int ncamid = QString("%1").arg(pProfile->vUnitaddrlist[i].at(0)).toInt();
            QStringList AIlablst = struGsh.vMegreLablst[ncamid];
            for(const QString &var : AIlablst) {
                QStringList childlst;
                if(pProfile->vMegerlabmap[ncamid].contains(var)) {
                    childlst = pProfile->vMegerlabmap[ncamid].values(var);
                }

                if(childlst.isEmpty()) {
                    childlst << var;
                }

                //! 遍历统计类别
                float ntotaRes = 0.0;
                const QStringList &catlst = struGsh.vlabStamap[i][childlst.at(0)].staRes.keys();
                for(int k = 0; k < catlst.size(); k++) {
                    ntotaRes = 0.0;

                    for(const QString &childname : childlst) {
                        const QString &str = struGsh.vlabStamap[i][childname].staInfo[catlst.at(k)];
                        if(str.isEmpty()) continue;

                        const QStringList &strParts = str.split("/");
                        if(strParts.size() < 2) continue;

                        const QString &strtype = strParts.at(1);
                        if(strtype.isEmpty()) continue;

                        const int statype = strtype.mid(1).toInt();
                        if(statype != STATISTIC_A3) continue;

                        if(struGsh.vlabTotalmap[i][str] > 0){
                            float nres = struGsh.vlabStamap[i][childname].staRes[catlst.at(k)][0] /
                                         struGsh.vlabTotalmap[i][str];

                            float nVal;
                            if(bPercentType){
                                nVal = QString::number(nres,'f',4).toFloat();
                            } else {
                                nVal = (int)(nres*1000000);
                            }
                            totalValue += nVal;
                            if(nVal > maxRes){
                                maxRes = nVal;
                                strmaxvalchild = childname;
                            }
                        }
                    }
                }
            }

            if(bPercentType){
                nDiff = QString::number(1.0 - totalValue, 'f', 4).toFloat();
            } else {
                nDiff = (int)(1000000.0 - totalValue);
            }
        }


        //! 上传标签统计信息
        int n = 0;
        if(i<struGsh.vlabStamap.size() && i<pProfile->nStaticGroup){//! 统计组
            QMap<QString,slabSta>::const_iterator itor = struGsh.vlabStamap[i].constBegin();
            QString labname;
            while (itor != struGsh.vlabStamap[i].constEnd()) {
                QJsonObject jsonlab;
                labname = itor.key();

                jsonlab["id"] = n++;
                jsonlab["name"]= labname;
                if(struGsh.vlabEngNamemap[i].contains(labname)){
                    jsonlab["eng_name"]= struGsh.vlabEngNamemap[i][labname];
                }else{
                    jsonlab["eng_name"]= QString("");
                }

                const QStringList &catlst = struGsh.vlabStamap[i][labname].staRes.keys();
                for(int k = 0; k < catlst.size(); k++){
                    const QString &str = struGsh.vlabStamap[i][labname].staInfo[catlst.at(k)];
                    if(str.isEmpty() || !(str.contains("M0/A1") || str.contains("M0/A3"))) continue;

                    const QStringList &strParts = str.split("/");
                    if(strParts.size() < 2) continue;

                    const QString &offstr = strParts.at(1);
                    if(offstr.isEmpty()) continue;

                    const int statype = offstr.mid(1).toInt();
                    switch (statype) {
                    case STATISTIC_A1: //! 直接用
                        jsonlab["reg_num"] = (int)itor.value().staRes.first().at(0);
                        jsonlab["sta"] =0;

                        if(struGsh.vlabTotalmap[i].contains("M0/A4") &&
                            struGsh.vlabTotalmap[i]["M0/A4"] >0){
                            float nval = itor.value().staRes.first().at(0) /
                                         struGsh.vlabTotalmap[i]["M0/A4"];
                            if(bSorterAdj){

                                if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(labname)){
                                    if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][labname].nlowthd > 100 *nval
                                        ||pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][labname].nhigthd < 100*nval){
                                        jsonlab["sta"] =1;
                                    }
                                }
                            }else{
                                if(pProfile->strctAI.vlabthdmap.contains(labname) &&
                                    100*nval >pProfile->strctAI.vlabthdmap[labname]){
                                    jsonlab["sta"] =1;
                                }
                            }
                        }
                        break;
                    case STATISTIC_A3: //! 乘系数 算占比
                        if(struGsh.vlabTotalmap[i][str] > 0){
                            int nVal =0; float nRes =0.0;
                            float valve = struGsh.vlabStamap[i][labname].staRes[catlst.at(k)][0] /
                                          struGsh.vlabTotalmap[i][str];

                            if(isBottleType){
                                if(bPercentType){
                                    nRes = QString::number(valve,'f',4).toFloat();
                                    if(labname==strmaxvalchild){
                                        nRes +=nDiff;
                                    }
                                    jsonlab["reg_num"] = (int)(nRes*1000000); //! 协议只支持整型
                                    jsonlab["sta"] = 0;

                                    if(bSorterAdj){
                                        if(pProfile->strcSortclass.vlabParm[struGsh.nAnalyzeIndex].contains(labname)){
                                            if(pProfile->strcSortclass.vlabParm[struGsh.nAnalyzeIndex][labname].nlowthd > 100*nRes
                                                || 100*nRes > pProfile->strcSortclass.vlabParm[struGsh.nAnalyzeIndex][labname].nhigthd){
                                                jsonlab["sta"] = 1;
                                            }
                                        }
                                    }else{
                                        if(pProfile->strctAI.vlabthdmap.contains(labname) &&
                                            100*nRes >pProfile->strctAI.vlabthdmap[labname]){
                                            jsonlab["sta"] = 1;
                                        }
                                    }
                                }else{
                                    nVal = (int)(valve * 1000000);
                                    if(labname==strmaxvalchild){
                                        nVal +=nDiff;
                                    }
                                    jsonlab["reg_num"] = nVal;
                                    jsonlab["sta"] = 0;

                                    if(bSorterAdj){
                                        if(pProfile->strcSortclass.vlabParm[struGsh.nAnalyzeIndex].contains(labname)){
                                            if(pProfile->strcSortclass.vlabParm[struGsh.nAnalyzeIndex][labname].nlowthd > nVal
                                                || nVal > pProfile->strcSortclass.vlabParm[struGsh.nAnalyzeIndex][labname].nhigthd){
                                                jsonlab["sta"] = 1;
                                            }
                                        }
                                    }else{
                                        if(pProfile->strctAI.vlabthdmap.contains(labname) &&
                                            nVal >pProfile->strctAI.vlabthdmap[labname]){
                                            jsonlab["sta"] = 1;
                                        }
                                    }
                                }
                            }else{
                                jsonlab["reg_num"] = 0;
                                jsonlab["sta"] = 0;
                            }
                        }
                        break;
                    case STATISTIC_A4:
                    case STATISTIC_A2:
                    case STATISTIC_A5:
                        break;
                    default:
                        break;
                    }
                }
                jsonlab["tick_num"] =0;

                labarry.append(jsonlab);
                ++itor;
            }

            jsongp["category"] = labarry;
            //  qDebug()<<labarry;

            materarray.append(jsongp);
        }

    }

    topjson["material_points"] = materarray;

    return topjson;
}




void LocalSocketThread::sendVirtualIP(QString virtualIP, QString responseID)
{
    QJsonObject json;
    json["key"] = KEY_VPN;
    json["src"] = "mqtt";

    QJsonObject jsonResponse;

    QJsonObject data;
    if (!virtualIP.isEmpty()) {
        data["code"] = 0;
        data["msg"] = "获取IP地址成功";
        data["data"] = virtualIP;
        qInfo().noquote() << "VPN connect successful(mqtt)，IP：" + virtualIP;
    } else {
        data["code"] = 2;
        data["msg"] = "获取IP地址失败";
    }
    jsonResponse["data"] = data;
    jsonResponse["msg"] = "获取ip";
    jsonResponse["type"] = CMD_TYPE_RESPONSE;
    jsonResponse["code"] = CMD_REMOTECTRL_OBTAIN_IP;
    jsonResponse["from"] = struCnfg.master_device_code;
    jsonResponse["to"] = responseID;

    json["raw_data"] = jsonResponse;
  
    sendJson(json);
}

/*!
 * \brief LocalSocketThread::sendJson
 * \param js
 */
void LocalSocketThread::sendJson(QJsonObject js)
{
    if (m_localSocket == nullptr || !m_localSocket->isConnected()) {
        return;
    }

    QJsonDocument jd;
    jd.setObject(js);
    m_localSocket->sendMessage(jd.toJson());
}

/*!
 * \brief 打包FPGA版本Json
 * \return
 */
QJsonObject LocalSocketThread::packProVersionFPGA()
{
    QJsonObject json;
    json["interface"] = packVersionInterface();
    json["camera"] = packVersionCamera();
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist) {
        json["assist_camera"] = packVersionAssistCamera();
    }
    json["lamp"] = packVersionLamp();
    if (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist) {
        json["halogen"] = packVersionHalogen();
    }
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV){
         json["ejector"] = packVersionEjector();
    }
    json["ai"] = packVersionAI();
    json["vibrator"] = packVersionVibrator();
    return json;
}


/*!
 * \brief 打包控制板
 * \return
 */
QJsonArray LocalSocketThread::packVersionInterface()
{
    QJsonArray array;
    for (int i = 0; i < 1; i++) {
        QJsonObject obj;
        obj["id"] = i;
        obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sCtrlVersion);
        obj["hardware"] = "";
        obj["batch"] = "";
        obj["coreid"] = "";
        array.append(obj);
    }
    return array;
}

/*!
 * \brief 打包相机板版本
 * \return
 */
QJsonArray LocalSocketThread::packVersionCamera()
{
    QJsonArray array;
    for(int i=0; i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i++) {
        QJsonObject obj;
        obj["id"] = i;
        obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sCamVersion[i]);
        obj["hardware"] = " ";
        obj["batch"] = "";
        obj["coreid"] ="";
        array.append(obj);
    }
    return array;
}

/*!
 * \brief 打包相机板版本
 * \return
 */
QJsonArray LocalSocketThread::packVersionAssistCamera()
{
    QJsonArray array;
    for (int i = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i<gFlow.getMachineCamNum(); i++) {
        QJsonObject obj;
        obj["id"] = 32;
        obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sCamVersion[i]);
        obj["hardware"] = "";
        obj["batch"] = "";
        obj["coreid"] = "";
        array.append(obj);
    }
    return array;
}

/*!
 * \brief 打包电磁阀驱动板
 * \return
 */
QJsonArray LocalSocketThread::packVersionEjector()
{
    QJsonArray array;
    for(int i=0; i<1; i++) {
        QJsonObject obj;
        obj["id"] = i;
        obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sSVBoardVersion[20]);
        obj["hardware"] = "";
        obj["batch"] = "";
        obj["coreid"] = "";
        array.append(obj);
    }
    return array;
}

/*!
 * \brief 打包AI版本
 * \return
 */
QJsonArray LocalSocketThread::packVersionAI()
{
    QJsonArray array;
    for(int i=0; i<gFlow.getMachineCamNum(); i++) {
        if(gFlow.bIsSupportAI(i)){
            QJsonObject obj;
            obj["id"] = i;
            obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sSocVersion[i]);
            obj["hardware"] ="";
            obj["batch"] = "";
            obj["coreid"] ="";
            array.append(obj);
        }
    }
    return array;
}


QJsonArray LocalSocketThread::packVersionVibrator()
{
    QJsonArray array;
    return array;
}

/*!
 * \brief LocalSocketThread::packProVersionArm
 * \return
 */
QJsonObject LocalSocketThread::packProVersionArm()
{
    QJsonObject armObj;
#ifdef Q_OS_UNIX
    armObj["ver"] =  QString("%1").arg(ARM_VERSION);
    armObj["svn"] =  QString("v%1").arg(SVN_VERSION);
    armObj["code"] = QString("%1").arg(ARM_CODE);
#endif
    QDate date = getBuildQdate();
    armObj["date"] = QString("%1 %2").arg(date.toString("yyyy-MM-dd")).arg(QString(__TIME__));
    return armObj;
}

/*!
 * \brief 打包恒流源
 * \return
 */
QJsonArray LocalSocketThread::packVersionLamp()
{
    QJsonArray array;
    for(int id=0; id<1; id++) {
        QJsonObject obj;
        obj["id"] = id;
        obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sPulseVersion);
        obj["hardware"] ="";
        obj["batch"] = "";
        obj["coreid"] = "";
        array.append(obj);
    }
    return array;
}

/*!
 * \brief 打包卤素灯
 * \return
 */
QJsonArray LocalSocketThread::packVersionHalogen()
{
    QJsonArray array;
    for(int id=0; id<1; id++) {
        QJsonObject obj;
        obj["id"] = id;
        obj["ver"] = QString(struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->sHalogenVersion);
        obj["hardware"] ="";
        obj["batch"] = "";
        obj["coreid"] = "";
        array.append(obj);
    }
    return array;
}

QDate LocalSocketThread::getBuildQdate()
{
    QString str = QString(__DATE__);
    QStringList dateList = str.split(' ');
    int year = 1970;
    int day  = 01;
    if(dateList.size() == 4){
        year = dateList.at(3).toInt();
        day  = dateList.at(2).toInt();
    } else if(dateList.size() == 3){
        year = dateList.at(2).toInt();
        day  = dateList.at(1).toInt();
    }
    QChar flag = dateList.at(0).at(2);
    QChar flag2 = dateList.at(0).at(0);
    int month = (flag == 'n' ? 1
                             : flag == 'b' ? 2
                             : flag == 'r' ? (flag2 == 'M' ?3:4)
                             : flag == 'y' ? 5
                             : flag == 'n' ? 6
                             : flag == 'l' ? 7
                             : flag == 'g' ? 8
                             : flag == 'p' ? 9
                             : flag == 't' ? 10
                             : flag == 'v' ? 11 : 12);
    QDate date(year, month, day);
    return date;
}

/*!
 * \brief 打包振动器振动量
 * \return
 */
QJsonObject LocalSocketThread::packVibrator()
{
    QJsonObject json;
    json[QString("vibrator_0")] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nFeeder[0];

    return json;
}

/*!
 * \brief 打包伺服电流
 * \return
 */
QJsonObject LocalSocketThread::packElectric()
{
    QJsonObject json;
    json[QString("electric_1")] = "";

    return json;
}


/*!
 * \brief 打包恒流源状态
 * \return
 */
QJsonArray LocalSocketThread::packLampstatus()
{
    auto *pParams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    QJsonArray array;
    for (int j = 0; j < LAMP_TYPE; j++) {//! 背景灯
        if (pParams->bEnlamp[j]) {
            QJsonObject jsObject;
            jsObject["name"] = pParams->nLampName[j];
            jsObject["switch"] = pParams->bEnlamp[j];
            jsObject["value"] = pParams->nLampValue[j];
            array.append(jsObject);
        }
    }

    for (int j = 0; j < PLUSE_TYPE; j++) {//! 物料灯
        if (pParams->bEnPluse[j]) {
            QJsonObject jsObject;
            jsObject["name"] = pParams->nPluseName[j];
            jsObject["switch"] = pParams->bEnPluse[j];
            jsObject["value"] = pParams->nPluseValue[j];
            array.append(jsObject);
        }
    }

    QJsonObject jsObject;
    jsObject["name"] = "lamp";
    jsObject["port"] = array;
    jsObject["power1"] = 0;
    jsObject["power2"] = 0;


    QJsonArray jsonArray;
    jsonArray.append(jsObject);


    return jsonArray;
}


/*!
 * \brief 打包卤素灯状态
 * \return
 */
QJsonObject LocalSocketThread::packHalogen()
{
    QJsonObject json;
    for(int i=0; i<2; i++){
        QJsonObject halogen;
        halogen["temp"] = "";
        halogen[QString("light_%1").arg(i)] = struGsh.nHalogenValue[i];
        json[QString("halogen_%1").arg(i+1)] = halogen;
    }

    return json;
}

/*!
 * \brief 打包相机温度
 * \return
 */
QJsonObject LocalSocketThread::packCameraTemperature()
{
    QJsonObject json;
    json["threshold"] = 60;
    QJsonArray array;

    for (int i = 0; i < gFlow.getMachineCamNum(); i++) {
        if (gFlow.bIsIMSerialCam(i)) {
            QJsonObject obj;
            obj["id"] = i;
            obj["max"] = struGsh.nInfraTempure;
            array.append(obj);
        }
    }
    json["temperature"] = array;
    return json;
}

/*!
 * \brief 打包像元信息
 * \return
 */
QJsonArray LocalSocketThread::packChannelMajor()
{
    auto *pParams = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;

    QJsonArray jsonArray;
    for (int i=0 ;i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i++) {
        QJsonObject jsonobj;
        jsonobj["id"] = i;
        jsonobj["begin"] = pParams->nPixelBegin[i];
        jsonobj["end"] = pParams->nPixelEnd[i];
        jsonArray.append(jsonobj);

    }
    return jsonArray;
}

/*!
 * \brief 打包像元信息
 * \return
 */
QJsonArray LocalSocketThread::packChannelAssist()
{
    auto *pParams = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;

    QJsonArray jsonArray;
    int  n = 0;
    for (int i= struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i<gFlow.getMachineCamNum(); i++) {
        QJsonObject jsonobj;
        jsonobj["id"] = 32 + n;
        jsonobj["begin"] = pParams->nPixelBegin[i];
        jsonobj["end"] = pParams->nPixelEnd[i];
        jsonArray.append(jsonobj);
        n += 2;
    }
    return jsonArray;
}

/*!
 * \brief 打包遥测数据
 * \return
 */
QJsonObject LocalSocketThread::packetTelemetry()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    QJsonObject json;

    json["runtime"] = QString("%1").arg(struGsh.nRuningTime/60);
    json["total_runtime"]=QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRuntime/60);
    json["feeder_time"]=QString("%1").arg(struGsh.nfeederTime/60);
    json["total_feeder_time"] =QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nFeedertime/60);
    json["feeder"] = struGsh.nFeederState ? 1 : 0;
    json["belt_1"] = struGsh.bBeltOnff ? 1: 0;
    json["network_abnormal_time"] = struGsh.strNetabtime;

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist) {
         json["halogen_1"] = pProfile->bHologenLampOn;

         if(struGsh.nHalogenValue[0] < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd
             || struGsh.nHalogenValue[1]<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd){
             json["alarm_halogen1"] = 1;
         }else{
             json["alarm_halogen1"] = 0;
         }
         json["alarm_telecom_halogen"] = struGsh.bSysBoardSta[BOARD_HALOGEN] ? 1:0;
     }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAirAlarm){
         json["alarm_press1"] = struGsh.nAirStatus?1:0;
    }


    json["alarm_telecom_interface"] = struGsh.bSysBoardSta[BOARD_CONTROL] ? 1:0;
    json["alarm_telecom_camera"] = struGsh.bSysBoardSta[BOARD_CAMERA] ? 1:0;
    json["alarm_telecom_lamp"] = struGsh.bSysBoardSta[BOARD_PLUS_LIGHT] ? 1:0;
    json["alarm_telecom_ai"] = struGsh.bSysBoardSta[BOARD_HALOGEN] ? 1:0;

    return json;
}





bool LocalSocketThread::isInterComError()
{
//    for (int i = 0; i < m_pmMachine->vLevel.size(); i++) {
//        if (m_pmShare->interVersion[i].isEmpty() || m_pmShare->interVersion[i].endsWith("_F")) {
//            return true;
//        }
//    }
    return false;
}

bool LocalSocketThread::isCameraComError(int role)
{
//    foreach (int id, m_pmShare->cameraVersion.keys()) {
//        if (ROLE_FROM_UUID(id) == role) {
//            if (m_pmShare->cameraVersion[id].isEmpty() || m_pmShare->cameraVersion[id].endsWith("_F")) {
//                return true;
//            }
//        }
//    }
    return false;
}

bool LocalSocketThread::isLampComError()
{
//    foreach (auto key, m_pmShare->lampVersion.keys()) {
//        if (m_pmShare->lampVersion[key].isEmpty()) {
//            return true;
//        }
//    }
    return false;
}

bool LocalSocketThread::isHalogenComError()
{
//    for (int i = 0; i < m_pmMachine->stBoard.nHalogenDetect; i++) {
//        if (m_pmShare->halogenDetectVersion[i].isEmpty()) {
//            return true;
//        }
//    }
    return false;
}



bool LocalSocketThread::isEjectorComError()
{
//    for (int i = 0; i < m_pmMachine->vEjDriverBoard.size(); i++) {
//        int id = m_pmMachine->vEjDriverBoard.at(i);
//        if (m_pmShare->ejectDriverVersion[id].isEmpty()) {
//            return true;
//        }
//    }
    return false;
}

bool LocalSocketThread::isAIComError()
{
//    foreach (int id, m_pmShare->AIVersion.keys()) {
//        if (m_pmShare->AIVersion[id].isEmpty()) {
//            return true;
//        }
//    }
    return false;
}

bool LocalSocketThread::isVibratorComError()
{
//    foreach (int id, m_pmShare->feedDriverVersion.keys()) {
//        if (m_pmShare->feedDriverVersion[id].isEmpty()) {
//            return true;
//        }
//    }
    return false;
}

/*!
 * \brief 处理服务器回传数据包
 * \param json
 */
void LocalSocketThread::handleMessage(QJsonObject json)
{
    if (json["key"] == KEY_VPN) {//! 处理VPN
        QJsonObject data = json["raw_data"].toObject();
        int code = data["code"].toInt();
        if (code == CMD_REMOTECTRL_OBTAIN_IP) {
            handleOpenVPN(json);
        } else if (code == CMD_REMOTECTRL_OPEN_CLOSE_VPN) {
            handleCloseVPN(json);
        }
    } else if (json["key"].toString() == "general") { //! web串口处理
        handleGeneral(json["data"].toObject());
    }else if (json["key"] == KEY_SCANCODE) { //! 云端中间件扫码
        handleScanCodeEvent(json["data"].toObject());
    }else if (json["key"] == KEY_NOTIFY) { //! 界面权限扫码
        handleNotifyEvent(json["data"].toObject());
    }
}

/*!
 * \brief 处理通用信息
 * \param json
 */
void LocalSocketThread::handleGeneral(QJsonObject json)
{
    if (json.contains(KEY_GENERAL_WINID)) {
        int id = json[KEY_GENERAL_WINID].toInt();
        emit sigsetWindow(id);
    }

//    if (json.contains(KEY_GENERAL_NETWORK)) {
//        int state = json[KEY_GENERAL_NETWORK].toInt();
//        if (state != m_pmShare->nNetworkState) {
//            m_pmShare->nNetworkState = state;
//            if (m_pmShare->nNetworkState == NETWORK_DISCONNECT
//                    || m_pmShare->nNetworkState == NETWORK_NO_INTERNET) {
//                m_pmShare->dtNetworkAbnormal = QDateTime::currentDateTime();
//            } else if (m_bNtp && m_pmGlobal->bNtp) {
//                m_bNtp = false;
//                emit sigSyncTime();
//            }
//        }
//    }


//    if (json.contains(KEY_TOKEN)) {
//        m_pmGlobal->sLoginToken = json[KEY_TOKEN].toString();
//        qDebug("Get login token");
//    }
}

void LocalSocketThread::handlePressEvent()
{
//    m_pmShare->nScreenLockCounter = 0;
//    if (m_pmShare->bScreenLockStat) {
//        Utility::setTsBackLight(true);
//        if (!m_pmGlobal->sScreenSaverPwd.isEmpty()) {
//            g_UISigManager->trigger(EVENT_SCREENLOCK_PANEL);
//        }
//    }
}

/*!
 * \brief 处理openvpn连接请求
 * \param json
 */
void LocalSocketThread::handleOpenVPN(QJsonObject json)
{
    QString responseID = json["from"].toString();

    if (struGsh.sIp.isEmpty()) {
        if (gFlow.connetToVPN()) { //! 连接服务器成功
            QString virtualAddr = struGsh.sIp;
            sendVirtualIP(virtualAddr, responseID);
            emit sigrefreshVPNStatus(true);//! 刷新UI
        } else {  //! 连接服务器失败
            gFlow.closeVPN();
            emit sigrefreshVPNStatus(false);
        }
    }else{
        sendVirtualIP(struGsh.sIp, responseID);
    }
}

/*!
 * \brief 槽: 关闭VPN
 * \param json
*/
void LocalSocketThread::handleCloseVPN(QJsonObject json)
{
    gFlow.closeVPN();
    emit sigrefreshVPNStatus(false);

    QJsonObject jsonResponse;
    jsonResponse["data"] = json["data"];
    jsonResponse["type"] = CMD_TYPE_RESPONSE;
    jsonResponse["code"] = CMD_REMOTECTRL_OPEN_CLOSE_VPN;
    jsonResponse["from"] = json["to"];
    jsonResponse["to"] =   json["from"];
    jsonResponse["msg"] =  json["msg"];

    QJsonObject jsonReturn;
    jsonReturn["key"] = KEY_VPN;
    jsonReturn["src"] = "mqtt";
    jsonReturn["raw_data"] = jsonResponse;

    sendJson(jsonReturn);
}

/*!
 * \brief 槽: 报警事件
 * \param json
 */
void LocalSocketThread::handleAlarmEvent(QJsonObject json)
{
    QString tips = json["msg"].toString();
    bool value = json["value"].toBool();
    tips = QString("%1(%2)").arg(tips).arg(value);

    qWarning().noquote() << tips;
}



void LocalSocketThread::handleUploadFile(QJsonObject json)
{
    int code = json["code"].toInt();
    QString categoryId = json["categoryId"].toString();
    if (code != 200) {
    }
}

void LocalSocketThread::handleQueryFile(QJsonObject json)
{
    int code = json["code"].toInt();
    if (code == 200) {
        QJsonArray array = json["list"].toArray();
    }
}

void LocalSocketThread::handleScanCodeEvent(QJsonObject jsonObj)
{
    int module = 0;
    int result = 0;
    if(jsonObj.contains("module")){
        module = jsonObj["module"].toInt();  //! 权限类型
        if(module == 9 && jsonObj.contains("protcol")){
            QString protcol = jsonObj["protcol"].toString();
//            MyQRCodeWidget kb;
//            QByteArray code = QrCodeData::getLanFeatureCode(protcol);
//            kb.setQRInfo(g_lang.msg_use_wx_scan, code);
//            if (kb.exec() == QDialog::Accepted){
//                QString pwd = kb.getText();
//                QString checkcode = QrCodeData::getCheckCode(QrCodeData::LAN_FEATURE);
//                if (checkcode != pwd) {
//                    g_UISigManager->showInfo(g_lang.msg_wrong_password);
//                    result = 0;
//                }else{
//                    result = 1;
//                }
//            }
        }
    }
}

void LocalSocketThread::handleNotifyEvent(QJsonObject jsonObj)
{
    int module = 0, result=0;
    QString device,time,detail;

    if (!jsonObj.contains("module")) {
        return;
    }

    module = jsonObj["module"].toInt();
    if (jsonObj.contains("device")) {
        device = jsonObj["device"].toString();
    }
    if (jsonObj.contains("time")) {
        time = jsonObj["time"].toString();
    }
    if (jsonObj.contains("detail")) {
        detail = jsonObj["detail"].toString();
    }
    if (jsonObj.contains("result")) {
        result = jsonObj["result"].toInt();
    }

    if (device == struCnfg.master_device_code) {
        if (result == 1) {
           emit sigScanCodeRet(true);
        } else {
            if (detail.isEmpty() || detail.isNull()) {
                emit sigScanCodeRet(false);
            }
        }
    }
}

//QString LocalSocketThread::getCheckCode(int moduleType)
//{
//#ifdef Q_OS_WIN
//    return "";
//#else
//    QString input = QString("%1%2%3").arg(m_QrDeviceId).arg(m_QrTime).arg("002690");
//    QCryptographicHash md(QCryptographicHash::Md5);
//    md.addData(input.toLatin1());
//    QString md5 = md.result().toHex().data();
//    if (md5.size() < 32) {
//        // 异常
//        return "error";
//    }
//    int module = moduleType;
//    auto getOneValue = [md5, module](int a, int b, int c, int d)->int {
//        return ((int)md5[a].unicode() + (int)md5[b].unicode() + (int)md5[c].unicode() + (int)md5[d].unicode() + module)%10;
//    };

//    int dd0 = getOneValue(5, 10, 25, 30);
//    int dd1 = getOneValue(3, 8, 15, 27);
//    int dd2 = getOneValue(6, 9, 16, 28);
//    int dd3 = getOneValue(12, 18, 22, 5);
//    int dd4 = getOneValue(9, 13, 19, 21);
//    int dd5 = getOneValue(20, 23, 24, 31);

//    QString result = QString("%1%2%3%4%5%6").arg(dd0).arg(dd1).arg(dd2).arg(dd3).arg(dd4).arg(dd5);
//    return result;
//#endif
//}

void LocalSocketThread::requestCtrlPackage(int res, QString str)
{
    QJsonObject jsonObj;
    jsonObj["key"]  = "control";
    jsonObj["src"]  = m_src;
    jsonObj["from"] = m_from;
    jsonObj["msg"]  = str;
    QJsonObject jsonData;
    jsonData["requestCode"] = res;
    jsonObj["data"] = jsonData;

    sendJson(jsonObj);
}

void LocalSocketThread::getBackupList(QString categoryId, int page, int pageSize)
{
    QJsonObject json;
    json["key"] = KEY_QUERYFILE;
    json["data"] = packReqBackup(categoryId, page, pageSize);
    sendJson(json);
}

QJsonObject LocalSocketThread::packReqBackup(QString categoryId, int page, int pageSize)
{
    QJsonObject jsonObj;
    jsonObj["deviceCode"] = "123345";
    jsonObj["categoryId"] = categoryId;
    jsonObj["appCode"]    = getAppCode();
    jsonObj["timeStart"]  = "1970-01-01 08:00:00";
    jsonObj["timeEnd"]    = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    jsonObj["page"] = QString::number(page);
    jsonObj["size"] = QString::number(pageSize);
    return jsonObj;
}

/*!
 * \brief 下载请求
 * \param categoryId
 * \param url
 * \param fileName
 */
void LocalSocketThread::downloadFile(QString categoryId, QString url, QString fileName)
{
    QJsonObject json;
    json["key"] = KEY_DOWNLOADFILE;
    QJsonObject data;
    data["categoryId"] = categoryId;
    data["appCode"]    = getAppCode();
    data["downloadUrl"] = url;
    data["filename"] = fileName;
    json["data"] = data;
    sendJson(json);
}

/*!
 * \brief 获取机器编码
 * \return
 */
QString LocalSocketThread::getAppCode()
{
    QString appCode = "MeyerIOT-ZA-LD";
    return appCode;
}

QString LocalSocketThread::getProductId()
{
    QString productId = "MeyerZaLvDai";
    return productId;
}
