#include "globalflow.h"
#include "common/mymessagebox.h"
#include "core/io/modbus/modbusctlclass.h"
#include "QDateTime"
#include "mylanguage.h"
#include "core/globalflow/sqliteinter.h"
#ifdef Q_OS_UNIX
#include <sys/statfs.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

GlobalFlow  gFlow;
struShare   struGsh;
struGlobal  struCnfg;
struct structEncryptInfo g_encryInfo;

GlobalFlow::GlobalFlow(QObject *parent) : QObject(parent)
{
    nNetNum = 0;
}

GlobalFlow::~GlobalFlow()
{
}

void GlobalFlow::initAll()
{
    myLan.getLanguageSetting(); //! 初始化语言列表

    myLan.setLang();

    getNetSetting();          //! 获取网络配置参数

    getGlobal();              //! 获取全局保存参数

    initParamsShare();        //! 初始化全局共享参数

    initDB();                //! 初始化数据库
}

/**
 * @brief GlobalFlow::发送所有参数
 */
void GlobalFlow::sendAll(bool bStart/* = false*/)
{
    sendMachineType();

    sendProfileParams();

    sendCameraParams(bStart);

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        setHSTempCtrl(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd,
                      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEninfratempCtrl);
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV){
        sendSNVEnstatus(struGsh.nAnalyzeIndex);
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal){ //! 低压阀
        sendvalBoardparams();
        // sendvalBoardOnff();
        sendcapLine2cotrlBoard(struGsh.nAnalyzeIndex);
        sendBlowTime();
        sendDelayTime();
        sendHigfreProtect(struGsh.nAnalyzeIndex);
        sendEjectorOnOff(struGsh.nAnalyzeIndex, struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEjectorOnoff);
    }

    resetSoc(bStart);
}

/**
 * @brief GlobalFlow::单个分析仪发送参数
 * @param index
 */
void GlobalFlow::sendAnalyzeParams(int index)
{
    for (int j = 0; j < gFlow.getMachineCamNum(); j++) {
        //! 1、发送行频
        sendRowFreqPara(index, j, SENSOR_D04);

        //! 2、发送增益
        sendGainPara(index, j);

        //! 3、发送像元
        sendCameraPixelChannel(index, j);
        msleep(30);

        setInfBiasingParams(index, j);
        msleep(30);

        //! 4、重置相机开窗
        setIM01Waves(index, j);
        setIM01Window(index, j);
        setIM01RowFreq(index, j);
    }

    //! 5、发送灯光参数
    sendLampPara(index);
    sendPlusePara(index);

    //! 6、发送背景波动
    for (int j = 0; j < gFlow.getMachineCamNum(); j++) {
        sendBackGroundValue(index, j);
    }
}

void GlobalFlow::sleep(int secs)
{
    QDateTime dieTime = QDateTime::currentDateTime().addSecs(secs);
    while(QDateTime::currentDateTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

void GlobalFlow::msleep(int mSecs)
{
    QDateTime dieTime = QDateTime::currentDateTime().addMSecs(mSecs);

    while(QDateTime::currentDateTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

/*!
 * \brief 是否存在U盘
 * \return
 */
bool GlobalFlow::isUdiskExist()
{
    bool    ret = true;

    QDir myDir("/proc/scsi/usb-storage");
    QFileInfoList list;
    do{
        if(!myDir.exists()){
            ret = false;
            break;
        }

        myDir.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
        list = myDir.entryInfoList();
        if(list.count() <= 0){
            ret = false;
            break;
        }

    }while(0);

    return ret;
}

/**
 * @brief GlobalFlow::U盘挂载
 * @return
 */
bool GlobalFlow::mountUDisk()
{
    //! 查找U盘盘符
    QProcess cmdCat;
    QString strRet;
    cmdCat.setReadChannelMode(QProcess::MergedChannels);
    cmdCat.start("cat /proc/partitions");
    if (cmdCat.waitForFinished(3000)) {
        strRet = cmdCat.readAll();
    } else {
        cmdCat.kill();
        return false;
    }

    QStringList strRetList = strRet.split(QRegExp("\\W+")).filter("sd");
    if (strRetList.isEmpty()) {
        return false;
    }
    QString devName = strRetList.last();

    //! 卸载后再挂载U盘
    umountUDisk();

    QDir dir;
    if (!dir.exists(MOUNT_UDISK_PATH)) {
        if (!dir.mkdir(MOUNT_UDISK_PATH)) {
            return false;
        }
    }

    QProcess cmdMount;
    cmdMount.setReadChannelMode(QProcess::MergedChannels);
    if (devName != "") {
        cmdMount.start("mount", QStringList()<<QString("/dev/%1").arg(devName)<<MOUNT_UDISK_PATH);
    }
    if (cmdMount.waitForFinished(10*1000)) {
        strRet = cmdMount.readAll();
    } else {
        cmdMount.kill();
        return false;
    }

    if (strRet.contains("failed")) {
        return false;
    }

    return true;
}

/**
 * @brief GlobalFlow::卸载U盘
 * @return
 */
bool GlobalFlow::umountUDisk(QString dir)
{
    QProcess cmdUmount;
    QString strRet;
    cmdUmount.setReadChannelMode(QProcess::MergedChannels);
    cmdUmount.start(QString("umount %1").arg(dir));

    //! 延长U盘卸载时间至120秒
    if (cmdUmount.waitForFinished(120*1000)) {
        strRet = cmdUmount.readAll();
        if (strRet.contains("busy")) {
            QString cmd = QString("umount -l %1").arg(dir);
            system(qPrintable(cmd));
            msleep(300);
        }
        return true;
    } else {
        cmdUmount.kill();
    }

    return false;
}

int GlobalFlow::compressDir(QString src, QString des)
{
    int result = 0;
    QString tarCmd = QString("tar -cvf %1 -C%2 .").arg(des).arg(src);
    result = system(qPrintable(tarCmd));
    msleep(1000);
    return result;
}

void GlobalFlow::uncompressDir(QString srcFile, QString des)
{
    QDir dir(des);
    if (!dir.exists()) {
        SystemCmd(QString("mkdir %1").arg(des));
        msleep(1000);
    } else {
        SystemCmd(QString("rm -rf %1").arg(des));
        msleep(1000);
        SystemCmd(QString("mkdir %1").arg(des));
        msleep(1000);
    }

    SystemCmd(QString("unzip -n %1 -d %2").arg(srcFile).arg(des));
    msleep(1000);
}

bool GlobalFlow::zipReader(QString sFile, QString sDir)
{
    QDir dir(sDir);
    if (!dir.exists()) {
        dir.mkpath(sDir);
    }
    QZipReader reader(sFile);
    return reader.extractAll(sDir);
}

bool GlobalFlow::zipWiter(QString sZipPath, QString sDir)
{
    QDir dir(sDir);
    if (!dir.exists()) {
        return false;
    }
    QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    QZipWriter *zipWriter = new QZipWriter(sZipPath);

    foreach (QFileInfo fileInfo, fileList) {
        QFile file(fileInfo.filePath());
        if(!file.exists())
            continue;
        file.open(QIODevice::ReadOnly);
        zipWriter->addFile(fileInfo.fileName(), file.readAll());
        file.close();
    }
    zipWriter->close();
    if(zipWriter != nullptr){
        delete zipWriter;
        zipWriter = nullptr;
    }
    return true;
}

int GlobalFlow::SystemCmd(QString sCmd)
{
    QByteArray latin1 = sCmd.toLatin1();
    const char *command = latin1.data();
#ifdef Q_OS_UNIX
    int pid = 0;
    int status = 0;
    char *argv[4];
    extern char **environ;

    if (NULL == command) {
        return -1;
    }

    //    pid = __libc_fork();
    pid = vfork();        /* vfork() also works */
    if (pid < 0) {
        return -1;
    }
    if (0 == pid) {             /* child process */
        argv[0] = (char *)"sh";
        argv[1] = (char *)"-c";
        argv[2] = (char *)command;
        argv[3] = NULL;

        execve ("/bin/sh", argv, environ);    /* execve() also an implementation of exec() */
        exit (127);
    }

    /* wait for child process to start */
    do {
        if (waitpid(pid, &status, 0) < 0) {
            if (errno == EINTR || errno == ECHILD) {
                return status;
            } else {
                return -1;
            }
        }
    } while(1);

    return 0;
#else
    return system(command);
#endif
}


qint64 GlobalFlow::getDirSize(const QString &path)
{
    QProcess process;
    process.start("du",{"-sb", path});
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();
    QStringList lst = output.split("\t");
    if(lst.size()>0){
        QString strsize = lst.at(0);
        return strsize.toLongLong();
    }

    return -1;
}

uint GlobalFlow::fpgaCrc(const char *sData, int nLen)
{
    uchar j;
    uchar tmp;
    uint nCrc = 0;

    while (nLen--) {
        for (j = 0x80; j != 0; j >>= 1)
        {
            if ((nCrc&0x8000) != 0) {
                nCrc <<= 1;
                nCrc ^= 0x1021;
            }
            else {
                nCrc <<= 1;
            }
            tmp = (uchar)(*sData);
            if ((tmp&j) != 0) {
                nCrc ^= 0x1021;
            }
        }
        sData++;
    }

    return nCrc;
}


/**
 * @brief GlobalFlow::版本查询
 */
void GlobalFlow::initCommunication(bool bManual /*= false*/)
{
    initVersion();
    for(int i=0; i<BOARD_ALL; i++){
        struGsh.bSysBoardSta[i] = false;
    }

    while (struGsh.nCheckTimes < 3) {
        for (int i = 0; i < struCnfg.analyzeNum; i++) {
            QString sCtrlVersion(struGsh.analyzeInfo[i]->sCtrlVersion);
            if (sCtrlVersion == "NULL") {
                reqCtrlVersion(i);
                msleep(50);
            }

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPulseLight){
                if (!QString(struGsh.analyzeInfo[i]->sPulseVersion).compare("NULL")) {
                    reqSourceVersion(i);
                    msleep(50);
                }
            }

            for (int j = 0; j < gFlow.getMachineCamNum(); j++) {
                if (!QString(struGsh.analyzeInfo[i]->sCamVersion[j]).compare("NULL")) {
                    reqCameraVersion(i,j);
                    msleep(50);
                }
            }

            //! 添加卤素灯检测
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
                QString sHalogenVersion(struGsh.analyzeInfo[i]->sHalogenVersion);
                if (sHalogenVersion == "NULL") {
                    reqHalogenVersion(i);
                    msleep(50);
                }
            }

            //! 查询电磁阀板
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV){
                QString sSVVersion(struGsh.analyzeInfo[i]->sSVBoardVersion);
                if (sSVVersion == "NULL") {
                    reqSVBoardVersion(i);
                    msleep(50);
                }
            }

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal){
                QString sValVersion(struGsh.analyzeInfo[i]->sValveBoardVersion);
                if (sValVersion == "NULL") {
                    reqValBoardVersion(i);
                    msleep(50);
                }
            }
        }
        struGsh.nCheckTimes++;
    }

    //! 开机自动查询SOC版本，最多30次。手动刷新查询版本时，最多3次，防止等待时间过长
    int maxTimes = bManual ? 1 : 3;
    QVector<bool> retSocVct(struCnfg.analyzeNum*MAX_CAM_NUM, false);
    int tmp = 0;
    while(tmp < maxTimes){
        bool bRet = true;
        for (int i = 0; i < struCnfg.analyzeNum; i++) {
            for(int j = 0; j < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; j++){

                if(retSocVct[i*MAX_CAM_NUM+j])
                    continue;

                QString sSocVersion(struGsh.analyzeInfo[i]->sSocVersion[j]);
                if (sSocVersion == "NULL") {
                    reqSocVersion(i, j);
                    bRet = false;
                }else {
                    retSocVct[i*MAX_CAM_NUM+j] = true;
                }
                msleep(100);
            }
            msleep(100);
        }

        if(bRet)
            break;

        tmp++;
        msleep(1000);
    }

    //! 查询soc加载模型状态
     tmp = 0;
     QVector<bool> retSocModel(MAX_CAM_NUM, false);
      while(tmp < maxTimes){
         bool bRet = true;
         for(int j = 0; j < gFlow.getMachineCamNum(); j++){
             if(retSocModel[j]){
                 retSocModel[j] = true;
                 continue;
             }

             if (!struGsh.bSocModelUnitResult[j]) {
                 reqSocModelinfo(struGsh.nAnalyzeIndex, j);
                 bRet = false;
             }else {
                 retSocModel[j] = true;
             }
             msleep(300);
         }

         if(bRet)
             break;

         tmp++;
         msleep(1000);
     }

      //! 查询soc标签
     tmp = 0;
     for(int i=0; i<gFlow.getMachineCamNum(); i++){
         struGsh.vlabNamemap[i].clear();
      }
      while(tmp < maxTimes){
          bool bRet = true;
          for(int j = 0; j < gFlow.getMachineCamNum(); j++){
              if(!struGsh.vlabNamemap[j].isEmpty())
                  continue;

              reqSocModelLabelinfo(struGsh.nAnalyzeIndex, j);
              bRet = false;

              msleep(300);
          }

          if(bRet)
              break;

          tmp++;
          msleep(300);
      }

      //! 查询统计标签类别
      tmp =0;
      for(int k=0; k<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaticGroup; k++){
          struGsh.vlabStamap[k].clear();
      }
      while(tmp < maxTimes){
          for(int n=0; n<gFlow.getMachineCamNum(); n++){
              int nGbid = gFlow.getStaticIdfromCamid(n);
              if(!struGsh.vlabStamap[nGbid].isEmpty()){
                  continue;
              }
              reqStainfo(n);
          }
          tmp++;
          msleep(1500);
      }

      msleep(struCnfg.analyzeNum*200);

//    emit versionCheckOK();

    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        struGsh.analyzeInfo[i]->bCheckVerStartUp = false;
    }
}

/**
 * @brief GlobalFlow::检查版本查询结果
 */
bool GlobalFlow::checkCommunicationRet()
{
    bool ret = false;
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        ret = checkAnalyzeVersion(i);
    }
    return ret;
}

int GlobalFlow::checkAnalyzeVersion(int analyIndex)
{
    bool ret = false;
    bool bModelSta = false, bCamSta =false, bSocSta = false,
         bHalogenSta = false, bPlusLightSta = false, bSVBoard = false,
         bValBoard = false, bControl = false;

    QString sCtrlVersion(struGsh.analyzeInfo[analyIndex]->sCtrlVersion);
    if(sCtrlVersion.compare("NULL") == 0){
       bControl = true;
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPulseLight){
        if(QString(struGsh.analyzeInfo[analyIndex]->sPulseVersion).compare("NULL") == 0){
            bPlusLightSta = true;
        }
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        if(QString(struGsh.analyzeInfo[analyIndex]->sHalogenVersion).compare("NULL") == 0){
            bHalogenSta = true;
        }
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV){
        if(QString(struGsh.analyzeInfo[analyIndex]->sSVBoardVersion).compare("NULL") == 0){
            bSVBoard = true;
        }
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal){
        if(QString(struGsh.analyzeInfo[analyIndex]->sValveBoardVersion).compare("NULL") == 0){
            bValBoard = true;
        }
    }

    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        if(QString(struGsh.analyzeInfo[analyIndex]->sCamVersion[i]).compare("NULL") == 0){
            bCamSta = true;
        }

        if(i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum){
            if(QString(struGsh.analyzeInfo[analyIndex]->sSocVersion[i]).compare("NULL") == 0){
                bSocSta = true;
            }
        }
    }

    for(int i =0;i<gFlow.getMachineCamNum();i++){
        if(!gFlow.bIsSupportAI(i)){
            continue;
        }
        if(!struGsh.bSocModelUnitResult[i]){
            bModelSta = true;
            break;
        }
    }


    if (bControl || bCamSta || bSocSta
        || bHalogenSta || bPlusLightSta|| bModelSta || bSVBoard || bValBoard) {
        ret = true;
    }

    return ret;
}

/**
 * @brief GlobalFlow::初始化分析仪各模块版本
 */
void GlobalFlow::initVersion()
{
    for(int i = 0; i < struCnfg.analyzeNum; i++) {
        strcpy(struGsh.analyzeInfo[i]->sCtrlVersion, "NULL");
        strcpy(struGsh.analyzeInfo[i]->sPulseVersion, "NULL");
        strcpy(struGsh.analyzeInfo[i]->sHalogenVersion, "NULL");
        strcpy(struGsh.analyzeInfo[i]->sSVBoardVersion, "NULL");
        strcpy(struGsh.analyzeInfo[i]->sValveBoardVersion, "NULL");


        for (int j = 0; j < MAX_CAM_NUM; j++) {
            strcpy(struGsh.analyzeInfo[i]->sCamVersion[j], "NULL");
            strcpy(struGsh.analyzeInfo[i]->sSocVersion[j], "NULL");
            struGsh.bSocModelUnitResult[j] = false;
            for(int k=0; k<MAX_SOC_MODEL_COUNT_L; k++){
                strcpy(struGsh.sModelUnitInfo[j].model[k].cnf,QString("NULL").toLatin1().data());
            }
        }
    }

    struGsh.nCheckTimes = 0;
}

/*!
 * \brief 获取当前机型总相机个数
 * \return
 */
int GlobalFlow::getMachineCamNum()
{
    int nCamnum =   struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist ?
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum +1 :
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
    return nCamnum;
}

/**
 * @brief GlobalFlow::字符校验
 * @param str
 * @return
 */
bool GlobalFlow::inputStrIsValid(QString str)
{
    return (!str.contains(",") && !str.contains(";"));
}

/**
 * @brief GlobalFlow::校验字符正则（中文，英文，数字，下划线）
 * @param str
 * @return
 */
bool GlobalFlow::inputCodeIsValid(QString str)
{
    return QRegExp(QString("[0-9]{13}")).exactMatch(str);
}

/*!
 * \brief 组A类包
 * \param sCameratype
 * \param sModuleType
 * \param sModuleAddr
 * \param nCmd
 * \param pData
 * \param nDataLength
 * \return
 */
