#include "common/mymessagebox.h"
#include "core/globalflow/globalflow.h"
#include "core/io/udp/myudp.h"
#include "core/mythread/myprocess.h"
#include <QDebug>

/**
 * @brief GlobalFlow::getGlobal
 */
void GlobalFlow::getGlobal() { initGlobalParams(); }

/**
 * @brief GlobalFlow::初始化全局参数
 */
void GlobalFlow::initGlobalParams() { getGlobalParams(CNF_GLOBAL); }

/**
 * @brief GlobalFlow::获取网络配置参数
 */
void GlobalFlow::getNetSetting() {
  QSettings settingCamName(CNF_NETNAME, QSettings::IniFormat);
  strcpy(struCnfg.sCamName,
         settingCamName.value("CamName", "eth1").toString().toLatin1().data());
  strcpy(struCnfg.sWifiName,
         settingCamName.value("WifiName", "eth0").toString().toLatin1().data());

  QSettings netConfig(CNF_NETWORK, QSettings::IniFormat);
  struCnfg.sLocalIP = netConfig.value("IP", "192.168.1.20").toString();
  struCnfg.sNetMask = netConfig.value("Mask", "255.255.255.0").toString();
  struCnfg.sGateWay = netConfig.value("Gateway", "192.168.1.1").toString();
  struCnfg.sServerAddr = netConfig.value("ServerAddr").toString();
  struCnfg.sPort = netConfig.value("Port").toString();
}

/**
 * @brief GlobalFlow::从配置文件中获取全局参数
 * @param fileName
 * @return
 */
bool GlobalFlow::getGlobalParams(const QString fileName) {
  QSettings setting(fileName, QSettings::IniFormat);

  struCnfg.bEnableDHCP = setting.value("EnableDHCP", true).toBool();

  struCnfg.analyzeNum = setting.value(QString("analyzeNum"), 1).toInt();

  struCnfg.nSchemTotal = setting.value(QString("SchemTotal"), 1).toInt();

  struCnfg.nCntScheme = setting.value(QString("CntScheme"), 0).toInt();

  struCnfg.nMachineType =
      setting.value("machinetype", 0).toInt(); //! 默认机型 瓶片

  struCnfg.gCompanyName =
      setting.value("CompanyName", "XXXXXX有限公司").toString();

  QString strdev;
  if (struCnfg.nMachineType == nMachine_Bottle) {
    strdev = QString("分析仪(KB1)");
  } else {
    strdev = QString("分析仪(KA1)");
  }

  struCnfg.gDeviceNmae = setting.value("DeviceName", strdev).toString();

  struCnfg.nEnableDelayCodeInfo = setting.value("DelayInfo", 0).toInt();

  struCnfg.master_device_code =
      setting.value("master_device_code", "6000000000000").toString();

  for (int i = 0; i < struCnfg.nSchemTotal; i++) {
    QString curDate = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    schemeInfo scheme;
    scheme.code =
        setting.value(QString("SchmCode_%1").arg(i), curDate).toString();
    scheme.name =
        setting.value(QString("SchmName_%1").arg(i), getInitSchemeName(i))
            .toString();
    struCnfg.schemeList.push_back(scheme);
  }

  //! 获取分析仪参数
  for (int i = 0; i < struCnfg.analyzeNum; i++) {
    analyzeGlobal *analyzeData = new analyzeGlobal();
    QString analyzeFile = QString("%1_%2").arg(CNF_ANALYZE).arg(i);
    getGlobalAnalyzeParams(analyzeData, analyzeFile);

    struCnfg.analyzeInfo.push_back(analyzeData);
  }

  for (int i = 0; i < 4; i++) {
    g_encryInfo.codeSize[i] =
        setting.value(QString("codeSize_%1").arg(i), 5).toInt();
  }

  if (setting.status() != QSettings::NoError) {
    return false;
  }

  return true;
}

/**
 * @brief GlobalFlow::初始化分析仪参数
 * @param globalParams
 * @param fileName
 * @return
 */
