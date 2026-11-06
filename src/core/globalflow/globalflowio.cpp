#include "globalflow.h"
#include "common/mymessagebox.h"
#include "core/io/udp/myudp.h"

/**
 * @brief GlobalFlow::获取波形
 * @param nUnit
 * @param nTime
 * @param mode
 * @param nRow
 * @param isEnable
 */
void GlobalFlow::sendImgWaveReq(char nAnalyzeIndex, char nUnit, int nTime, char mode, int nRow, bool isSimulate, bool isGetBak, bool isEnable)
{
    int burstMode = 0;

    char nDataLength = 10;
    char data[nDataLength] = {0};
    int nCmd = CMD_CURVE;
    data[0] = nTime;
    data[1] = mode;
    data[2] = burstMode;
    data[3] = nRow/256;
    data[4] = nRow%256;
    data[5] = isSimulate;
    data[6] = isGetBak;
    data[7] = isEnable;
    data[8] = struGsh.nThreadValue/256;
    data[9] = struGsh.nThreadValue%256;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnit, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::切控制板工作模式
 */
void GlobalFlow::sendCtrlSignalMode(char nAnalyzeIndex, char mode)
{
    char data;

    data = mode;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_CTRL_SIGNAL, &data, 1);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
//    g_myUdp->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::原始信号和校正信号切换
 * @param mode
 */
void GlobalFlow::sendCamSignalMode(char nAnalyzeIndex, int nUnitAddr, char mode)
{
    char data[2];
    data[0] = 0;
    data[1] = mode;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, CMD_CAM_SIGNAL, data, 2);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*正常校正系数状态    前置板   */
//!  查看校正波形时，下位机ram中存放校正波形，退出校正波形界面要发送ram恢复成校正系数命令，主要目的是下位机资源节省问题。
void GlobalFlow::switchWaveToCalibrateID(int nAnalyzeIndex, int nUnitAddr)
{
    char data = 1;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, CMD_CAM_WAVE_BACK, &data, 1);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::发送校正目标
 * @param nCalibrationID
 * @param mode
 * @param nUnitAddr
 */
void GlobalFlow::sendCalibratePara(char nAnalyzeIndex, char nCalibrationID, char mode, int nUnitAddr)
{
    char nDataLength = 6;
    char data[nDataLength] = {0};
    int nCmd = CMD_CAMERA;
    data[0] = nCalibrationID;
    data[1] = mode;
    data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nAdjustTarget[nUnitAddr][COLOR_TYPE_R];
    data[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nAdjustTarget[nUnitAddr][COLOR_TYPE_G];
    data[4] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nAdjustTarget[nUnitAddr][COLOR_TYPE_B];
    data[5] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nAdjustTarget[nUnitAddr][COLOR_TYPE_N];

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

//    g_myUdp->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::发送行频
 * @param nFreq
 * @param mode
 */
void GlobalFlow::sendRowFreqPara(char nAnalyzeIndex, int nUnitAddr, int mode)
{
    char data[3] = {0};
    int expose = 0;
    int nCmd = CMD_ROW_FREQUENCY;
    expose = 10000000/struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nFreQuence[nUnitAddr];
    data[0] = expose/256;
    data[1] = expose%256;
    data[2] = mode;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, nUnitAddr, nCmd, data, 3);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

    //高分辨率相机需要下发行频给相机板
    if(gFlow.bHighResolution((int)nAnalyzeIndex,nUnitAddr)){
//        for(int i = 0; i < gFlow.getMachineCamNum(); i++){
            QByteArray ba = packet_TypeA_NetData(CAMERA_QD, MODULE_CAM, nUnitAddr, nCmd, data, 3);

            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
//        }
    }
}

void GlobalFlow::setIM01Waves(char nAnalyzeIndex, int nUnitAddr)
{
    if(!gFlow.bIsIMSerialCam(nUnitAddr))
        return;

    struCamera *pCaParams = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams;

    int nCmd = CMD_UNIT_IM01_WAVE;
    char data[25]={0};

    int nWave = 6;
    if(pCaParams->nWaveSeg[nUnitAddr] == 1){
        nWave = 12;
    }

    data[0] = nWave;
    for(int m = 0; m < nWave; m++){
        data[1+m*2] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.n12WavesVal[nUnitAddr][m]/256;
        data[1+m*2+1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.n12WavesVal[nUnitAddr][m]%256;
    }

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, 2*nWave+1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

void GlobalFlow::setIM01Window(char nAnalyzeIndex, int nUnitAddr)
{
    if(SENSOR_IM01 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]
        &&SENSOR_IM04 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr])
        return;

    int nCmd = CMD_UNIT_IM01_WINDOW;
    char data[8]={0};
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;

    auto *p = &struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile;
    struCamera *pCaParams = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams;
    QVector<int> startRows;
    int row = 0;

//    row = pCaParams->nWavesSix[nUnitAddr][0]-(1-pCaParams->nWavesSix[nUnitAddr][0]%2);
//    startRows<<row;
//    for(int m = 1; m < 6; m++){
//        if(pCaParams->nWavesSix[nUnitAddr][m]-startRows.last()>8){
//            int row = pCaParams->nWavesSix[nUnitAddr][m]-(1-pCaParams->nWavesSix[nUnitAddr][m]%2);
//            if(!startRows.contains(row)){
//                startRows<<row;
//            }
//        }
//    }

    row =p->n12WavesVal[nUnitAddr][0]-(1-p->n12WavesVal[nUnitAddr][0]%2);
    startRows<<row;
    for(int m = 1; m < 6; m++){
        if(p->n12WavesVal[nUnitAddr][m]-startRows.last()>8){
            int row = p->n12WavesVal[nUnitAddr][m]-(1-p->n12WavesVal[nUnitAddr][m]%2);
            if(!startRows.contains(row)){
                startRows<<row;
            }
        }
    }

    for(int m = 0; m < startRows.size() && m < 4; m++){
        data[4+m] = startRows.at(m);
    }

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, 8);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 高光谱相机行频
 * \param nAnalyzeIndex
 * \param nUnitAddr
 */
void GlobalFlow::setIM01RowFreq(char nAnalyzeIndex, int nUnitAddr)
{
    if(SENSOR_IM01 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]
        && SENSOR_IM04 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr])
        return;

    if(SENSOR_IM01 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
        char data[8] = {0};
        int nCmd = CMD_UNIT_IM01_ROWFREQ;
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        int rowFreq = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nFreQuence[nUnitAddr]*10;
        int exposure = (struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nFreQuence[nUnitAddr] -struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->exposeValue[nUnitAddr] - 480)*5;
        data[4] = ((rowFreq>>8)&0xff);
        data[5] = ((rowFreq)&0xff);
        data[6] = ((exposure>>8)&0xff);
        data[7] = ((exposure)&0xff);

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, 8);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }else{
        char data[14] = {0};
        memset(data,0,14*sizeof(char));

        int rowFreq = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nFreQuence[nUnitAddr];
        int exposure = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->exposeValue[nUnitAddr];
        data[4] = (rowFreq>>8)&0xff;
        data[5] = rowFreq&0xff;
        data[6] = 0;
        data[7] = 0;
        data[8]=((struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->NoneexposeValue[nUnitAddr]>>16)&0x7f);
        data[9]=((struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->NoneexposeValue[nUnitAddr]>>8)&0xff);
        data[10]=((struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->NoneexposeValue[nUnitAddr])&0xff);
        data[11]=((exposure>>16)&0x7f);
        data[12]=((exposure>>8)&0xff);
        data[13]=((exposure)&0xff);

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, CMD_UNIT_IM01_ROWFREQ, data, 14);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

}

/*!
 * \brief 查询传感器温度
 * \param nAnalyzeIndex
 * \param nUnitAddr
 */
void GlobalFlow::reqIM01SensorTempure(char nAnalyzeIndex, int nUnitAddr)
{
    char data =1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, CMD_UNIT_IM01_TEMPUTERE, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 是否是紫外相机
 * \param nCamId
 * \return
 */
bool GlobalFlow::bIsUltraCam(int nCamId)
{
    if((nCamId < 0) || (nCamId > gFlow.getMachineCamNum())){
        return false;
    }

    if(SENSOR_VL08 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nCamId]){
        return true;
    }
    return false;
}

/*!
 * \brief 判断是否是IM系列的传感器相机
 * \param nCamId
 * \return
 */
bool GlobalFlow::bIsIMSerialCam(int nCamId)
{
    if((nCamId < 0) || (nCamId > gFlow.getMachineCamNum())){
        return false;
    }

    if(SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nCamId] ||
        SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nCamId]){
        return true;
    }
    return false;
}


/*!
 * \brief 相机是否支持深度学习
 * \param nCamId
 * \return
 */
bool GlobalFlow::bIsSupportAI(int nCamId)
{
    bool bRes = true;
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[nCamId] ==1){
        bRes = false;
    }
    return bRes;
}

/*!
 * \brief 重置标签阈值
 */
void GlobalFlow::ResetLabthdhod()
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabthdmap.clear();
    for(int i=0; i<gFlow.getMachineCamNum();i++){
        foreach (QString var, struGsh.vlabNamemap[i]){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabthdmap[var] = 1.0;
        }
    }
}

/*!
 * \brief 重置标签的转换系数
 */
void GlobalFlow::ResetLabArea2Wcof()
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.Area2weightmap.clear();
    for(int i=0; i<gFlow.getMachineCamNum();i++){
        foreach (QString var, struGsh.vlabNamemap[i]){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.Area2weightmap[var] = 1.0;
        }
    }
}

/*!
 * \brief 重置标签的属性
 */
void GlobalFlow::ResetLabproperty()
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.labprotymap.clear();
    for(int i=0; i<gFlow.getMachineCamNum();i++){
        foreach (QString var, struGsh.vlabNamemap[i]){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.labprotymap[var] = false;
        }
    }
}

/*!
 * \brief 查询金属个数
 */
void GlobalFlow::reqMetalNum()
{
    char data =1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_CTRL_REQMETAL, &data, 1);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);

}

/*!
 * \brief 气压报警
 */
void GlobalFlow::reqAiralarm()
{
    char data =1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_AIR_ALARM, &data, 1);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
}

/*!
 * \brief 发送色选联调的标签参数
 * \param bclear
 */