QByteArray GlobalFlow::packet_TypeA_NetData(char sCameratype, char sModuleType, char sModuleAddr, int nCmd, const char *pData, int nDataLength)
{
    char sPacket[LEN_PACKET_A];

    memset(sPacket, 0, sizeof(sPacket));

    //! 数据头 2位
    sPacket[0] = 0xA5;
    sPacket[1] = 0x5A;

    //! 相机类型 0-处理板 1-驱动板
    sPacket[2] = sCameratype;

    //! 预留位
    sPacket[3] = 0;

    //! 模块类型
    sPacket[4] = sModuleType;

    if((MODULE_CAM == sModuleType || MODULE_SOC == sModuleType) && gFlow.bIsIMSerialCam(sModuleAddr)){
        sModuleAddr = 32;
        if(CMD_VERSION == nCmd){
            nCmd = CMD_UNIT_IM01_VERSION;
        }
    }

    //! 模块地址
    sPacket[5] = sModuleAddr;

    //! 命令编码 2位
    sPacket[6] = nCmd/256;
    sPacket[7] = nCmd%256;

    //! 命令序号
    sPacket[8] = nNetNum++;

    if (nNetNum > 255) {
        nNetNum = 0;
    }

    //! 命令数据
    memcpy(sPacket + 9, pData, nDataLength);

    //! CRC校验
    uint nCrcItt = fpgaCrc(sPacket + 2, LEN_PACKET_A_CRC);

    sPacket[40] = nCrcItt/256;
    sPacket[41] = nCrcItt%256;

        //包尾
    sPacket[42] = 0xFF;
    sPacket[43] = 0xFF;

    QByteArray ba;
    ba.append(sPacket, LEN_PACKET_A);
    return ba;
}

/*!
 * \brief GlobalFlow::packet_TypeI_NetData
 * \param sCameratype
 * \param sModuleType
 * \param sModuleAddr
 * \param nCmd
 * \param pData
 * \param nDataLength
 * \return
 */
QByteArray GlobalFlow::packet_TypeI_NetData(char sModuleType, char sModuleAddr, int nCmd, const char *pData, int nDataLength)
{
    char sPacket[MAX_LEN_PACKET_I];

    memset(sPacket, 0, sizeof(sPacket));

    //! 数据头 2位
    sPacket[0] = 0xAA;
    sPacket[1] = 0xAA;
    sPacket[2] = 0xAA;
    sPacket[3] = 0xAA;

    //! 模块类型
    sPacket[6] = sModuleType;


    //! 模块地址
    sPacket[7] = sModuleAddr;

    //! 命令编码 2位
    sPacket[8] = nCmd/256;
    sPacket[9] = nCmd%256;

    //! 命令序号
    sPacket[10] = nDataLength/256;
    sPacket[11] = nDataLength%256;

    //! 命令数据
    memcpy(sPacket + 12, pData, nDataLength);

    //! CRC校验
    uint nCrcItt = fpgaCrc(sPacket + 6, nDataLength+6);

    sPacket[nDataLength+12] = nCrcItt/256;
    sPacket[nDataLength+13] = nCrcItt%256;

    //包尾
    sPacket[nDataLength+14] = 0x55;
    sPacket[nDataLength+15] = 0x55;

    QByteArray ba;
    ba.append(sPacket, nDataLength+16);
    return ba;
}

analyzeShare *GlobalFlow::getSpecifyAnalyzeShare(int anaIndex)
{
    auto &lstAnalyzeShare = struGsh.analyzeInfo;
    if(anaIndex < lstAnalyzeShare.size())
        return lstAnalyzeShare[anaIndex];
    else
        return nullptr;
}

analyzeGlobal *GlobalFlow::getSpecifyAnalyzeGlobal(int anaIndex)
{
    auto &lstAnalyzeGlobal = struCnfg.analyzeInfo;
    if(anaIndex < lstAnalyzeGlobal.size())
        return lstAnalyzeGlobal[anaIndex];
    else
        return nullptr;
}

/**
 * @brief GlobalFlow::获取默认方案名称
 * @param type
 * @return
 */
QString GlobalFlow::getInitSchemeName(int index)
{
    QString str = QString("%1_%2").arg(myLan.schme).arg(index+1);

    return str;
}


/**
 * @brief GlobalFlow::设置打印结果
 * @param strPrint
 * @param anaIndex
 * @param machineIndex
 */
void GlobalFlow::onPrint(QList<QStringList> conttitle, QList<QStringList> contlst,
                         QStringList titlelst, QStringList taillst)
{
#ifdef Q_OS_UNIX
    void *handle = NULL;
    for(int i = 0; i < 2; ++i) {
        handle = Port_OpenCOMIO(qPrintable(QString("%1%2").arg(COM_VIRTUAL).arg(i)), 9600);
        if(handle != NULL){
            break;
        }
    }

    if(handle == NULL)
        handle = Port_OpenCOMIO(COM_PRINT, 9600);

    if(handle == NULL){
        gFlow.printLog("Port_OpenCOMIO failed!");
        QString sResult;
        sResult = QString("Can not open port. please check the serial io.");
        infoWidget->raise();
        infoWidget->setLabelText(sResult);
        infoWidget->showSecs();
        return;
    }
    Port_SetPort(handle);


    QString companyName = QString("%1\n").arg(struCnfg.gCompanyName);
    Pos_Text(companyName.toStdString().c_str(), 0, -2, 0.5, 0.5, 0, 0x08);

    // putPrintInfoInOrder(true, conttitle,contlst, titlelst);

    Pos_FeedLine();
    Pos_ImagePrint("Meyer1.png");

    Pos_Feed_N_Line(5);
    Pos_FullCutPaper();
    Port_ClosePort(handle);
#endif
}

/*!
 * \brief 按行打印
 * \param nCol：列数
 * \param vRow：打印信息按行排布
 * \param titlelst：头信息
 * \param taillst：尾信息
 */
void GlobalFlow::PrintByrow(int nCol, QVector<QStringList> vRow, QStringList titlelst, QStringList taillst)
{
#ifdef Q_OS_UNIX
    void *handle = NULL;
    for(int i = 0; i < 2; ++i) {
        handle = Port_OpenCOMIO(qPrintable(QString("%1%2").arg(COM_VIRTUAL).arg(i)), 9600);
        if(handle != NULL){
            break;
        }
    }

    if(handle == NULL)
        handle = Port_OpenCOMIO(COM_PRINT, 9600);

    if(handle == NULL){
        gFlow.printLog("Port_OpenCOMIO failed!");
        QString sResult;
        sResult = QString("Can not open port. please check the serial io.");
        infoWidget->raise();
        infoWidget->setLabelText(sResult);
        infoWidget->showSecs();
        return;
    }
    Port_SetPort(handle);


    QString companyName = QString("%1\n").arg(struCnfg.gCompanyName);
    Pos_Text(companyName.toStdString().c_str(), 0, -2, 1, 1, 0, 0x08);

    if(renderReport2Pixmap(nCol, vRow, titlelst, taillst)) {
        Pos_ImagePrint("/userdata/meyer/report.png");
    }else {
        qDebug()  << "renderReport2Pixmap failed!";
        // gFlow.printLog("renderReport2Pixmap failed!");
    }   
    

    QString content;
    content += QString("%1\n").arg(myLan.meyer_company); //!合肥美亚光电技术股份有限公司
    content += QString("%1\n\n").arg(myLan.provide_technical_support); //! 提供技术支持
    content += QString("%1: \n%2").arg(myLan.friendly_reminder).arg(myLan.warn_store); //! 温馨提示：\n在常温、遮光下保存
    Pos_Text(content.toStdString().c_str(), 0, -2, 0, 0, 0, 0);

    Pos_FeedLine();
    Pos_ImagePrint("Meyer1.png");

    Pos_Feed_N_Line(5);
    Pos_FullCutPaper();
    Port_ClosePort(handle);
#endif
}

bool GlobalFlow::renderReport2Pixmap(int cols, const QVector<QStringList> &data,
                                        QStringList titlelist, QStringList tailedliste)
{
    bool bSucess = true;

    // 可配置宽度（与 renderReportPixmap 保持一致的缩放策略）
    const int defaultWidth = 2800;
    const int canvasWidth = defaultWidth;
    const double baseWidth = 1200.0;
    const double uiScale = qMax(1.0, double(canvasWidth) / baseWidth);

    // 使 titleFont 与 cellFont 保持一致的字体设置
    QFont cellFont;
    cellFont.setBold(false);
    cellFont.setPointSizeF(70.0 * uiScale);

    QFont titleFont = cellFont; // 与 cellFont 一致（按需可调整 bold/size）
    // 如果需要 title 更醒目，可启用加粗： titleFont.setBold(true);

    QFontMetrics fmTitle(titleFont);
    QFontMetrics fmCell(cellFont);

    const int margin = 0;
    const int left = margin;
    const int right = canvasWidth - margin;
    const int usableWidth = right - left;
    const int colGap = 0;//qMax(6, qRound(8 * uiScale));
    const int titlePaddingV = qMax(8, qRound(12 * uiScale));

    // Titlelist 按 2 列布局（第一列左对齐，第二列右对齐）
    const int titleCols = 2;
    int titleRows = titlelist.size() / 2 + (titlelist.size() % 2 ? 1 : 0);
    int titleColWidth = (usableWidth - (titleCols - 1) * colGap) / titleCols;

    // 预先计算每一行的实际高度（考虑换行）
    QVector<int> titleRowHeightsList;
    int totalTitleHeight = 0;
    for (int r = 0; r < titleRows; ++r) {
        int rowH = 0;
        for (int c = 0; c < titleCols; ++c) {
            int idx = r * 2 + c;
            if (idx < titlelist.size()) {
                QRect rc(0, 0, titleColWidth, 10000);
                QRect br = fmTitle.boundingRect(rc, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, titlelist.value(idx));
                rowH = qMax(rowH, br.height());
            }
        }
        rowH = qMax(rowH, fmTitle.height()) + titlePaddingV;
        titleRowHeightsList.append(rowH);
        totalTitleHeight += rowH;
    }
    int titleRowHeight = totalTitleHeight;

    // Tail list：每个 QString 单独占一行，宽度为 usableWidth
    int tailCols = 1;
    int tailRows = tailedliste.size();
    int tailColWidth = usableWidth;
    int tailRowHeight = 0;
    for (int r = 0; r < tailRows; ++r) {
        QRect rc(0,0,tailColWidth,10000);
        QRect br = fmTitle.boundingRect(rc, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, tailedliste.value(r));
        // 每行都加上垂直间距（与绘制时一致）
        tailRowHeight += qMax(br.height(), fmTitle.height()) + titlePaddingV;
    }

    // 表格列宽计算（均分）
    int columnCount = qMax(1, cols);
    QVector<int> colWidth(columnCount);
    int baseW = (usableWidth - (columnCount - 1) * colGap) / columnCount;
    for (int i = 0; i < columnCount; ++i) {
        colWidth[i] = baseW;
        if(i!=columnCount-1 && i!=columnCount-2){
            colWidth[i] = baseW;
        }else if(i==columnCount-2){
            colWidth[i] = baseW+270;
        }else{
            colWidth[i] = baseW-270;
        }
    }

    // 计算行高度（基于 cell 字体）
    const int cellVPadding = qMax(10, qRound(12 * uiScale));
    const int minRowHeight = qMax(qRound(36 * uiScale), fmCell.height() + cellVPadding);
    QVector<int> rowHeights;
    int rowCount = data.size();
    int totalRowHeight = 0;
    for (int r = 0; r < rowCount; ++r) {
        int rh = 0;
        for (int c = 0; c < columnCount; ++c) {
            QString cell;
            if (r < data.size()) {
                const QStringList &rl = data.at(r);
                if (c < rl.size()) cell = rl.at(c);
            }
            QRect rc(0,0,colWidth[c],10000);
            QRect br = fmCell.boundingRect(rc, Qt::TextWordWrap | Qt::AlignCenter, cell);
            rh = qMax(rh, br.height());
        }
        rh += cellVPadding;
        if (rh < minRowHeight) rh = minRowHeight;
        rowHeights.append(rh);
        totalRowHeight += rh;
    }

    // 网格线宽
    const int gridLineWidth = qMax(4, qRound(2 * uiScale));
    const int bottomExtra = margin + gridLineWidth;

    // 总体高度 = margin + title区 + table区 + tail区 + bottomExtra
    int requiredHeight = margin + titleRowHeight + totalRowHeight + tailRowHeight + bottomExtra;
    QSize canvasSize(canvasWidth, requiredHeight);

    QPixmap pixmap(canvasSize);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(Qt::black);

    //! 绘制 titlelist（表格上方，不在网格内）
    painter.setFont(titleFont);
    int curY = margin;
    int titleY = curY;
    for (int r = 0; r < titleRows; ++r) {
        int rowH = titleRowHeightsList.at(r);
        for (int c = 0; c < titleCols; ++c) {
            int idx = r * 2 + c;
            if (idx >= titlelist.size()) continue;
            int x = left + c * (titleColWidth + colGap);
            QRect cellRect(x, titleY, titleColWidth, rowH);
            if (c == 0) {
                // 第一列左对齐
                painter.drawText(cellRect.adjusted(6, 6, -6, -6), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, titlelist.value(idx));
            } else {
                // 第二列右对齐
                painter.drawText(cellRect.adjusted(6, 6, -6, -6), Qt::AlignRight | Qt::AlignVCenter | Qt::TextWordWrap, titlelist.value(idx));
            }
        }
        titleY += rowH;
    }
    curY = titleY;

    // 绘制表格（数据）
    const int boldFactor = 3; // 可调整为 2/3 以改变粗细
    const int boldGridWidth = qMax(gridLineWidth, gridLineWidth * boldFactor);
    QPen gridPen(QColor(0,0,0));
    gridPen.setWidth(boldGridWidth);
    gridPen.setJoinStyle(Qt::MiterJoin);
    gridPen.setCapStyle(Qt::SquareCap);
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setCosmetic(true);

    painter.setFont(cellFont);
    int tableTop = curY;
    int currentY = tableTop;
    for (int r = 0; r < rowCount; ++r) {
        int rh = rowHeights.value(r);
        int x = left;
        // 内容
        for (int c = 0; c < columnCount; ++c) {
            QRect cellRect(x, currentY, colWidth[c], rh);
            QString cell;
            if (r < data.size()) {
                const QStringList &rl = data.at(r);
                if (c < rl.size()) cell = rl.at(c);
            }
            painter.setPen(Qt::black);
            painter.drawText(cellRect.adjusted(8, 6, -8, -6),
                             Qt::AlignCenter | Qt::TextWordWrap, cell);
            x += colWidth[c] + colGap;
        }
        // 网格线（每个单元格四边）
        painter.setPen(gridPen);
        x = left;
        for (int c = 0; c < columnCount; ++c) {
            QRect cellRect(x, currentY, colWidth[c], rh);
            painter.drawRect(cellRect);
            x += colWidth[c] + colGap;
        }
        currentY += rh;
    }

    //! 绘制尾部文本（tailedliste）在表格下方，每项单独一行且左对齐
    int tailTop = currentY + titlePaddingV;
    painter.setFont(titleFont);
    int tailY = tailTop;
    for (int r = 0; r < tailRows; ++r) {
        QRect cellRect(left, tailY, tailColWidth, fmTitle.height() + titlePaddingV);
        painter.setPen(Qt::black);
        painter.drawText(cellRect.adjusted(6,6,-6,-6), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, tailedliste.value(r));
        QRect br = fmTitle.boundingRect(QRect(0,0,tailColWidth,10000), Qt::TextWordWrap | Qt::AlignLeft, tailedliste.value(r));
        int rowH = qMax(br.height(), fmTitle.height()) + titlePaddingV;
        tailY += rowH;
    }
    currentY = tailY;

    //! 绘制总体外框（将 title/table/tail 全部框住）
    // outerFrameWidth 单独控制外框粗细，便于打印时增强边框可见性
    const int outerFrameScale = 3; // 可调整（1 = 与表格同宽，2 = 两倍，...）
    const int outerFrameWidth = qMax(1, boldGridWidth * outerFrameScale);
    QPen outerPen(QColor(0,0,0));
    outerPen.setWidth(outerFrameWidth);
    outerPen.setStyle(Qt::SolidLine);
    outerPen.setCosmetic(true);
    painter.setPen(outerPen);
    int outerTop = margin;
    int outerLeft = qMax(0, left - outerFrameWidth/2);
    int outerRight = qMin(pixmap.width() - 1, left + usableWidth + outerFrameWidth/2);
    int outerBottom = qMin(pixmap.height() - 1, currentY + outerFrameWidth/2);
    painter.drawRect(QRect(outerLeft, outerTop, outerRight - outerLeft, outerBottom - outerTop));

    painter.end();

    // 转为 QImage 并设置打印用 DPI（例如 300 DPI）
    QImage image = pixmap.toImage();
    const int dpi = 300;
    const int dotsPerMeter = qRound(dpi * 39.37007874); // DPI -> dots per meter
    image.setDotsPerMeterX(dotsPerMeter);
    image.setDotsPerMeterY(dotsPerMeter);

    QString outPng = QString("%1/report.png").arg("/userdata/meyer");
    QImageWriter pngWriter(outPng, "png");
    pngWriter.setCompression(0);   // 0..9，0 = 不压缩（文件更大，但快速且保真）
    pngWriter.setQuality(100);     // PNG 不常用 quality，但设置无害
    if (!pngWriter.write(image)) {
        bSucess = false;
        qWarning() << "Save PNG failed:" << pngWriter.errorString();
    }

    return bSucess;
}



/**
 * @brief GlobalFlow::putPrintInfoInOrder
 * @param strPrint
 * @param anaIndex
 * @param machineIndex
 * @param bPrint
 * @return
 */
QString GlobalFlow::putPrintInfoInOrder(bool bPrint,QList<QStringList> labtileLst,
                                        QList<QStringList> contlst,
                                        QList<QStringList> titlelst)
{
    QString content;
    content += QString("================================\n"); //! 36 12

    int num =0;
    for(QStringList title : titlelst){
        for(auto str:title){
            content +=str;
            num +=1;
            if(num %2 ==0){
                content+="\n";
            }
        }
    }


    content += QString("================================\n"); //! 36 12

    for(int n=0; n<labtileLst.size(); n++){
        int ncol = labtileLst[n].size();
        int nColwid = 36/ncol;
        int nHead =1;
        for(QString headline : labtileLst[n]){
            if(nHead % ncol !=0 ){
                content += joinStr(headline, "", nColwid);
            }else{
                content += headline;
                content += "\n";
            }
            nHead +=1;
        }
        int nIdx = 1;
        for(QString lab : contlst[n]){
            if(nIdx % ncol !=0 ){
                content += joinStr(lab, "", nColwid);
            }else{
                content += lab;
                content += "\n";
            }
            nIdx +=1;
        }
    }
//    for(int i=0; i<contlst.size(); i++){
//        QString str;
//        if((i%3==0)&&(i!=0)){
//            content+="\n";
//        }
//        switch(i%3){
//        case 0:
////            content += joinStr(contlst.at(i), "", 19);
//            content += gFlow.preProcessStr(contlst.at(i),12);
//            break;
//        case 1:
////            content += joinStr(contlst.at(i), "", 7);
//            content += gFlow.preProcessStr(contlst.at(i), 7);
//            break;
//        case 2:
////            if(contlst.at(i).length()>2){

////                content += contlst.at(i);
////            }else{
////                content += joinStr(contlst.at(i), "", 6);
////            }
//            content += gFlow.preProcessStr(contlst.at(i), 6);
//            break;
//        default:
//            content+=contlst.at(i);
//            break;
//        }
//    }

    content += "\n";
    content += QString("================================\n\n");

    content += QString("合肥美亚光电技术股份有限公司\n");
    content += QString("提供技术支持\n\n");
    if(bPrint)
        content += QString("温馨提示：\n在常温、遮光下保存");

#ifdef Q_OS_UNIX
    if(bPrint)
        Pos_Text(content.toStdString().c_str(), 0, -2, 0, 0, 0, 0);
#endif

#if 0
    gFlow.printLog(content);
#endif

    return content;
}