bool GlobalFlow::getGlobalAnalyzeParams(analyzeGlobal *globalParams,
                                        const QString fileName) {
  QSettings setting(fileName, QSettings::IniFormat);

  QString sAnalyzeName =
      setting
          .value(QString("AnalyzeName"),
                 QString("%1%2").arg(myLan.analyze).arg(struCnfg.analyzeNum))
          .toString();
  qStringTochar(sAnalyzeName, globalParams->sAnalyzeName, 50);

  QString locapIp = setting.value(QString("sIp"), "192.168.1.x").toString();
  qStringTochar(locapIp, globalParams->sIp, 25);

  for (int i = 0; i < MAX_VIBRATOR; i++) {
    globalParams->nFeeder[i] =
        setting.value(QString("Feeder_%1").arg(i), 10).toInt();
  }

  globalParams->nMotorSpeed = setting.value(QString("speed"), 1.2).toFloat();

  globalParams->nHalogenThd = setting.value(QString("halogenthd"), 240).toInt();

  globalParams->nWipeInterval =
      setting.value(QString("WipeInterval"), 15).toInt();

  globalParams->nWipeDelay = setting.value(QString("WipeDelay"), 3000).toInt();

  globalParams->nknifetime = setting.value(QString("nknifetime"), 3).toInt();

  globalParams->nknifechanel =
      setting.value(QString("nknifechanel"), 7).toInt();

  globalParams->nBlowDelay = setting.value(QString("blowDelay"), 20).toInt();

  globalParams->nBlowTime = setting.value(QString("blowtime"), 20).toInt();

  globalParams->nTruncTime = setting.value(QString("nTruncTime"), 10).toInt();

  globalParams->nDegTime = setting.value(QString("nDegTime"), 5).toInt();

  globalParams->nhfpTime = setting.value(QString("nhfpTime"), 50).toInt();

  globalParams->nCacheLine = setting.value(QString("nCacheLine"), 5).toInt();

  globalParams->nCacheType = setting.value(QString("nCacheType"), 0).toInt();

  globalParams->nRectResize =
      setting.value(QString("nRectResize"), 100).toInt();

  globalParams->bHasAssist =
      setting.value(QString("bHasAssist"), true).toBool();

  globalParams->bEnHalogen =
      setting.value(QString("bEnHalogen"), true).toBool();

  globalParams->bEnBackdgrod =
      setting.value(QString("bEnBackdgrod"), false).toBool();

  globalParams->bEnAirAlarm = setting.value(QString("bEnAir"), false).toBool();

  globalParams->bEnSNV = setting.value(QString("bEnSNV"), false).toBool();

  globalParams->bEnVal = setting.value(QString("bEneject"), false).toBool();

  globalParams->bEjectorOnoff =
      setting.value(QString("bEjectorOnoff"), true).toBool();

  globalParams->bEnmetalDetect =
      setting.value(QString("benmetaldect"), false).toBool();

  globalParams->bEnweb = setting.value(QString("bEnweb"), false).toBool();

  globalParams->bEnlightSync =
      setting.value(QString("bEnlightSync"), false).toBool();

  globalParams->bEnAdjSorterTips =
      setting.value(QString("bEnAdjSorterTips"), false).toBool();

  globalParams->bEnCamTempureAlarm =
      setting.value(QString("bEnCamTempureAlarm"), true).toBool();

  globalParams->nInfratempthd =
      setting.value(QString("nInfratempthd"), 50).toInt();

  globalParams->bEninfratempCtrl =
      setting.value(QString("bEninfratempCtrl"), false).toBool();

  globalParams->bMergerColorMaterial =
      setting.value(QString("bMergerColorMaterial"), false).toBool();

  globalParams->nCamTrigerType =
      setting.value(QString("ncamTriger"), 0).toInt();

  globalParams->nRuntime = setting.value(QString("nRuntime"), 0).toULongLong();

  globalParams->nFeedertime =
      setting.value(QString("nFeedertime"), 0).toULongLong();

  bool bEnLight = struCnfg.nMachineType == nMachine_Crop ? true : false;
  globalParams->bEnPulseLight =
      setting.value(QString("bEnPulseLight"), bEnLight).toBool();
  globalParams->nMajorCamNum = setting.value(QString("nmajorcam"), 3).toInt();
  globalParams->bEnshape = setting.value(QString("bEnshape"), false).toBool();
  globalParams->bEncleanjar = setting.value(QString("bEnjar"), false).toBool();
  globalParams->bEnPurityStats =
      setting.value(QString("bEnPuritysta"), false).toBool();
  globalParams->bEnAutocalIMSensor =
      setting.value(QString("bEnautocalIM"), false).toBool();
  globalParams->bEnMultivibrator =
      setting.value(QString("bEnMultivibrator"), false).toBool();

  globalParams->svalParms.bEn =
      setting.value(QString("bEnval"), false).toBool();
  globalParams->svalParms.nacctime =
      setting.value(QString("nvalacctime"), 1000).toInt();
  globalParams->svalParms.nacctime2 =
      setting.value(QString("nvalacctime2"), 0.5).toFloat();
  globalParams->svalParms.D2type = setting.value(QString("nvalD2"), 2).toInt();
  globalParams->svalParms.voltype =
      setting.value(QString("nvalvol"), 1).toInt();
  globalParams->svalParms.nfre = setting.value(QString("nvalfre"), 2).toInt();
  globalParams->svalParms.nspan = setting.value(QString("nvalspan"), 2).toInt();

  //! 相机参数
  for (int i = 0; i < MAX_CAM_NUM; ++i) {
    QString camName;
    int nfre = 11000;
    int nSensortype = SENSOR_VL05;
    int nwork = 0;
    int ndefalutvalue = 30;
    int startpiex = 115;
    int endpiexl = 1934;
    int Rtar = 150, Gtar = 150, Btar = 150;

    switch (i) {
    case 0:
      nSensortype = SENSOR_VL05;
      nfre = 11000;
      nwork = 2;
      camName = myLan.up_camer;
      startpiex = 285;
      endpiexl = 1890;
      Rtar = 180;
      Gtar = 180;
      Btar = 180;
      break;
    case 1:
      ndefalutvalue = 110;
      camName = myLan.ultra_camer;
      nSensortype = SENSOR_VL08;
      nfre = 11000;
      nwork = 3;
      startpiex = 150;
      endpiexl = 1700;
      Rtar = 0;
      Gtar = 160;
      Btar = 180;
      break;
    case 2:
      camName = myLan.down_camer;
      nSensortype = SENSOR_VL05;
      nfre = 11000;
      nwork = 1;
      startpiex = 1025;
      endpiexl = 1934;
      Rtar = 130;
      Gtar = 150;
      Btar = 130;
      break;
    case 3:
      camName = myLan.hyperspec_camer;
      nSensortype = SENSOR_IM04;
      nfre = 24000;
      nwork = 0;
      startpiex = 100;
      endpiexl = 500;
      break;
    default:
      break;
    }

    globalParams->CamName[i] =
        setting.value(QString("camname_%1").arg(i), camName).toString();
    globalParams->nCamoffPiexl[i] =
        setting.value(QString("camoffpiex_%1").arg(i), 110).toInt();

    globalParams->struCaParams->nFreQuence[i] =
        setting.value(QString("FreQuence_%1").arg(i + 1), nfre).toInt();
    globalParams->struCaParams->sensorType[i] =
        setting.value(QString("sensorType_%1").arg(i + 1), nSensortype).toInt();
    globalParams->struCaParams->exposeValue[i] =
        setting.value(QString("exposeValue_%1").arg(i + 1), 18000).toInt();
    globalParams->struCaParams->NoneexposeValue[i] =
        setting.value(QString("NoneexposeValue_%1").arg(i + 1), 3000).toInt();
    globalParams->struCaParams->nWorkMode[i] =
        setting.value(QString("workmode_%1").arg(i + 1), nwork).toInt();
    globalParams->struCaParams->nWaveSeg[i] =
        setting.value(QString("waveseg_%1").arg(i + 1), 1).toInt();
    globalParams->struCaParams->nPixelBegin[i] =
        setting.value(QString("PixelBegin_%1").arg(i + 1), startpiex).toInt();
    globalParams->struCaParams->nPixelEnd[i] =
        setting.value(QString("PixelEnd_%1").arg(i + 1), endpiexl).toInt();
    globalParams->struCaParams->nEjectorBegin[i] =
        setting.value(QString("nEjectorBegin_%1").arg(i + 1), 1).toInt();
    globalParams->struCaParams->nEjectorEnd[i] =
        setting.value(QString("nEjectorEnd_%1").arg(i + 1), EJ_NUM).toInt();
    globalParams->struCaParams->nEjectorBeginPiexl[i] =
        setting.value(QString("nEjectorBeginPiexl_%1").arg(i + 1), 1024)
            .toInt();
    globalParams->struCaParams->nEjectorEndPiexl[i] =
        setting.value(QString("nEjectorEndPiexl_%1").arg(i + 1), 2047).toInt();

    int nPxTotal = endpiexl - startpiex + 1;
    int nEjTotal = EJ_NUM;
    float fAvr = (float)nPxTotal / nEjTotal;
    globalParams->struCaParams->npEjBegin[i][0] =
        setting
            .value(QString("nejcBegin_1024_%1_%2").arg(i + 1).arg(0), startpiex)
            .toInt();
    globalParams->struCaParams->npEjEnd[i][0] =
        setting
            .value(QString("nejcEnd_1024_%1_%2").arg(i + 1).arg(0),
                   startpiex + fAvr - 0.5)
            .toInt();
    for (int k = 1; k < EJ_NUM; k++) {
      int index = k;
      globalParams->struCaParams->npEjBegin[i][k] =
          setting
              .value(QString("nejcBegin_1024_%1_%2").arg(i + 1).arg(k),
                     globalParams->struCaParams->npEjEnd[i][index - 1] + 1)
              .toInt();
      globalParams->struCaParams->npEjEnd[i][k] =
          setting
              .value(QString("nejcEnd_1024_%1_%2").arg(i + 1).arg(k),
                     startpiex + (k + 1) * fAvr - 0.5)
              .toInt();
    }
    globalParams->struCaParams->npEjEnd[i][EJ_NUM - 1] =
        setting
            .value(QString("nejcEnd_1024_%1_%2").arg(i + 1).arg(EJ_NUM - 1),
                   endpiexl)
            .toInt();

    for (int j = 0; j < 4; j++) {
      globalParams->struCaParams->infraBias[i][j] =
          setting.value(QString("infraBias_%1_%2").arg(i + 1).arg(j + 1), 7)
              .toInt();
    }

    globalParams->struCaParams->nAdjustTarget[i][COLOR_TYPE_R] =
        setting.value(QString("AdjustTarget_%1_1").arg(i + 1), Rtar).toInt();
    globalParams->struCaParams->nAdjustTarget[i][COLOR_TYPE_G] =
        setting.value(QString("AdjustTarget_%1_2").arg(i + 1), Gtar).toInt();
    globalParams->struCaParams->nAdjustTarget[i][COLOR_TYPE_B] =
        setting.value(QString("AdjustTarget_%1_3").arg(i + 1), Btar).toInt();
    globalParams->struCaParams->nAdjustTarget[i][COLOR_TYPE_N] =
        setting.value(QString("AdjustTarget_%1_4").arg(i + 1), 0).toInt();

    globalParams->struCaParams->IM01_back_thread[i] =
        setting.value(QString("IM01_back_thread_%1").arg(i + 1), 50).toInt();
    globalParams->struCaParams->IM01_back_limit[i] =
        setting.value(QString("IM01_back_limit_%1").arg(i + 1), 50).toInt();
  }

  //! 背景波动范围 条件 阈值
  for (int i = 0; i < MAX_CAM_NUM; ++i) {
    globalParams->struCaParams->nThreadValue[i] =
        setting.value(QString("ThreadValue_%1").arg(i + 1), 50).toInt();
    globalParams->struCaParams->nBackgdIdx[i] =
        setting.value(QString("nBckIdx_%1").arg(i), 4).toInt();
    globalParams->struCaParams->nBackgdValue[i] =
        setting.value(QString("nBckValue_%1").arg(i), 70).toInt();
    globalParams->struCaParams->nBackgdRatio[i] =
        setting.value(QString("nBckRatio_%1").arg(i), 40).toInt();
  }

  for (int i = 0; i < MAX_CAM_NUM; ++i) {
    globalParams->nPixel[i] =
        (PIXEL_SIZE)(setting
                         .value(QString("Pixel_%1").arg(i + 1), (int)PIXEL_2048)
                         .toInt());
  }

  //! 分析仪的方案参数
  for (int j = 0; j < struCnfg.nSchemTotal; j++) {
    QString curDate =
        fileName + QDateTime::currentDateTime().toString("_yyyyMMddhhmmss");
    globalParams->anlyze_schemeList[j].code =
        setting.value(QString("SchmCode_%1").arg(j), curDate).toString();
  }

  QString profile_name =
      globalParams->anlyze_schemeList[struCnfg.nCntScheme].code;
  getAnalyzeProfileParams(&globalParams->analyzeProfile, profile_name);

  if (setting.status() != QSettings::NoError) {
    return false;
  }
  return true;
}