void GlobalFlow::sendSorterlabparams(bool bclear)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    QJsonObject objjson;
    QJsonArray jsontop;
    for(int i=0; i<pProfile->strcSortclass.nmaxNum; i++){//! 取样口
        QJsonObject jsonmid;
        QJsonArray jsonary;
        for(int n=0; n<gFlow.getMachineCamNum() && n<pProfile->nStaticGroup; n++){//! 遍历相机
            int nCamId = pProfile->vUnitaddrlist[n].at(0).toInt();
            QStringList namelist = struGsh.vMegreLablst[nCamId];
            QMultiMap<QString, QString> mergeLabMap;
            if(pProfile->vMegerlabmap.size() > nCamId) {
                mergeLabMap = pProfile->vMegerlabmap[nCamId];
            }
             
            for(const QString &label : namelist) {
                if(mergeLabMap.contains(label)) {//! 父标签：父标签及其对应的子标签都需塞进新变量
                    if(pProfile->strcSortclass.vlabParm[i].contains(label)&&
                        pProfile->strcSortclass.vlabParm[i][label].bEn){
                        QJsonObject obj;
                        obj["name"] = label;
                        obj["adjust"] =    pProfile->strcSortclass.vlabParm[i][label].bEn;
                        obj["min_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][label].nlowthd,'f',2);
                        obj["max_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][label].nhigthd,'f',2);
                        jsonary.append(obj);
                    }

                    QList<QString> subLabels = mergeLabMap.values(label);
                    for(const QString &subLabel : subLabels) {
                        if(pProfile->strcSortclass.vlabParm[i].contains(subLabel)&&
                           pProfile->strcSortclass.vlabParm[i][subLabel].bEn){
                            QJsonObject obj;
                            obj["name"] = subLabel;
                            obj["adjust"] =    pProfile->strcSortclass.vlabParm[i][subLabel].bEn;
                            obj["min_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][subLabel].nlowthd,'f',2);
                            obj["max_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][subLabel].nhigthd,'f',2);
                            jsonary.append(obj);
                        }
                    }
                } else {//! 是子标签：只需塞进子标签
                    //! 检查是否已经作为某个父标签的子标签被添加过
                    bool alreadyAdded = false;
                    for(auto it = mergeLabMap.begin(); it != mergeLabMap.end(); ++it) {
                        if(it.value() == label && namelist.contains(it.key())) {
                            alreadyAdded = true;
                            break;
                        }
                    }
        
                    if(!alreadyAdded) {
                        if(pProfile->strcSortclass.vlabParm[i].contains(label) &&
                            pProfile->strcSortclass.vlabParm[i][label].bEn){
                            QJsonObject obj;
                            obj["name"] = label;
                            obj["adjust"] =    pProfile->strcSortclass.vlabParm[i][label].bEn;
                            obj["min_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][label].nlowthd,'f',2);
                            obj["max_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][label].nhigthd,'f',2);
                            jsonary.append(obj);
                        }
                    }
                }
            }

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape){
                for(int j=0; j<struGsh.vShapeLab[i].size(); j++){
                    QString labname =  struGsh.vShapeLab[i].at(j);
                    if(pProfile->strcSortclass.vlabParm[i].contains(labname)){
                        QJsonObject jsonshape;
                        jsonshape["name"] =labname;
                        jsonshape["adjust"] =    pProfile->strcSortclass.vlabParm[i][labname].bshapeEn;
                        jsonshape["min_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][labname].nshapeLowthd,'f',2);
                        jsonshape["max_value"] = QString::number(pProfile->strcSortclass.vlabParm[i][labname].nshapeHigthd,'f',2);
                        jsonary.append(jsonshape);
                    }
                }
            }
        }//! end 相机

        jsonmid["analysis_id"] = i;
        jsonmid["analysis_name"] = QString(pProfile->strcSortclass.sortclassname[i]);
        jsonmid["material"] = pProfile->strcSortclass.nsourceIdx[i]+1;
        jsonmid["adjust"] = pProfile->strcSortclass.bEnClass[i];
        jsonmid["clear_history"] = bclear;
        jsonmid["model"] = jsonary;

        jsontop.append(jsonmid);
        // qDebug()<<jsontop;
        // qDebug()<<"\n\n";
    }//! end 调节口

    objjson["code"] = 1;
    objjson["data"] = jsontop;
    g_localSocket->writedata(objjson);
}
/*!
 * \brief 发送色选联调的ip信息
 */
void GlobalFlow::sendSorterIPparams(bool bshow)
{
    QJsonObject jsonnettop;
    QJsonObject jsonnet;
    jsonnet["show"] = bshow;
#if SORTERADJ_BY_VIRTUALIP
    jsonnet["ip"] = struGsh.sIp;
#else
    jsonnet["ip"] = struCnfg.sLocalIP;
#endif

    jsonnet["langue"] = myLan.nLang == 0 ? 0:1;
    jsonnettop["code"] = 0;
    jsonnettop["data"] = jsonnet;
    g_localSocket->writedata(jsonnettop);
}

/*!
 * \brief 发送分析结果至色选机
 */
void GlobalFlow::sendStaticsrestoSorter()
{
    QStringList abnorlablst;

    QJsonObject jsonobj, jsonresult;
    QJsonArray jsonarray;


    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    for(int i=0; i<pProfile->nStaticGroup; i++){
        //! 根据统计组下标获取对应的相机下标
        auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
        if(i >= pProfile->vUnitaddrlist.size() || pProfile->vUnitaddrlist[i].size() == 0) {
            continue;
        }
        
        int nCamId = QString("%1").arg(pProfile->vUnitaddrlist[i].at(0)).toInt();
        if(nCamId >= struGsh.vMegreLablst.size()) {
            continue;
        }
        
        //! 填充物料统计
        int ntotal = 0;
        int nAvlg = 0 , nAvwid = 0;
        for(QString stacat : struGsh.vsMatinfo[i].staInfo.keys()){
            QString valstr = struGsh.vsMatinfo[i].staInfo[stacat];            
            if(valstr.compare("M0/A1") == 0){
                ntotal = struGsh.vsMatinfo[i].staRes[stacat][0];
            }else if(valstr.compare("M1/A6") ==0){//! 平均长度
                nAvlg = struGsh.vsMatinfo[i].staRes[stacat][0] /
                        struGsh.vmatTotalmap[i][valstr];
            }else if(valstr.compare("M2/A6")==0){//! 平均宽度
                nAvwid = struGsh.vsMatinfo[i].staRes[stacat][0] /
                        struGsh.vmatTotalmap[i][valstr];
            }else if(valstr.contains("M1/A10") && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape){
                QJsonObject matjsonobj;
                QJsonObject matjsonobj1;
                matjsonobj["name"] = struGsh.vShapeLab[i][1];
                matjsonobj["ratio"] = struGsh.vsMatinfo[i].staRes[stacat][0]/
                                      struGsh.vmatTotalmap[i][valstr];
                matjsonobj1["name"] = struGsh.vShapeLab[i][0];
                matjsonobj1["ratio"] = struGsh.vsMatinfo[i].staRes[stacat][2]/
                                       struGsh.vmatTotalmap[i][valstr];
                
                jsonarray.append(matjsonobj);
                jsonarray.append(matjsonobj1);

            }
        }

        //! 填充标签
        QStringList mergeLabelList = struGsh.vMegreLablst[nCamId];
        for(QString mergeLabel : mergeLabelList){
            QJsonObject siglelabobj;
            siglelabobj["name"] = mergeLabel;
            
            bool isParentLabel = (pProfile->vMegerlabmap.size() > nCamId && 
                                  pProfile->vMegerlabmap[nCamId].contains(mergeLabel));
            
            if(isParentLabel) { //! 父标签：统计结果为所有子标签的统计和，同时也要填充所有子标签
                QStringList childLabels = pProfile->vMegerlabmap[nCamId].values(mergeLabel);
                double totalValue = 0.0;
                bool hasValidData = false;
                bool bChg = false; //! 标志位---> 判定统计方式是否为A3
                for(QString childLabel : childLabels) {
                    if(struGsh.vlabStamap[i].contains(childLabel)) {
                        for(QString stacat: struGsh.vlabStamap[i][childLabel].staInfo.keys()){
                            QString statype = struGsh.vlabStamap[i][childLabel].staInfo[stacat];
                            if(statype == QString("M0/A1") || statype == QString("M0/A3")){
                                if(struGsh.vlabStamap[i][childLabel].staRes.contains(stacat) &&
                                   struGsh.vlabStamap[i][childLabel].staRes[stacat].size() > 0) {
                                    double childValue =0.0;
                                    if(statype == QString("M0/A1")){
                                        childValue  = struGsh.vlabStamap[i][childLabel].staRes[stacat][0]/ntotal;
                                                       
                                    }else{
                                        childValue  = struGsh.vlabStamap[i][childLabel].staRes[stacat][0]/
                                                      struGsh.vlabTotalmap[i][statype];
                                    }
                                   
                                    totalValue += childValue;
                                    hasValidData = true;
                                    
                                    if(statype == QString("M0/A3")){
                                        if(pProfile->nStaisticType == 0){
                                            childValue = childValue*1000000;
                                        }else{
                                            childValue = childValue*100;
                                        }
                                        bChg = true;
                                    }

                                    if(statype == QString("M0/A1")){
                                        childValue = childValue*100;
                                    }

                                    QJsonObject childlabobj;
                                    childlabobj["name"] = childLabel;
                                    childlabobj["ratio"] = childValue;
                                    jsonarray.append(childlabobj);


                                    //! 统计状态标签状态用于提示
                                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAdjSorterTips &&
                                        pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(childLabel)){
                                        if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][childLabel].nlowthd >childValue||
                                             childValue > pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][childLabel].nhigthd)
                                             abnorlablst<<childLabel;
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
                if(hasValidData) {//! 填充父标签（值为所有子标签的和）
                    if(bChg){ //! "M0/A3"
                        if(pProfile->nStaisticType == 0){
                            totalValue = totalValue*1000000;
                        }else{
                            totalValue = totalValue*100;
                        }
                    }else{//! "M0/A1"
                        totalValue = totalValue*100;
                    }
                    siglelabobj["ratio"] = totalValue;
                    jsonarray.append(siglelabobj);


                    //! 统计状态标签状态用于提示
                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAdjSorterTips &&
                        pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(mergeLabel)){
                        if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][mergeLabel].nlowthd >totalValue||
                             totalValue > pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][mergeLabel].nhigthd)
                            abnorlablst<<mergeLabel;
                    }
                }
            } else {//! 子标签：统计结果为本身
                if(struGsh.vlabStamap[i].contains(mergeLabel)) {
                    for(QString stacat: struGsh.vlabStamap[i][mergeLabel].staInfo.keys()){
                        QString statype = struGsh.vlabStamap[i][mergeLabel].staInfo[stacat];
                        if(statype == QString("M0/A1") || statype == QString("M0/A3")){
                            double childValue =0;
                            if(statype == QString("M0/A1")){
                                 childValue = struGsh.vlabStamap[i][mergeLabel].staRes[stacat][0]/
                                                ntotal;
                            }else{
                                 childValue = struGsh.vlabStamap[i][mergeLabel].staRes[stacat][0]/
                                                struGsh.vlabTotalmap[i][statype];
                            }
                            
                            if(statype == QString("M0/A3")){
                                if(pProfile->nStaisticType == 0){
                                    childValue = childValue*1000000;
                                }else{
                                    childValue = childValue*100;
                                }
                            }

                            if(statype == QString("M0/A1")){
                                childValue = childValue*100;
                            }

                            siglelabobj["ratio"] = childValue;
                            jsonarray.append(siglelabobj);

                            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAdjSorterTips&&
                                pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(mergeLabel)){
                                if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][mergeLabel].nlowthd >childValue||
                                     childValue > pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][mergeLabel].nhigthd)
                                    abnorlablst<<mergeLabel;
                            }

                            break;
                        }
                    }
                }
            }
        }
    }

    if(!jsonarray.isEmpty()){
        jsonobj["total"] = 1;
        jsonobj["analysis_id"] = struGsh.nSorterAdjIdx;
        jsonobj["material"] = pProfile->strcSortclass.nsourceIdx[struGsh.nSorterAdjIdx] + 1;
        jsonobj["result"] = jsonarray;
        jsonresult["code"] = 2;
        jsonresult["data"] = jsonobj;
        g_localSocket->writedata(jsonresult);
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAdjSorterTips && !abnorlablst.isEmpty()){
        emit sigNotifyAdjTips(abnorlablst);
    }
}