/**
 * @brief GlobalFlow::获取方案名称
 * @param index
 * @return
 */
QString GlobalFlow::getSchemeName(int index)
{
    return struCnfg.schemeList.at(index).name;
}

/**
 * @brief GlobalFlow::获取目标方案编码
 * @param index
 * @return
 */
QString GlobalFlow::getSchmCodeFromIndex(int index)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;
    if(index < schmLst.size())
        return schmLst[index].code;
    return "";
}

bool GlobalFlow::bAiSetCpu(int nCpuId)
{
#ifdef Q_OS_UNIX
    struct sched_param parms;
    int policy = SCHED_FIFO;
    parms.sched_priority = sched_get_priority_max(policy);

    if(nCpuId > MAX_CPU_CORE || nCpuId < 0) return false;
    if(nCpuId != MAX_CPU_CORE)
    {
        cpu_set_t mask;                                           //! cpu核的集合
        CPU_ZERO(&mask);                                          //! 将集合置为空集
        CPU_SET(nCpuId, &mask);                                   //! 设置亲和力值  绑定0核心上
        if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) //! 设置线程cpu亲和力
        {
            printf("SetCpu error\n");
            return false;
        }
        sched_setscheduler(0, policy, &parms); //! 设置当前进程的调度优先级为最高
    }
#endif
    return true;
}

/*!
 * \brief 重置统计信息
 */
void GlobalFlow::resetStats()
{
    struGsh.nMetaNum =0;

    for(int i=0; i<struGsh.vlabStamap.size(); i++){
        for (QString lab : struGsh.vlabStamap[i].keys()){
            for (QString key : struGsh.vlabStamap[i][lab].staRes.keys()) {
                for(float &var : struGsh.vlabStamap[i][lab].staRes[key]){
                    var =0;
                }
            }
        }
    }

    for(int i=0; i<struGsh.vsMatinfo.size(); i++){
        for (QString key :  struGsh.vsMatinfo[i].staRes.keys()) {
            for(float &var :struGsh.vsMatinfo[i].staRes[key]){
                var = 0;
            }
        }
    }

    for(int i=0; i<struGsh.vlabTotalmap.size(); i++){
        for(QString key : struGsh.vlabTotalmap[i].keys()){
             struGsh.vlabTotalmap[i][key] = 0.0;
        }

    }

    for(int i=0; i<struGsh.vmatTotalmap.size(); i++){
        for(QString key : struGsh.vmatTotalmap[i].keys()){
              struGsh.vmatTotalmap[i][key] = 0.0;
        }
    }

    for(int i=0; i<struGsh.vCrossBoxesOld.size(); i++){
        struGsh.vCrossBoxesOld[i].clear();
    }

    for(int i=0; i<struGsh.vCrossBoxesOldSingleNum.size(); i++){
        struGsh.vCrossBoxesOldSingleNum[i].clear();
    }


    for(int i=0; i<struGsh.vSocInfer.size(); i++){
        struGsh.vSocInfer[i].clear();
    }

    struGsh.lineBoxMap.clear();

    //! 清空标签映射记录
    for(int i=0; i<struGsh.vCamLabelMapping.size(); i++){
        struGsh.vCamLabelMapping[i].clear();
    }


    for(int i=0; i<struGsh.vSocInferReady.size(); i++){
        struGsh.vSocInferReady[i].clear();
    }
}

bool GlobalFlow::calibrate(const QVector<QMap<QString, double> > &ratios, const QVector<std::vector<std::vector<BBox> > > &samples)
{
    Q_UNUSED(ratios);
    Q_UNUSED(samples);
    return true;
}

/*!
 * \brief 获取vpn地址
 * \return
 */
bool GlobalFlow::getVPNAddr()
{
    struGsh.sIp.clear();

    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface inter, interfaceList)
    {
        if(inter.name() == QString("tun0"))
        {
            QList<QNetworkAddressEntry> hostAddressList = inter.addressEntries();
            foreach (QNetworkAddressEntry address, hostAddressList)
            {
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    struGsh.sIp = address.ip().toString();
                }
            }

            return true;
        }
    }

    return false;
}

/*!
 * \brief 连接vpn
 * \return
 */
bool GlobalFlow::connetToVPN()
{
#ifdef Q_OS_UNIX
    char cmd[256];
    int ret;

    QString appDir = QCoreApplication::applicationDirPath();
    QString confPath = appDir + "/vpn/CHS/client.conf";
    QString openvpnCmd = QString("cd /usr/sbin; ./openvpn --config \"%1\" &").arg(confPath);
    QByteArray ba = openvpnCmd.toLocal8Bit();
    sprintf(cmd, "%s", ba.data());
    ret = system(cmd);
#endif

    int maxTimes = 30;

    for(int i = 0; i < maxTimes; i++){
        if(getVPNAddr()){
            return true;
        }

        gFlow.msleep(500);
    }

    return false;
}

/*!
 * \brief 关闭vpn
 * \return
 */
void GlobalFlow::closeVPN()
{
#ifdef Q_OS_UNIX
    system("killall openvpn");
#endif
}



/**
 * @brief GlobalFlow::获取目标方案名称
 * @param str
 * @return
 */
QString GlobalFlow::getSchmNameFromCode(QString code)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;

    for(int i = 0; i < schmLst.size(); ++i){
        if(schmLst[i].code.compare(code) == 0)
            return schmLst[i].name;
    }

    return "";
}

int GlobalFlow::getSchmIndexFromCode(QString code)
{
    QList<schemeInfo> &schmLst = struCnfg.schemeList;

    for(int i = 0; i < schmLst.size(); ++i){
        if(schmLst[i].code.compare(code) == 0)
            return i;
    }
    return -1;
}

void GlobalFlow::qStringTochar(QString src, char *dst, int ndstlen)
{
    QByteArray  ba = src.toLocal8Bit();
    int nmin = qMin(ndstlen -1, ba.length());
    memset(dst,0,ndstlen);
    memcpy(dst,ba.data(),nmin);
}

/*!
 * \brief 根据value获取Key值
 * \param str
 * \return
 */
QString GlobalFlow::getMegerlabKeyByValue(const QString &str, const int &ncamId)
{
    QString key;
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    if(ncamId >= pProfile->vMegerlabmap.size()||
            pProfile->vMegerlabmap[ncamId].size() ==0){
        return key;
    }
    for(auto itor = pProfile->vMegerlabmap[ncamId].constBegin();
         itor!=pProfile->vMegerlabmap[ncamId].end(); ++itor){
        if(itor.value() == str){
            key = itor.key();
            break;
        }
    }
    return key;
}

/*!
 * \brief GlobalFlow::bMegerlabshow
 * \param str
 * \return
 */
bool GlobalFlow::bMegerlabshow(const QString &str, const int& ncamid)
{
    bool bshow = true;
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    if(pProfile->vMegerlabmap[ncamid].contains(str)){
        for(QString var: pProfile->vMegerlabmap[ncamid].values(str)){
            if(pProfile->blabshowmap.contains(var)&&
                pProfile->blabshowmap[var]){
                bshow = false;
                break;
            }
        }
    }
    return bshow;
}

/*!
 * \brief 重置全局共享的统计数据结构
 */
void GlobalFlow::ResetGlobalStaParams()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    int nNum  = pProfile->nStaticGroup;

    struGsh.socLineinfomap.clear();

    for(int i=0; i<nNum; i++){
        if(pProfile->vUnitaddrlist[i].size()>1){//! 重置合并相机
            for(QString idxstr : pProfile->vUnitaddrlist[i]){
                struGsh.socLineinfomap[idxstr.toInt()] = 0;
            }
        }
    }
}

/*!
 * \brief 重置合并标签lst
 */
void GlobalFlow::ResetMegreLabLst(int nCamID)
{
    QString merlab;
    QStringList strlst;
    QList<QString> lablst = struGsh.vlabNamemap[nCamID].values();
    for(QString str : lablst){
        merlab = getMegerlabKeyByValue(str,nCamID);
        if(merlab.isEmpty()){
            strlst<<str;
        }else{
            if(!strlst.contains(merlab))
                strlst<<merlab;
        }
    }

    struGsh.vMegreLablst[nCamID].clear();
    struGsh.vMegreLablst[nCamID] = strlst;
}

/*!
 * \brief 重置标签的序号
 * \param nCamId
 */
void GlobalFlow::ResetLabIdx(int nCamId)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    pProfile->vlabIdxmap[nCamId].clear();

    int nIdx = 0;
    QStringList lablst = struGsh.vMegreLablst[nCamId];
    for(QString str : lablst){
        if(pProfile->vMegerlabmap[nCamId].contains(str)){
            for(QString &child : pProfile->vMegerlabmap[nCamId].values(str)){
                pProfile->vlabIdxmap[nCamId][child] = nIdx;
            }
        }else{
            pProfile->vlabIdxmap[nCamId][str] = nIdx;
        }
        nIdx++;
    }
}

/*!
 * \brief 设置系统时间
 * \param dateTime
 */
void GlobalFlow::setTime(QDateTime dateTime)
{
    SystemCmd(QString("date -s \"%1\"").arg(dateTime.toString("yyyy-MM-dd hh:mm:ss")));
    SystemCmd("hwclock -uw");
}

/*!
 * \brief 保存信息-测试使用
 * \param str
 */
void GlobalFlow::saveInfo(QString str)
{
    QString strLog = QString("%1: %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(str);
    qDebug("%s", strLog.toLatin1().data());

    QFile file(LOG_FILE);
    if (file.size() > MAX_FILE_SIZE) {
        file.remove();
    }

    if (file.open(QIODevice::Append)) {
        strLog += "\r\n";
        file.write(strLog.toLatin1());
        file.close();
    }
}

/*! 获取高级权限密码 */
QString GlobalFlow::getAdminPwd()
{
    QString pwd;
    if(1/*myLan.IsChineseLang()*/){
        QByteArray data = struCnfg.master_device_code.toLatin1();
        int nData[13];
        for(int i = 0 ; i < 13; ++i) {
            nData[i] = (int)data[i];
        }

        int tmp = 0;

        //! 第一位 前7位数字和取个位数
        for (int i = 0; i < 7; i++) {
            tmp += (nData[i]-48);
        }

        //1、高级权限旧密码
        int nAdminPwd[6];
        nAdminPwd[0] = tmp % 10;
        tmp = 0;
        //! 第二位 6-7位相加取个位
        tmp = nData[5] + nData[6] - 48*2;
        nAdminPwd[1] = tmp % 10;
        tmp = 0;
        //! 第三位 8-9-12-13位相加取个位
        tmp = nData[7] + nData[8] + nData[11] + nData[12] - 48*4;
        nAdminPwd[2] = tmp % 10;
        tmp = 0;
        //! 第四位 10-11-13位相加取个位
        tmp = nData[9] + nData[10] + nData[12] - 48*3;
        nAdminPwd[3] = tmp % 10;
        tmp = 0;
        //! 第五位 10-11-13位相加取个位
        tmp = nData[9] + nData[10] + nData[11] + nData[12] - 48*4;
        nAdminPwd[4] = tmp % 10;
        tmp = 0;
        //! 第六位 后6位相加取个位
        for (int i = 7; i < 13; i++) {
            tmp += (nData[i]-48);
        }
        nAdminPwd[5] = tmp % 10;

        //2、高级权限新密码
        QDateTime  dateTime = QDateTime::currentDateTime();
        QString dateStr = dateTime.toString("yyMMdd");

        int newPwd[6];
        for(int j=0; j<6; j++){
            newPwd[j] = (((dateStr[j]).toLatin1() - '0') + nAdminPwd[j])%10;
        }
        pwd = QString("%1%2%3%4%5%6").arg(newPwd[0]).arg(newPwd[1]).arg(newPwd[2]).arg(newPwd[3]).arg(newPwd[4]).arg(newPwd[5]);
    } else {
        pwd = "002690";
    }

    return pwd;
}

QString GlobalFlow::getCheckCode(int moduleType)
{
#ifdef Q_OS_WIN
   return "";
#else
   QString input = QString("%1%2%3").arg(struCnfg.master_device_code)
                                    .arg(QDateTime::currentDateTime().toString("yyyyMMdd") + QString("000000"))
                                    .arg("002690");
   QCryptographicHash md(QCryptographicHash::Md5);
   md.addData(input.toLatin1());
   QString md5 = md.result().toHex().data();
   if (md5.size() < 32) {
       // 异常
       return "error";
   }
   int module = moduleType;
   auto getOneValue = [md5, module](int a, int b, int c, int d)->int {
       return ((int)md5[a].unicode() + (int)md5[b].unicode() + (int)md5[c].unicode() + (int)md5[d].unicode() + module)%10;
   };

   int dd0 = getOneValue(5, 10, 25, 30);
   int dd1 = getOneValue(3, 8, 15, 27);
   int dd2 = getOneValue(6, 9, 16, 28);
   int dd3 = getOneValue(12, 18, 22, 5);
   int dd4 = getOneValue(9, 13, 19, 21);
   int dd5 = getOneValue(20, 23, 24, 31);

   QString result = QString("%1%2%3%4%5%6").arg(dd0).arg(dd1).arg(dd2).arg(dd3).arg(dd4).arg(dd5);
   return result;
#endif  
}

int GlobalFlow::getHanNumFromString(QString str)
{
    int count = 0;
    QRegExp regex = QRegExp(QString("^[\u4E00-\u9FA5]{0,}$"));
    for(int i = 0; i < str.length(); i++) {
        if(regex.exactMatch(QString(str[i])))
            ++count;
    }
    return count;
}

QString GlobalFlow::joinStr(QString leftStr, QString rightStr, int maxLen)
{
    int len = (leftStr+rightStr).length()+getHanNumFromString(leftStr+rightStr);
    if(leftStr.contains("≤"))
        len++;

    QString tmp;
    if(maxLen > len){
        tmp = tmp.fill(' ', maxLen-len);
    } else {
        if(len>18){
            leftStr = leftStr.left(leftStr.length()-2)+" ";
        }
        tmp = " ";
    }
    tmp = leftStr + tmp + rightStr/*+"\n"*/;

    return tmp;
}

/*!
 * \brief 字符串打印预处理
 * \param str  第一列13 第二列7 第三列7
 * \return
 */
QString GlobalFlow::preProcessStr(QString str, int nMaxlen)
{
    QString printstr;
    int nChines = getHanNumFromString(str);
    int nlen = str.length() - nChines;
    int ntotal = 2*nChines + nlen;
    if( ntotal  > nMaxlen){//! 裁
        int noffset = 0, nSize =0;
        for(int i=0; i<str.length(); i++){
            int n = getHanNumFromString(QString(str[i]));
            if( n > 0){
                nSize +=2;
            }else{
                nSize +=1;
            }
            noffset +=1;
            if(nSize >= nMaxlen){
                break;
            }
        }
        printstr = str.left(noffset);
    }else{//! 补
        for(int i=0; i<nMaxlen - ntotal; i++){
            str.append(" ");
        }
        printstr = str;
    }
    return printstr;
}

void GlobalFlow::char2Hex(char cChar, uchar& rHex)
{
    if((cChar >= '0') && (cChar <= '9'))
    {
        rHex = cChar - '0' + 0x30;
    }
    else if((cChar >= 'A') && (cChar <= 'Z'))
    {
        rHex = cChar - 'A' + 0x41;
    }
    else if((cChar >= 'a') && (cChar <= 'z'))
    {
        rHex = cChar - 'a' + 0x61;
    }
}

void GlobalFlow::hex2Char(uchar rHex, char& rChar)
{
    if((rHex >= 0x30) && (rHex <= 0x39))
    {
        rChar = '0' + rHex - 0x30;
    }
    else if((rHex >= 0x41) && (rHex <= 0x5A))
    {
        rChar = 'A' + rHex - 0x41;
    }
    else if((rHex >= 0x61) && (rHex <= 0x7A))
    {
        rChar = 'a' + rHex - 0x61;
    }
}

void GlobalFlow::printLog(QString sLog)
{
    QFile file(LOG_FILE);
    QTextStream in(&file);
    if (!file.open(QIODevice::Append)) {
        qDebug("%s: %s open failed!", __FUNCTION__, file.fileName().toLatin1().data());
        return;
    }
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    in << date << " " << sLog << "\n";
    file.close();
}

/*!
 * \brief 根据相机地址获取统计组的id
 * \param nCamid
 * \return
 */
int GlobalFlow::getStaticIdfromCamid(int nCamid)
{
    int nGbid = -1;
    for(int i=0; i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist.size(); i++){
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist[i].contains(QString::number(nCamid))){
            nGbid = i;
            break;
        }
    }

    return nGbid;
}
/*!
 * \brief 重置标签优先级
 */
void GlobalFlow::ResetAIlabPri()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;


    for(int i=0; i<gFlow.getMachineCamNum();i++){
        pProfile->strctAI.vlabelidxmap[i].clear();
        int num = 0;
        if(i < struGsh.vlabNamemap.size()){
            foreach(QString val , struGsh.vlabNamemap[i]){
                pProfile->strctAI.vlabelidxmap[i][val] = num++;
            }
        }
    }
}

/*!
 * \brief 重置互斥标签标签
 */
void GlobalFlow::ResetAIlabMutex()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;


    for(int i=0; i<gFlow.getMachineCamNum();i++){
        pProfile->strctAI.vlabMutexmap[i].clear();
        int num = 0;
        if(i < struGsh.vlabNamemap.size()){
            foreach(QString val , struGsh.vlabNamemap[i]){
                pProfile->strctAI.vlabMutexmap[i][val] = num++;
            }
        }
    }
}