/**
 * @brief GlobalFlow::saveGlobalParams
 * @param fileName
 * @return
 */
bool GlobalFlow::saveGlobalParams(const QString fileName) {
  QSettings setting(fileName, QSettings::IniFormat);

  setting.setValue("EnableDHCP", struCnfg.bEnableDHCP);
  setting.setValue(QString("analyzeNum"), struCnfg.analyzeNum);
  setting.setValue(QString("SchemTotal"), struCnfg.nSchemTotal);
  setting.setValue(QString("CntScheme"), struCnfg.nCntScheme);
  setting.setValue("CompanyName", struCnfg.gCompanyName);
  setting.setValue("DeviceName", struCnfg.gDeviceNmae);
  setting.setValue("DelayInfo", struCnfg.nEnableDelayCodeInfo);

  //! 保存分析仪的参数
  for (int i = 0; i < struCnfg.analyzeNum; i++) {
    saveSpecifyAnalyze(i);
  }

  for (int i = 0; i < struCnfg.nSchemTotal; i++) {
    schemeInfo scheme = struCnfg.schemeList.at(i);
    setting.setValue(QString("SchmCode_%1").arg(i), scheme.code);
    setting.setValue(QString("SchmName_%1").arg(i), scheme.name);
  }

  setting.setValue("master_device_code", struCnfg.master_device_code);
  setting.setValue("machinetype", struCnfg.nMachineType);

  for (int i = 0; i < 4; i++) {
    setting.setValue(QString("codeSize_%1").arg(i), g_encryInfo.codeSize[i]);
  }

  saveNetSetting();

  // saveSpecifyAnalyze(struGsh.nAnalyzeIndex);

  if (setting.status() == QSettings::NoError) {
    return true;
  }

  return false;
}