/*!
 * \brief 查询色线状态
 */
void GlobalFlow::ReqSorterStatus()
{
    QJsonObject topjson, json;

    json["data"] = 1;

    topjson["code"] = 3;
    topjson["data"] = json;

    g_localSocket->writedata(topjson);
}

/*!
 * \brief 通知供料状态
 */
void GlobalFlow::sendFeederstatoSorter()
{
    QJsonObject topjson, stajson;

    stajson["onoff"] = struGsh.nFeederState;

    topjson["code"] = 4;
    topjson["data"] = stajson;

    g_localSocket->writedata(topjson);
}

void GlobalFlow::sendEdgetraditoncmd()
{
    auto *pProfile = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.traditionArith;
    char data[5] = {0};

    for(int nArith = 0; nArith <TRADITION_ARITH_COUNT; nArith++){
        data[3] = pProfile[nArith].cutEdge ? 1 : 0;
        data[4] = 0;
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_TARDITON_EDGE_PARAMS, data, 5);
            g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
        }
    }

}

void GlobalFlow::sendRowtraditoncmd()
{
    char data = 3;
    for(int nArith = 0; nArith <TRADITION_ARITH_COUNT; nArith++){
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_TARDITON_ROW_PARAMS, &data, 1);
            g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
        }
    }
}

void GlobalFlow::sendgrayAtraditoncmd(int nArithId)
{

    auto *pProfile = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.traditionArith;

    QByteArray data;
    int nMin = 0, nMax = 0;
    int nMain = 0, nVice = 0;


    uchar cVal = (pProfile[nArithId].bEnable?1:0)  //! 厂家设置不勾选，算法使能为false
                 | pProfile[nArithId].pass <<1   //! 传染
                 |(pProfile[nArithId].cutEdge?1:0)<<2       //! 切边缘使能
                 | 0<<3    //! 比列参数使能
                 | 0<<5    //! 独立延迟
                 | (pProfile[nArithId].retain[0].bEn ? 1:0)<<6   //! 色差保留使能
                 | (pProfile[nArithId].retain[1].bEn ? 1:0)<<7;  //! 灰度保留
    data.append(cVal);

    data.append(qPow(2, pProfile[nArithId].mode));   //! 颜色
    data.append(pProfile[nArithId].rowCount - 1);
    data.append((pProfile[nArithId].badPointCount-1)/256);
    data.append((pProfile[nArithId].badPointCount-1)%256);

    int nSens = pProfile[nArithId].nSens;                 //! 灵敏度偏置？
    nSens = (nSens > 0) ? nSens : 0;
    if (pProfile[nArithId].bright) {
        nMin = 255-(int)(nSens*0.255);
        nMax = 255;
        nMin = (nMin > 0) ? nMin : 0;
    } else {
        nMin = 0;
        nMax = (nSens*0.255+0.5);
        nMax = (nMax < 255) ? nMax : 255;
    }
    data.append(nMin/256);
    data.append(nMin%256);
    data.append(nMax/256);
    data.append(nMax%256);

    data.append(static_cast<char>(0));  //! 比例参数
    data.append(static_cast<char>(0));

    nMin = 100;
    nMax = 255;
    data.append(nMin);
    data.append(nMax);

    //! 保留参数
    if (pProfile[nArithId].retain[1].bright) { //! 灰度保留
        nMin = 255-(int)(pProfile[nArithId].retain[1].sensitivity*0.255);
        nMax = 255;
        nMin = (nMin > 0) ? nMin : 0;
    } else {
        nMin = 0;
        nMax = (pProfile[nArithId].retain[1].sensitivity*0.255+0.5);
        nMax = (nMax < 255) ? nMax : 255;
    }
    data.append(qPow(2, pProfile[nArithId].retain[1].mode));
    data.append(nMin);
    data.append(nMax);

    if (pProfile[nArithId].retain[0].bright) { //! 色差保留
        nMin = 255-(int)(pProfile[nArithId].retain[0].sensitivity*0.255);
        nMax = 255;
        nMin = (nMin > 0) ? nMin : 0;
    } else {
        nMin = 0;
        nMax = (pProfile[nArithId].retain[0].sensitivity*0.255+0.5);
        nMax = (nMax < 255) ? nMax : 255;
    }

    auto getDistype = [&nMain, &nVice](int nDiscolor){
        QMap<int, QPair<int, int>> map = {
            {0, {1, 2}},
            {1, {1, 4}},
            {2, {2, 4}},
            {3, {8, 1}},
            {4, {8, 2}},
            {5, {8, 4}}
        };
        if (map.keys().contains(nDiscolor)) {
            nMain = map[nDiscolor].first;
            nVice = map[nDiscolor].second;
        }else {
            nMain = 1;
            nVice = 2;
        }
    };

    getDistype(pProfile[nArithId].retain[0].mode);
    data.append(nMain<<4|nVice);
    data.append(nMin);
    data.append(nMax);

    data.append(4, (char)0);

    data.append(static_cast<char>(0));  //! 切边缘圈数怎么设置？

    int nCmd = 0;
    if (ARITH_GREY_A == nArithId) {
        nCmd = CMD_TARDITON_GRAYA_PARAMS;
    } else if (ARITH_GREY_B == nArithId) {
        nCmd = CMD_TARDITON_GRAYB_PARAMS;
    }


    for(int i=0; i<getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, nCmd, data.data(), data.size());
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}

void GlobalFlow::sendgrayBtraditoncmd()
{

}

void GlobalFlow::senddiscolorAtraditoncmd(int nArithId)
{
    auto *pProfile = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.traditionArith;

    QByteArray data;
    int nMin = 0, nMax = 0;
    int nMain = 0, nVice = 0;

    uchar cVal = (pProfile[nArithId].bEnable?1:0)
                 | pProfile[nArithId].pass <<1
                 |(pProfile[nArithId].cutEdge?1:0)<<2
                 | 0<<3
                 | 0<<4
                 | 0<<5
                 |(pProfile[nArithId].retain[0].bEn ? 1: 0)<<6
                 |(pProfile[nArithId].retain[1].bEn ? 1:0)<<7;

    data.append(cVal);

    auto getDistype = [&nMain, &nVice](int nDiscolor){
        QMap<int, QPair<int, int>> map = {
            {0, {1, 2}},
            {1, {1, 4}},
            {2, {2, 4}},
            {3, {8, 1}},
            {4, {8, 2}},
            {5, {8, 4}}
        };
        if (map.keys().contains(nDiscolor)) {
            nMain = map[nDiscolor].first;
            nVice = map[nDiscolor].second;
        }else {
            nMain = 1;
            nVice = 2;
        }
    };
    getDistype(pProfile[nArithId].mode);
    data.append(nMain<<4|nVice);
    data.append(pProfile[nArithId].rowCount - 1);
    data.append((pProfile[nArithId].badPointCount-1)/256);
    data.append((pProfile[nArithId].badPointCount-1)%256);

    int nSens = pProfile[nArithId].nSens;

    nSens = (nSens > 0) ? nSens : 0;
    if (pProfile[nArithId].bright) {
        nMin = 16383-(int)(nSens*1.6383);
        nMax = 16383;
        nMin = (nMin > 0) ? nMin : 0;
    } else {
        nMin = 0;
        nMax = (nSens*1.6383+0.5);
        nMax = (nMax < 16383) ? nMax : 16383;
    }
    data.append(nMin/256);
    data.append(nMin%256);
    data.append(nMax/256);
    data.append(nMax%256);

    data.append(static_cast<char>(0));

    data.append(static_cast<char>(0));
    data.append(static_cast<char>(0));

    nMin = 0;
    nMax = 255;
    data.append(nMin);
    data.append(nMax);

    if (pProfile[nArithId].retain[1].bright) {
        nMin = 255-(int)(pProfile[nArithId].retain[1].sensitivity*0.255);
        nMax = 255;
        nMin = (nMin > 0) ? nMin : 0;
    } else {
        nMin = 0;
        nMax = (pProfile[nArithId].retain[1].sensitivity*0.255+0.5);
        nMax = (nMax < 255) ? nMax : 255;
    }
    data.append(qPow(2, pProfile[nArithId].retain[1].mode));
    data.append(nMin);
    data.append(nMax);

    if (pProfile[nArithId].retain[0].bright) {
        nMin = 255-(int)(pProfile[nArithId].retain[0].sensitivity*0.255);
        nMax = 255;
        nMin = (nMin > 0) ? nMin : 0;
    } else {
        nMin = 0;
        nMax = (pProfile[nArithId].retain[0].sensitivity*0.255+0.5);
        nMax = (nMax < 255) ? nMax : 255;
    }

    getDistype(pProfile[nArithId].retain[0].mode);
    data.append(nMain<<4|nVice);
    data.append(nMin);
    data.append(nMax);

    data.append(4, (char)0);

    data.append(static_cast<char>(0));

    int nCmd = 0;
    if(ARITH_DISCOLOR_A == nArithId){
        nCmd = CMD_TARDITON_DISCOLORA_PARAMS;
    }else if(ARITH_DISCOLOR_B==nArithId){
        nCmd = CMD_TARDITON_DISCOLORB_PARAMS;
    }

    for(int i=0; i<getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, nCmd, data.data(), data.size());
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}

