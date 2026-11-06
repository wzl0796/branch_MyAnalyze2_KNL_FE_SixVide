#include "core/globalflow/globalflow.h"


/**
 * @brief GlobalFlow::初始化全局共享参数
 */
void GlobalFlow::initParamsShare()
{
    //! 每月1号/16号删除日志
    QFile file(LOG_FILE);
    int day = QDateTime::currentDateTime().date().day();
    if (file.exists() && (day == 1 || day == 16)) {
        file.remove();
    }

    //----加密数据结构初始化
    g_encryInfo.getStat = 0x00;
    g_encryInfo.nEna = 0;
    g_encryInfo.nLenth = 0;
    g_encryInfo.sCode.clear();
    for(int j = 0; j < 25; j++){
        g_encryInfo.sCode.push_back(QString::number(0));
    }
    g_encryInfo.savedTime = "19700101";
    g_encryInfo.nLimitCnt = 0xFF;
    memset(g_encryInfo.codeTmp, 0x00, 25*sizeof(char));

    struGsh.bEnSimulate =false;
    struGsh.nUnit = 0;
    struGsh.nGetBak = false;
    struGsh.nFeederState = false;
    struGsh.nIsCapaturing = false;

    struGsh.nThreadValue = 50;
    struGsh.nRunCnt = 0;
    struGsh.nAuthority = AUTHORITY_PRIMARY;
    struGsh.nAnalyzeIndex = 0;

    struGsh.bCalibrate = false;
    struGsh.bStatics = false;
    struGsh.bSorterstalst.clear();
    struGsh.bAutosampleAlarm = false;

    struGsh.bCalibrateSoc = false;
    struGsh.aiLabelRatio.clear();
    struGsh.vSocInferReady.resize(MAX_CAM_NUM);

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nFreQuence[0]==0){
        struGsh.nlegthRatio = 0.11;
    }else{
       struGsh.nlegthRatio = 1000*struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMotorSpeed/struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nFreQuence[0];
    }
    struGsh.nwidRatio = 0.11;
    struGsh.nCheckTimes = 0;

    struGsh.nNetwortStat = NETWORK_DISCONNECT;
    struGsh.sIp = "";

    struGsh.bUpgrading = false;
    struGsh.nHalogenValue[0] = 1000;
    struGsh.nHalogenValue[1] = 1000;

    struGsh.nInfraTempure = -1;

    struGsh.bAISimulate = false;
    struGsh.bAISaveMat = false;

    memset(struGsh.nIsWiping, 0, sizeof(struGsh.nIsWiping));
    memset(struGsh.sRgbRow,   0, sizeof(struGsh.sRgbRow));
    memset(struGsh.sRowRed,   0, sizeof(struGsh.sRowRed));
    memset(struGsh.sRowGreen, 0, sizeof(struGsh.sRowGreen));
    memset(struGsh.sRowBlue,  0, sizeof(struGsh.sRowBlue));
    memset(struGsh.sRowNInf,  0, sizeof(struGsh.sRowNInf));
    memset(struGsh.sRowNInf1,  0, sizeof(struGsh.sRowNInf1));
    memset(struGsh.sRowNInf2,  0, sizeof(struGsh.sRowNInf2));
    struGsh.nWaveIndex = 0;

    getciku();

    for(int j=0; j< struCnfg.analyzeNum; j++) {
        analyzeShare* analyzeTmp = new analyzeShare;
        initAnalyzeShareInfo(analyzeTmp, getSpecifyAnalyzeGlobal(j));

        struGsh.analyzeInfo.push_back(analyzeTmp);
    }


    //! 初始化形选标签
    struGsh.vShapeLab.resize(MAX_SORTERCLASS_NUM);
    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        QStringList lablst;
        if(myLan.nLang == LANG_CHS){
            lablst<<myLan.big<<myLan.small;
        }else{
            lablst<<"Big"<<"Small";
        }
        struGsh.vShapeLab[i] = lablst;
    }

    //! 初始化辅配相机挂载的主配相机地址
    struGsh.nAssisttomainCamId = 0;
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[i] == 2){
            struGsh.nAssisttomainCamId = i;
            break;
        }
    }

    struGsh.vCrossBoxesOldSingleNum.resize(MAX_CAM_NUM);
    struGsh.vCrossBoxesOld.resize(MAX_CAM_NUM);
    struGsh.nMetaNum =0;
    struGsh.batchname = QString("%1").arg(myLan.batch);
    struGsh.duty_shift = QString("%1").arg(myLan.detector);
    struGsh.detect_yield = 2.0;
    struGsh.nSorterAdjIdx = 0;

    //! Soc相关参数
    struGsh.nSoccaptype = 0;
    struGsh.nSoccapthd = 20;
    struGsh.nSocImgHei = 512;
    struGsh.nSocImgWid = 1024;

    struGsh.vlabNamemap.resize(gFlow.getMachineCamNum());
    struGsh.vlabEngNamemap.resize(MAX_CAM_NUM);


    memset(struGsh.bEnCaplabs,1,sizeof(struGsh.bEnCaplabs));


    //! 初始化标签阈值(传统算法的相机无标签，特殊处理)
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabthdmap.isEmpty()){
        gFlow.ResetLabthdhod();
    }

    for(int i=0; i<MAX_CAM_NUM; i++){//! 初始化模型
        struGsh.bSocModelUnitResult[i] = false;
        struGsh.sModelUnitInfo[i].state =-1;
        struGsh.nAIModel[i] = 0;
        for(int j=0;j<20;j++){
           strcpy(struGsh.sModelUnitInfo[i].model[j].cnf, QString("NULL").toLatin1().data());
        }
    }

    int nNum = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaticGroup;
    struGsh.vSocInfer.resize(MAX_CAM_NUM);
    struGsh.vCamLabelMapping.resize(MAX_CAM_NUM);

    for(int i=0; i<nNum; i++){
         int n =struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist[i].size();
         if(n>1){
             for(int j=0; j<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist[i].size();j++){
                int idx = QString(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist[i].at(j)).toInt();
                struGsh.socLineinfomap[idx] = 0;
             }
         }
     }
    struGsh.vMegreLablst.resize(MAX_CAM_NUM);

    struGsh.vlabStamap.resize(MAX_CAM_NUM);
    struGsh.vsMatinfo.resize(MAX_CAM_NUM);
    struGsh.vlabTotalmap.resize(MAX_CAM_NUM);
    struGsh.vmatTotalmap.resize(MAX_CAM_NUM);
    struGsh.vlabMutex = {0, 0, 0, 0, 1, 2, 0, 3, 2, 3, 4, 4, 1, 0, 0, 2, 2, 2};
    if(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabelidxmap[0].isEmpty()){
        foreach(int nidx, struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabelidxmap[0]){
            struGsh.PriorityForKFD.push_back(nidx);
        }
    }else{
        for(int i=0; i<MAX_LABELS;i++){
            struGsh.PriorityForKFD.push_back(i);
        }
    }
}