void GlobalFlow::validateLabMixMap(int nCamId)
{
    if(nCamId < 0 || nCamId >= struGsh.vlabNamemap.size()){
        return;
    }
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    if(pProfile->vlabMixmap.size() <= nCamId){
        pProfile->vlabMixmap.resize(gFlow.getMachineCamNum());
    }
    QList<QString> listValid = struGsh.vlabNamemap[nCamId].values();
    QMap<QString, QString> &mixmap = pProfile->vlabMixmap[nCamId];
    auto it = mixmap.begin();
    while (it != mixmap.end()) {
        QString origLab = it.key();
        QString mappedLab = it.value();
        if (!listValid.contains(origLab) || !listValid.contains(mappedLab) || origLab == mappedLab) {
            it.value() = QString("");
        }
        ++it;
    }
}


/*!
 * \brief 根据统计组的序号获取对应相机的AI标签
 * \param nStaId
 * \return
 */
QList<QString> GlobalFlow::getAIlabfromstagp(int nStaId)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    QList<QString> AIlablst;

    if(pProfile->vUnitaddrlist.size() > nStaId){
        if(pProfile->vUnitaddrlist[nStaId].size()>0){
            int ncamId = QString("%1").arg(pProfile->vUnitaddrlist[nStaId].at(0)).toInt();
            if(struGsh.vlabNamemap.size() > ncamId){
                AIlablst = struGsh.vlabNamemap[ncamId].values();
            }
        }
    }

    return AIlablst;
}

/*!
 * \brief QString 转 json
 * \param str
 * \return
 */
QJsonObject GlobalFlow::qstr2Json(const QString& str)
{
    QJsonParseError parserror;
    QJsonDocument jdoc = QJsonDocument::fromJson(str.toUtf8(), &parserror);

    if(parserror.error == QJsonParseError::NoError){
        if(jdoc.isObject()){
            return jdoc.object();
        }
    }

    return QJsonObject();
}

/*!
 * \brief GlobalFlow::getLabstacatNum
 * \param nCamid
 * \return
 */
int GlobalFlow::getLabstacatNum(int nCamid)
{
    int nNum = 0;
    int nGbid = getStaticIdfromCamid(nCamid);
    if(nGbid !=-1 && nGbid<struGsh.vlabStamap.size()){
        if(!struGsh.vlabStamap[nGbid].isEmpty()){
            QList<QString> lst = struGsh.vlabStamap[nGbid].first().staInfo.keys();
            nNum = lst.size();
        }
    }
    return nNum;
}

/*!
 * \brief GlobalFlow::getMatstacatNum
 * \return
 */
int GlobalFlow::getMatstacatNum(int nCamid)
{
    int nNum = 0;
    int nGbid = getStaticIdfromCamid(nCamid);
    if(nGbid !=-1 && nGbid<struGsh.vsMatinfo.size()){
        QList<QString> lst = struGsh.vsMatinfo[nGbid].staInfo.keys();
        nNum = lst.size();
    }
    return nNum;
}

bool GlobalFlow::copyDir(const QString &srcPath, const QString &dstPath)
{
    QDir srcDir(srcPath);
    QDir dstDir(dstPath);

    if(!srcDir.exists()){
        qDebug() << "srcPath not exists:" << srcPath;
        return false;
    }

    // 如果目标目录不存在，则创建
    if (!dstDir.exists()) {
        if (!dstDir.mkpath(dstPath)) {
            qDebug() << "Failed to create dstPath:" << dstPath;
            return false;
        }
    } else {
        // 如果目标目录存在，删除其下所有文件和文件夹
        QFileInfoList dstFileList = dstDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
        for (const QFileInfo &info : dstFileList) {
            if (info.isDir()) {
                QDir(info.filePath()).removeRecursively();
            } else {
                QFile::remove(info.filePath());
            }
        }
    }

    // 拷贝源目录内容到目标目录
    QFileInfoList fileList = srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo &info : fileList) {
        QString dstFilePath = dstPath + "/" + info.fileName();
        if (info.isDir()) {
            if (!copyDir(info.filePath(), dstFilePath)) return false;
        } else {
            if (!QFile::copy(info.filePath(), dstFilePath)) return false;
        }
    }
    return true;
}

/*!
 * \brief 写入数据库
 * \fn
 */