void GlobalFlow::senddiscolorBtraditoncmd()
{

}

void GlobalFlow::packetTrationcmd()
{

}

void GlobalFlow::sendlaboftraditon2Soc()
{
//    bool bTraditionLabelEn[MAX_CAM_NUM][MAX_LABELS] = {}; //! 标签传统算法使能
//    int nTraditionLabelAlgorithm[MAX_CAM_NUM][MAX_LABELS] = {}; //! 标签关联的传统算法

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    QByteArray data;

    for(int i=0; i<MAX_LABELS; i++){
        if(pProfile->bTraditionLabelEn[0][i]){
            data.append(static_cast<char>(1));
            data.append(pProfile->nTraditionLabelAlgorithm[0][i]);
        }else{
            data.append(static_cast<char>(0));
            data.append(0xFF);
        }
    }

    for(int j=0; j<getMachineCamNum(); j++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, j, CMD_LABEL_TRADITONARITH, data.data(), data.size());
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}


/* 发送红外偏置      前置板 */
void GlobalFlow::setInfBiasingParams(char nAnalyzeIndex, int nUnitAddr)
{
    if(SENSOR_IM01 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr] &&
        SENSOR_IM04 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr])
        return;

    char data[3] = {0};
    int nCmd = CMD_UNIT_IM01_BIAS;

    for(int m = COLOR_TYPE_R; m < COLOR_TYPE_NUM; m++){
        if(SENSOR_IM01 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
            data[0] = m;
            data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->infraBias[nUnitAddr][m];
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, 2);

            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        }else{
            data[0] = m;
            data[1] = 0;
            data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->infraBias[nUnitAddr][m];
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, 3);

            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
            break;
        }

    }
}

void GlobalFlow::setBackgroundSensorIM01(char nAnalyzeIndex, int nUnitAddr)
{
    if(SENSOR_IM01 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]&&
         SENSOR_IM04 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr])
        return;

    char data[14] = {0};
    int nCmd = CMD_UNIT_IM01_BKVALUE;

    data[0] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nBackgdIdx[nUnitAddr] == 0 ? 0 : 1;
    data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_thread[nUnitAddr];
    data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_thread[nUnitAddr];
    data[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_thread[nUnitAddr];

    data[4] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_limit[nUnitAddr];
    data[5] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_limit[nUnitAddr];
    data[6] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_limit[nUnitAddr];

    data[8] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_thread[nUnitAddr];
    data[9] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_thread[nUnitAddr];
    data[10] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_thread[nUnitAddr];

    data[11] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_limit[nUnitAddr];
    data[12] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_limit[nUnitAddr];
    data[13] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->IM01_back_limit[nUnitAddr];

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, 14);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    msleep(5);

    QByteArray babck = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nUnitAddr, nCmd, data, 14);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(babck);
    msleep(2);
}

//IM01模拟增益
void GlobalFlow::setsetAnaGainParams(char nAnalyzeIndex, int nUnitAddr)
{
    if(SENSOR_IM01 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]&&
        SENSOR_IM04 != struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr])
        return;

    char nDataLength = 6;
    char data[nDataLength] = {0};
    int nCmd = CMD_UNIT_IM01_GAIN;

    analyzeGlobal *pAnalyzeGlobal = gFlow.getSpecifyAnalyzeGlobal(nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    for(int m = COLOR_TYPE_R; m < COLOR_TYPE_NUM; m++){
        if(SENSOR_IM01 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
            data[0] = m;
            data[1] = pProfile->nGainSlow[nUnitAddr][m];
            data[2] = pProfile->nGainFast[nUnitAddr][m];

            QByteArray ba = packet_TypeA_NetData(CAMERA_QD, MODULE_CAM, nUnitAddr, nCmd, data, 3);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        }else{
            data[0] = m;
            data[3] = pProfile->nGainSlow[nUnitAddr][m];
            data[4] = (pProfile->nGainFast[nUnitAddr][m]>>8)&0xff;
            data[5] = pProfile->nGainFast[nUnitAddr][m]&0xff;
            QByteArray ba = packet_TypeA_NetData(CAMERA_QD, MODULE_CAM, nUnitAddr, nCmd, data, 6);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
            break;
        }
    }
}

/**
 * @brief GlobalFlow::发送数字增益
 * @param Nfast
 * @param Nslow
 * @param Rfast
 * @param Rslow
 * @param Gfast
 * @param Gslow
 * @param Bfast
 * @param Bslow
 */
void GlobalFlow::sendGainPara(char nAnalyzeIndex, int nUnitAddr)
{
    if(SENSOR_IM01 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]
        ||SENSOR_IM04 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
        gFlow.setsetAnaGainParams(nAnalyzeIndex, nUnitAddr);
    } else{
        char nDataLength = 12;
        char data[nDataLength] = {0};
        int nCmd = CMD_DGIGAIN;

        analyzeGlobal *pAnalyzeGlobal = gFlow.getSpecifyAnalyzeGlobal(nAnalyzeIndex);
        struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

        data[0] = pProfile->nGainFast[nUnitAddr][COLOR_TYPE_N];
        data[1] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_N]/256;
        data[2] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_N]%256;

        data[3] = pProfile->nGainFast[nUnitAddr][COLOR_TYPE_R];
        data[4] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_R]/256;
        data[5] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_R]%256;

        data[6] = pProfile->nGainFast[nUnitAddr][COLOR_TYPE_G];
        data[7] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_G]/256;
        data[8] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_G]%256;

        data[9] = pProfile->nGainFast[nUnitAddr][COLOR_TYPE_B];
        data[10] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_B]/256;
        data[11] = pProfile->nGainSlow[nUnitAddr][COLOR_TYPE_B]%256;

        QByteArray ba = packet_TypeA_NetData(CAMERA_QD, MODULE_CAM, nUnitAddr, nCmd, data, nDataLength);

        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }
}

/*!
 * \brief 相机触发方式
 * \param nAnalyzeIndex
 */
void GlobalFlow::sendcamtrigerType(int nAnalyzeIndex)
{
    char data = struCnfg.analyzeInfo[nAnalyzeIndex]->nCamTrigerType;
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_CAM_TRIGER_TYPE, &data, 1);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }
}

/**
 * @brief GlobalFlow::发送灯光值
 * @param nMat
 * @param nBak
 * @param nInf
 */