/**
 * @brief GlobalFlow::存储网络配置参数
 */
void GlobalFlow::saveNetSetting() {
  QSettings settingCamName(CNF_NETNAME, QSettings::IniFormat);
  settingCamName.setValue("CamName", struCnfg.sCamName);
  settingCamName.setValue("WifiName", struCnfg.sWifiName);

  /* config setting file */
  QSettings netConfig(CNF_NETWORK, QSettings::IniFormat);
  netConfig.setValue("IP", struCnfg.sLocalIP);
  netConfig.setValue("Mask", struCnfg.sNetMask);
  netConfig.setValue("Gateway", struCnfg.sGateWay);
  netConfig.setValue("ServerAddr", struCnfg.sServerAddr);
  netConfig.setValue("Port", struCnfg.sPort);
  netConfig.sync();
}

/**
 * @brief GlobalFlow::保存分析仪数据至方案文件中
 * @param analyzeIndex
 */
bool GlobalFlow::saveSpecifyAnalyze(int analyzeIndex) {
  analyzeGlobal *analyzeData = getSpecifyAnalyzeGlobal(analyzeIndex);

  QString fileName = QString("%1_%2").arg(CNF_ANALYZE).arg(analyzeIndex);

  QSettings setting(fileName, QSettings::IniFormat);

  setting.setValue(QString("AnalyzeName"), QString(analyzeData->sAnalyzeName));
  setting.setValue(QString("sIp"), QString(analyzeData->sIp));

  for (int i = 0; i < MAX_VIBRATOR; i++) {
    setting.setValue(QString("Feeder_%1").arg(i), analyzeData->nFeeder[i]);
  }
  setting.setValue(QString("speed"), analyzeData->nMotorSpeed);
  setting.setValue(QString("halogenthd"), analyzeData->nHalogenThd);
  setting.setValue(QString("WipeInterval"), analyzeData->nWipeInterval);
  setting.setValue(QString("WipeDelay"), analyzeData->nWipeDelay);
  setting.setValue(QString("nknifetime"), analyzeData->nknifetime);
  setting.setValue(QString("nknifechanel"), analyzeData->nknifechanel);
  setting.setValue(QString("blowDelay"), analyzeData->nBlowDelay);
  setting.setValue(QString("nBlowTime"), analyzeData->nBlowTime);
  setting.setValue(QString("nTruncTime"), analyzeData->nTruncTime);
  setting.setValue(QString("nDegTime"), analyzeData->nDegTime);
  setting.setValue(QString("nhfpTime"), analyzeData->nhfpTime);
  setting.setValue(QString("nCacheLine"), analyzeData->nCacheLine);
  setting.setValue(QString("nCacheType"), analyzeData->nCacheType);
  setting.setValue(QString("nRectResize"), analyzeData->nRectResize);
  setting.setValue(QString("bHasAssist"), analyzeData->bHasAssist);
  setting.setValue(QString("bEnHalogen"), analyzeData->bEnHalogen);
  setting.setValue(QString("bEnBackdgrod"), analyzeData->bEnBackdgrod);
  setting.setValue(QString("bEnAir"), analyzeData->bEnAirAlarm);
  setting.setValue(QString("bEnSNV"), analyzeData->bEnSNV);
  setting.setValue(QString("bEneject"), analyzeData->bEnVal);
  setting.setValue(QString("bEjectorOnoff"), analyzeData->bEjectorOnoff);
  setting.setValue(QString("benmetaldect"), analyzeData->bEnmetalDetect);
  setting.setValue(QString("bEnweb"), analyzeData->bEnweb);
  setting.setValue(QString("bEnlightSync"), analyzeData->bEnlightSync);
  setting.setValue(QString("bEnAdjSorterTips"), analyzeData->bEnAdjSorterTips);
  setting.setValue(QString("bEnCamTempureAlarm"),
                   analyzeData->bEnCamTempureAlarm);
  setting.setValue(QString("nInfratempthd"), analyzeData->nInfratempthd);
  setting.setValue(QString("bEninfratempCtrl"), analyzeData->bEninfratempCtrl);
  setting.setValue(QString("bMergerColorMaterial"),
                   analyzeData->bMergerColorMaterial);
  setting.setValue(QString("ncamTriger"), analyzeData->nCamTrigerType);
  setting.setValue(QString("bEnPulseLight"), analyzeData->bEnPulseLight);
  setting.setValue(QString("nmajorcam"), analyzeData->nMajorCamNum);
  setting.setValue(QString("bEnshape"), analyzeData->bEnshape);
  setting.setValue(QString("bEnjar"), analyzeData->bEncleanjar);
  setting.setValue(QString("bEnPuritysta"), analyzeData->bEnPurityStats);
  setting.setValue(QString("nRuntime"), analyzeData->nRuntime);
  setting.setValue(QString("nFeedertime"), analyzeData->nFeedertime);
  setting.setValue(QString("bEnval"), analyzeData->svalParms.bEn);
  setting.setValue(QString("nvalacctime"), analyzeData->svalParms.nacctime);
  setting.setValue(QString("nvalacctime2"), analyzeData->svalParms.nacctime2);
  setting.setValue(QString("nvalD2"), analyzeData->svalParms.D2type);
  setting.setValue(QString("nvalvol"), analyzeData->svalParms.voltype);
  setting.setValue(QString("nvalfre"), analyzeData->svalParms.nfre);
  setting.setValue(QString("nvalspan"), analyzeData->svalParms.nspan);
  setting.setValue(QString("bEnautocalIM"), analyzeData->bEnAutocalIMSensor);
  setting.setValue(QString("bEnMultivibrator"), analyzeData->bEnMultivibrator);

  for (int i = 0; i < MAX_CAM_NUM; i++) {
    setting.setValue(QString("camname_%1").arg(i), analyzeData->CamName[i]);
    setting.setValue(QString("camoffpiex_%1").arg(i),
                     analyzeData->nCamoffPiexl[i]);
  }

  //! 相机参数
  for (int i = 0; i < MAX_CAM_NUM; ++i) {
    setting.setValue(QString("FreQuence_%1").arg(i + 1),
                     analyzeData->struCaParams->nFreQuence[i]);
    setting.setValue(QString("sensorType_%1").arg(i + 1),
                     analyzeData->struCaParams->sensorType[i]);
    setting.setValue(QString("exposeValue_%1").arg(i + 1),
                     analyzeData->struCaParams->exposeValue[i]);
    setting.setValue(QString("NoneexposeValue_%1").arg(i + 1),
                     analyzeData->struCaParams->NoneexposeValue[i]);
    setting.setValue(QString("workmode_%1").arg(i + 1),
                     analyzeData->struCaParams->nWorkMode[i]);
    setting.setValue(QString("waveseg_%1").arg(i + 1),
                     analyzeData->struCaParams->nWaveSeg[i]);

    setting.setValue(QString("nEjectorBegin_%1").arg(i + 1),
                     analyzeData->struCaParams->nEjectorBegin[i]);
    setting.setValue(QString("nEjectorEnd_%1").arg(i + 1),
                     analyzeData->struCaParams->nEjectorEnd[i]);

    setting.setValue(QString("nEjectorBeginPiexl_%1").arg(i + 1),
                     analyzeData->struCaParams->nEjectorBeginPiexl[i]);
    setting.setValue(QString("nEjectorEndPiexl_%1").arg(i + 1),
                     analyzeData->struCaParams->nEjectorEndPiexl[i]);

    setting.setValue(QString("PixelBegin_%1").arg(i + 1),
                     analyzeData->struCaParams->nPixelBegin[i]);
    setting.setValue(QString("PixelEnd_%1").arg(i + 1),
                     analyzeData->struCaParams->nPixelEnd[i]);

    for (int k = 0; k < EJ_NUM; k++) {
      setting.setValue(QString("nejcBegin_1024_%1_%2").arg(i + 1).arg(k),
                       analyzeData->struCaParams->npEjBegin[i][k]);
      setting.setValue(QString("nejcEnd_1024_%1_%2").arg(i + 1).arg(k),
                       analyzeData->struCaParams->npEjEnd[i][k]);
    }

    for (int j = 0; j < 4; j++)
      setting.setValue(QString("infraBias_%1_%2").arg(i + 1).arg(j + 1),
                       analyzeData->struCaParams->infraBias[i][j]);

    setting.setValue(QString("AdjustTarget_%1_1").arg(i + 1),
                     analyzeData->struCaParams->nAdjustTarget[i][COLOR_TYPE_R]);
    setting.setValue(QString("AdjustTarget_%1_2").arg(i + 1),
                     analyzeData->struCaParams->nAdjustTarget[i][COLOR_TYPE_G]);
    setting.setValue(QString("AdjustTarget_%1_3").arg(i + 1),
                     analyzeData->struCaParams->nAdjustTarget[i][COLOR_TYPE_B]);
    setting.setValue(QString("AdjustTarget_%1_4").arg(i + 1),
                     analyzeData->struCaParams->nAdjustTarget[i][COLOR_TYPE_N]);

    setting.setValue(QString("IM01_back_thread_%1").arg(i + 1),
                     analyzeData->struCaParams->IM01_back_thread[i]);
    setting.setValue(QString("IM01_back_limit_%1").arg(i + 1),
                     analyzeData->struCaParams->IM01_back_limit[i]);
  }

  for (int i = 0; i < MAX_CAM_NUM; ++i) {
    setting.setValue(QString("ThreadValue_%1").arg(i + 1),
                     analyzeData->struCaParams->nThreadValue[i]);
    setting.setValue(QString("nBckIdx_%1").arg(i),
                     analyzeData->struCaParams->nBackgdIdx[i]);
    setting.setValue(QString("nBckValue_%1").arg(i),
                     analyzeData->struCaParams->nBackgdValue[i]);
    setting.setValue(QString("nBckRatio_%1").arg(i),
                     analyzeData->struCaParams->nBackgdRatio[i]);
  }

  for (int j = 0; j < struCnfg.nSchemTotal; j++) {
    setting.setValue(QString("SchmCode_%1").arg(j),
                     analyzeData->anlyze_schemeList[j].code);
  }

  saveAnalyzeProfileParams(
      &analyzeData->analyzeProfile,
      QString(analyzeData->anlyze_schemeList[struCnfg.nCntScheme].code));

  setting.sync();

  if (setting.status() == QSettings::NoError) {
    return true;
  }
  return false;
}