/**
 * @brief GlobalFlow::获取词库信息
 */
void GlobalFlow::getciku()
{
    QFile pinyin_file(g_sCikuDir);
    if (!pinyin_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    //! 设置正则表达式的参数,Qt::CaseInsensitive,大小写敏感，获取a-z
    QRegExp regExp;
    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("([a-z]+)"));

    int ret = 0;

    QTextStream toText(&pinyin_file);
    while(!toText.atEnd())
    {
        QString str = toText.readLine().trimmed();
        ret = regExp.indexIn(QString(str.data()),0,QRegExp::CaretAtZero);
        struGsh.pinyin_map.insert(regExp.cap(1),str.left(ret));
    }
}


/**
 * @brief GlobalFlow::初始化分析仪全局不保存参数
 * @param anaInfo
 * @param anaGlobal
 */
void GlobalFlow::initAnalyzeShareInfo(analyzeShare *anaInfo, analyzeGlobal *anaGlobal)
{

    for(int i = 0; i < MAX_CAM_NUM; ++i) {
        strcpy(anaInfo->sCamVersion[i], "NULL");
        strcpy(anaInfo->sSocVersion[i], "NULL");
    }

    strcpy(anaInfo->sCtrlVersion, "NULL");
    strcpy(anaInfo->sPulseVersion, "NULL");
    strcpy(anaInfo->sHalogenVersion, "NULL");
    strcpy(anaInfo->sSVBoardVersion, "NULL");
    strcpy(anaInfo->sValveBoardVersion, "NULL");

    anaInfo->bFeeder = false;

    anaInfo->bWipe = false;
    anaInfo->bCheckVerStartUp = true;

    anaInfo->nUpdateType = 0;
    anaInfo->nUpgradeStatusInfo = false;
    anaInfo->bAIUpdStatus = 0;

}