void GlobalFlow::sendLampPara(char nAnalyzeIndex)
{
    char nDataLength = 12;
    char data[nDataLength] = {0};
    int nCmd = CMD_SRC_LIGHT;

    analyzeGlobal *pAnalyzeGlobal = getSpecifyAnalyzeGlobal(nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    for(int i=0; i<LAMP_TYPE; i++){
        int nvalue =0 ;
        if(pProfile->bEnlamp[i]){
            nvalue = pProfile->nLampValue[i];
        }
       data[2*i] = nvalue/256;
       data[2*i+1] = nvalue%256;
    }

    data[8] = pProfile->bHologenLampOn ? 1 : 0;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

    //! 发送恒流源
//    sendPlusePara(nAnalyzeIndex);
}

/**
 * @brief GlobalFlow::发送脉冲灯光值
 * @param nMat
 * @param nBak
 * @param nInf
 */
void GlobalFlow::sendPlusePara(char nAnalyzeIndex)
{
    char nDataLength = 12;
    char data[nDataLength] = {0};
    int nCmd = CMD_SRC_LIGHT;

    analyzeGlobal *pAnalyzeGlobal = getSpecifyAnalyzeGlobal(nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    for(int i = 0; i < PLUSE_TYPE; i++){
        int nvalue = 0;
        if(pProfile->bEnPluse[i]){
            nvalue = pProfile->nPluseValue[i];
        }
        data[2*i] = nvalue/256;
        data[2*i+1] = nvalue%256;
    }

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_PULSE, 0, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::下发背景波动范围
 * @param value
 * @param nUnitAddr
 */
void GlobalFlow::sendBackGroundValue(char nAnalyzeIndex, int nUnitAddr)
{
    bool bIM01 =false;
    if(SENSOR_IM01 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]
        || SENSOR_IM04 == struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
        bIM01 = true;
    }

    if(bIM01)
        setBackgroundSensorIM01(struGsh.nAnalyzeIndex, nUnitAddr);
    else{
        char nDataLength = 2;
        char data[nDataLength] = {0};
        int nCmd = CMD_BKG_VALUE;
        data[0] = 1;
        data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nThreadValue[nUnitAddr];

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, nDataLength);

        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }
}

/**
 * @brief GlobalFlow::下发像元划分
 * @param value
 * @param nUnitAddr
 */
void GlobalFlow::sendCameraPixelChannel(char nAnalyzeIndex, int nUnitAddr)
{
    char nDataLength = 5;
    char data[nDataLength] = {0};
    int nCmd = CMD_CAMERA_PIXEL;
    data[0] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nPixelBegin[nUnitAddr]/256;
    data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nPixelBegin[nUnitAddr]%256;

    data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nPixelEnd[nUnitAddr]/256;
    data[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->struCaParams->nPixelEnd[nUnitAddr]%256;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnitAddr, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::振动器控制
 */
void GlobalFlow::sendFeederPara(char nAnalyzeIndex, bool bMansw, bool onoff, bool onoff2)
{
    char nDataLength = 3*MAX_VIBRATOR;
    char data[nDataLength] = {0};
    int nCmd = CMD_CTRL_FEED;
    double nFeeder = struCnfg.analyzeInfo[nAnalyzeIndex]->nFeeder[0]*12.267+790.81;
    double nFeeder2 = struCnfg.analyzeInfo[nAnalyzeIndex]->nFeeder[1]*12.267+790.81;

    int sendFeeder = nFeeder;
    int sendFeeder2 = nFeeder2;

    if(bMansw){
        data[0] = onoff?1:0;
        data[3] = onoff2?1:0;
    }else{
        data[0] = struGsh.analyzeInfo[nAnalyzeIndex]->bFeeder?1:0;
        data[3] = struGsh.analyzeInfo[nAnalyzeIndex]->bFeeder?1:0;
    }

    data[1] = sendFeeder/256;
    data[2] = sendFeeder%256;
    data[4] = sendFeeder2/256;
    data[5] = sendFeeder2%256;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

void GlobalFlow::setEjectorSelfCheck(int nEjNum, int nOnOff, int nId)
{
    Q_UNUSED(nId);

    QByteArray data;
    data.append(nOnOff);
    char ej[10];
    memset(ej, 0, sizeof(ej));
    int ejectors = nEjNum%EJ_NUM;       //! 喷嘴在通道内的序号
    ej[9-ejectors/8] = 1 << (ejectors%8);
    data.append(ej, 10);

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_INT_EJTEST, data, data.size());
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);

}

/**
 * @brief GlobalFlow::关闭震动量
 * @param nAnalyzeIndex
 */
void GlobalFlow::resetFeederPara(char nAnalyzeIndex)
{
    char nDataLength = 3;
    char data[nDataLength] = {0};
    int nCmd = CMD_CTRL_FEED;

    data[1] = 0/256;
    data[2] = 0%256;


    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::发送清灰参数
 * @param turn::1-清灰 2-吸料
 * @param nAnalyzeIndex
 */
void GlobalFlow::sendWipePara(char turn, char nAnalyzeIndex)
{
    char nDataLength = 3;
    char data[nDataLength] = {0};
    int nCmd = CMD_CTRL_WIPE;
    int nWipeDelay = struCnfg.analyzeInfo[nAnalyzeIndex]->nWipeDelay;
    data[0] = turn;

    data[1] = nWipeDelay/256;
    data[2] = nWipeDelay%256;


    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::sendAbsorbPara
 * @param turn::2 吸料
 * @param nAnalyzeIndex
 * @param timeout
 */
void GlobalFlow::sendAbsorbPara(char turn, char nAnalyzeIndex, int timeout)
{
    char nDataLength = 3;
    char data[nDataLength] = {0};
    int nCmd = CMD_CTRL_WIPE;
    data[0] = turn;

    data[1] = timeout/256;
    data[2] = timeout%256;


    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}


/*!
 * \brief 发送喷阀数据包
 * \param array
 */
void GlobalFlow::sendEjtpacket(QByteArray array, int nDataLength)
{
    QByteArray ba = packet_TypeI_NetData(MODULE_CTRL, 0, CMD_EJECTOR_PACKET, array.data(), nDataLength);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
}

void GlobalFlow::setHSTempCtrl(int threshold, bool enable)
{
    QByteArray data;
    int tempThreshold = 577.78176-threshold*1.51552;
    data.append((int)enable);
    data.append(tempThreshold/256);
    data.append(tempThreshold%256);


    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, gFlow.getMachineCamNum()-1, CMD_CAM_HIGH_SPECTRAL_TEMPCTRL, data, 3);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
}

/*!
 * \brief 发送吹气延迟和吹气时间
 */
void GlobalFlow::sendBlowTime()
{
    char data[3] = {0};
    int nCmd = CMD_BLOW_TIME;
    data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowTime;
    data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nTruncTime;
    data[2] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nDegTime;


    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, 3);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 发送吹气延迟和吹气时间
 */
void GlobalFlow::sendDelayTime()
{
    char data[2] = {0};
    int nCmd = CMD_DELAY_TIME;
    data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay/256;
    data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay%256;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, 2);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::发送开关下料
 */
void GlobalFlow::sendOnOff(int nAnalyzeIndex)
{ 
    char data;
    int nCmd = CMD_CAM_ONOFF;
    data = struGsh.analyzeInfo[nAnalyzeIndex]->bFeeder?1:0;

    bool bFeeder = struGsh.analyzeInfo[nAnalyzeIndex]->bFeeder;

    if(bFeeder){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, &data, 1);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

    for (int i = 0; i < gFlow.getMachineCamNum(); i++) {

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, nCmd, &data, 1);

        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

    if(!bFeeder){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, &data, 1);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }
}

/**
 * @brief GlobalFlow::发送SOC开关
 */
void GlobalFlow::sendSocOnOff(int nAnalyzeIndex)
{
    char data[6]={0,0,0,0,0,0};
    int nCmd = CMD_SOC_ONOFF;
    data[0] = struGsh.analyzeInfo[nAnalyzeIndex]->bFeeder?1:0;
    data[1] =  0;

    for (int i = 0; i < gFlow.getMachineCamNum(); i++) {

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, nCmd, data, 6);

        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }
}

/*!
 * \brief 发送背景棍开关命令
 */
void GlobalFlow::sendbackgdrodOnoffcmd(bool bsta)
{
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnBackdgrod){
        int nCmd= 0x0011;
        char data = bsta ? 1:0;
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, &data, 1);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}

/**
 * @brief GlobalFlow::重置相机板、高算力，使得图片行数从1开始
 */
void GlobalFlow::ResetCameraData(int nAnalyzeIndex)
{
    auto setCamera = [&](bool bOn){
        int nCmd = CMD_CAM_ONOFF;
        char data = bOn ? 1 : 0;

        for (int i = 0; i < MAX_CAM_NUM; i++) {
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, nCmd, &data, 1);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        }
    };

    auto setSoc = [&](bool bOn){
        int nCmd2 = CMD_SOC_ONOFF;
        char data2[4];
        data2[0] = bOn ?  1 : 0;
        data2[1] = 0;

        int yellowTmp =0;
        data2[2] = yellowTmp/256;
        data2[3] = yellowTmp%256;

        for (int i = 0; i < MAX_CAM_NUM; i++) {
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, nCmd2, data2, 4);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        }
    };

    setCamera(false);
    setSoc(false);
    msleep(100);
    setSoc(true);
    setCamera(true);
}

/**
 * @brief GlobalFlow::控制板版本查询
 */
void GlobalFlow::reqCtrlVersion(int nAnalyzeIndex)
{
    char data = 1;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_VERSION, &data, 1);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::恒流源控制板版本查询
 */
void GlobalFlow::reqSourceVersion(int nAnalyzeIndex)
{
    char data = 1;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_PULSE, 0, CMD_SOURCE_VERSION, &data, 1);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 卤素灯检测板版本查询
 * \param nAnalyzeIndex
 */
void GlobalFlow::reqHalogenVersion(int nAnalyzeIndex)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_HALOGEN, 0, CMD_HALOGEN_VERSION, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 电磁阀版本
 * \param nAnalyzeIndex
 */
void GlobalFlow::reqSVBoardVersion(int nAnalyzeIndex)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SVBOARD, 0, CMD_VERSION, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 低压阀版本查询
 * \param nAnalyzeIndex
 */
void GlobalFlow::reqValBoardVersion(int nAnalyzeIndex)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_VAL, 0, CMD_VERSION, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 查询卤素灯的状态
 * \param nAnalyzeIndex
 */
void GlobalFlow::reqHalogenStatus(int nAnalyzeIndex)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_HALOGEN, 0, CMD_HALOGEN_STATUS, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba,1);
}

/*!
 * \brief 查询控制板的状态
 * \param nAnalyzeIndex
 */
void GlobalFlow::reqCtrlBoardalarm(void)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_CTRL_VLAVE_STA, &data, 1);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
}

/**
 * @brief GlobalFlow::获取SOC版本信息
 * @param nAnalyzeIndex
 */
void GlobalFlow::reqSocVersion(int nAnalyzeIndex, int camIndex)
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int nDataLength = strlen(time.toLatin1().data()) + 2;

    char data[nDataLength] = {0};
    data[0] = 1;
    data[1] = (char)(nDataLength-2);
    memcpy(data+2, time.toLatin1().data(), nDataLength-2);

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camIndex, CMD_SOC_VERSION, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 查询模型信息
 * \param nAnalyzeIndex
 * \param camIndex
 */
void GlobalFlow::reqSocModelinfo(int nAnalyzeIndex, int camIndex)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camIndex, CMD_SOC_MODELINFO, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

}

/*!
 * \brief Soc参数
 * \param nAnalyzeIndex
 * \param camIndex
 */
void GlobalFlow::SocParamssend(int nAnalyzeIndex, int camIndex)
{
    Q_UNUSED(camIndex);
    int nIMCam = -1, nUltraCam = -1;

    for(int nUnit=0; nUnit<gFlow.getMachineCamNum(); nUnit++){
        int index =0;
        char data[31];
        memset(data, 0, 31);
        int arithSens = 0;

        int nsize = struGsh.vlabNamemap[nUnit].size();
        if(nsize > 15){
            for(int nArithId = 0; nArithId < 15; nArithId++){ //! 前15个标签

                data[index++] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[nUnit][nArithId] ? 1:0;

                arithSens = 100 - struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUnit][nArithId];

                if(arithSens < 0){
                    arithSens = 0;
                }

                if(struCnfg.nMachineType == nMachine_Crop){
                    if(struGsh.bCapimg){
                        data[index++] = arithSens;
                    }else{
                        data[index++] = 10;
                    }
                }else{
                    data[index++] = arithSens;
                }
            }
            data[30] = 0;
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nUnit, CMD_SOC_PARAMS, data, 31);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
            gFlow.msleep(20);


            int index1 =0;
            char data1[31];
            int arithSens1 = 0;
            for(int nId = 15; nId < nsize; nId++){//! 后15个标签
                data1[index1++] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[nUnit][nId] ? 1:0;
                arithSens1 = 100 - struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUnit][nId];
                if(arithSens1 < 0){
                    arithSens1 = 0;
                }
                if(struCnfg.nMachineType == nMachine_Crop){
                    if(struGsh.bCapimg){
                        data1[index1++] = arithSens1;
                    }else{
                        data1[index1++] = 10;
                    }
                }else{
                    data1[index1++] = arithSens1;
                }
            }
            data1[30] = 0;
            QByteArray ba1 = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nUnit, CMD_SOC_PARAMS2, data1, 31);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba1);
            gFlow.msleep(20);

        }else{
            for(int nArithId = 0; nArithId < nsize; nArithId++){
                data[index++] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[nUnit][nArithId] ? 1:0;

                arithSens = 100 - struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUnit][nArithId];

                if(arithSens < 0){
                    arithSens = 0;
                }

                if(struCnfg.nMachineType == nMachine_Crop){
                    if(struGsh.bCapimg){
                        data[index++] = arithSens;
                    }else{
                        data[index++] = 10;
                    }
                }else{
                    data[index++] = arithSens;
                }
            }

            data[30] = 0;
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nUnit, CMD_SOC_PARAMS, data, 31);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
            gFlow.msleep(20);
        }

        if(gFlow.bIsIMSerialCam(nUnit)){
            nIMCam = nUnit;
        }

        if(gFlow.bIsUltraCam(nUnit)){
            nUltraCam = nUnit;
        }
    }

    if((nIMCam !=-1)&&(nUltraCam != -1)){//! 瓶片 高光谱和紫外灵敏度
        QList<QString> namelist = struGsh.vlabNamemap[nIMCam].values();

        char da[6];
        da[0] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nIMCam][namelist.size()]/255;
        da[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nIMCam][namelist.size()]%255;
        da[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUltraCam][0]/255;
        da[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUltraCam][0]%255;
        da[4] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUltraCam][1]/255;
        da[5] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[nUltraCam][1]%255;
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nIMCam, CMD_SOC_EXTRAPARAMS, da, 6);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

        QByteArray ba1 = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nUltraCam, CMD_SOC_EXTRAPARAMS, da, 6);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba1);
    }
}