/**
 * @brief GlobalFlow::增加每个分析仪下面的线程信息
 * @param analyzeIndex
 */
void GlobalFlow::addAnalyzeThreadInfo(int analyzeIndex) {
  QThread *udpThread = new QThread();
  myUdp *udpSocket = new myUdp(analyzeIndex); //! UDP通信类
  udpSocket->setAnalyzeIndex(analyzeIndex);
  udpSocket->moveToThread(udpThread);
  udpThread->start();
  udpThread->setPriority(QThread::HighPriority);

  g_udpthread.push_back(udpThread);
  g_udpSocket.push_back(udpSocket);

  MyProcess *process = new MyProcess(); //! 自动取样线程
  process->setAnalyzeIndex(analyzeIndex);
  g_Process.push_back(process);

  connect(this, SIGNAL(initSocketSig()), udpSocket,
          SLOT(sltInitSocket()) /*, Qt::DirectConnection*/);
  connect(udpSocket, SIGNAL(sigImageDataReady(QByteArray, int, int)), this,
          SIGNAL(sigGetImageData(QByteArray, int, int))); //! 智能拍照
  connect(udpSocket, SIGNAL(sigemitdata(QByteArray, int, quint16)), udpSocket,
          SLOT(onSenddataSlt(QByteArray, int, quint16)));
  emit initSocketSig();
  disconnect(this, SIGNAL(initSocketSig()), udpSocket, SLOT(sltInitSocket()));
}