void GlobalFlow::recordStatusToDB()
{
    if(gFlow.getDirSize("/ssd")== -1 || gFlow.getDirSize("/ssd")/1024/1024 > 800){
        return;
    }

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const bool isMergerColorMat = (struCnfg.nMachineType == nMachine_Bottle) &&
                                  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bMergerColorMaterial &&
                                  (pProfile->nStaticGroup >= 2) &&
                                  (pProfile->vUnitaddrlist.size() >= pProfile->nStaticGroup);
    if(isMergerColorMat){
        recordStatusToDBMerger();
        return;
    }

    const bool isBottleType = (struCnfg.nMachineType == nMachine_Bottle);
    bool bPercent = pProfile->nStaisticType ==1 ? true:false;
    int  nResult = 0;

    QJsonObject jsonobj;
    QJsonArray  jsonarray;
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sSchcode = struCnfg.schemeList[struCnfg.nCntScheme].code;
    QString batchname = struGsh.batchname;
    if(batchname.contains("%")){
        batchname.replace("%","%*");
    }

    for(int i=0; i<pProfile->nStaticGroup; i++){
        const auto &matInfo = struGsh.vsMatinfo[i];
        const auto &matTotalMap = struGsh.vmatTotalmap[i];

        int ncamid = QString("%1").arg(pProfile->vUnitaddrlist[i].at(0)).toInt();
        QStringList AIlablst = struGsh.vMegreLablst[ncamid];

        if(AIlablst.empty()) continue;

        //! 瓶片增加预遍历
        float nDiff = 0;
        QString strmaxvalchild;
        if(isBottleType) {
            float maxRes = 0.0;
            float totalValue = 0;
            for(const QString &var : AIlablst) {
                QStringList childlst;
                if(pProfile->vMegerlabmap[ncamid].contains(var)) {
                    childlst = pProfile->vMegerlabmap[ncamid].values(var);
                }

                if(childlst.isEmpty()) {
                    childlst << var;
                }

                //! 遍历统计类别
                const QStringList &catlst = struGsh.vlabStamap[i][childlst.at(0)].staRes.keys();
                for(int k = 0; k < catlst.size(); k++) {
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
                            if(bPercent){
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
            if(totalValue >0){
                if(bPercent){
                    nDiff = QString::number(1.0 - totalValue,'f',4).toFloat();
                } else {
                    nDiff = (int)(1000000.0 - totalValue);
                }
            }
        }


        //! 统计标签结果
        QJsonObject dataobj;
        QJsonArray  labjsonarray;
        for(QString Merlabstr : AIlablst){
            QJsonArray  toparray;
            QJsonObject toplabjson;
            bool btopstat = false;

            QStringList childlst;
            if(pProfile->vMegerlabmap[ncamid].contains(Merlabstr)){
                childlst = pProfile->vMegerlabmap[ncamid].values(Merlabstr);
            }

            QString labname = Merlabstr;
            if(!childlst.isEmpty()){
                labname = childlst.at(0);
            }

            QStringList catlst = struGsh.vlabStamap[i][labname].staRes.keys();  //! 统计类别

            for(QString child :childlst){
                bool bsta = false;
                QJsonObject childjson;
                childjson["name"] = child;
                for(QString stacat : catlst){
                    if(pProfile->staclassEnmap.contains(stacat) &&
                        pProfile->staclassEnmap[stacat]){
                        childjson[stacat] = getLabStaResult(i,child,stacat,bsta,nDiff,strmaxvalchild);
                    }
                }
                childjson["stat"] = bsta;
                childjson["child"] = QJsonArray();
                toparray.append(childjson);
                if(bsta){
                    nResult = 1;
                }
            }

            toplabjson["name"] = Merlabstr;
            for(QString stacat : catlst){
                if(pProfile->staclassEnmap.contains(stacat) &&
                    pProfile->staclassEnmap[stacat]){
                    toplabjson[stacat] = getLabStaResult(i,Merlabstr,stacat,btopstat,nDiff,strmaxvalchild);
                }
            }
            toplabjson["stat"] = btopstat;
            toplabjson["child"] = toparray;
            if(btopstat){
                nResult = 1;
            }

            labjsonarray.append(toplabjson);
        }

        //! 统计物料
        QJsonObject objmat;
        for(QString stacat : struGsh.vsMatinfo[i].staInfo.keys()){
            QString statype = struGsh.vsMatinfo[i].staInfo[stacat];
            QString tmpstr = statype;
            QStringList stainfolst = tmpstr.split("/");
            if(tmpstr.isEmpty() || stainfolst.size()<2){
                break;
            }
            int nstatype = stainfolst[1].mid(1).toInt();
            QString valStr;
            float nres = 0.0;
            switch (nstatype) {
            case STATISTIC_A1://! 直接用
                nres = struGsh.vsMatinfo[i].staRes[stacat][0];
                valStr = QString("%1").arg((int)nres);
                break;
            case STATISTIC_A2:
                break;
            case STATISTIC_A3://! 乘系数 算占比
                break;
            case STATISTIC_A4://! 直接算占比
                if(struGsh.vmatTotalmap[i][statype] > 0){
                    nres = struGsh.vsMatinfo[i].staRes[stacat][0]/
                           struGsh.vmatTotalmap[i][statype];
                }
                valStr = QString("%1%").arg(QString::number(nres*100,'f',2));
                break;
            case STATISTIC_A5:
            case STATISTIC_A6:
                if(struGsh.vmatTotalmap[i][statype] > 0){
                    nres = struGsh.vsMatinfo[i].staRes[stacat][0]/
                           struGsh.vmatTotalmap[i][statype];
                    valStr = QString("%1").arg(QString::number(nres,'f',2));
                }
                break;
            case STATISTIC_A7:
                valStr = QString("%1   %2   %3   %4")
                             .arg((int)matInfo.staRes[stacat][0])
                             .arg((int)matInfo.staRes[stacat][1])
                             .arg((int)matInfo.staRes[stacat][2])
                             .arg((int)matInfo.staRes[stacat][3]);
                break;
            case STATISTIC_A8:
            case STATISTIC_A10:
            {
                const float totalValue = matTotalMap.value(statype, 0.0f);
                if(totalValue > 0.0f){
                    const float nVal =  matInfo.staRes[stacat][0] / totalValue;
                    const float nVal1 = matInfo.staRes[stacat][1] / totalValue;
                    const float nVal2 = matInfo.staRes[stacat][2] / totalValue;

                    valStr = QString("%1%   %2%   %3%")
                                 .arg(QString::number(nVal*100, 'f', 2))
                                 .arg(QString::number(nVal1*100, 'f', 2))
                                 .arg(QString::number(nVal2*100, 'f', 2));
                }
            }
            break;
            default:
                break;
            }
            objmat[stacat] = valStr;
        }

        dataobj[QString("stagp_%1_lab").arg(i)] = labjsonarray;
        dataobj[QString("stagp_%1_mat").arg(i)] = objmat;

        jsonarray.append(dataobj);
    }


    jsonobj["date"] = date;
    jsonobj["batch"] = batchname;
    jsonobj["dutyshift"] =struGsh.duty_shift;
    jsonobj["data"] = jsonarray;

    //! 金属数据表
    if(struCnfg.nMachineType == nMachine_Bottle &&
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect){
        bool bover = false;
        if(struGsh.nMetaNum > pProfile->nMeatal){
            bover = true;
        }
        jsonobj["metal"] = struGsh.nMetaNum;
        jsonobj["metalsta"] = bover;
    }

    QJsonDocument doc(jsonobj);
    QString querycmd = doc.toJson(QJsonDocument::Compact);

    QString cmd = QString("INSERT INTO '%1' VALUES('%2', '%3', '%4', %5, %6,'%7','%8')")
                      .arg(sSchcode).arg(date)
                      .arg(batchname).arg(struGsh.duty_shift)
                      .arg(nResult).arg(struGsh.detect_yield)
                      .arg(struCnfg.schemeList[struCnfg.nCntScheme].name).arg(querycmd);
                      
    g_sqliteInter->executeCmd(cmd);

}

/*!
 * \brief 颜色材质融合模式写入数据库
 * \fn    最后一个材质统计组数据不需要写入，融合到组0中
 */
void GlobalFlow::recordStatusToDBMerger()
{
    if(gFlow.getDirSize("/ssd") == -1 || gFlow.getDirSize("/ssd") / 1024 / 1024 > 800){
        return;
    }

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const int nStaticGroup = pProfile->nStaticGroup;
    if(nStaticGroup < 2 || pProfile->vUnitaddrlist.size() < nStaticGroup ||
       pProfile->vUnitaddrlist[0].isEmpty()){
        return;
    }

    const int lastGpId = nStaticGroup - 1;
    if(pProfile->vUnitaddrlist[lastGpId].isEmpty()){
        return;
    }

    const int colorCamId = QString("%1").arg(pProfile->vUnitaddrlist[0].at(0)).toInt();
    const int matCamId = QString("%1").arg(pProfile->vUnitaddrlist[lastGpId].at(0)).toInt();
    const QString secondaryLab = pProfile->slabSecondary[matCamId].trimmed();
    if(secondaryLab.isEmpty()){
        return;
    }

    const bool isBottleType = (struCnfg.nMachineType == nMachine_Bottle);
    const bool bPercent = (pProfile->nStaisticType == 1);
    int nResult = 0;

    QJsonObject jsonobj;
    QJsonArray  jsonarray;
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sSchcode = struCnfg.schemeList[struCnfg.nCntScheme].code;
    QString batchname = struGsh.batchname;
    if(batchname.contains("%")){
        batchname.replace("%", "%*");
    }

    // 1. 计算红外材质组次一级标签占比 secRatioMap
    QStringList secSubLabs;
    if(pProfile->vMegerlabmap[matCamId].contains(secondaryLab)){
        secSubLabs = pProfile->vMegerlabmap[matCamId].values(secondaryLab);
    }
    if(secSubLabs.isEmpty()){
        secSubLabs << secondaryLab;
    }

    if(struGsh.vlabStamap[0].isEmpty()){
        return;
    }

    const QStringList &catlst = struGsh.vlabStamap[0].first().staRes.keys();
    QMap<QString, float> secRatioMap;
    for(const QString &cat : catlst){
        float secRatio = 0.0f;
        for(const QString &subName : secSubLabs){
            if(!struGsh.vlabStamap[lastGpId].contains(subName)) continue;
            const QString &infoStr = struGsh.vlabStamap[lastGpId][subName].staInfo.value(cat);
            if(infoStr.isEmpty()) continue;
            if(struGsh.vlabTotalmap[lastGpId].value(infoStr) > 0){
                secRatio += struGsh.vlabStamap[lastGpId][subName].staRes[cat][0] /
                            struGsh.vlabTotalmap[lastGpId][infoStr];
            }
        }
        secRatioMap[cat] = secRatio;
    }

    const QStringList &colorLabList = struGsh.vMegreLablst[colorCamId];
    const QStringList &matLabList = struGsh.vMegreLablst[matCamId];

    //! 计算可见组与剩余材质组的原始占比总和，推导剩余材质缩放系数 matScaleMap
    QMap<QString, float> visRawSumMap;
    QMap<QString, float> matOtherRawSumMap;

    for(const QString &var : colorLabList){
        bool bshow = true;
        if(pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]){
            bshow = false;
        } else if(!bMegerlabshow(var, colorCamId)){
            bshow = false;
        }
        if(!bshow) continue;

        QStringList childlst;
        if(pProfile->vMegerlabmap[colorCamId].contains(var)){
            childlst = pProfile->vMegerlabmap[colorCamId].values(var);
        }
        if(childlst.isEmpty()){
            childlst << var;
        }

        for(const QString &cat : catlst){
            for(const QString &childname : childlst){
                if(!struGsh.vlabStamap[0].contains(childname)) continue;
                const QString &str = struGsh.vlabStamap[0][childname].staInfo.value(cat);
                if(str.isEmpty()) continue;
                const QStringList strParts = str.split("/");
                if(strParts.size() < 2) continue;
                const int statype = strParts.at(1).mid(1).toInt();
                if(statype != STATISTIC_A3 && statype != STATISTIC_A4) continue;

                if(struGsh.vlabTotalmap[0].value(str) > 0){
                    visRawSumMap[cat] += struGsh.vlabStamap[0][childname].staRes[cat][0] /
                                         struGsh.vlabTotalmap[0][str];
                }
            }
        }
    }

    for(const QString &var : matLabList){
        if(var == secondaryLab) continue;
        if(pProfile->vMegerlabmap[matCamId].contains(var) &&
           pProfile->vMegerlabmap[matCamId].values(var).contains(secondaryLab)){
            continue;
        }

        bool bshow = true;
        if(pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]){
            bshow = false;
        } else if(!bMegerlabshow(var, matCamId)){
            bshow = false;
        }
        if(!bshow) continue;

        QStringList childlst;
        if(pProfile->vMegerlabmap[matCamId].contains(var)){
            childlst = pProfile->vMegerlabmap[matCamId].values(var);
        }
        if(childlst.isEmpty()){
            childlst << var;
        }

        for(const QString &cat : catlst){
            for(const QString &childname : childlst){
                if(childname == secondaryLab) continue;
                if(!struGsh.vlabStamap[lastGpId].contains(childname)) continue;
                const QString &str = struGsh.vlabStamap[lastGpId][childname].staInfo.value(cat);
                if(str.isEmpty()) continue;
                const QStringList strParts = str.split("/");
                if(strParts.size() < 2) continue;
                const int statype = strParts.at(1).mid(1).toInt();
                if(statype != STATISTIC_A3 && statype != STATISTIC_A4) continue;

                if(struGsh.vlabTotalmap[lastGpId].value(str) > 0){
                    matOtherRawSumMap[cat] += struGsh.vlabStamap[lastGpId][childname].staRes[cat][0] /
                                              struGsh.vlabTotalmap[lastGpId][str];
                }
            }
        }
    }

    QMap<QString, float> matScaleMap;
    for(const QString &cat : catlst){
        const float rRatio = secRatioMap.value(cat, 0.0f);
        const float sVis = visRawSumMap.value(cat, 0.0f);
        const float sMatOther = matOtherRawSumMap.value(cat, 0.0f);
        const float rRemain = qMax(0.0f, 1.0f - sVis * rRatio);
        matScaleMap[cat] = (sMatOther > 0.0f) ? (rRemain / sMatOther) : 0.0f;
    }

    // 2. 遍历统计组：排除最后一个材质组 lastGpId（不写入数据库）
    for(int i = 0; i < lastGpId; i++){
        QJsonObject dataobj;
        QJsonArray  labjsonarray;

        if(i == 0){
            // ---------------- 统计组 0：融合颜色与材质 ----------------
            // 2.1 针对每个统计类别独立预遍历求差额 nDiffMap 与最大标签项 maxValChildMap (针对 STATISTIC_A3)
            QMap<QString, float> nDiffMap;
            QMap<QString, QString> maxValChildMap;

            for(const QString &cat : catlst){
                const float rRatio = secRatioMap.value(cat, 0.0f);
                const float scale = matScaleMap.value(cat, 0.0f);
                float totalValue = 0.0f;
                float maxRes = -1.0f;
                QString strmaxchild;

                // A. 颜色项子项预遍历
                for(const QString &var : colorLabList){
                    bool bshow = true;
                    if(pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]){
                        bshow = false;
                    } else if(!bMegerlabshow(var, colorCamId)){
                        bshow = false;
                    }
                    if(!bshow) continue;

                    QStringList childlst;
                    if(pProfile->vMegerlabmap[colorCamId].contains(var)){
                        childlst = pProfile->vMegerlabmap[colorCamId].values(var);
                    }
                    if(childlst.isEmpty()){
                        childlst << var;
                    }

                    for(const QString &childname : childlst){
                        if(!struGsh.vlabStamap[0].contains(childname)) continue;
                        const QString &str = struGsh.vlabStamap[0][childname].staInfo.value(cat);
                        if(str.isEmpty()) continue;
                        const QStringList strParts = str.split("/");
                        if(strParts.size() < 2) continue;
                        const int statype = strParts.at(1).mid(1).toInt();
                        if(statype != STATISTIC_A3) continue;

                        if(struGsh.vlabTotalmap[0].value(str) > 0){
                            float nres = (struGsh.vlabStamap[0][childname].staRes[cat][0] /
                                          struGsh.vlabTotalmap[0][str]) * rRatio;
                            float nVal = 0.0f;
                            if(bPercent){
                                nVal = QString::number(nres, 'f', 4).toFloat();
                            } else {
                                nVal = (int)(nres * 1000000);
                            }
                            totalValue += nVal;
                            if(nVal > maxRes){
                                maxRes = nVal;
                                strmaxchild = childname;
                            }
                        }
                    }
                }

                // B. 材质项子项预遍历（排除次一级标签）
                for(const QString &var : matLabList){
                    if(var == secondaryLab) continue;
                    if(pProfile->vMegerlabmap[matCamId].contains(var) &&
                       pProfile->vMegerlabmap[matCamId].values(var).contains(secondaryLab)){
                        continue;
                    }

                    bool bshow = true;
                    if(pProfile->blabshowmap.contains(var) && pProfile->blabshowmap[var]){
                        bshow = false;
                    } else if(!bMegerlabshow(var, matCamId)){
                        bshow = false;
                    }
                    if(!bshow) continue;

                    QStringList childlst;
                    if(pProfile->vMegerlabmap[matCamId].contains(var)){
                        childlst = pProfile->vMegerlabmap[matCamId].values(var);
                    }
                    if(childlst.isEmpty()){
                        childlst << var;
                    }

                    for(const QString &childname : childlst){
                        if(childname == secondaryLab) continue;
                        if(!struGsh.vlabStamap[lastGpId].contains(childname)) continue;
                        const QString &str = struGsh.vlabStamap[lastGpId][childname].staInfo.value(cat);
                        if(str.isEmpty()) continue;
                        const QStringList strParts = str.split("/");
                        if(strParts.size() < 2) continue;
                        const int statype = strParts.at(1).mid(1).toInt();
                        if(statype != STATISTIC_A3) continue;

                        if(struGsh.vlabTotalmap[lastGpId].value(str) > 0){
                            float nres = (struGsh.vlabStamap[lastGpId][childname].staRes[cat][0] /
                                         struGsh.vlabTotalmap[lastGpId][str]) * scale;
                            float nVal = 0.0f;
                            if(bPercent){
                                nVal = QString::number(nres, 'f', 4).toFloat();
                            } else {
                                nVal = (int)(nres * 1000000);
                            }
                            totalValue += nVal;
                            if(nVal > maxRes){
                                maxRes = nVal;
                                strmaxchild = childname;
                            }
                        }
                    }
                }

                if(totalValue > 0){
                    if(bPercent){
                        nDiffMap[cat] = QString::number(1.0 - totalValue, 'f', 4).toFloat();
                    } else {
                        nDiffMap[cat] = (int)(1000000.0 - totalValue);
                    }
                    maxValChildMap[cat] = strmaxchild;
                }
            }

            // 2.2 构建颜色标签数据（名称格式：次一级标签名称-颜色标签名称）
            for(const QString &Merlabstr : colorLabList){
                bool bshow = true;
                if(pProfile->blabshowmap.contains(Merlabstr) && pProfile->blabshowmap[Merlabstr]){
                    bshow = false;
                } else if(!bMegerlabshow(Merlabstr, colorCamId)){
                    bshow = false;
                }
                if(!bshow) continue;

                QJsonArray toparray;
                QJsonObject toplabjson;
                bool btopstat = false;

                QStringList childlst;
                if(pProfile->vMegerlabmap[colorCamId].contains(Merlabstr)){
                    childlst = pProfile->vMegerlabmap[colorCamId].values(Merlabstr);
                }

                // 子项
                for(const QString &child : childlst){
                    bool bsta = false;
                    QJsonObject childjson;
                    childjson["name"] = QString("%1-%2").arg(secondaryLab, child);
                    for(const QString &stacat : catlst){
                        if(pProfile->staclassEnmap.contains(stacat) &&
                           pProfile->staclassEnmap[stacat]){
                            childjson[stacat] = getLabStaResultMerger(0, child, stacat,
                                                                      secRatioMap.value(stacat, 0.0f),
                                                                      bsta, nDiffMap.value(stacat, 0.0f),
                                                                      maxValChildMap.value(stacat));
                        }
                    }
                    childjson["stat"] = bsta;
                    childjson["child"] = QJsonArray();
                    toparray.append(childjson);
                    if(bsta){
                        nResult = 1;
                    }
                }

                // 父项
                toplabjson["name"] = QString("%1-%2").arg(secondaryLab, Merlabstr);
                for(const QString &stacat : catlst){
                    if(pProfile->staclassEnmap.contains(stacat) &&
                       pProfile->staclassEnmap[stacat]){
                        toplabjson[stacat] = getLabStaResultMerger(0, Merlabstr, stacat,
                                                                  secRatioMap.value(stacat, 0.0f),
                                                                  btopstat, nDiffMap.value(stacat, 0.0f),
                                                                  maxValChildMap.value(stacat));
                    }
                }
                toplabjson["stat"] = btopstat;
                toplabjson["child"] = toparray;
                if(btopstat){
                    nResult = 1;
                }

                labjsonarray.append(toplabjson);
            }

            // 2.3 构建剩余材质标签数据（名称为材质原名）
            for(const QString &Merlabstr : matLabList){
                if(Merlabstr == secondaryLab) continue;
                if(pProfile->vMegerlabmap[matCamId].contains(Merlabstr) &&
                   pProfile->vMegerlabmap[matCamId].values(Merlabstr).contains(secondaryLab)){
                    continue;
                }

                bool bshow = true;
                if(pProfile->blabshowmap.contains(Merlabstr) && pProfile->blabshowmap[Merlabstr]){
                    bshow = false;
                } else if(!bMegerlabshow(Merlabstr, matCamId)){
                    bshow = false;
                }
                if(!bshow) continue;

                QJsonArray toparray;
                QJsonObject toplabjson;
                bool btopstat = false;

                QStringList childlst;
                if(pProfile->vMegerlabmap[matCamId].contains(Merlabstr)){
                    childlst = pProfile->vMegerlabmap[matCamId].values(Merlabstr);
                }

                // 子项
                for(const QString &child : childlst){
                    if(child == secondaryLab) continue;
                    bool bsta = false;
                    QJsonObject childjson;
                    childjson["name"] = child;
                    for(const QString &stacat : catlst){
                        if(pProfile->staclassEnmap.contains(stacat) &&
                           pProfile->staclassEnmap[stacat]){
                            childjson[stacat] = getLabStaResultMerger(lastGpId, child, stacat,
                                                                      matScaleMap.value(stacat, 0.0f),
                                                                      bsta, nDiffMap.value(stacat, 0.0f),
                                                                      maxValChildMap.value(stacat));
                        }
                    }
                    childjson["stat"] = bsta;
                    childjson["child"] = QJsonArray();
                    toparray.append(childjson);
                    if(bsta){
                        nResult = 1;
                    }
                }

                // 父项
                toplabjson["name"] = Merlabstr;
                for(const QString &stacat : catlst){
                    if(pProfile->staclassEnmap.contains(stacat) &&
                       pProfile->staclassEnmap[stacat]){
                        toplabjson[stacat] = getLabStaResultMerger(lastGpId, Merlabstr, stacat,
                                                                  matScaleMap.value(stacat, 0.0f),
                                                                  btopstat, nDiffMap.value(stacat, 0.0f),
                                                                  maxValChildMap.value(stacat));
                    }
                }
                toplabjson["stat"] = btopstat;
                toplabjson["child"] = toparray;
                if(btopstat){
                    nResult = 1;
                }

                labjsonarray.append(toplabjson);
            }

            // 2.4 物料统计（组 0）
            QJsonObject objmat;
            for(const QString &stacat : struGsh.vsMatinfo[0].staInfo.keys()){
                const QString statype = struGsh.vsMatinfo[0].staInfo[stacat];
                const QStringList stainfolst = statype.split("/");
                if(statype.isEmpty() || stainfolst.size() < 2){
                    break;
                }
                const int nstatype = stainfolst[1].mid(1).toInt();
                QString valStr;
                float nres = 0.0f;
                switch (nstatype) {
                case STATISTIC_A1:
                    nres = struGsh.vsMatinfo[0].staRes[stacat][0];
                    valStr = QString("%1").arg((int)nres);
                    break;
                case STATISTIC_A2:
                case STATISTIC_A3:
                    break;
                case STATISTIC_A4:
                    if(struGsh.vmatTotalmap[0].value(statype) > 0){
                        nres = struGsh.vsMatinfo[0].staRes[stacat][0] /
                               struGsh.vmatTotalmap[0][statype];
                    }
                    valStr = QString("%1%").arg(QString::number(nres * 100, 'f', 2));
                    break;
                case STATISTIC_A5:
                case STATISTIC_A6:
                    if(struGsh.vmatTotalmap[0].value(statype) > 0){
                        nres = struGsh.vsMatinfo[0].staRes[stacat][0] /
                               struGsh.vmatTotalmap[0][statype];
                        valStr = QString("%1").arg(QString::number(nres, 'f', 2));
                    }
                    break;
                case STATISTIC_A7:
                    valStr = QString("%1   %2   %3   %4")
                                 .arg((int)struGsh.vsMatinfo[0].staRes[stacat][0])
                                 .arg((int)struGsh.vsMatinfo[0].staRes[stacat][1])
                                 .arg((int)struGsh.vsMatinfo[0].staRes[stacat][2])
                                 .arg((int)struGsh.vsMatinfo[0].staRes[stacat][3]);
                    break;
                case STATISTIC_A8:
                case STATISTIC_A10: {
                    const float totalValue = struGsh.vmatTotalmap[0].value(statype, 0.0f);
                    if(totalValue > 0.0f){
                        const float nVal = struGsh.vsMatinfo[0].staRes[stacat][0] / totalValue;
                        const float nVal1 = struGsh.vsMatinfo[0].staRes[stacat][1] / totalValue;
                        const float nVal2 = struGsh.vsMatinfo[0].staRes[stacat][2] / totalValue;
                        valStr = QString("%1%   %2%   %3%")
                                     .arg(QString::number(nVal * 100, 'f', 2))
                                     .arg(QString::number(nVal1 * 100, 'f', 2))
                                     .arg(QString::number(nVal2 * 100, 'f', 2));
                    }
                    break;
                }
                default:
                    break;
                }
                objmat[stacat] = valStr;
            }

            dataobj[QString("stagp_%1_lab").arg(0)] = labjsonarray;
            dataobj[QString("stagp_%1_mat").arg(0)] = objmat;
            jsonarray.append(dataobj);

        } else {
            // 中间统计组按原有逻辑写入
            const auto &matInfo = struGsh.vsMatinfo[i];
            const auto &matTotalMap = struGsh.vmatTotalmap[i];
            int ncamid = QString("%1").arg(pProfile->vUnitaddrlist[i].at(0)).toInt();
            QStringList AIlablst = struGsh.vMegreLablst[ncamid];
            if(AIlablst.empty()) continue;

            float nDiffMid = 0;
            QString strmaxvalchildMid;
            if(isBottleType) {
                float maxResMid = 0.0;
                float totalValueMid = 0;
                for(const QString &var : AIlablst) {
                    QStringList childlst;
                    if(pProfile->vMegerlabmap[ncamid].contains(var)) {
                        childlst = pProfile->vMegerlabmap[ncamid].values(var);
                    }
                    if(childlst.isEmpty()) {
                        childlst << var;
                    }
                    const QStringList &midCatlst = struGsh.vlabStamap[i][childlst.at(0)].staRes.keys();
                    for(int k = 0; k < midCatlst.size(); k++) {
                        for(const QString &childname : childlst) {
                            const QString &str = struGsh.vlabStamap[i][childname].staInfo[midCatlst.at(k)];
                            if(str.isEmpty()) continue;
                            const QStringList &strParts = str.split("/");
                            if(strParts.size() < 2) continue;
                            const int statype = strParts.at(1).mid(1).toInt();
                            if(statype != STATISTIC_A3) continue;
                            if(struGsh.vlabTotalmap[i][str] > 0){
                                float nres = struGsh.vlabStamap[i][childname].staRes[midCatlst.at(k)][0] /
                                             struGsh.vlabTotalmap[i][str];
                                float nVal;
                                if(bPercent){
                                    nVal = QString::number(nres, 'f', 4).toFloat();
                                } else {
                                    nVal = (int)(nres * 1000000);
                                }
                                totalValueMid += nVal;
                                if(nVal > maxResMid){
                                    maxResMid = nVal;
                                    strmaxvalchildMid = childname;
                                }
                            }
                        }
                    }
                }
                if(totalValueMid > 0){
                    if(bPercent){
                        nDiffMid = QString::number(1.0 - totalValueMid, 'f', 4).toFloat();
                    } else {
                        nDiffMid = (int)(1000000.0 - totalValueMid);
                    }
                }
            }

            for(QString Merlabstr : AIlablst){
                QJsonArray toparray;
                QJsonObject toplabjson;
                bool btopstat = false;
                QStringList childlst;
                if(pProfile->vMegerlabmap[ncamid].contains(Merlabstr)){
                    childlst = pProfile->vMegerlabmap[ncamid].values(Merlabstr);
                }
                QString labname = Merlabstr;
                if(!childlst.isEmpty()){
                    labname = childlst.at(0);
                }
                QStringList midCatlst = struGsh.vlabStamap[i][labname].staRes.keys();
                for(QString child : childlst){
                    bool bsta = false;
                    QJsonObject childjson;
                    childjson["name"] = child;
                    for(QString stacat : midCatlst){
                        if(pProfile->staclassEnmap.contains(stacat) &&
                           pProfile->staclassEnmap[stacat]){
                            childjson[stacat] = getLabStaResult(i, child, stacat, bsta, nDiffMid, strmaxvalchildMid);
                        }
                    }
                    childjson["stat"] = bsta;
                    childjson["child"] = QJsonArray();
                    toparray.append(childjson);
                    if(bsta){
                        nResult = 1;
                    }
                }

                toplabjson["name"] = Merlabstr;
                for(QString stacat : midCatlst){
                    if(pProfile->staclassEnmap.contains(stacat) &&
                       pProfile->staclassEnmap[stacat]){
                        toplabjson[stacat] = getLabStaResult(i, Merlabstr, stacat, btopstat, nDiffMid, strmaxvalchildMid);
                    }
                }
                toplabjson["stat"] = btopstat;
                toplabjson["child"] = toparray;
                if(btopstat){
                    nResult = 1;
                }
                labjsonarray.append(toplabjson);
            }

            QJsonObject objmat;
            for(QString stacat : struGsh.vsMatinfo[i].staInfo.keys()){
                QString statype = struGsh.vsMatinfo[i].staInfo[stacat];
                QString tmpstr = statype;
                QStringList stainfolst = tmpstr.split("/");
                if(tmpstr.isEmpty() || stainfolst.size() < 2){
                    break;
                }
                int nstatype = stainfolst[1].mid(1).toInt();
                QString valStr;
                float nres = 0.0;
                switch (nstatype) {
                case STATISTIC_A1:
                    nres = struGsh.vsMatinfo[i].staRes[stacat][0];
                    valStr = QString("%1").arg((int)nres);
                    break;
                case STATISTIC_A2:
                case STATISTIC_A3:
                    break;
                case STATISTIC_A4:
                    if(struGsh.vmatTotalmap[i][statype] > 0){
                        nres = struGsh.vsMatinfo[i].staRes[stacat][0] /
                               struGsh.vmatTotalmap[i][statype];
                    }
                    valStr = QString("%1%").arg(QString::number(nres * 100, 'f', 2));
                    break;
                case STATISTIC_A5:
                case STATISTIC_A6:
                    if(struGsh.vmatTotalmap[i][statype] > 0){
                        nres = struGsh.vsMatinfo[i].staRes[stacat][0] /
                               struGsh.vmatTotalmap[i][statype];
                        valStr = QString("%1").arg(QString::number(nres, 'f', 2));
                    }
                    break;
                case STATISTIC_A7:
                    valStr = QString("%1   %2   %3   %4")
                                 .arg((int)matInfo.staRes[stacat][0])
                                 .arg((int)matInfo.staRes[stacat][1])
                                 .arg((int)matInfo.staRes[stacat][2])
                                 .arg((int)matInfo.staRes[stacat][3]);
                    break;
                case STATISTIC_A8:
                case STATISTIC_A10: {
                    const float totalValue = matTotalMap.value(statype, 0.0f);
                    if(totalValue > 0.0f){
                        const float nVal = matInfo.staRes[stacat][0] / totalValue;
                        const float nVal1 = matInfo.staRes[stacat][1] / totalValue;
                        const float nVal2 = matInfo.staRes[stacat][2] / totalValue;
                        valStr = QString("%1%   %2%   %3%")
                                     .arg(QString::number(nVal * 100, 'f', 2))
                                     .arg(QString::number(nVal1 * 100, 'f', 2))
                                     .arg(QString::number(nVal2 * 100, 'f', 2));
                    }
                    break;
                }
                default:
                    break;
                }
                objmat[stacat] = valStr;
            }

            dataobj[QString("stagp_%1_lab").arg(i)] = labjsonarray;
            dataobj[QString("stagp_%1_mat").arg(i)] = objmat;
            jsonarray.append(dataobj);
        }
    }

    jsonobj["date"] = date;
    jsonobj["batch"] = batchname;
    jsonobj["dutyshift"] = struGsh.duty_shift;
    jsonobj["data"] = jsonarray;

    // 金属数据表
    if(struCnfg.nMachineType == nMachine_Bottle &&
       struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect){
        bool bover = false;
        if(struGsh.nMetaNum > pProfile->nMeatal){
            bover = true;
        }
        jsonobj["metal"] = struGsh.nMetaNum;
        jsonobj["metalsta"] = bover;
    }

    QJsonDocument doc(jsonobj);
    QString querycmd = doc.toJson(QJsonDocument::Compact);

    QString cmd = QString("INSERT INTO '%1' VALUES('%2', '%3', '%4', %5, %6,'%7','%8')")
                      .arg(sSchcode).arg(date)
                      .arg(batchname).arg(struGsh.duty_shift)
                      .arg(nResult).arg(struGsh.detect_yield)
                      .arg(struCnfg.schemeList[struCnfg.nCntScheme].name).arg(querycmd);

    g_sqliteInter->executeCmd(cmd);
}