/*!
 * \brief 拍标记图使能标签
 */
void GlobalFlow::SocCaplabEnParamsSend()
{
    int nCamNum = qMin(gFlow.getMachineCamNum(), struGsh.vlabNamemap.size());

    for (int i = 0; i < nCamNum; i++) {
        char data[30];
        memset(data, 0, 30);
        int nIdx =0;
        for(auto labidx : struGsh.vlabNamemap[i].keys()){
            data[nIdx++] = struGsh.bEnCaplabs[i][labidx];
        }
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_SOC_CAPIMG_LAB_ENABLE, data, nIdx);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }

}

void GlobalFlow::SocParamssendOne(int nAnalyzeIndex, int camIndex, int camAddr)
{
    int index =0;
    char data[31];
    int arithSens = 0;

    int nsize = struGsh.vlabNamemap[camIndex].size();
    if(nsize > 15){
        for(int nArithId = 0; nArithId < 15; nArithId++){ //! 前15个标签

            data[index++] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[camIndex][nArithId] ? 1:0;

            arithSens = 100 - struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[camIndex][nArithId];
            if(arithSens < 0){
                arithSens = 0;
            }
//                data[index++] = arithSens;
            data[index++] = 10;
        }
        data[30] = 0;
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camAddr, CMD_SOC_PARAMS, data, 31);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        gFlow.msleep(20);

        int index1 =0; //! 后15个标签
        char data1[31];
        int arithSens1 = 0;

        for(int nId = 15; nId < nsize; nId++){
            data1[index1++] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[camIndex][nId] ? 1:0;
            arithSens1 = 100 - struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[camIndex][nId];
            if(arithSens1 < 0){
                arithSens1 = 0;
            }
//                data1[index1++] = arithSens1;
            data1[index1++] = 10;
        }
        data1[30] = 0;
        QByteArray ba1 = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camAddr, CMD_SOC_PARAMS2, data1, 31);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba1);
        gFlow.msleep(20);

    }else{
        for(int nArithId = 0; nArithId < nsize; nArithId++){
            data[index++] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[camIndex][nArithId] ? 1:0;

            arithSens = 100 - struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.sens[camIndex][nArithId];
            if(arithSens < 0){
                arithSens = 0;
            }
//                data[index++] = arithSens;
            data[index++] = 10;
        }

        data[30] = 0;
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camAddr, CMD_SOC_PARAMS, data, 31);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        gFlow.msleep(20);
    }
}

/*!
 * \brief Soc高级参数
 * \param nAnalyzeIndex
 * \param camIndex
 */
void GlobalFlow::SocAdParamssend(int nAnalyzeIndex, int camIndex)
{
    Q_UNUSED(camIndex);

    char data[11] = {0};
    int nCmd = CMD_SOC_ADPARAMS;

    for (int i = 0; i < gFlow.getMachineCamNum(); i++) {
        data[0] = 1;
        data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[i]/256;
        data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[i]%256;
        data[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[i]/256;
        data[4] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[i]%256;

        data[6] =  struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nRes;
        data[7] =  struGsh.nSoccapthd/256;
        data[8] =  struGsh.nSoccapthd%256;
        data[9] =   struGsh.bAISimulate;
        data[10] =   struGsh.bAISaveMat;

        if(struGsh.nCamidvc.contains(i)){
            data[5] =  struGsh.nSoccaptype;
        }else{
            data[5] =  0;
        }
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, nCmd, data, 11);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

}

/*!
 * \brief Soc模型切换
 * \param nAnalyzeIndex
 * \param camIndex
 */
void GlobalFlow::SocModelSWitch(int nAnalyzeIndex, int camIndex)
{
    char data = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nModelIdx[camIndex];
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camIndex, CMD_SOC_MODEL_SWITCH, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

}

/*!
 * \brief Soc图片数据清除
 * \param nAnalyzeIndex
 * \param camIndex
 */
void GlobalFlow::SocImgClear(int nAnalyzeIndex, int camIndex)
{
    int nLen = 1;
    char data[1] = {0};
    data[0]= 1;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camIndex, CMD_SOC_IMG_CLEAR, data, nLen);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief Soc标签信息查询
 * \param nAnalyzeIndex
 * \param camIndex
 */
void GlobalFlow::reqSocModelLabelinfo(int nAnalyzeIndex, int camIndex)
{
    int nLen = 1;
    char data[1] = {0};
    data[0]= 1;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, camIndex, CMD_SOC_MODELLAB_INFO, data, nLen);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

}

/*!
 * \brief Soc通道像元划分
 */
void GlobalFlow::SocChannelinfo()
{
    char data[6] = {0};

    for(int n=0; n<MAX_CAM_NUM; n++){
        data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nPixelBegin[n]/256;
        data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nPixelBegin[n]%256;
        data[2] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nPixelEnd[n]/256;
        data[3] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nPixelEnd[n]%256;
        data[4] = 0;
        data[5] = 59;

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, n, CMD_SOC_CHANNEL_INFO, data, 6);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}
/*!
 * \brief Soc关机
 */
void GlobalFlow::SocShutdown()
{
    int nLen = 1;
    char data[1] = {0};
    data[0]= 1;

    for(int i=0; i<MAX_CAM_NUM; i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_SOC_SHUTDOWN, data, nLen);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}

/*!
 * \brief 查询统计类别信息
 */
void GlobalFlow::reqStainfo(int nCamId)
{
    int nLen = 1;
    char data[1] = {0};
    data[0]= 1;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nCamId, CMD_SOC_STAINFO, data, nLen);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}


/*!
 * \brief SOC参数下发
 * \param nCmd : 命令编码
 * \param nAnalyzeIndex: 下标序号
 * \param nCamId 下标序号
 * \param pData
 * \param nLen
 */
void GlobalFlow::SendSocParams(int nCmd, char *pData, int nLen, int nAnalyzeIndex, int nCamId, bool bAllCam)
{

    char *pDa = new char[nLen];
    memcpy(pDa,pData,nLen);

    if(bAllCam){
        for(int i=0; i<MAX_CAM_NUM;i++){
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, nCmd, pDa, nLen);
            g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
        }
    }else{
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nCamId, nCmd, pDa, nLen);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

    delete[] pDa;

}

/*!
 * \brief 发送soc相关参数
 */
void GlobalFlow::resetSoc(bool bReset)
{
    SocParamssend(struGsh.nAnalyzeIndex, 0);

    SocAdParamssend(struGsh.nAnalyzeIndex, 0);

    sendSOCDateTime();

    sendSocBackConditon();

    SocChannelinfo();

    SocModelLine();

    if(bReset){
        sendSocWorkMode(0);
    }

    gFlow.sendMachineType();
}

/*!
 * \brief 发送soc系统时间
 */
void GlobalFlow::sendSOCDateTime()
{
    int dataLen = 6;
    char data[6]={0};

    QDateTime dateTime = QDateTime::currentDateTime();
    data[0] = dateTime.date().year()-1970;
    data[1] = dateTime.date().month();
    data[2] = dateTime.date().day();
    data[3] = dateTime.time().hour();
    data[4] = dateTime.time().minute();
    data[5] = dateTime.time().second();


    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_SOC_SYSTEM_TIME, data, dataLen);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);

    }
}
/*!
 * \brief 背景条件
 */
void GlobalFlow::sendSocBackConditon()
{
    int dataLen = 4;
    char data[4]={0};

    for(int i=0; i<gFlow.getMachineCamNum(); i++){//! 下发可见的背景条件
        data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdIdx[i];
        if(data[0] == 0 || data[0] ==7){ //! 黑背景or白背景时背景值*3下发
            data[1] = 3*struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdValue[i]/256;
            data[2] = 3*struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdValue[i]%256;
        }else{
            data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdValue[i]/256;
            data[2] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdValue[i]%256;
        }

        data[3] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdRatio[i];


        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] != SENSOR_IM01&&
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] != SENSOR_IM04){//! 发送到SOC
            QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_SOC_BACKGROUND_CONDITION, data, dataLen);
            g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
            msleep(3);
        }

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] == SENSOR_VL08 ||
           struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] == SENSOR_VL05 ){//! 发送到相机
            data[3] = (int)(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nBackgdRatio[i]*2.55+0.5);
            QByteArray babck = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_SOC_BACKGROUND_CONDITION, data, dataLen);
            g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(babck);
            msleep(3);
        }

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] == SENSOR_IM01
            ||struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] == SENSOR_IM04){//! 下发高光谱的背景条件
                gFlow.setBackgroundSensorIM01(struGsh.nAnalyzeIndex, i);
        }
    }
}

/*!
 * \brief 三线 or 四线
 */
void GlobalFlow::SocModelLine()
{
    char data = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelLine + 1;

    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_SOC_Model_LINE, &data, 1);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }

    gFlow.msleep(5);
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_SOC_Model_LINE, &data, 1);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }

}

/*!
 * \brief 设置SOC工作模式
 * \param nCamId
 */
void GlobalFlow::sendSocWorkMode(int nCamId)
{
    Q_UNUSED(nCamId);
    for(int i=0; i<getMachineCamNum();i++){
        char data[3] ={0,0,1};
        data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[i];
        data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[i] ==0?6:12;

        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_SOC_WORKMODE, data, 2);
        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    }
}

/*!
 * \brief SOC传统算法灵敏地
 * \param nCamId
 */
void GlobalFlow::sendSocTraditionSens(int nCamId)
{
    char data[2]= {0};
    data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.sens[nCamId][0];
    data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.sens[nCamId][1];
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nCamId, CMD_SOC_TRADITONSENS, data, 2);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 获取相机地址
 * \param nCamid: 相机序号
 * \return
 */