/**
 * @brief GlobalFlow::删除分析仪信息
 * @param index
 */
void GlobalFlow::deleteAnalyzeInfo(int analyeIndex) {
  //! 删除当前分析仪下的保存参数
  int index = 0;
  foreach (QThread *thread, g_udpthread) {
    if (index == analyeIndex && thread != nullptr) {
      g_udpthread.removeAt(analyeIndex);
      thread->quit();
      thread->wait();
      delete thread;
      thread = nullptr;
      break;
    }
    index++;
  }

  index = 0;
  foreach (myUdp *socket, g_udpSocket) {
    if (index == analyeIndex && socket != nullptr) {
      g_udpSocket.removeAt(analyeIndex);
      delete socket;
      socket = nullptr;
      break;
    }
    index++;
  }

  msleep(200);

  index = 0;
  foreach (MyProcess *process, g_Process) {
    if (index == analyeIndex && process != nullptr) {
      g_Process.removeAt(analyeIndex);
      delete process;
      process = nullptr;
      break;
    }
    index++;
  }

  msleep(2000);

  index = 0;
  foreach (analyzeGlobal *globalTmp, struCnfg.analyzeInfo) {
    if (index == analyeIndex && globalTmp != nullptr) {
      struCnfg.analyzeInfo.removeAt(analyeIndex);
      delete globalTmp;
      globalTmp = nullptr;
      break;
    }
    index++;
  }

  index = 0;
  foreach (analyzeShare *globalTmp, struGsh.analyzeInfo) {
    if (index == analyeIndex && globalTmp != nullptr) {
      struGsh.analyzeInfo.removeAt(index);
      delete globalTmp;
      globalTmp = nullptr;
      break;
    }
    index++;
  }

  index = 0;
  foreach (myUdp *socket, g_udpSocket) {
    socket->setAnalyzeIndex(index);
    connect(this, SIGNAL(rebindSocketSig()), socket,
            SLOT(rebindSocket()) /*, Qt::DirectConnection*/);
    emit rebindSocketSig();
    disconnect(this, SIGNAL(rebindSocketSig()), socket, SLOT(rebindSocket()));
    index++;
  }

  index = 0;
  foreach (MyProcess *process, g_Process) {
    process->setAnalyzeIndex(index);
    index++;
  }

  QString analyzeFile = QString("%1_%2").arg(CNF_ANALYZE).arg(analyeIndex);
  QDir anaDir(analyzeFile);
  anaDir.removeRecursively();

  struGsh.nAnalyzeIndex = 0;
  struCnfg.analyzeNum--;
}