/*!
 * \brief 获取标签的统计结果
 * \fn    遍历统计类别,获取统计结果
 * \param lab
 */
QString GlobalFlow::getLabStaResult(int nGbid, QString lab, QString staCat,
                                    bool &bSta,float nDiff, QString strchild)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const bool bSorterAdj = pProfile->bEnSorterClass;
    int nCamid = QString("%1").arg(pProfile->vUnitaddrlist[nGbid].at(0)).toInt();
    bool bPercentType = pProfile->nStaisticType == 1 ? true:false;

    QStringList childlst;
    if(pProfile->vMegerlabmap[nCamid].contains(lab)){
        childlst = pProfile->vMegerlabmap[nCamid].values(lab);
    }

    if(childlst.isEmpty()){
        childlst<<lab;
    }

    QString strRes;
    float nres = 0.0;
    for(QString child : childlst){
        if(struGsh.vlabStamap[nGbid][child].staInfo.contains(staCat)){
            QString str = struGsh.vlabStamap[nGbid][child].staInfo[staCat];
            QString tmpstr = str;
            QStringList stainfolst = tmpstr.split("/");
            if(tmpstr.isEmpty() || stainfolst.size()<2){
                qDebug()<<"Get LAB RES: Sta info Error";
                break;
            }

            int statype = QString("%1").arg(stainfolst[1]).at(1).digitValue();
            switch (statype) {
            case STATISTIC_A1://! 直接用
                nres += struGsh.vlabStamap[nGbid][child].staRes[staCat][0];
                strRes = QString("%1").arg((int)nres);
                break;
            case STATISTIC_A2:
                break;
            case STATISTIC_A3://! 乘系数 算占比
                if(struGsh.vlabTotalmap[nGbid][str] >0 ){
                    float tmpvalue = struGsh.vlabStamap[nGbid][child].staRes[staCat][0]/
                                     struGsh.vlabTotalmap[nGbid][str];

                    if(struCnfg.nMachineType == nMachine_Bottle){
                        if(bPercentType){
                            float nVal = QString::number(tmpvalue,'f',4).toFloat();
                            if(child == strchild){
                                nVal+=nDiff;
                            }
                            nres += QString::number(nVal,'f',4).toFloat();
                        }else{
                            int nVal = (int)(tmpvalue*1000000);
                            if(child == strchild){
                                nVal+=nDiff;
                            }
                            nres += nVal;
                        }
                    }else{
                        nres += tmpvalue;
                    }

                    if(struCnfg.nMachineType == nMachine_Bottle && !bPercentType){
                        qlonglong intVal = qRound(nres);
                        strRes = QString::number(intVal);
                        if(bSorterAdj){
                            if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(lab)){
                                if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nlowthd >nres ||
                                    pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nhigthd < nres){
                                    bSta = true;
                                }
                            }
                        }else{
                            if (pProfile->strctAI.vlabthdmap.contains(lab) &&
                                nres > pProfile->strctAI.vlabthdmap[lab]) {
                                bSta = true;
                            }
                        }
                    }else{
                        strRes =  QString("%1%").arg(QString::number(nres*100,'f',4));
                        if(bSorterAdj){
                            if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(lab)){
                                if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nlowthd > 100*nres ||
                                    pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nhigthd < 100*nres){
                                    bSta=true;
                                }
                            }
                        }else{
                            if(pProfile->strctAI.vlabthdmap.contains(lab)){
                                bSta = (100*nres > pProfile->strctAI.vlabthdmap[lab]) ? true : false;
                            }
                        }
                    }
                }else{
                    strRes =  QString("0");
                    bSta = false;
                }
                break;
            case STATISTIC_A4://! 直接算占比
                if(struGsh.vlabTotalmap[nGbid][str] > 0){
                    nres += struGsh.vlabStamap[nGbid][child].staRes[staCat][0]/
                            struGsh.vlabTotalmap[nGbid][str];
                    strRes =  QString("%1%").arg(QString::number(nres*100,'f',2));

                    if(bSorterAdj){
                        if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(lab)){
                            if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nlowthd > 100*nres ||
                                pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nhigthd < 100*nres){
                                bSta = true;
                            }
                        }
                    }else{
                        if(pProfile->strctAI.vlabthdmap.contains(lab) &&
                            100*nres > pProfile->strctAI.vlabthdmap[lab]){
                            bSta = true;
                        }
                    }
                }else{
                    strRes =  QString("0");
                    bSta = false;
                }
                break;
            case STATISTIC_A5:
                break;
            default:
                break;
            }
        }
    }

    return strRes;
}

/*!
 * \brief 获取颜色材质融合后的标签统计结果
 * \param nGbid 组ID（0为颜色组，lastGpId为材质组）
 * \param lab 原始标签名
 * \param staCat 统计类别
 * \param rRatio 标签占比缩放系数（颜色组为次一级标签占比，材质组为材质缩放系数）
 * \param bSta 输出是否超标状态
 * \param nDiff 归一化补偿差额
 * \param strchild 最大项标签名
 */
QString GlobalFlow::getLabStaResultMerger(int nGbid, QString lab, QString staCat, float rRatio,
                                          bool &bSta, float nDiff, QString strchild)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const bool bSorterAdj = pProfile->bEnSorterClass;
    int nCamid = QString("%1").arg(pProfile->vUnitaddrlist[nGbid].at(0)).toInt();
    bool bPercentType = (pProfile->nStaisticType == 1);

    QStringList childlst;
    if(pProfile->vMegerlabmap[nCamid].contains(lab)){
        childlst = pProfile->vMegerlabmap[nCamid].values(lab);
    }
    if(childlst.isEmpty()){
        childlst << lab;
    }

    QString strRes;
    float nres = 0.0f;
    for(const QString &child : childlst){
        if(struGsh.vlabStamap[nGbid][child].staInfo.contains(staCat)){
            QString str = struGsh.vlabStamap[nGbid][child].staInfo[staCat];
            QStringList stainfolst = str.split("/");
            if(str.isEmpty() || stainfolst.size() < 2){
                continue;
            }

            int statype = QString("%1").arg(stainfolst[1]).at(1).digitValue();
            switch (statype) {
            case STATISTIC_A1: {
                float val = struGsh.vlabStamap[nGbid][child].staRes[staCat][0];
                val *= rRatio;
                nres += val;
                strRes = QString("%1").arg((int)nres);
                break;
            }
            case STATISTIC_A2: {
                float val = struGsh.vlabStamap[nGbid][child].staRes[staCat][0];
                val *= rRatio;
                nres += val;
                strRes = QString::number(nres, 'f', 2);
                break;
            }
            case STATISTIC_A3: {
                if(struGsh.vlabTotalmap[nGbid].value(str) > 0){
                    float tmpvalue = struGsh.vlabStamap[nGbid][child].staRes[staCat][0] /
                                     struGsh.vlabTotalmap[nGbid][str];
                    tmpvalue *= rRatio;

                    if(struCnfg.nMachineType == nMachine_Bottle){
                        if(bPercentType){
                            float nVal = QString::number(tmpvalue, 'f', 4).toFloat();
                            if(child == strchild){
                                nVal += nDiff;
                            }
                            nres += QString::number(nVal, 'f', 4).toFloat();
                        } else {
                            int nVal = (int)(tmpvalue * 1000000);
                            if(child == strchild){
                                nVal += nDiff;
                            }
                            nres += nVal;
                        }
                    } else {
                        nres += tmpvalue;
                    }

                    if(struCnfg.nMachineType == nMachine_Bottle && !bPercentType){
                        qlonglong intVal = qRound(nres);
                        strRes = QString::number(intVal);
                        if(bSorterAdj){
                            if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(lab)){
                                if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nlowthd > nres ||
                                   pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nhigthd < nres){
                                    bSta = true;
                                }
                            }
                        } else {
                            if(pProfile->strctAI.vlabthdmap.contains(lab) &&
                               nres > pProfile->strctAI.vlabthdmap[lab]){
                                bSta = true;
                            }
                        }
                    } else {
                        strRes = QString("%1%").arg(QString::number(nres * 100, 'f', 4));
                        if(bSorterAdj){
                            if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(lab)){
                                if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nlowthd > 100 * nres ||
                                   pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nhigthd < 100 * nres){
                                    bSta = true;
                                }
                            }
                        } else {
                            if(pProfile->strctAI.vlabthdmap.contains(lab)){
                                bSta = (100 * nres > pProfile->strctAI.vlabthdmap[lab]);
                            }
                        }
                    }
                } else {
                    strRes = QString("0");
                    bSta = false;
                }
                break;
            }
            case STATISTIC_A4: {
                if(struGsh.vlabTotalmap[nGbid].value(str) > 0){
                    float tmpvalue = struGsh.vlabStamap[nGbid][child].staRes[staCat][0] /
                                     struGsh.vlabTotalmap[nGbid][str];
                    tmpvalue *= rRatio;
                    nres += tmpvalue;
                    strRes = QString("%1%").arg(QString::number(nres * 100, 'f', 2));

                    if(bSorterAdj){
                        if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx].contains(lab)){
                            if(pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nlowthd > 100 * nres ||
                               pProfile->strcSortclass.vlabParm[struGsh.nSorterAdjIdx][lab].nhigthd < 100 * nres){
                                bSta = true;
                            }
                        }
                    } else {
                        if(pProfile->strctAI.vlabthdmap.contains(lab) &&
                           100 * nres > pProfile->strctAI.vlabthdmap[lab]){
                            bSta = true;
                        }
                    }
                } else {
                    strRes = QString("0");
                    bSta = false;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return strRes;
}


/**
 * @brief GlobalFlow::初始化数据库
 */
void GlobalFlow::initDB()
{
    g_sqliteInter->initializationDB();
}

/*!
 * \brief GlobalFlow::CurBoxMerge 新合框函数
 * \fn    缓存各个需要合框的相机的Box
 * \param vecFvKA1Boxes：算法输出框
 * \param vecBoxes：相机的原始框
 * \param camID：相机的序号
 * \param IOUThr：交并比
 * \param conf：灵敏度
 */
void GlobalFlow::CurBoxMerge(std::vector<myfvKA1Box> &vecFvKA1Boxes,
                             std::vector<BBox> &vecBoxes, int camID,
                             float IOUThr, std::vector<float> conf)
{
    int wx = 1;
    int bx = 0;
    int noff = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.noffsetvalue; //! 合并框边长
    int bias = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamoffPiexl[camID];         //! 相机偏移

    if(camID >= 2){
        wx = -1;
        bx = 1024;
    }

    if(camID != 0){
        bx -= bias;
    }

    for(BBox vecBox: vecBoxes){
        bool newBox = true;
        int lbox[]={0,0,0,0,0};
        if(noff != 0){
            lbox[0] = (wx*vecBox.cx + bx) - noff/2;
            lbox[1] = vecBox.cy - noff/2;
            lbox[2] = (wx*vecBox.cx + bx) + noff/2;
            lbox[3] = vecBox.cy + noff/2;
            lbox[4] = noff * noff;
        }else{
            lbox[0] = (wx*vecBox.cx + bx) - vecBox.bw/2;
            lbox[1] = vecBox.cy - vecBox.bh/2;
            lbox[2] = (wx*vecBox.cx + bx) + vecBox.bw/2;
            lbox[3] = vecBox.cy + vecBox.bh/2;
            lbox[4] = vecBox.bw * vecBox.bh;
        }

        for(int i = 0; i < vecFvKA1Boxes.size(); i++){
            int rbox[] = {0,0,0,0,0};
            if(noff != 0){
                    rbox[0] = vecFvKA1Boxes[i].cx - noff/2;
                    rbox[1] = vecFvKA1Boxes[i].cy - noff/2;
                    rbox[2] = vecFvKA1Boxes[i].cx + noff/2;
                    rbox[3] = vecFvKA1Boxes[i].cy + noff/2;
                    rbox[4] = noff * noff;
            }else{
                 rbox[0] = vecFvKA1Boxes[i].cx - vecFvKA1Boxes[i].bw/2;
                 rbox[1] = vecFvKA1Boxes[i].cy - vecFvKA1Boxes[i].bh/2;
                 rbox[2] = vecFvKA1Boxes[i].cx + vecFvKA1Boxes[i].bw/2;
                 rbox[3] = vecFvKA1Boxes[i].cy + vecFvKA1Boxes[i].bh/2;
                 rbox[4] = vecFvKA1Boxes[i].bw * vecFvKA1Boxes[i].bh;
            }

            int interBox[] = {
                              std::max(lbox[0], rbox[0]),
                              std::max(lbox[1], rbox[1]),
                              std::min(lbox[2], rbox[2]),
                              std::min(lbox[3], rbox[3]),
                              };
            if(interBox[1] >= interBox[3] || interBox[0] >= interBox[2])
                continue;
            int interArea = (interBox[3] - interBox[1] + 1) * (interBox[2] - interBox[0] + 1);
            if(interArea > (lbox[4] + rbox[4] - interArea) * IOUThr){
                newBox = false;
                bool newCam = true;
                for(int mergedcamID:  vecFvKA1Boxes[i].camID){
                    if(mergedcamID == camID){
                        newCam = false;
                    }
                }
                if(newCam){
                    vecFvKA1Boxes[i].n += 1;
                }
                vecFvKA1Boxes[i].camID.push_back(camID);
                std::vector<float> ex_info;
                for(int ex_info_id = 0; ex_info_id < 40; ex_info_id++){
                    ex_info.push_back(vecBox.ex_info[ex_info_id]);
                }
                vecFvKA1Boxes[i].ori_ex_info.push_back(ex_info);
//                if(vecBox.prob > conf[vecBox.label]) {
                    vecFvKA1Boxes[i].oriLabel.push_back(vecBox.label);
                    vecFvKA1Boxes[i].prob.push_back(vecBox.prob);
//                }else{
//                    vecFvKA1Boxes[i].oriLabel.push_back(-1);
//                    vecFvKA1Boxes[i].prob.push_back(-1);
//                }
            }
        }
        if(newBox){
            myfvKA1Box fvKA1Box;
            fvKA1Box.cx = wx*vecBox.cx + bx;
            fvKA1Box.cy = vecBox.cy;
            fvKA1Box.bw = vecBox.bw;
            fvKA1Box.bh = vecBox.bh;
            fvKA1Box.topx = fvKA1Box.cx - fvKA1Box.bw/2 < 0 ? 0 : fvKA1Box.cx - fvKA1Box.bw/2;
            fvKA1Box.topy = fvKA1Box.cy - fvKA1Box.bh/2 < 0 ? 0 : fvKA1Box.cy - fvKA1Box.bh/2;
            fvKA1Box.n = 1;
            fvKA1Box.camID.push_back(camID);
            std::vector<float> ex_info;
            for(int ex_info_id = 0; ex_info_id < 40; ex_info_id++){
                ex_info.push_back(vecBox.ex_info[ex_info_id]);
            }
            fvKA1Box.ori_ex_info.push_back(ex_info);
            fvKA1Box.count_flag = 0;
//            if(vecBox.prob > conf[vecBox.label]) {
                fvKA1Box.oriLabel.push_back(vecBox.label);
                fvKA1Box.prob.push_back(vecBox.prob);
//            }else{
//                fvKA1Box.oriLabel.push_back(-1);
//                fvKA1Box.prob.push_back(-1);
//            }
            vecFvKA1Boxes.push_back(fvKA1Box);
        }
    }
}
/*!
 * \brief GlobalFlow::CatchedBoxMerge
 * \fn    缓存合框函数
 * \param vecFvKA1Boxes:当前框
 * \param vecCatchedFvKA1Boxes:缓存的上一幅的识别框
 * \param IOUThr：交并比
 * \param curRows：图像的采集行
 */