QString GlobalFlow::getCamName(int nCamid)
{
    QString name;
    switch (nCamid) {
    case 0:
        name = QString("前视1");
        break;
    case 1:
        name = QString("前视2");
        break;
    case 2:
        name = QString("后视1");
        break;
    case 3:
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[3] == SENSOR_IM01 ||
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[3] == SENSOR_IM04){
            name = QString("辅配");
        }else{
            name = QString("后视2");
        }
        break;
    default:
        name = QString("相机");
        break;
    }
    return name;
}

/*!
 * \brief 获取SOC推理关键参数（模型的行数、分辨率）
 */
void GlobalFlow::ReqSocInferParams(int nCamId)
{
    char data = 1;
    QByteArray ba;
    ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, nCamId, CMD_SOC_IMG_PARAMS, &data, 1);

    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/**
 * @brief GlobalFlow::sendInfoToSoc
 * @param nAnalyzeIndex
 * @param module_type
 * @param module_addr
 * @param arg1
 * @param arg2
 * @param data
 */
void GlobalFlow::sendInfoToSoc(int nAnalyzeIndex, char module_type, char module_addr, int nCmd, const char *data, int nDataLength)
{
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, module_type, module_addr, nCmd, data, nDataLength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

bool GlobalFlow::bHighResolution(int nAnalyzeIndex, int nUnit)
{
    if(nAnalyzeIndex < struCnfg.analyzeInfo.size())
        return (PIXEL_2048 == struCnfg.analyzeInfo[nAnalyzeIndex]->nPixel[nUnit]);
    else
        return false;
}

/*!
 * \brief 发送标定系数值
 * \param vValue
 */
void GlobalFlow::sendCalibrateVal2Soc(QVector<float> vValue, bool bClear)
{

    int nSize = vValue.size()/2;
    char data[31] ={0};
    data[0] = bClear;
    data[1] = nSize;
    for(int i=0; i<vValue.size(); i++){
        int nVal = vValue.at(i)*10;
        data[2*i+2] = (nVal>>8)&0xff;
        data[2*i+3] = nVal&0xff;
    }
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC,  gFlow.getMachineCamNum() -1, CMD_SOC_CALIBRATE_VALUE, data, 31);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
    
    // if(nSize <=7){
    //     char data[31] ={0};
    //     data[0] = bClear;
    //     data[1] = nSize;
    //     for(int i=0; i<vValue.size(); i++){
    //         int nVal = vValue.at(i)*10;
    //         data[2*i+2] = (nVal>>8)&0xff;
    //         data[2*i+3] = nVal&0xff;
    //     }
    //     QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC,  gFlow.getMachineCamNum() -1, CMD_SOC_CALIBRATE_VALUE, data, 31);
    //     g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
    // }else{
    //     char data[31] ={0};
    //     char data1[31] = {0};
    //     data[0] = bClear;
    //     data[1] = 7;
    //     data1[0] = bClear;
    //     data1[1] = nSize-7;
    //     for(int i=0; i<7; i++){
    //         for(int j=0; j<2; j++){
    //             int nVal = vValue.at(2*i+j)*10;
    //             data[4*i+2*j+2] = (nVal>>8)&0xff;
    //             data[4*i+2*j+3] = nVal&0xff;
    //         }
            
    //     }
    //     QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC,  gFlow.getMachineCamNum() -1, CMD_SOC_CALIBRATE_VALUE, data, 31);
    //     g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);


    //     for(int k=0; k<nSize-7; k++){
    //         for(int j=0; j<2; j++){
    //              int nVal = vValue.at(2*k+j+14)*10;
    //             data1[4*k+2*j+2] = (nVal>>8)&0xff;
    //             data1[4*k+2*j+3] = nVal&0xff;
    //         }
           
    //     }
    //     QByteArray ba1 = packet_TypeA_NetData(CAMERA_CK, MODULE_SOC,  gFlow.getMachineCamNum() -1, CMD_SOC_CALIBRATE_VALUE_2, data1, 31);
    //     g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba1,1);
    // }
}


/**
 * @brief GlobalFlow::reqCameraVersion
 */
void GlobalFlow::reqCameraVersion(int nAnalyzeIndex, int nUnit)
{
    char data = 1;

    QByteArray ba;

    ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnit, CMD_VERSION, &data, 1);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

}

/**
 * @brief GlobalFlow::发送所有相机参数
 */
void GlobalFlow::sendCameraParams(bool bStart/* = false*/)
{
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        //! 1、发送行频
        for (int j = 0; j < gFlow.getMachineCamNum(); j++) {
            sendRowFreqPara(i, j, SENSOR_D04);
        }

        sleep(1);
        for (int j = 0; j < gFlow.getMachineCamNum(); j++) {
            //! 2、发送增益
            sendGainPara(i, j);

            //! 3、发送像元
            sendCameraPixelChannel(i, j);

            msleep(30);

            setInfBiasingParams(i, j);
            msleep(30);

            //! 4、重置相机开窗
            setIM01Waves(i, j);
            setIM01Window(i, j);
            setIM01RowFreq(i, j);

            //! 5、开机切到校正信号
            if(bStart){
                sendCamSignalMode(i, j, 1);
            }

            msleep(30);

            //! 6、发送背景波动
            sendBackGroundValue(i, j);
        }

        //! 相机触发方式
        sendcamtrigerType(struGsh.nAnalyzeIndex);
    }
}

/**
 * @brief GlobalFlow::发送升级参数
 * @param nAnalyzeIndex
 * @param cmd
 * @param nUnit
 */
void GlobalFlow::sendUpgradeInfo(int nAnalyzeIndex, int cmd, char type, int nUnit, char *data, int nlength)
{
    QByteArray ba;

    ba = packet_TypeA_NetData(CAMERA_CK, type, nUnit, cmd, data, nlength);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);

}

/**
 * @brief GlobalFlow::发送板载通讯方式
 * @param nAnalyzeIndex
 * @param cmd
 * @param nUnit
 */
void GlobalFlow::sendFpgaFacType(int nAnalyzeIndex, char type, int nUnit)
{
    char data;
    data = type;

    QByteArray ba;
    ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnit, 0x0041, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}


/**
 * @brief GlobalFlow::下发升级文件
 * @param nAnalyzeIndex
 * @param module_type
 * @param module_addr
 * @param arg1
 * @param arg2
 * @param data
 */
void GlobalFlow::sendFpgaUpgradeData(int nAnalyzeIndex, char module_type, char module_addr, int arg1, int arg2, QByteArray data)
{
    char sPacket[LEN_FRAME_FPGA+14];

    memset(sPacket, 0, sizeof(sPacket));

    //! 数据头 2位
    sPacket[0] = 0xF0;
    sPacket[1] = 0xA5;

    //! 预留位
    sPacket[3] = 0;

    //! 模块类型
    sPacket[4] = module_type;

    //! 模块地址
    if((MODULE_CAM == module_type || MODULE_SOC == module_type) &&
        (SENSOR_IM01 == struCnfg.analyzeInfo[0]->struCaParams->sensorType[module_addr]||
         SENSOR_IM04 == struCnfg.analyzeInfo[0]->struCaParams->sensorType[module_addr])
        ){
        module_addr = 32;
    }
    sPacket[5] = module_addr;

    //! 命令编码 2位
    sPacket[6] = 0x10;
    sPacket[7] = 0x06;

    //! 命令序号
    int nPacketIndex = arg1*256+arg2;
    sPacket[8] = (nPacketIndex*4)/256;
    sPacket[9] = (nPacketIndex*4)%256;


    //! 命令数据
    for(int i = 0; i < LEN_FRAME_FPGA; i++){
        sPacket[10+i] = data[nPacketIndex*LEN_FRAME_FPGA+i];
    }

    //! CRC校验
    uint nCrcItt = fpgaCrc(sPacket + 2, LEN_FRAME_FPGA+8);

    sPacket[LEN_FRAME_FPGA+10] = nCrcItt/256;
    sPacket[LEN_FRAME_FPGA+11] = nCrcItt%256;
    sPacket[LEN_FRAME_FPGA+12] = 0x00;
    sPacket[LEN_FRAME_FPGA+13] = 0x00;


    QByteArray ba;
//    ba.append(sPacket, LEN_FRAME_FPGA+36);
    ba.append(sPacket, LEN_FRAME_FPGA+14);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba, 1);
}

/**
 * @brief GlobalFlow::writeUpdateDataForSoc
 * @param nAnalyzeIndex
 * @param sModuleType
 * @param moduleAddr
 * @param nCmd
 * @param nPackageId
 * @param baFile
 * @param validDataLen
 * @param bIsMD5
 */
void GlobalFlow::writeUpdateDataForSoc(int nAnalyzeIndex, char sModuleType, char moduleAddr, int nCmd, int nPackageId, const QByteArray baFile, int validDataLen, bool bIsMD5)
{
    QByteArray sPacket;
    sPacket.resize(61440+34+14); //! 60k  34 14
    sPacket.fill(0);


    //! 数据头 2位
    sPacket[0] = 0xE7;
    sPacket[1] = 0x0C;

    //! 预留位
    sPacket[3] = 0;

    //! 模块类型
    sPacket[4] = sModuleType;

    //! 模块地址
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist&&gFlow.bIsIMSerialCam(moduleAddr)){
        sPacket[5] = 32;
    }else{
        sPacket[5] = moduleAddr;
    }

    //! 命令编码
    sPacket[6] = nCmd/256;
    sPacket[7] = nCmd%256;

    //! 升级包序号
    sPacket[8] = nPackageId/256;
    sPacket[9] = nPackageId%256;

    //! 有效数据长度
    sPacket[10] = validDataLen/256;
    sPacket[11] = validDataLen%256;

    //! 数据
    if(bIsMD5){
        sPacket.replace(12, validDataLen, baFile);
    }else{
        sPacket.replace(12, validDataLen, baFile.mid(nPackageId*(61440), validDataLen));
    }

    //! 包尾
    sPacket[61440+34+12] = 0xFF;
    sPacket[61440+34+12+1] = 0xFF;

    g_udpSocket[nAnalyzeIndex]->writeDatagram(sPacket, 1);
}

/**
 * @brief GlobalFlow::sendImgTo1808
 * @param nAnalyzeIndex
 * @param module_type
 * @param module_addr
 * @param arg1
 * @param arg2
 * @param data
 */