///*!
// * \brief 重置喷嘴像元
// * \param nCamid
// */
void GlobalFlow::ResetPxOfEj(int nCamid) {
  auto *p = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;

  int nStEjector = p->nEjectorBegin[0];
  int nEndEjector = p->nEjectorEnd[0];
  int nEjTotal = nEndEjector - nStEjector + 1;

  //! 每个喷阀对应的像元宽度（由起始阀像元到终止阀像元均匀划分）
  int nEjPxTotal =
      p->nEjectorEndPiexl[nCamid] - p->nEjectorBeginPiexl[nCamid] + 1;
  float fAvr = (float)nEjPxTotal / nEjTotal;

  memset(p->npEjBegin, 0, sizeof(p->npEjBegin));
  memset(p->npEjEnd, 0, sizeof(p->npEjEnd));

  //! 根据nPixelBegin/nPixelEnd找到它们分别落在哪个喷阀上
  int nPixBegin = p->nPixelBegin[nCamid];
  int nPixEnd = p->nPixelEnd[nCamid];
  int nEjPxBegin = p->nEjectorBeginPiexl[nCamid];

  //! 计算nPixelBegin落在的喷阀序号（从nStEjector开始计）
  int nBeginEj = nStEjector + (int)((nPixBegin - nEjPxBegin) / fAvr);
  if (nBeginEj < nStEjector)
    nBeginEj = nStEjector;
  if (nBeginEj > nEndEjector)
    nBeginEj = nEndEjector;

  //! 计算nPixelEnd落在的喷阀序号
  int nEndEj = nStEjector + (int)((nPixEnd - nEjPxBegin) / fAvr);
  if (nEndEj < nStEjector)
    nEndEj = nStEjector;
  if (nEndEj > nEndEjector)
    nEndEj = nEndEjector;

  //! 实际需要划分的喷阀数量
  int nActualEjTotal = nEndEj - nBeginEj + 1;
  int nActualPxTotal = nPixEnd - nPixBegin + 1;
  float fActualAvr = (float)nActualPxTotal / nActualEjTotal;

  //! 按nPixelBegin ~ nPixelEnd范围和对应的喷阀序号进行均匀划分
  for (int j = nBeginEj; j <= nEndEj; j++) {
    int idx = j - nBeginEj; //! 在实际范围内的偏移
    p->npEjBegin[nCamid][j - 1] =
        (idx == 0) ? nPixBegin : p->npEjEnd[nCamid][j - 2] + 1;
    p->npEjEnd[nCamid][j - 1] =
        (j == nEndEj) ? nPixEnd
                      : (int)(nPixBegin + (idx + 1) * fActualAvr - 0.5);

  }

  qDebug() << "ResetPxOfEj: nCamid =" << nCamid
           << "nBeginEj =" << nBeginEj
           << "pixel range =" << p->npEjBegin[nCamid][nBeginEj - 1] << "-"
           << p->npEjEnd[nCamid][nBeginEj - 1]
           << "nEndEj =" << nEndEj
           << "pixel range =" << p->npEjBegin[nCamid][nEndEj - 1] << "-"
           << p->npEjEnd[nCamid][nEndEj - 1];
}