void GlobalFlow::CatchedBoxMerge(std::vector<myfvKA1Box> &vecFvKA1Boxes,
                                 std::vector<myfvKA1Box> &vecCatchedFvKA1Boxes,
                                 float IOUThr, int curRows)
{
    int noff = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.noffsetvalue; //合并框边长
    for(int i = 0; i < vecFvKA1Boxes.size(); i++){
        int lbox[] = {0,0,0,0,0};

        if(noff != 0){
            lbox[0] = vecFvKA1Boxes[i].cx - noff/2;
            lbox[1] = vecFvKA1Boxes[i].cy - noff/2;
            lbox[2] = vecFvKA1Boxes[i].cx + noff/2;
            lbox[3] = vecFvKA1Boxes[i].cy + noff/2;
            lbox[4] = noff * noff;
        }else{
            lbox[0] = vecFvKA1Boxes[i].cx - vecFvKA1Boxes[i].bw/2;
            lbox[1] = vecFvKA1Boxes[i].cy - vecFvKA1Boxes[i].bh/2;
            lbox[2] = vecFvKA1Boxes[i].cx + vecFvKA1Boxes[i].bw/2;
            lbox[3] = vecFvKA1Boxes[i].cy + vecFvKA1Boxes[i].bh/2;
            lbox[4] = vecFvKA1Boxes[i].bw * vecFvKA1Boxes[i].bh;
        }
        for(int j = 0; j < vecCatchedFvKA1Boxes.size(); j++){
            int rbox[] = {0,0,0,0,0};
            if(noff != 0){
                rbox[0] = vecCatchedFvKA1Boxes[i].cx - noff/2;
                rbox[1] = vecCatchedFvKA1Boxes[i].cy - noff/2;
                rbox[2] = vecCatchedFvKA1Boxes[i].cx + noff/2;
                rbox[3] = vecCatchedFvKA1Boxes[i].cy + noff/2;
                rbox[4] = noff * noff;
            }else{
                 rbox[0] = vecCatchedFvKA1Boxes[i].cx - vecCatchedFvKA1Boxes[i].bw/2;
                 rbox[1] = vecCatchedFvKA1Boxes[i].cy - vecCatchedFvKA1Boxes[i].bh/2;
                 rbox[2] = vecCatchedFvKA1Boxes[i].cx + vecCatchedFvKA1Boxes[i].bw/2;
                 rbox[3] = vecCatchedFvKA1Boxes[i].cy + vecCatchedFvKA1Boxes[i].bh/2;
                 rbox[4] = vecCatchedFvKA1Boxes[i].bw * vecCatchedFvKA1Boxes[i].bh;
            }
            int interBox[] = {
                              std::max(lbox[0], rbox[0]),
                              std::max(lbox[1], rbox[1]),
                              std::min(lbox[2], rbox[2]),
                              std::min(lbox[3], rbox[3]),
                              };
            if(interBox[1] >= interBox[3] || interBox[0] >= interBox[2])
                continue;
            int interArea = (interBox[3] - interBox[1] + 1) * (interBox[2] - interBox[0] + 1);
            if(interArea > (lbox[4] + rbox[4] - interArea) * IOUThr){
                for(int catcheMergedcamID: vecCatchedFvKA1Boxes[j].camID){
                    bool newCam = true;
                    for(int mergedcamID:  vecFvKA1Boxes[i].camID){
                        if (mergedcamID == catcheMergedcamID) {
                            newCam = false;
                        }
                    }
                    if(newCam){
                        vecFvKA1Boxes[i].n += 1;
                    }
                    vecFvKA1Boxes[i].camID.push_back(catcheMergedcamID);
                }
                //vecFvKA1Boxes[i].camID.insert(vecFvKA1Boxes[i].camID.end(), vecCatchedFvKA1Boxes[j].camID.begin(), vecCatchedFvKA1Boxes[j].camID.end());
                vecFvKA1Boxes[i].ori_ex_info.insert(vecFvKA1Boxes[i].ori_ex_info.end(), vecCatchedFvKA1Boxes[j].ori_ex_info.begin(), vecCatchedFvKA1Boxes[j].ori_ex_info.end());
                vecFvKA1Boxes[i].oriLabel.insert(vecFvKA1Boxes[i].oriLabel.end(), vecCatchedFvKA1Boxes[j].oriLabel.begin(), vecCatchedFvKA1Boxes[j].oriLabel.end());
                vecFvKA1Boxes[i].prob.insert(vecFvKA1Boxes[i].prob.end(), vecCatchedFvKA1Boxes[j].prob.begin(), vecCatchedFvKA1Boxes[j].prob.end());
                if(vecCatchedFvKA1Boxes[j].count_flag > vecFvKA1Boxes[i].count_flag){
                    vecFvKA1Boxes[i].count_flag = vecCatchedFvKA1Boxes[j].count_flag;
                }
            }
        }
        vecFvKA1Boxes[i].cy -= curRows;
    }
    //vecCatchedFvKA1Boxes.clear();
    //vecCatchedFvKA1Boxes.assign(vecFvKA1Boxes.begin(), vecFvKA1Boxes.end());
}

/*!
 * \brief GlobalFlow::NormalMergedBoxProcess
 * \fn    通用合框函数
 *       常规标签合并函数，当物料满足计数条件（被所有相机预测过且未计数）时，
 *       将物料状态由0不可计数改为1可计数，并按界面设置的优先级，决定物料最终预测结果，
 *       当物料状态为1时，表示物料已被计数，将状态改为2，表示已计数且此后不可再被计数
 * \param vecFvKA1Boxes
 * \param vlabidx
 * \param CamNum
 */