void GlobalFlow::sendImgTo1808(int nAnalyzeIndex, char module_type, char module_addr, int arg1, int arg2, QByteArray data)
{
    char sPacket[LEN_FRAME_FPGA+13];

    memset(sPacket, 0, sizeof(sPacket));

    //! 数据头 2位
    sPacket[0] = 0xF0;
    sPacket[1] = 0x0F;

    //! 预留位
//    sPacket[3] = 0;

    //! 模块类型
    sPacket[4] = module_type;

    //! 模块地址
    sPacket[5] = module_addr;

    //! 命令编码 2位
    sPacket[6] = 0x51;
    sPacket[7] = 0x00;

    //! 命令序号
    int nPacketIndex = arg1*256+arg2;
    sPacket[3] = (nPacketIndex)/256;
    sPacket[8] = (nPacketIndex)%256;


    //! 命令数据
    for(int i = 0; i < LEN_FRAME_FPGA; i++){
        sPacket[9+i] = data[nPacketIndex*LEN_FRAME_FPGA+i];
    }

    //! CRC校验
   // uint nCrcItt = fpgaCrc(sPacket + 2, LEN_FRAME_FPGA*3*2+8);

    //sPacket[LEN_FRAME_FPGA*3*2+10] = nCrcItt/256;
    //sPacket[LEN_FRAME_FPGA*3*2+11] = nCrcItt%256;
    sPacket[LEN_FRAME_FPGA+9] = 0;
    sPacket[LEN_FRAME_FPGA+10] = 0;

    sPacket[LEN_FRAME_FPGA+11] = 0xFF;
    sPacket[LEN_FRAME_FPGA+12] = 0xFF;


    QByteArray ba;
    ba.append(sPacket, LEN_FRAME_FPGA+13);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba,1);
}

/**
 * @brief GlobalFlow::1808采集行数
 * @param nAnalyzeIndex
 * @param nSampleLine
 */
void GlobalFlow::sendSampleLinePara(int nAnalyzeIndex, int nSampleLine)
{
    QByteArray ba;

    char data[2] = {0};
    data[0] = nSampleLine/256;
    data[1] = nSampleLine%256;

    for(int i = 0; i < MAX_CAM_NUM; i++){
        ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_SOC_SAMPLE, data, 2);

        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

}



/*!
 * \brief 槽: 初始化色选机联调参数
 */
void GlobalFlow::onInitSorterAdjparams(bool binit)
{
    Q_UNUSED(binit);
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        sendSorterIPparams();
        sendSorterlabparams(false);
    }
}

/*!
 * \brief 发送伺服电机相关命令
 * \param nCmd
 * \param array
 * \param nDatalen
 */
void GlobalFlow::sendMotorCtrl(int nCmd, QByteArray array, int nDatalen)
{
    if(nDatalen>LEN_PACKET_A||nDatalen<0)
        return;

    char data[LEN_PACKET_A];
    memcpy(data,array.data(),nDatalen);
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0 , nCmd, data, nDatalen);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);

}

/*!
 * \brief 重置标签名
 */
bool GlobalFlow::ResetStatuslabNamelist()
{
    bool bret = true;
    struGsh.totallabNamelst.clear();

    for(int i=0; i<struGsh.vlabNamemap.size(); i++){
        if(struGsh.vlabNamemap[i].isEmpty()){
            bret = false;
            break;
        }
        QList<QString> tmplst = struGsh.vlabNamemap[i].values();
        for(int j=0; j<tmplst.size();j++){
            if(struCnfg.nMachineType ==1){
                QString namestr = tmplst.at(j).split("/")[0];
                if(!struGsh.totallabNamelst.contains(namestr)){
                    struGsh.totallabNamelst<<namestr;
                    struGsh.totallabNamelst<<QString("%1%2").arg(namestr).arg("LOW");
                    struGsh.totallabNamelst<<QString("%1%2").arg(namestr).arg("MID");
                    struGsh.totallabNamelst<<QString("%1%2").arg(namestr).arg("HIG");
                }
            }else{
                struGsh.totallabNamelst<<tmplst.at(j);
            }
        }
    }

    if(!bret){
       struGsh.totallabNamelst.clear();
    }

    return bret;
}

/*!
 * \brief 设置或者获取清灰缸状态
 * \param ntype
 */
void GlobalFlow::Setorgetcleanjarstatus(int ntype)
{
    if(ntype==1||ntype==2){
        char data = ntype;
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_CLEANJAR_STATUS, &data, 1);

        g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba,1);
    }
}

/*!
 * \brief 电磁阀使能
 * \param nAnalyzeIndex
 */
void GlobalFlow::sendSNVEnstatus(int nAnalyzeIndex)
{
    char data = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_EN_SNV, &data, 1);

    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 电磁阀通道控制
 * \param array
 */
void GlobalFlow::snvchannelonoffctrl(quint8 data)
{
    QByteArray array;
    array.resize(2);
    array.fill(0);
    array[1] = data;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_SVBOARD, 0, CMD_SNVCHANNEL_ONOFF, array.data(), 2);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 发送低压阀参数
 */
void GlobalFlow::sendvalBoardparams()
{
    char data[11];
    auto *pParams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms;

    int nacctime = 80*pParams->nacctime;
    int nacctime2 = 1000*pParams->nacctime2;
    int nspantime = 80000*pParams->nspan;

    data[0] = (nacctime>>16)&0xFF;
    data[1] = (nacctime>>8)&0xFF;
    data[2] = nacctime & 0xff;
    data[3] = pParams->nfre;
    data[4] = pParams->D2type;
    data[5] = pParams->voltype;
    data[6] = (nacctime2>>8)&0xFF;
    data[7] = nacctime2 & 0xff;
    data[8] = (nspantime>>16)&0xFF;
    data[9] = (nspantime>>8)&0xFF;
    data[10] = nspantime & 0xff;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_VAL, 0, CMD_VALBOARD_PARAMS, data, 11);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 低压阀开关控制
 */
void GlobalFlow::sendvalBoardOnff()
{
    char data;
    data = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.bEn;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_VAL, 0, CMD_VALBOARD_ONOFF, &data, 1);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 低压阀开关控制
 */
void GlobalFlow::sendEjectorOnOff(int nAnalyzeIndex, bool bOnOff)
{
    char data;
    data = bOnOff ? 1 : 0;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_EJECTOR_ONOFF, &data, 1);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

/*!
 * \brief 采集行参数发送到控制板
 * \param nAnalyzeIndex
 */
void GlobalFlow::sendcapLine2cotrlBoard(int nAnalyzeIndex)
{
    char data[6] ;
    data[0] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[0]/256;
    data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[0]%256;
    data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[0]/256;
    data[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[0]%256;
    data[4] = struCnfg.analyzeInfo[nAnalyzeIndex]->nCacheLine;
    data[5] = struCnfg.analyzeInfo[nAnalyzeIndex]->nCacheType;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_CAPLINE_TOCTRL, data, 6);
    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

void GlobalFlow::sendcapLine2camBoard(int nAnalyzeIndex)
{
    char data[6] ;
    data[0] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[0]/256;
    data[1] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[0]%256;
    data[2] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[0]/256;
    data[3] = struCnfg.analyzeInfo[nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[0]%256;
    data[4] = struCnfg.analyzeInfo[nAnalyzeIndex]->nCacheLine;
    data[5] = struCnfg.analyzeInfo[nAnalyzeIndex]->nCacheType;

    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i, CMD_CAPLINE_TOCTRL, data, 6);
        g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
    }

}

void GlobalFlow::sendHigfreProtect(int nAnalyzeIndex)
{
    char data[2] = {0};
    int nCmd = CMD_HIGFRE_PROTECT;
    data[0] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nhfpTime/256;
    data[1] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nhfpTime%256;


    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, nCmd, data, 2);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
}

void GlobalFlow::sendBoardRevise(int nAnalyzeIndex, int nUnit)
{
    char data = 1;
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nUnit, CMD_CAM_REVISE, &data, 1);

    g_udpSocket[nAnalyzeIndex]->writeDatagram(ba);
}

 /*! 加密相关 */
void GlobalFlow::getEncryptInfo()
{
    g_encryInfo.getStat = 0x00;

    char nDataLength = 3;
    char data[nDataLength] = {0};

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_GET_ENCRYPTINFO, data, nDataLength);
    g_udpSocket[0]->writeDatagram(ba);
    msleep(100);
}

void GlobalFlow::WriteEncryptedStatus()
{
#if 0
    printf("-----WriteEncryptedStatus--g_encryInfo.nEna = %x----\n",g_encryInfo.nEna);
    printf("-----WriteEncryptedStatus--g_encryInfo.nLenth = %x----\n",g_encryInfo.nLenth);
    printf("-----WriteEncryptedStatus--g_encryInfo.nLimitCnt = %x----\n",g_encryInfo.nLimitCnt);
    fflush(0);
#endif
    char nDataLength = 3;
    char data[nDataLength] = {0};

    data[0] = g_encryInfo.nEna;
    data[1] = g_encryInfo.nLenth;
    data[2] = g_encryInfo.nLimitCnt;

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_SET_ENCRYP_STATUS, data, nDataLength);
    g_udpSocket[0]->writeDatagram(ba);
}

void GlobalFlow::WriteEncryptedDateLimit()
{
#if 0
    printf("-----WriteEncryptedDateLimit--limit: %s----\n",g_encryInfo.sCode.toLatin1().data());
    fflush(0);
#endif
    int addNum = 25-g_encryInfo.sCode.size();
    for(int i=0; i<addNum; i++){
        g_encryInfo.sCode.push_back("0");
    }

    char nDataLength = 25;
    char data[nDataLength] = {0};

    for(int j=0; j<25; j++)
    {
        uchar c;
        char2Hex(g_encryInfo.sCode.at(j).toLatin1(), c);
        data[j] = c;
    }

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_SET_ENCRYP_DATA1, data, nDataLength);
    g_udpSocket[0]->writeDatagram(ba);
}

void GlobalFlow::WriteEncryptedCurDate()
{
#if 0
    printf("-----WriteEncryptedCurDate--save time: %s----\n",g_encryInfo.savedTime.toLatin1().data());
    fflush(0);
#endif

    char nDataLength = 4;
    char data[nDataLength] = {0};
    bool ok;
    data[0] = g_encryInfo.savedTime.mid(0,2).toInt(&ok,10);
    data[1] = g_encryInfo.savedTime.mid(2,2).toInt(&ok,10);
    data[2] = g_encryInfo.savedTime.mid(4,2).toInt(&ok,10);
    data[3] = g_encryInfo.savedTime.mid(6,2).toInt(&ok,10);

    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_SET_ENCRYP_DATA2, data, nDataLength);
    g_udpSocket[0]->writeDatagram(ba);
}