// /*!
//  * \brief 重置喷嘴像元
//  * \param nCamid
//  */
// void GlobalFlow::ResetPxOfEj(int nCamid) {
//   auto *p = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;

//   int nStEjector = p->nEjectorBegin[0];
//   int nEndEjector = p->nEjectorEnd[0];
//   int nPxTotal = p->nPixelEnd[nCamid] - p->nPixelBegin[nCamid] + 1;

//   int nEjTotal = nEndEjector - nStEjector + 1;
//   float fAvr = (float)nPxTotal / nEjTotal;

//   memset(p->npEjBegin, 0, sizeof(p->npEjBegin));
//   memset(p->npEjEnd, 0, sizeof(p->npEjEnd));

//   p->npEjBegin[nCamid][nStEjector - 1] = p->nPixelBegin[nCamid];
//   p->npEjEnd[nCamid][nStEjector - 1] = p->nPixelBegin[nCamid] + fAvr - 0.5;
//   for (int j = nStEjector; j < nEndEjector; j++) {
//     int index = j;
//     p->npEjBegin[nCamid][j] = p->npEjEnd[nCamid][index - 1] + 1;
//     p->npEjEnd[nCamid][j] = p->nPixelBegin[nCamid] + (j + 1) * fAvr - 0.5;
//   }
//   p->npEjEnd[nCamid][nEndEjector - 1] = p->nPixelEnd[nCamid];
// }

/*!
 * \brief 设置机型
 * \param nMachine
 */
void GlobalFlow::sendMachineType(void) {
  char data = struCnfg.nMachineType;
  for (int i = 0; i < gFlow.getMachineCamNum(); i++) {
    QByteArray ba = packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, i,
                                         CMD_MACHINE_TYPE, &data, 1);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
  }

  char da[3] = {0, 0, 0};
  da[0] = struCnfg.nMachineType;
  da[2] =
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnMultilab;
  for (int i = 0; i < gFlow.getMachineCamNum(); i++) {
    QByteArray ba =
        packet_TypeA_NetData(CAMERA_CK, MODULE_SOC, i, CMD_MACHINE_TYPE, da, 3);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
  }
}