void GlobalFlow::NormalMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes,
                                        std::vector<int> vlabidx, int CamNum)
{
    for(int i = 0; i < vecFvKA1Boxes.size(); i++){
        if(vecFvKA1Boxes[i].count_flag == 1){
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if(vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0){ //根据需要修改可计数条件
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info02(3, 0);
            int cam02 = 0;
            std::vector<float> ex_info13(3, 0);
            int cam13 = 0;

            for(int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++){
                
                bool newLabel = true;
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id];
                if(ori_label == -1)
                    newLabel = false;
                else{
                    for(int label_id = 0; label_id < vecFvKA1Boxes[i].label.size(); label_id++){
                        int label = vecFvKA1Boxes[i].label[label_id];
                        if(vlabidx[ori_label] < vlabidx[label]){
                            vecFvKA1Boxes[i].label.clear();
                        }else if(vlabidx[ori_label] == vlabidx[label]){
                            if(ori_label == label)
                                newLabel = false;
                        }else{
                            newLabel = false;
                        }
                    }
                }
                //通用标签处理，根据需要自定义

                if(newLabel){
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                }
                //通用长、宽、面积整合，根据需要处理其他ex_info
                if(vecFvKA1Boxes[i].camID[ori_label_id] == 0 || vecFvKA1Boxes[i].camID[ori_label_id] == 2){
                    ex_info02[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info02[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info02[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam02 ++;

                }else{
                    ex_info13[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info13[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info13[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam13 ++;
                }
            }
            if(ex_info02[2] > ex_info13[2]){
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[0] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[1] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[2] / cam02);
            }
            else{
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[0] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[1] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[2] / cam13);
            }
        }
    }
}

/*!
 * \brief GlobalFlow::KHZMergedBoxProcess
 * \fn    葵花子定制和框函数
 * \param vecFvKA1Boxes
 * \param vlabidx: 标签优先级
 * \param vmutuallylabidx: 标签互斥
 * \param CamNum
 */
void GlobalFlow::KHZMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx,
                                     std::vector<int> vmutuallylabidx, int CamNum)
{
    for(int i = 0; i < vecFvKA1Boxes.size(); i++){
        if(vecFvKA1Boxes[i].count_flag == 1){
            vecFvKA1Boxes[i].count_flag = 2;    //已被记过数，count_flag置为2
        }

        //! 1.上视或者下视中：窄的角度相机有脱皮或者花皮，同时宽的角度相机没有脱皮和花皮，则窄的角度相机脱皮或者花皮不参与合并
        bool bht_0 = false, bht_1 = false, bht_2 = false, bht_3 = false;
        double fhtwidth_0 = 0.0, fhtwidth_1 = 0.0, fhtwidth_2 = 0.0, fhtwidth_3 = 0.0;
        for(int ht_label_id = 0; ht_label_id < vecFvKA1Boxes[i].oriLabel.size(); ht_label_id++){
            int ht_label = vecFvKA1Boxes[i].oriLabel[ht_label_id]; //当前的标签序号
            int ht_camID= vecFvKA1Boxes[i].camID[ht_label_id]; //当前的相机序号
            float ht_width = vecFvKA1Boxes[i].ori_ex_info[ht_label_id][1]; //当前的标签的宽度

            //当前物料逐相机判断是否有花皮或者脱皮
            if(ht_camID == 0){
                if(ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19){
                    bht_0 = true;
                    fhtwidth_0 = ht_width;
                }
            }
            if(ht_camID == 1){
                if(ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19){
                    bht_1 = true;
                    fhtwidth_1 = ht_width;
                }
            }
            if(ht_camID == 2){
                if(ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19){
                    bht_2 = true;
                    fhtwidth_2 = ht_width;
                }
            }
            if(ht_camID == 3){
                if(ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19){
                    bht_3 = true;
                    fhtwidth_3 = ht_width;
                }
            }
        }
        //上视判断
        if(fhtwidth_0 < fhtwidth_1 && (bht_0 && !bht_1)){  //相机0较窄且满足条件1
            for(int ht_label_id = 0; ht_label_id < vecFvKA1Boxes[i].oriLabel.size(); ht_label_id++){
                int ht_label = vecFvKA1Boxes[i].oriLabel[ht_label_id]; //当前的标签序号
                int ht_camID= vecFvKA1Boxes[i].camID[ht_label_id]; //当前的相机序号
                if(ht_camID == 0 && (ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19)){
                    vecFvKA1Boxes[i].oriLabel[ht_label_id] = -1;
                }
            }

        }else if(fhtwidth_0 > fhtwidth_1 && (!bht_0 && bht_1)){  //相机1较窄且满足条件1
            for(int ht_label_id = 0; ht_label_id < vecFvKA1Boxes[i].oriLabel.size(); ht_label_id++){
                int ht_label = vecFvKA1Boxes[i].oriLabel[ht_label_id]; //当前的标签序号
                int ht_camID= vecFvKA1Boxes[i].camID[ht_label_id]; //当前的相机序号
                if(ht_camID == 1 && (ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19)){
                    vecFvKA1Boxes[i].oriLabel[ht_label_id] = -1;
                }
            }
        }
        //下视判断
        if(fhtwidth_2 < fhtwidth_3 && (bht_2 && !bht_3)){  //相机2较窄且满足条件1
            for(int ht_label_id = 0; ht_label_id < vecFvKA1Boxes[i].oriLabel.size(); ht_label_id++){
                int ht_label = vecFvKA1Boxes[i].oriLabel[ht_label_id]; //当前的标签序号
                int ht_camID= vecFvKA1Boxes[i].camID[ht_label_id]; //当前的相机序号
                if(ht_camID == 2 && (ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19)){
                    vecFvKA1Boxes[i].oriLabel[ht_label_id] = -1;
                }
            }

        }else if(fhtwidth_2 > fhtwidth_3 && (!bht_2 && bht_3)){  //相机3较窄且满足条件1
            for(int ht_label_id = 0; ht_label_id < vecFvKA1Boxes[i].oriLabel.size(); ht_label_id++){
                int ht_label = vecFvKA1Boxes[i].oriLabel[ht_label_id]; //当前的标签序号
                int ht_camID= vecFvKA1Boxes[i].camID[ht_label_id]; //当前的相机序号
                if(ht_camID == 3 && (ht_label == 4 || ht_label == 12 || ht_label == 18 || ht_label == 19)){
                    vecFvKA1Boxes[i].oriLabel[ht_label_id] = -1;
                }
            }
        }

        if(vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0){
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info02(3, 0);
            int cam02 = 0;
            std::vector<float> ex_info13(3, 0);
            int cam13 = 0;

            //! 2.遍历当前物料不同相机的所有标签
            std::vector<float> vec_ori_prob;
            std::vector<int> vec_ori_camID;
            std::vector<float>  vec_ori_ex_info;
            for(int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++){
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id]; //当前未合并的标签序号
                float ori_prob = vecFvKA1Boxes[i].prob[ori_label_id]; //当前未合并的标签得分
                int ori_camID= vecFvKA1Boxes[i].camID[ori_label_id]; //当前未合并的标签得分
                float ori_width = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1]; //当前未合并的标签的宽度
                //! 统计上下视对应相机ori_ex_info信息
                if(vecFvKA1Boxes[i].camID[ori_label_id] == 0 || vecFvKA1Boxes[i].camID[ori_label_id] == 2){
                    ex_info02[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info02[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info02[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam02 ++;

                }else{
                    ex_info13[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info13[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info13[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam13 ++;
                }
                if(ori_label == -1){
                    continue;
                }

                //! 3.遍历当前物料已合并的所有标签
                bool bHighPriority = false;
                bool bNonExclusiveSamePriority = false;
                bool bMutualExclusiveSamePriority = false;
                bool same_label = false;
                int same_id = -1;
                int mutuallylabel_id = -1;
                for(int tmp_id = 0; tmp_id < vecFvKA1Boxes[i].label.size(); tmp_id++){
                    int tmp_label = vecFvKA1Boxes[i].label[tmp_id]; //当前已合并的标签
                    if(vlabidx[ori_label] < vlabidx[tmp_label]){
                        //当前优先级大，需保存
                        bHighPriority = true;
                    }else if(vlabidx[ori_label] == vlabidx[tmp_label]){
                        //当前优先级相同
                        if((vmutuallylabidx[ori_label] != vmutuallylabidx[tmp_label]) || (vmutuallylabidx[ori_label] == 0 && vmutuallylabidx[tmp_label] == 0)){
                            //同级不互斥
                            bNonExclusiveSamePriority = true;
                            if(ori_label == tmp_label){
                                //同级不互斥的相同标签，需保留prob大的标签
                                same_label = true;
                                same_id = tmp_id;
                            }else{
                                //需push_back进
                            }
                        }else if(vmutuallylabidx[ori_label] == vmutuallylabidx[tmp_label] && vmutuallylabidx[ori_label] != 0){
                            //同级互斥
                            bMutualExclusiveSamePriority = true;
                            mutuallylabel_id = tmp_id;
                        }
                    }
                }

                //! 4.1 优先级大的处理
                if(bHighPriority){
                    vecFvKA1Boxes[i].label.clear(); //清空已保存的优先级小的标签
                    vec_ori_prob.clear(); //清空已保存的优先级小的标签得分
                    vec_ori_camID.clear(); //清空已保存的优先级小的标签相机序号
                    vec_ori_ex_info.clear(); //清空已保存的优先级小的标签的宽度

                    vecFvKA1Boxes[i].label.push_back(ori_label);  //保存当前级别高的标签
                    vec_ori_prob.push_back(ori_prob);
                    vec_ori_camID.push_back(ori_camID);
                    vec_ori_ex_info.push_back(ori_width);
                }else if(bMutualExclusiveSamePriority){
                    //! 4.2.1 优先级同级互斥的处理
                    int tmp_label = vecFvKA1Boxes[i].label[mutuallylabel_id]; //当前已合并的标签
                    float tmp_prob = vec_ori_prob[mutuallylabel_id]; //当前已合并的标签得分
                    int tmp_camID= vec_ori_camID[mutuallylabel_id]; //当前已合并的标签相机序号
                    float tmp_width = vec_ori_ex_info[mutuallylabel_id]; //当前已合并的标签的宽度

                    if(ori_label == 4 || ori_label == 12 || ori_label == 18 || ori_label == 19){
                        //! 花皮和脱皮的处理逻辑
                        if(ori_camID == tmp_camID){
                            //互斥统计原则1：同一相机选择得分大的
                            if(ori_prob > tmp_prob){
                                vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                                vec_ori_prob[mutuallylabel_id] = ori_prob;
                                vec_ori_camID[mutuallylabel_id] = ori_camID;
                                vec_ori_ex_info[mutuallylabel_id] = ori_width;
                            }
                        }else if(((ori_camID == 0 || ori_camID == 1) && (tmp_camID == 0 || tmp_camID == 1)) || ((ori_camID == 2 || ori_camID == 3) && (tmp_camID == 2 || tmp_camID ==3))){
                            //互斥统计原则2：同一侧的相机选择宽度大的
                            if(ori_width > tmp_width){
                                vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                                vec_ori_prob[mutuallylabel_id] = ori_prob;
                                vec_ori_camID[mutuallylabel_id] = ori_camID;
                                vec_ori_ex_info[mutuallylabel_id] = ori_width;
                            }
                        }else{
                            //互斥统计原则3：不同侧的相机选择得分大的
                            if(ori_prob > tmp_prob){
                                vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                                vec_ori_prob[mutuallylabel_id] = ori_prob;
                                vec_ori_camID[mutuallylabel_id] = ori_camID;
                                vec_ori_ex_info[mutuallylabel_id] = ori_width;
                            }
                        }
                    }else if(ori_label == 5 || ori_label == 8 || ori_label == 15 || ori_label == 16 || ori_label == 17){
                        //! 霉变（苦籽黄皮、霉灰轻、霉灰重、苦籽白、苦籽黑）的处理逻辑:1、苦籽黄皮和其他霉变比较得分，保留得分大的；2非苦籽黄皮的霉变根据优先级:霉灰重>苦籽白>苦籽黑>霉灰轻.
                        std::vector<int> vmblabidx = {0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 1, 2, 3};
                        if((ori_label == 5 || tmp_label == 5) && ori_prob > tmp_prob){
                            //苦籽黄皮和其他霉变比较得分，保留得分大的；
                            vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                            vec_ori_prob[mutuallylabel_id] = ori_prob;
                            vec_ori_camID[mutuallylabel_id] = ori_camID;
                            vec_ori_ex_info[mutuallylabel_id] = ori_width;
                        }else if(ori_label != 5 && tmp_label != 5){
                            //非苦籽黄皮的霉变根据优先级:霉灰重>苦籽白>苦籽黑>霉灰轻;同级选择得分大的
                            if(vmblabidx[ori_label] < vmblabidx[tmp_label]){ //保留霉变标签优先级大的
                                vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                                vec_ori_prob[mutuallylabel_id] = ori_prob;
                                vec_ori_camID[mutuallylabel_id] = ori_camID;
                                vec_ori_ex_info[mutuallylabel_id] = ori_width;
                            }else if(vmblabidx[ori_label] == vmblabidx[tmp_label] && ori_prob > tmp_prob){ //保留霉变标签优先级同级但得分大的
                                vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                                vec_ori_prob[mutuallylabel_id] = ori_prob;
                                vec_ori_camID[mutuallylabel_id] = ori_camID;
                                vec_ori_ex_info[mutuallylabel_id] = ori_width;
                            }
                        }
                    }else{
                        //! 其他同级互斥的标签，默认根据得分，保留得分大的标签
                        if(ori_prob > tmp_prob){
                            vecFvKA1Boxes[i].label[mutuallylabel_id] = ori_label;
                            vec_ori_prob[mutuallylabel_id] = ori_prob;
                            vec_ori_camID[mutuallylabel_id] = ori_camID;
                            vec_ori_ex_info[mutuallylabel_id] = ori_width;
                        }
                    }
                }else if(bNonExclusiveSamePriority){
                    //! 4.2.2 优先级同级不互斥的处理
                    if(same_label){
                        float tmp_prob = vec_ori_prob[same_id]; //当前已合并的标签得分
                        if(ori_prob > tmp_prob){  //相同标签，选择得分大的
                            vecFvKA1Boxes[i].label[same_id] = ori_label;
                            vec_ori_prob[same_id] = ori_prob;
                            vec_ori_camID[same_id] = ori_camID;
                            vec_ori_ex_info[same_id] = ori_width;
                        }
                    }else{
                        vecFvKA1Boxes[i].label.push_back(ori_label); //保存当前未合并的标签
                        vec_ori_prob.push_back(ori_prob);
                        vec_ori_camID.push_back(ori_camID);
                        vec_ori_ex_info.push_back(ori_width);
                    }
                }//! 4.3 优先级小的不处理

                //! 当合并的label为空时，保存当前标签ori_label
                if(vecFvKA1Boxes[i].label.empty()){
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                    vec_ori_prob.push_back(ori_prob);
                    vec_ori_camID.push_back(ori_camID);
                    vec_ori_ex_info.push_back(ori_width);
                }



            }

            //! 保留大的ex_info
            if(ex_info02[2] > ex_info13[2]){
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[0] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[1] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[2] / cam02);
            }
            else{
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[0] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[1] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[2] / cam13);
            }
            if(vecFvKA1Boxes[i].label.empty()){
                vecFvKA1Boxes[i].label.push_back(3);
            }

        }
    }
}

/*!
 * \brief GlobalFlow::KxgLHMergedBoxProcess
 * \fn    开心果合框函数定制
 *        ex_info 0：长 1：宽 2：面积 3：黄度 其中黄度0~100 越低表示越黄
 *        需搭配开心果黄度量化模型使用
 * \param vecFvKA1Boxes
 * \param vlabidx
 * \param CamNum
 * \param conf
 */
void GlobalFlow::KxgLHMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum, std::vector<float> conf)
{
    for(int i = 0; i < vecFvKA1Boxes.size(); i++){
        if(vecFvKA1Boxes[i].count_flag == 1){
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if(vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0){
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info02(3, 0);
            int cam02 = 0;
            std::vector<float> ex_info13(3, 0);
            int cam13 = 0;
            float yellow_score = 0;
            for(int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++){
                bool newLabel = true;
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id];
                if(ori_label == -1){
                    newLabel = false;
                }else{//通用标签处理，根据需要自定义
                    for(int label_id = 0; label_id < vecFvKA1Boxes[i].label.size(); label_id++){
                        int label = vecFvKA1Boxes[i].label[label_id];
                        if(vlabidx[ori_label] < vlabidx[label]){
                            vecFvKA1Boxes[i].label.clear();
                        }else if(vlabidx[ori_label] == vlabidx[label]){
                            if(ori_label == label)
                                newLabel = false;
                        }else{
                            newLabel = false;
                        }
                    }
                }

                if(newLabel){
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                }
                if(vecFvKA1Boxes[i].camID[ori_label_id] == 0 || vecFvKA1Boxes[i].camID[ori_label_id] == 2){
                    ex_info02[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info02[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info02[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam02 ++;

                }else{
                    ex_info13[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info13[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info13[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam13 ++;
                }
                yellow_score += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][3];
            }
            if(ex_info02[2] > ex_info13[2]){
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[0] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[1] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[2] / cam02);
            }
            else{
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[0] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[1] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[2] / cam13);
            }
            vecFvKA1Boxes[i].ex_info.push_back(yellow_score / (cam02 + cam13));
            

            if (vecFvKA1Boxes[i].label.empty()){
                vecFvKA1Boxes[i].label.push_back(1);
            }

            if(!vecFvKA1Boxes[i].label.empty() && vecFvKA1Boxes[i].label[0] <= 2){
                //判断大小开口、闭口是否满足深黄条件，不满足时，再判断大小开口是否满足浅黄条件，深黄优先于浅黄
                if(vecFvKA1Boxes[i].ex_info[3] < 1 - conf[4]){
                    //若目标标签为大小开口、闭口，根据其黄度赋深黄标签，并覆盖原标签。
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(4);
                }else if(vecFvKA1Boxes[i].label[0] != 0 && vecFvKA1Boxes[i].ex_info[3] < 1 - conf[3]){
                    //若目标标签为大小开口，根据其黄度赋浅黄标签，并覆盖原标签。
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(3);
                }
            }
        }
    }
}

/*!
 * \brief 玉米粒定制函数
 * \param vecFvKA1Boxes
 * \param vlabidx
 * \param CamNum
 * \param conf
 */
void GlobalFlow::YuMiLHMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum, std::vector<float> conf)
{
    //标签顺序 hao,sh,sm,qm,ht,small,sui,zz
    //ex_info 0：长 1：宽 2：面积 3：黄度 其中黄度0~100 越高表示越红
    //需搭配玉米黄度量化模型使用
    for(int i = 0; i < vecFvKA1Boxes.size(); i++){
        if(vecFvKA1Boxes[i].count_flag == 1){
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if(vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0){
            bool isSui = false;
            bool isZZ = false;
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info(3, 0);
            float yellow_score = 0;

            int max_area_camID = 0;
            for(int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++){
                if(ex_info[2] < vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2]){
                    ex_info[0] = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info[1] = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info[2] = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    max_area_camID = vecFvKA1Boxes[i].camID[ori_label_id];
                }
                yellow_score += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][3];
            }
            vecFvKA1Boxes[i].ex_info.push_back(ex_info[0]);
            vecFvKA1Boxes[i].ex_info.push_back(ex_info[1]);
            vecFvKA1Boxes[i].ex_info.push_back(ex_info[2]);
            yellow_score = 1 - yellow_score / vecFvKA1Boxes[i].camID.size();
            if(yellow_score < 0.24)
                yellow_score = 0.24;
            if(yellow_score > 0.83)
                yellow_score = 0.83;
            yellow_score = (yellow_score - 0.24) / (0.83 - 0.24);
            vecFvKA1Boxes[i].ex_info.push_back(yellow_score);

            if(max_area_camID >= 2)
                max_area_camID -= 2;

            for(int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++){
                bool newLabel = true;
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id];
                if(ori_label == -1)
                    newLabel = false;

                //过滤非最大面积下预测的深霉和浅霉

                if((ori_label == 2 || ori_label == 3) &&
                    (vecFvKA1Boxes[i].camID[ori_label_id] != max_area_camID &&
                     vecFvKA1Boxes[i].camID[ori_label_id] != max_area_camID + 2)){
                    newLabel = false;

                }

                //通用标签处理，根据需要自定义
                if(newLabel) {
                    for (int label_id = 0; label_id < vecFvKA1Boxes[i].label.size(); label_id++) {
                        int label = vecFvKA1Boxes[i].label[label_id];
                        if (vlabidx[ori_label] < vlabidx[label]) {
                            vecFvKA1Boxes[i].label.clear();
                        } else if (vlabidx[ori_label] == vlabidx[label]) {
                            if (ori_label == label) {
                                newLabel = false;
                            }
                        } else {
                            newLabel = false;
                        }
                    }
                }
                if(newLabel){
                    if(ori_label == 6){
                        isSui = true;
                    }
                    if(ori_label == 7){
                        isZZ = true;
                    }
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                }
            }
            //所有标签均被过滤，认为是好料
            if(vecFvKA1Boxes[i].label.empty()) {
                vecFvKA1Boxes[i].label.push_back(0);
            }

            //如果是好料，判断是否为深黄
            if(vecFvKA1Boxes[i].label[0] == 0){
                if(1 - vecFvKA1Boxes[i].ex_info[3] < 1 - conf[1]){
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(1);
                }
            }

            //如果不是杂质，判断是否为小，若小，仅保留小标签，再判断是否为碎。
            if(!isZZ && vecFvKA1Boxes[i].ex_info[2] < (1-conf[5])*10000) {
                vecFvKA1Boxes[i].label.clear();
                vecFvKA1Boxes[i].label.push_back(5);
            }
            if(!isZZ && isSui) {
                vecFvKA1Boxes[i].label.push_back(6);
            }
        }
    }
}

/*!
 * \brief 绿豆定制函数
 * \param vecFvKA1Boxes
 * \param vlabidx
 * \param CamNum
 * \param conf
 */
void GlobalFlow::LvDouLHMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum,std::vector<float> conf)
{
    //标签顺序 abnc,abnc1,abnc2,hairy,half,good,tk
    //        黄   黄绿   灰褐
    //ex_info 0：长 1：宽 2：面积 3：黄度 其中黄度0~100 越低表示越黄 4：红绿灰度 越低越暗
    //需搭配绿豆颜色量化模型使用

    for (int i = 0; i < vecFvKA1Boxes.size(); i++) {
        if (vecFvKA1Boxes[i].count_flag == 1) {
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if (vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0) {
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info02(3, 0);
            int cam02 = 0;
            std::vector<float> ex_info13(3, 0);
            int cam13 = 0;
            float yellow_score = 0;
            float gray_score = 0;
            for (int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++) {
                bool newLabel = true;
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id];
                if (ori_label == -1)
                    newLabel = false;
                //通用标签处理，根据需要自定义
                for (int label_id = 0; label_id < vecFvKA1Boxes[i].label.size(); label_id++) {
                    int label = vecFvKA1Boxes[i].label[label_id];
                    if (vlabidx[ori_label] < vlabidx[label]) {
                        vecFvKA1Boxes[i].label.clear();
                    } else if (vlabidx[ori_label] == vlabidx[label]) {
                        if (ori_label == label)
                            newLabel = false;
                    } else {
                        newLabel = false;
                    }
                }
                if (newLabel) {
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                }
                if (vecFvKA1Boxes[i].camID[ori_label_id] == 0 || vecFvKA1Boxes[i].camID[ori_label_id] == 2) {
                    ex_info02[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info02[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info02[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam02++;

                } else {
                    ex_info13[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info13[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info13[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam13++;
                }
                yellow_score += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][3];
                gray_score += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][4];
            }
            if (ex_info02[2] > ex_info13[2]) {
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[0] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[1] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[2] / cam02);
            } else {
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[0] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[1] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[2] / cam13);
            }
            vecFvKA1Boxes[i].ex_info.push_back(yellow_score / (cam02 + cam13));
            vecFvKA1Boxes[i].ex_info.push_back(gray_score / (cam02 + cam13));
            //所有标签均被过滤，认为是好料
            if (vecFvKA1Boxes[i].label.empty()) {
                vecFvKA1Boxes[i].label.push_back(5);
            }
            if (!vecFvKA1Boxes[i].label.empty() && vecFvKA1Boxes[i].label[0] == 0) {
                //仅细分异色标签

                if (vecFvKA1Boxes[i].ex_info[4] < 1 - conf[2]) {
                    //优先判断灰、褐色、墨绿
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(2);
                } else if (vecFvKA1Boxes[i].ex_info[3] < 1 - conf[1]) {
                    //黄绿过渡色
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(1);
                } else {
                    //黄色
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(0);
                }
            }
        }
    }
}


/*!
 * \brief 孜然定制函数
 * \param vecFvKA1Boxes
 * \param vlabidx
 * \param CamNum
 * \param conf
 */
void GlobalFlow::ZiRanMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum)
{
    //根据面积大小选预测结果
    for (int i = 0; i < vecFvKA1Boxes.size(); i++) {
        if (vecFvKA1Boxes[i].count_flag == 1) {
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if (vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0) {
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info(3, 0);
            int max_area_camID = 0;
            for (int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++) {
                if (ex_info[2] < vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2]) {
                    ex_info[0] = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info[1] = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info[2] = vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    max_area_camID = vecFvKA1Boxes[i].camID[ori_label_id];
                }
            }
            vecFvKA1Boxes[i].ex_info.push_back(ex_info[0]);
            vecFvKA1Boxes[i].ex_info.push_back(ex_info[1]);
            vecFvKA1Boxes[i].ex_info.push_back(ex_info[2]);

            if (max_area_camID >= 2)
                max_area_camID -= 2;

            for (int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++) {
                bool newLabel = true;
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id];
                if (ori_label == -1)
                    newLabel = false;

                //过滤非最大面积下的预测结果
                if ((vecFvKA1Boxes[i].camID[ori_label_id] != max_area_camID ||
                vecFvKA1Boxes[i].camID[ori_label_id] != max_area_camID + 2))
                    newLabel = false;

                //通用标签处理，根据需要自定义
                if (newLabel) {
                    for (int label_id = 0; label_id < vecFvKA1Boxes[i].label.size(); label_id++) {
                        int label = vecFvKA1Boxes[i].label[label_id];
                        if (vlabidx[ori_label] < vlabidx[label]) {
                            vecFvKA1Boxes[i].label.clear();
                        } else if (vlabidx[ori_label] == vlabidx[label]) {
                            if (ori_label == label) {
                                newLabel = false;
                            }
                        } else {
                            newLabel = false;
                        }
                    }
                }
                if (newLabel) {
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                }
            }
            //所有标签均被过滤，认为是好料
            if (vecFvKA1Boxes[i].label.empty()) {
                vecFvKA1Boxes[i].label.push_back(2);
            }
        }
    }
}
void GlobalFlow::ShanZhaPianLHMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum,
                                   std::vector<float> conf)
{
    //标签顺序 nolab, sz
    //       山楂皮 山楂肉
    //新标签顺序 0, 1, 2, 3, 4, 5

    //ex_info 0：长 1：宽 2：面积 3：黄度 其中黄度0~100 越低表示越黄 4：亮度 越低越暗
    //需搭配绿豆颜色量化模型使用
    for (int i = 0; i < vecFvKA1Boxes.size(); i++) {
        if (vecFvKA1Boxes[i].count_flag == 1) {
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if (vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0) {
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info02(3, 0);
            int cam02 = 0;
            std::vector<float> ex_info13(3, 0);
            int cam13 = 0;
            float yellow_score = 0;
            float gray_score = 0;
            int cls_1_num = 0;
            for (int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++) {
                bool newLabel = true;
                int ori_label = vecFvKA1Boxes[i].oriLabel[ori_label_id];
                if (ori_label == -1)
                    newLabel = false;
                //通用标签处理，根据需要自定义
                for (int label_id = 0; label_id < vecFvKA1Boxes[i].label.size(); label_id++) {
                    int label = vecFvKA1Boxes[i].label[label_id];
                    if (vlabidx[ori_label] < vlabidx[label]) {
                        vecFvKA1Boxes[i].label.clear();
                    } else if (vlabidx[ori_label] == vlabidx[label]) {
                        if (ori_label == label)
                            newLabel = false;
                    } else {
                        newLabel = false;
                    }
                }
                if (newLabel) {
                    vecFvKA1Boxes[i].label.push_back(ori_label);
                }
                if (vecFvKA1Boxes[i].camID[ori_label_id] == 0 || vecFvKA1Boxes[i].camID[ori_label_id] == 2) {
                    ex_info02[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info02[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info02[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam02++;

                } else {
                    ex_info13[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info13[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info13[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam13++;
                }
                if(ori_label == 1){
                    yellow_score += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][3];
                    gray_score += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][4];
                    cls_1_num += 1;
                }
            }
            if (ex_info02[2] > ex_info13[2]) {
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[0] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[1] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[2] / cam02);
            } else {
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[0] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[1] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[2] / cam13);
            }
            if(cls_1_num != 0) {
                yellow_score = 1 - yellow_score / cls_1_num;
                gray_score = 1 - gray_score / cls_1_num;
                /*if(gray_score < 0.4)
                    gray_score = 0.4;
                if(gray_score > 0.8)
                    gray_score = 0.8;
                gray_score = (gray_score - 0.4) / (0.8 - 0.4);*/
            }
            else{
                yellow_score = 0;
                gray_score = 0;
            }

            vecFvKA1Boxes[i].ex_info.push_back(yellow_score);
            vecFvKA1Boxes[i].ex_info.push_back(gray_score);

            //所有标签均被过滤，认为是山楂肉侧
            if (vecFvKA1Boxes[i].label.empty()) {
                vecFvKA1Boxes[i].label.push_back(1);
            }
            if (!vecFvKA1Boxes[i].label.empty() && vecFvKA1Boxes[i].label[0] == 1) {
                //仅细分山楂肉标签
                if (vecFvKA1Boxes[i].ex_info[4] > conf[1]) {
                    //一级黑
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(1);
                } else if (vecFvKA1Boxes[i].ex_info[4] > conf[2]) {
                    //二级黑
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(2);
                } else if (vecFvKA1Boxes[i].ex_info[4] > conf[3]) {
                    //三级黑
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(3);
                } else if (vecFvKA1Boxes[i].ex_info[4] > conf[4]) {
                    //四级黑
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(4);
                } else if (vecFvKA1Boxes[i].ex_info[4] > conf[5]) {
                    //五级黑
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(5);
                }
                else{
                    //不分类
                    vecFvKA1Boxes[i].label.clear();
                    vecFvKA1Boxes[i].label.push_back(0);
                }
            }
        }
    }
}

/*!
 * \brief 薯条定制合框函数
 * \param vecFvKA1Boxes
 * \param vlabidx
 * \param CamNum
 * \param conf
 */
void GlobalFlow::STMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum, std::vector<float> conf)
{
    //标签顺序 25/25 2550/2550 5075/5075 75/75
    //        黄   黄绿   灰褐
    //ex_info 0：长 （*0.14后对应真实长度mm） 1：宽 2：面积
    for (int i = 0; i < vecFvKA1Boxes.size(); i++) {
        if (vecFvKA1Boxes[i].count_flag == 1) {
            vecFvKA1Boxes[i].count_flag = 2;
        }
        if (vecFvKA1Boxes[i].n == CamNum && vecFvKA1Boxes[i].count_flag == 0) {
            vecFvKA1Boxes[i].count_flag = 1;
            std::vector<float> ex_info02(3, 0);
            int cam02 = 0;
            std::vector<float> ex_info13(3, 0);
            int cam13 = 0;
            for (int ori_label_id = 0; ori_label_id < vecFvKA1Boxes[i].oriLabel.size(); ori_label_id++) {
                if (vecFvKA1Boxes[i].camID[ori_label_id] == 0 || vecFvKA1Boxes[i].camID[ori_label_id] == 2) {
                    ex_info02[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info02[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info02[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam02++;

                } else {
                    ex_info13[0] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][0];
                    ex_info13[1] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][1];
                    ex_info13[2] += vecFvKA1Boxes[i].ori_ex_info[ori_label_id][2];
                    cam13++;
                }
            }
            if (ex_info02[2] > ex_info13[2]) {
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[0] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[1] / cam02);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info02[2] / cam02);
            } else {
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[0] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[1] / cam13);
                vecFvKA1Boxes[i].ex_info.push_back(ex_info13[2] / cam13);
            }
            vecFvKA1Boxes[i].label.clear();

            if (vecFvKA1Boxes[i].ex_info[0] * 0.14 / 100 < 1 - conf[0]) {
                //最短
                vecFvKA1Boxes[i].label.push_back(0);
            } else if (vecFvKA1Boxes[i].ex_info[0] * 0.14 / 100 < 1 - conf[1]) {
                //偏短
                vecFvKA1Boxes[i].label.push_back(1);
            } else if (vecFvKA1Boxes[i].ex_info[0] * 0.14 / 100 < 1 - conf[2]) {
                //偏长
                vecFvKA1Boxes[i].label.push_back(2);
            } else{
                //最长
                vecFvKA1Boxes[i].label.push_back(3);
            }
        }
    }
}
