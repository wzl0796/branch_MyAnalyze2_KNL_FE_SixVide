#include "myudpcontrol.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"

namespace {
void synchronizeTraditionLabelSettings(struProfile *profile, int cameraIndex,
                                       const QMap<int, QString> &currentLabels)
{
    if (cameraIndex < 0 || cameraIndex >= MAX_CAM_NUM || currentLabels.isEmpty()) {
        return;
    }

    QStringList currentNames;
    for (auto it = currentLabels.constBegin(); it != currentLabels.constEnd(); ++it) {
        if (it.key() >= 0 && it.key() < MAX_LABELS) {
            currentNames[it.key()] = it.value();
        }
    }

    QStringList &savedNames = profile->traditionLabelNames[cameraIndex];
    if (!profile->bTraditionLabelNamesInitialized[cameraIndex]) {
        for (int i = 0; i < MAX_LABELS; ++i) {
            if (currentNames.at(i).isEmpty()) {
                profile->bTraditionLabelEn[cameraIndex][i] = false;
                profile->nTraditionLabelAlgorithm[cameraIndex][i] = 0;
            }
        }
    } else if (savedNames != currentNames) {
        bool oldIndexUsed[MAX_LABELS] = {};
        bool migrated[MAX_LABELS] = {};
        bool migratedEnabled[MAX_LABELS] = {};
        int migratedAlgorithm[MAX_LABELS] = {};

        for (auto it = currentLabels.constBegin(); it != currentLabels.constEnd(); ++it) {
            const int newIndex = it.key();
            if (newIndex < 0 || newIndex >= MAX_LABELS || it.value().isEmpty()) {
                continue;
            }
            for (int oldIndex = 0; oldIndex < MAX_LABELS && oldIndex < savedNames.size(); ++oldIndex) {
                if (!oldIndexUsed[oldIndex] && savedNames.at(oldIndex) == it.value()) {
                    migrated[newIndex] = true;
                    oldIndexUsed[oldIndex] = true;
                    migratedEnabled[newIndex] = profile->bTraditionLabelEn[cameraIndex][oldIndex];
                    migratedAlgorithm[newIndex] =
                        profile->nTraditionLabelAlgorithm[cameraIndex][oldIndex];
                    break;
                }
            }
        }

        for (auto it = currentLabels.constBegin(); it != currentLabels.constEnd(); ++it) {
            const int newIndex = it.key();
            if (newIndex < 0 || newIndex >= MAX_LABELS || migrated[newIndex] ||
                newIndex >= savedNames.size() || savedNames.at(newIndex).isEmpty() ||
                oldIndexUsed[newIndex]) {
                continue;
            }
            migrated[newIndex] = true;
            oldIndexUsed[newIndex] = true;
            migratedEnabled[newIndex] = profile->bTraditionLabelEn[cameraIndex][newIndex];
            migratedAlgorithm[newIndex] =
                profile->nTraditionLabelAlgorithm[cameraIndex][newIndex];
        }

        for (int i = 0; i < MAX_LABELS; ++i) {
            profile->bTraditionLabelEn[cameraIndex][i] =
                migrated[i] && migratedEnabled[i];
            profile->nTraditionLabelAlgorithm[cameraIndex][i] =
                migrated[i] ? qBound(0, migratedAlgorithm[i], TRADITION_ARITH_COUNT - 1) : 0;
        }
    }

    savedNames = currentNames;
    profile->bTraditionLabelNamesInitialized[cameraIndex] = true;
}
}

QMutex g_udpMutex;

myUdpControl::myUdpControl(int n_port, QObject *parent) : QObject(parent)
{
    m_port = n_port;
    m_udpSocket = new QUdpSocket();
    m_udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,200*1024*1024);

    if (m_udpSocket->bind(QHostAddress::AnyIPv4, m_port)) {
        connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(sltReadData()));
    }
}

/**
 * @brief myUdpControl::析构函数
 */
myUdpControl::~myUdpControl()
{

}

/**
 * @brief myUdpControl::setAnalyzeIndex
 * @param analyzeIndex
 */
void myUdpControl::setAnalyzeIndex(int analyzeIndex)
{
    m_port           = PORT_DATA + analyzeIndex;
    m_analyzeIndex   = analyzeIndex;
    m_pAnalyzeGlobal = gFlow.getSpecifyAnalyzeGlobal(m_analyzeIndex);
    m_pAnalyzeShare  = gFlow.getSpecifyAnalyzeShare(m_analyzeIndex);
}

/**
 * @brief myUdpControl::rebindSocket
 */
void myUdpControl::rebindSocket()
{
    if(m_udpSocket->state() == QAbstractSocket::BoundState){
        m_udpSocket->abort();
    }

    if (m_udpSocket->bind(QHostAddress::AnyIPv4, m_port)) {
        connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(sltReadData()));
    }
}

void myUdpControl::sltReadData()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        datagram.fill('\0');
        QHostAddress peerAddress;
        quint16 peerPort;

        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &peerAddress, &peerPort);

#if DEBUG_PRINT_READ
        int nSize = datagram.size() > 44 ? 16: datagram.size();
        printf("UDP R(%d)>> ", nSize);
        for(int i = 0; i < nSize ; ++i) {
            printf("%02x ", (uchar)datagram.at(i));
        }
        if (nSize >= 4030) {
            printf("..");
            for(int i = 4017; i < 4030; ++i) {
                printf("%02x ", (uchar)datagram.at(i));
            }
        }
        printf("\n");
        fflush(0);
#endif
        processDatagram(datagram);

    }
}

/**
 * @brief myUdpControl::控制板数据解析
 * @param datagram
 */
void myUdpControl::processDatagram(QByteArray datagram)
{
    if (datagram.data() == NULL || datagram.size() == 0) {
        return;
    }

    if (((char)datagram[0] != HEAD_AA || (char)datagram[1] != HEAD_AA
            || (char)datagram[2] != HEAD_AA || (char)datagram[3] != HEAD_AA)&&
            ((char)datagram[0] != HEAD_AD || (char)datagram[1] != HEAD_AD
             || (char)datagram[2] != HEAD_AD || (char)datagram[3] != HEAD_AD)) {
        printf("Head wrong! \n");
        fflush(0);
        return;
    }
    int nCmd = (quint8)datagram[8]*256 + (quint8)datagram[9];
    int nBoard = (uchar)datagram[6];

    switch (nCmd) {
    case CMD_VERSION: //! 控制板和相机板版本
        _analysisVersion(datagram);
        break;
    case CMD_SOC_VERSION:
        if(MODULE_SOC == nBoard)
            _analysisSocVersion(datagram);
        else if(MODULE_PULSE == nBoard)
            _analysisVersion(datagram);
        else if(MODULE_CAM == nBoard)
            _analysisVersion(datagram);
        else if(MODULE_HALOGEN == nBoard)
            _analysisVersion(datagram);
        break;
    case CMD_CURVE:
        _analysisCurve(datagram);
        break;
    case CMD_CHECK_MODE:
        if(nBoard == MODULE_SOC){
            _analysisAIUpdateStatus(datagram);
        }else{
            _analysisFpgaStatus(datagram);
        }
        break;
    case CMD_SOC_MODELINFO:  //! 模型信息查询
        _analysisSocmodelinfo(datagram);
        break;
    case CMD_SOC_MODELLAB_INFO: //! 模型标签信息查询
        _analysisSocmodellabinfo(datagram);
        break;
    case CMD_SOC_INFER_RESULT:
    case CMD_SOC_BOTTLE_INFERRESULT:
        _analysisSocInferRes(datagram);
        break;
    case CMD_UNIT_IM01_TEMPUTERE:
        _analysisIM01SensorTempure(datagram);
        break;
    case CMD_SOC_IMG_PARAMS:
        _analysisSocImgParams(datagram);
        break;
    case CMD_CTRL_REQMETAL:
        _analysisMetalNum(datagram);
        break;
    case CMD_HALOGEN_STATUS:
        _analysisHalogenStatus(datagram);
        break;
    case CMD_CLEANJAR_STATUS:
        _analysiscleanjarStatus(datagram);
        break;
    case CMD_AIR_ALARM:
        _analysisAirAlarm(datagram);
        break;
    case CMD_SOC_STAINFO: //! 统计类别
        _analysisStainfo(datagram);
        break;
    case CMD_CTRL_VLAVE_STA:
        _analysisCtrlValveSta(datagram);
        break;
    default:
        break;
    }
}

QByteArray myUdpControl::getContent(QByteArray array)
{

   return array.mid(12, getContentMarkSize(array));

}

/*!
 * \brief 获取回报的数据长度
 * \param array
 * \return
 */
int myUdpControl::getContentMarkSize(QByteArray array)
{

   return ((array.at(10) << 8) | (array.at(11)));

}

/**
 * @brief myUdpControl::解析版本
 * @param array
 */
void myUdpControl::_analysisVersion(QByteArray array)
{
    int nBoard = (uchar)array[6];
    int nAddr = (uchar)array[7];
    char data = 1;

    switch (nBoard) {
    case MODULE_CTRL:
        if (array[18] & (0x01 << 7)) {//! 控制板工厂模式下切回用户
            if(m_pAnalyzeShare->bCheckVerStartUp) {
                QByteArray ba;
                ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_CTRL, 0, CMD_UPDATE_USER, &data, 1);
                writeDatagram(ba);
                gFlow.msleep(200);
            } else{
                QString sctrl = QString("%1%2_V%3.0%4_F").arg((uchar)array[15],2,10,QChar('0'))
                        .arg((uchar)array[16],2,10,QChar('0'))
                        .arg((uchar)array[18] - 128)
                        .arg((uchar)array[19]);
                gFlow.qStringTochar(sctrl,m_pAnalyzeShare->sCtrlVersion,20);
            }
        } else {
            QString sctrl = QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                    .arg((uchar)array[16],2,10,QChar('0'))
                     .arg((uchar)array[18])
                     .arg((uchar)array[19]);
            gFlow.qStringTochar(sctrl,m_pAnalyzeShare->sCtrlVersion,20);
        }
        break;
    case MODULE_PULSE:
        if (array[18] & (0x01 << 7)) {  //! 光源板工厂模式切到用户模式
            if(m_pAnalyzeShare->bCheckVerStartUp) {
                QByteArray ba;
                ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_PULSE, 0, CMD_UPDATE_USER, &data, 1);
                writeDatagram(ba);
                gFlow.msleep(200);
            } else{
                QString splus = QString("%1%2_V%3.0%4_F").arg((uchar)array[15],2,10,QChar('0'))
                        .arg((uchar)array[16],2,10,QChar('0'))
                        .arg((uchar)array[18] - 128)
                        .arg((uchar)array[19]);
                gFlow.qStringTochar(splus,m_pAnalyzeShare->sPulseVersion,20);
            }
        } else {
            QString splus= QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                    .arg((uchar)array[16],2,10,QChar('0'))
                     .arg((uchar)array[18])
                     .arg((uchar)array[19]);
            gFlow.qStringTochar(splus,m_pAnalyzeShare->sPulseVersion,20);
        }
        break;
    case MODULE_CAM:
        if (array[18] & (0x01 << 7)) {
            if(m_pAnalyzeShare->bCheckVerStartUp) {
                QByteArray ba;
                ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, nAddr, CMD_UPDATE_USER, &data, 1);
                writeDatagram(ba);
                gFlow.msleep(200);
            } else {
                if(32 == nAddr && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
                    nAddr = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
                }
                QString scam = QString("%1%2_V%3.0%4_F").arg((uchar)array[15],2,10,QChar('0'))
                        .arg((uchar)array[16],2,10,QChar('0'))
                        .arg((uchar)array[18] - 128)
                        .arg((uchar)array[19]);
                gFlow.qStringTochar(scam,m_pAnalyzeShare->sCamVersion[nAddr],20);
            }
        } else {
            if(32 == nAddr && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
                nAddr = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
            }
            QString scam = QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                    .arg((uchar)array[16],2,10,QChar('0'))
                    .arg((uchar)array[18])
                    .arg((uchar)array[19]);
            gFlow.qStringTochar(scam,m_pAnalyzeShare->sCamVersion[nAddr],20);
        }
        break;
    case MODULE_HALOGEN:{
        QString shalogen = QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                .arg((uchar)array[16],2,10,QChar('0'))
                .arg((uchar)array[17])
                .arg((uchar)array[18]);
         gFlow.qStringTochar(shalogen, m_pAnalyzeShare->sHalogenVersion, 20);
        break;
    }
    case MODULE_SVBOARD:{
        QString ssnv = QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                .arg((uchar)array[16],2,10,QChar('0'))
                .arg((uchar)array[17])
                .arg((uchar)array[18]);
        gFlow.qStringTochar(ssnv,m_pAnalyzeShare->sSVBoardVersion,20);
        break;
    }
    case MODULE_VAL:{
        QString sval = QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                           .arg((uchar)array[16],2,10,QChar('0'))
                           .arg((uchar)array[17])
                           .arg((uchar)array[18]);
        gFlow.qStringTochar(sval,m_pAnalyzeShare->sValveBoardVersion,20);
        break;
    }
    default:
        break;
    }
}

/**
 * @brief myUdpControl::解析SOC版本
 * @param array
 */
void myUdpControl::_analysisSocVersion(QByteArray array)
{
    int nAddr = (uchar)array[7];

    if((nAddr >MAX_CAM_NUM &&nAddr!=32)||(nAddr<0)){
        printf("wave error: camaddr error");fflush(0);
        return;
    }

    if(32 == nAddr && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        nAddr = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
    }

    if(nAddr < MAX_CAM_NUM){
        QString ssoc = QString("%1%2_V%3.0%4").arg((uchar)array[15],2,10,QChar('0'))
                .arg((uchar)array[16],2,10,QChar('0'))
                .arg((uchar)array[18])
                .arg((uchar)array[19]);
        gFlow.qStringTochar(ssoc,m_pAnalyzeShare->sSocVersion[nAddr],20);
    }
}

/**
 * @brief myUdpControl::解析波形
 * @param array
 */
void myUdpControl::_analysisCurve(QByteArray array)
{
    int nDataLength = (array[10]&0xff)*256+(array[11]&0xff);
    QByteArray ba = array.mid(12, nDataLength);

    int nAddr = (uchar)array[7];

    if((nAddr >MAX_CAM_NUM &&nAddr!=32)||(nAddr<0)){
        printf("wave error: camaddr error");fflush(0);
        return;
    }

    if(32 == nAddr && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        nAddr = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
    }

    if (struGsh.nIsCapaturing) {
        emit sigImageDataReady(ba, nDataLength, nAddr);
    } else {
        memcpy(struGsh.sRgbRow, ba.data(), nDataLength);
    }
}

/**
 * @brief myUdpControl::解析升级状态
 * @param array
 */
void myUdpControl::_analysisFpgaStatus(QByteArray array)
{
    switch (m_pAnalyzeShare->nUpdateType) {
    case UPGRADE_FACTORY:
        if ((uchar)array[17] & MASK_CTRL_LOAD_FACTORY_MODE) {
            m_pAnalyzeShare->nUpgradeStatusInfo = true;
        }
        break;
    case UPGRADE_SEND:
        if ((uchar)array[17] & MASK_CTRL_BURN_FLASH_FILE) {
            m_pAnalyzeShare->nUpgradeStatusInfo = true;
        }
        break;
    case UPGRADE_BURN:
        if ((uchar)array[17] & MASK_CTRL_BURN_SDRAM_PAGE) {
            m_pAnalyzeShare->nUpgradeStatusInfo = true;
        }
        break;
    case UPGRADE_USER:
        if ((uchar)array[17] & MASK_CTRL_LOAD_CUSTOM_MODE) {
            m_pAnalyzeShare->nUpgradeStatusInfo = true;
        }
        break;
    case UPGRADE_ERASE:
        if ((uchar)array[17] & MASK_CTRL_ERASE) {
            m_pAnalyzeShare->nUpgradeStatusInfo = true;
        }
        break;
    default:
        break;
    }
}


/**
 * @brief myUdpControl::解析AI板升级状态
 * @param array
 */
void myUdpControl::_analysisAIUpdateStatus(QByteArray array)
{
    m_pAnalyzeShare->bAIUpdStatus = (uchar)array[12];
    qDebug("rsv soc upgrade result  %d",m_pAnalyzeShare->bAIUpdStatus);
}

/**
 * @brief  解析模型信息
 */
void myUdpControl::_analysisSocmodelinfo(QByteArray array)
{
    uchar nUintadd = (uchar)array.at(7);
    if(nUintadd > MAX_CAM_NUM && nUintadd !=32){
        printf("rsv model info err: cam addr error");fflush(0);
        return;
    }

    if(nUintadd == 32 && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        nUintadd = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
    }

    QByteArray infoarrry = array.mid(12, getContentMarkSize(array));

    StruSocModelInfo_L modelInfoTmp;
    memset(&modelInfoTmp, 0, sizeof(StruSocModelInfo_L));

    int nmin =qMin(infoarrry.size(), (int)sizeof(StruSocModelInfo_L));
    memcpy(&modelInfoTmp, infoarrry.data(), nmin);

    memcpy(struGsh.sModelUnitInfo[nUintadd].model, (modelInfoTmp.model),
           sizeof(SocModelInfo_L )* MAX_SOC_MODEL_COUNT_L );
    struGsh.sModelUnitInfo[nUintadd].currentId = modelInfoTmp.currentId;
    struGsh.sModelUnitInfo[nUintadd].state = modelInfoTmp.state;
    if(struGsh.sModelUnitInfo[nUintadd].state == 0){
        struGsh.bSocModelUnitResult[nUintadd] = true;
    }else{
        struGsh.bSocModelUnitResult[nUintadd] = false;
    }

    //! 判断当前SOC模型总数
    int nModel = 0;
    for(int j=0; j<20; j++){
        if(QString(struGsh.sModelUnitInfo[nUintadd].model[j].cnf) != "NULL"){
            nModel += 1;
            struGsh.nAIModel[nUintadd] = nModel;
        }
    }
}

/**
 * @brief 解析模型标签信息
 */
void myUdpControl::_analysisSocmodellabinfo(QByteArray array)
{
    uchar nUintadd = (uchar)array.at(7); //! 相机地址

    if(nUintadd > MAX_CAM_NUM && nUintadd !=32){
        printf("rsv model info err: cam addr error");fflush(0);
        return;
    }

    if(nUintadd == 32 || nUintadd >= gFlow.getMachineCamNum()){
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum >= struGsh.vlabNamemap.size()){
            nUintadd = struGsh.vlabNamemap.size() - 1;
        }else{
            nUintadd = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
        }
    }



    char fileName[MAX_SOC_MODEL_LABEL_L][MAX_SOC_MODEL_NAME+1];
    memset(fileName, 0, sizeof(fileName));
    int nMaxCount = 0, nLablen = 0;

    QByteArray infoarrry = array.mid(12, getContentMarkSize(array));
    if(infoarrry.size() == 904 || infoarrry.size() == 902){ //!30个标签
        nMaxCount = MAX_SOC_MODEL_LABEL_L;
        nLablen = 30;
    }else if(infoarrry.size() == 1204 || infoarrry.size() == 1202){
        nMaxCount = MAX_SOC_MODEL_LABEL_L;
        nLablen = 40;
    }else if(infoarrry.size() == 452){
        nMaxCount = MAX_SOC_MODEL_LABEL;
        nLablen = 30;
    }else if(infoarrry.size() == 602){  //!15个标签
        nMaxCount = MAX_SOC_MODEL_LABEL;
        nLablen = 40;
    }else{
        return;
    }

    for(int i=0; i<nMaxCount; i++){
        int srcoff = 2+i*nLablen;
        if(srcoff+nLablen <=infoarrry.size()){
            int copyLen = qMin(nLablen,(int)MAX_SOC_MODEL_NAME);
            memcpy(fileName[i],infoarrry.constData()+srcoff,copyLen);
            fileName[i][copyLen] = '\0';
        }
    }

    struGsh.vlabNamemap[nUintadd].clear();
    struGsh.vlabEngNamemap[nUintadd].clear();
    int nSocModellabnum = infoarrry.at(0)*256 + infoarrry.at(1); //! 标签个数
    int nCount = qMin(nMaxCount, nSocModellabnum);

    for(int nLabelIndex=0; nLabelIndex<nCount; nLabelIndex++){
        QString labelName = QString::fromLocal8Bit(fileName[nLabelIndex]);
        QStringList labstrlst = labelName.split("/");
        QString labname,labEngname;

        labname = labstrlst.at(0);

        if(myLan.nLang != LANG_CHS && labstrlst.size()>1){
            labname = labstrlst.at(1);
        }

        if(labstrlst.size()>1){
            labEngname = labstrlst.at(1);
        }

        if(labelName.length()>0){
            struGsh.vlabNamemap[nUintadd][nLabelIndex] = labname;

            if(!labEngname.isEmpty()){
                struGsh.vlabEngNamemap[nUintadd][labelName] = labEngname;
            }
        }
    }

    bool bchg = false;
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    QList<QString> list1 = struGsh.vlabNamemap[nUintadd].values();
    QList<QString> list2 = pProfile->strctAI.vlabelidxmap[nUintadd].keys();
    QList<QString> list3 = pProfile->strctAI.vlabthdmap.keys();
    QList<QString> Mergerlab = pProfile->vMegerlabmap[nUintadd].values();
    QList<QString> labidxlst = pProfile->vlabIdxmap[nUintadd].keys();
    
    
    synchronizeTraditionLabelSettings(pProfile, nUintadd, struGsh.vlabNamemap[nUintadd]);


    if(list1.contains(pProfile->slabSecondary[nUintadd]) == false){
        pProfile->slabSecondary[nUintadd] = QString();
    }

    //! 重置标签优先级
    int n= list1.size();
    if(n == list2.size()){
        for(int i=0 ;i<n; i++){
            if(!list2.contains(list1.at(i))){
                bchg = true;
                break;
            }
        }
    }else{
        bchg = true;
    }


    if(bchg || list2.size() ==0){
        gFlow.ResetAIlabPri();
        gFlow.ResetAIlabMutex();
    }


    //! 重置合并标签
    bool bMegerChg = false;
    for(QString var : Mergerlab){
        if(!list1.contains(var)){
            bMegerChg = true;
            break;
        }
    }

    if(bMegerChg){
        pProfile->vMegerlabmap[nUintadd].clear();
    }


    //! 重置标签系数及阈值
    bool bthlabchg = false;
    for(int i=0; i<n; i++){
        if(!list3.contains(list1.at(i))){
            bthlabchg = true;
            break;
        }
    }

    if(bthlabchg || list3.size() ==0){
        gFlow.ResetLabthdhod();
        gFlow.ResetLabArea2Wcof();
        gFlow.ResetLabproperty();
    }


    //! 重置合并后的标签集合
    gFlow.ResetMegreLabLst(nUintadd);

    //! 重置标签顺序
    bool bLabIdx = false;
    if(labidxlst.size() != list1.size()){
        bLabIdx = true;
    }else{
        for(QString &var : list1){
            if(!labidxlst.contains(var)){
                bLabIdx = true;
                break;
            }
        }
    }

    if(bLabIdx){
        gFlow.ResetLabIdx(nUintadd);
    }

    //! 校验映射标签是否有效
    gFlow.validateLabMixMap(nUintadd);

}


/*!
 * \brief 解析SOC推理结果
 * \param array
 */
void myUdpControl::_analysisSocInferRes(QByteArray array)
{
    bool bIMserialCam = false;
    int ncamid = (uchar)array.at(7);
    int nMajor = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;


    if(((ncamid >= MAX_CAM_NUM || ncamid >= nMajor )&&(ncamid!=32)) || ncamid <0){
        printf("camid error \n");fflush(0);
        return;
    }

    if(ncamid ==32){
        ncamid = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
        bIMserialCam = true;
    }

    uint nRow = (array.at(12)&0xff)*256*256*256
                + (array.at(12+1)&0xff)*256*256
                +(array.at(12+2)&0xff)*256
                + (array.at(12+3)&0xff);

    if(array.size() == 4392){//! 框
        int nRect = array.at(12+4);
        int nsize = sizeof(BBox);

        std::vector<BBox> vBox;
        for(int i=0; i<nRect; i++){
            BBox box;
            int noffset = 17 + i*nsize;
            memcpy(&box, array.data() + noffset, nsize);
            vBox.push_back(box);
        }

//        for(int n=0; n<vBox.size(); n++){
//            qDebug("nCamid =%d, pro=%02lf",ncamid,vBox[n].prob);
//        }

        if(struGsh.bCalibrate){//! 标定坐标点
            if(vBox.size() != 0){
                struGsh.calipoint[ncamid].setX(vBox[0].cx);
                struGsh.calipoint[ncamid].setY(vBox[0].cy);
            }
        }else if(struGsh.bCalWaterContent && bIMserialCam){//! 标定含水量
            if(vBox.size() > 0 && vBox[0].ex_info[1] >0){
                struGsh.nCalValue =  vBox[0].ex_info[1];
            }
        }else{
            emit sigSocInferResultReady(vBox, ncamid, nRow);
        }
    }else{
//        int nlab = gFlow.getLabstacatNum(ncamid);
//        int nmat = gFlow.getMatstacatNum(ncamid);
//        int ntotal = 30*nlab*4 + nmat*4;
        int nTotal = ((array.at(10) &0xff)<<8) +(array.at(11) &0xff);
        QByteArray parray = array.mid(16, nTotal);
        emit sigSocInferBottleRest(parray, ncamid, nRow);
    }
}

/*!
 * \brief 解析IM01传感器类型
 * \param array
 */
void myUdpControl::_analysisIM01SensorTempure(QByteArray array)
{
    int tmpture = (array[13]&0xff)*256 + (array[14]&0xff);
    struGsh.nInfraTempure = tmpture;
    emit sigIM01SensorTempture(tmpture);
}

/*!
 * \brief 解析soc图像信息
 * \param array
 */
void myUdpControl::_analysisSocImgParams(QByteArray array)
{
   //! 输入图片的高度
    struGsh.nSocImgHei = ((array[12]&0xff)*256) + (array[13]&0xff);

    //! 输入图片的宽度
    struGsh.nSocImgWid = ((array[14]&0xff)*256) + (array[15]&0xff);
}

/*!
 * \brief 解析金属检测个数
 * \param array
 */
void myUdpControl::_analysisMetalNum(QByteArray array)
{
    struGsh.nMetaNum = ((array[13]&0xff) <<8) + (uchar)array[14];
}

/*!
 * \brief 解析卤素灯状态
 * \param array
 */
void myUdpControl::_analysisHalogenStatus(QByteArray array)
{
    struGsh.nHalogenValue[0] = ((array[12]&0xff)<<8) +(uchar)array[13];
    struGsh.nHalogenValue[1] = ((array[14]&0xff)<<8) +(uchar)array[15];
}

/*!
 * \brief 清灰缸状态
 * \param array
 */
void myUdpControl::_analysiscleanjarStatus(QByteArray array)
{
    struGsh.ncleanjarstatus = array[13]&0x01;
}

/*!
 * \brief 解析气压报警
 * \param array
 */
void myUdpControl::_analysisAirAlarm(QByteArray array)
{
    struGsh.nAirStatus = array[13]&0x01;
}

/*!
 * \brief 解析气压报警
 * \param array
 */
void myUdpControl::_analysisCtrlValveSta(QByteArray array)
{
    struGsh.bCtrlVlaveAlarm = array[13]&0x01;
}

void myUdpControl::_analysisInfraTempure(QByteArray array)
{
    struGsh.nInfraTempure = ((array[10]&0xff)<<8) | (array[11]&0xff);
}

/*!
 * \brief 解析统计类别信息
 * \param array
 */
void myUdpControl::_analysisStainfo(QByteArray array)
{
    int ncamid = (uchar)array.at(7);
    if(((ncamid >= MAX_CAM_NUM)&&(ncamid!=32)) || ncamid <0){
        printf("camid error \n");fflush(0);
        return;
    }

    if(ncamid == 32 || ncamid>=gFlow.getMachineCamNum()){
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum >= struGsh.vlabNamemap.size()){
            ncamid = struGsh.vlabNamemap.size() - 1;
        }else{
            ncamid = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum;
        }
    }

    int nLabcat = ((array[16]&0xFf)<<24) + ((array[17]&0xFf)<<16)
                  +((array[18]&0xFf)<<8) + (array[19]&0xFf);

    int nLabpro = ((array[20]&0xFf)<<24) + ((array[21]&0xFf)<<16)
                  +((array[22]&0xFf)<<8) + (array[23]&0xFf);

    int nMatcat = ((array[24]&0xFf)<<24) + ((array[25]&0xFf)<<16)
                  +((array[26]&0xFf)<<8) + (array[27]&0xFf);

    int nMatPro = ((array[28]&0xFf)<<24) + ((array[29]&0xFf)<<16)
                  +((array[30]&0xFf)<<8) + (array[31]&0xFf);


    struGsh.nModeltype =  (uchar)array[32];
    struGsh.nMergertype = (uchar)array[33];

    QByteArray  balabcat;
    QStringList lstLabcat;
    if(nLabcat>0){
        balabcat= array.mid(34,nLabcat);
        QStringList tmplst = QString(balabcat).trimmed().split(" ");
        for(QString val : tmplst){
            if(val.contains("/")){
                if(myLan.nLang == LANG_CHS){
                    lstLabcat<<val.split("/").at(0);
                }else{
                    lstLabcat<<val.split("/").at(1);
                }
            }else{
                lstLabcat<< val;
            }
        }
    }

    QByteArray  balabpro;
    QStringList lstLabpro;
    if(nLabpro>0){
        balabpro =  array.mid(34+nLabcat , nLabpro);
        lstLabpro = QString(balabpro).trimmed().split(" ");
    }


    QByteArray  bamatcat;
    QStringList lstmatcat;
    if(nMatcat>0){
        bamatcat = array.mid(34+nLabcat+nLabpro, nMatcat);
        QStringList tmpmatlst = QString(bamatcat).trimmed().split(" ");
        for(QString val : tmpmatlst){
            if(val.contains("/")){
                if(myLan.nLang == LANG_CHS){
                    lstmatcat<<val.split("/").at(0);
                }else{
                    lstmatcat<<val.split("/").at(1);
                }
            }else{
                lstmatcat<<val;
            }
        }
    }

    QByteArray  bamatpro;
    QStringList lstmatpro;
    if(nMatPro>0){
        bamatpro = array.mid(34+nLabcat+nLabpro+nMatcat, nMatPro);
        lstmatpro = QString(bamatpro).trimmed().split(" ");
    }

    int nGbid =  gFlow.getStaticIdfromCamid(ncamid);
    struGsh.vlabStamap[nGbid].clear();
    struGsh.vlabTotalmap[nGbid].clear();

    qDebug()<<"ncamid"<<ncamid << "nGbid" << nGbid<<"labca: "<<lstLabcat;
    qDebug()<<"ncamid"<<ncamid << "nGbid" << nGbid<<"labpro: "<<lstLabpro;
    qDebug()<<"ncamid"<<ncamid << "nGbid" << nGbid<<"matcat: "<<lstmatcat;
    qDebug()<<"ncamid"<<ncamid << "nGbid" << nGbid<<"matpro: "<<lstmatpro;

    foreach (QString var, struGsh.vlabNamemap[ncamid]) {
        int nMin = qMin(lstLabpro.size(),lstLabcat.size());
        if(!lstLabpro.isEmpty() && !lstLabcat.isEmpty()){
            slabSta labsta;
            labsta.name = var;

            for(int k=0; k<nMin; k++){
                QList<float> lst={0,0,0,0};
                labsta.staRes[lstLabcat.at(k)] = lst;
                labsta.staInfo[lstLabcat.at(k)] = lstLabpro.at(k);


                QString typestr =  lstLabpro.at(k);
                QString nstatype =  typestr.split("/").at(1);
                // 正确解析 ntype，支持两位数类型（如 A10）
                int ntype =  nstatype.mid(1).toInt();
                if(ntype == STATISTIC_A3 || ntype == STATISTIC_A4 ||
                    ntype == STATISTIC_A5 ||ntype == STATISTIC_A6 ||
                    ntype == STATISTIC_A8 || ntype == STATISTIC_A10){
                    struGsh.vlabTotalmap[nGbid][lstLabpro.at(k)] = 0.0;
                }
            }
            struGsh.vlabStamap[nGbid][var] = labsta;
        }
    }

    struGsh.vsMatinfo[nGbid].staInfo.clear();
    struGsh.vsMatinfo[nGbid].staRes.clear();
    struGsh.vmatTotalmap[nGbid].clear();


    int nMinidx = qMin(lstmatcat.size(),lstmatpro.size());
    if(nMinidx>0){
        for(int n=0; n<nMinidx; n++){
            QList<float> reslst={0, 0, 0, 0};
            struGsh.vsMatinfo[nGbid].staRes[lstmatcat.at(n)] = reslst;
            struGsh.vsMatinfo[nGbid].staInfo[lstmatcat.at(n)] = lstmatpro.at(n);

            QString typestr  =  lstmatpro.at(n);
            QString nstatype =  typestr.split("/").at(1);
            int ntype =  nstatype.mid(1).toInt();

            if(ntype == STATISTIC_A3 || ntype == STATISTIC_A4 ||
                ntype == STATISTIC_A5 ||ntype == STATISTIC_A6 ||
                ntype == STATISTIC_A8 || ntype == STATISTIC_A10 ){
                struGsh.vmatTotalmap[nGbid][lstmatpro.at(n)] = 0.0;
            }
        }
    }

    //! 解析统计类别
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    bool bChg = false;

    if(pProfile->staclassEnmap.isEmpty()){
        bChg = true;
    }else{
        for(const auto val : lstLabcat){
            if(!pProfile->staclassEnmap.contains(val)){
                bChg = true;
                break;
            }
        }
    }

    if(bChg){
        pProfile->staclassEnmap.clear();
        for(const auto val : lstLabcat){
            pProfile->staclassEnmap[val] = true;
        }
    }
}


/**
 * @brief myUdpControl::UDP发送数据
 * @param data
 * @param nCount
 * @param port
 */
void myUdpControl::writeDatagram(QByteArray data, int nCount, quint16 port)
{
    QHostAddress address = QHostAddress(DEST_IP+QString(".%1").arg(m_analyzeIndex+1));
    for (int i = 0; i < nCount; i++) {
        m_udpSocket->writeDatagram(data, data.size(), address, port);
    }
#if DEBUG_PRINT_WRITE
    int nSize = data.size();
    printf("udp send(size: %d)>> ", nSize);
    if(nSize<100){
        for(int i = 0; i < nSize; ++i) {
            printf("%02x ", (uchar)data[i]);
        }
    }else{
        for(int i = 0; i < 20; ++i) {
            printf("%02x ", (uchar)data[i]);
        }

        printf("....");
        for(int i = nSize-20; i < nSize; ++i) {
            printf("%02x ", (uchar)data[i]);
        }

    }

    printf("\n");
    fflush(0);
#endif
}

/**
 * @brief myUdpControl::UDP数据接收
 * @param data
 * @param nCount
 * @param port
 */
int myUdpControl::readDatagram(char *data, int ndataLen)
{
    return m_udpSocket->readDatagram(data, ndataLen);
}

/*!
 * \brief 清空udp数据包
 */
void myUdpControl::clearUdpSocketPortData()
{
    while(m_udpSocket->hasPendingDatagrams()){
        char *tmp = (char *)malloc(m_udpSocket->pendingDatagramSize());
        if(tmp == NULL){
            return;
        }

        m_udpSocket->readDatagram(tmp, sizeof(tmp));
        free(tmp);
    }
}

/*!
 * \brief myUdpControl::write
 * \param ba
 * \param bResponse
 * \param bBoradCast
 * \return
 */
bool myUdpControl::write(QByteArray ba, bool bResponse, bool bBoradCast)
{
#ifdef PRINT_UDP
    print("UDP W", ba);
#endif

//    //! 检查网口状态
//    if (!m_socketW->isValid() || !m_socketWR->isValid()) {
//        return false;
//    }

//    //! 获取地址
//    int idx = ba[BOARD_INDEX] >> 4;
//    QHostAddress addr = _getAddress(idx, bBoradCast);

//    //! 若字节数超过阈值则不再重发
//    int nTimes = ba.size() < BYTEARRAY_SIZE ? REPEAT_TIMES : 1;

//    qint64 nRet = 0;
//    if (bResponse) {
//        _clearBuffer(); //! 带回包命令先清除读缓存
//        for (int i = 0; i < nTimes; i++) {
//            nRet = m_socketWR->writeDatagram(ba, addr, PORT_DST);
//        }
//    } else {
//        for (int i = 0; i < nTimes; i++) {
//            nRet = m_socketW->writeDatagram(ba, addr, PORT_DST);
//        }
//    }

//    if (nRet == ba.size()) {
//        return true;
//    }
    return false;
}

QByteArray myUdpControl::read(int nWaitTime)
{
    //! 检查网口状态
//    if (!m_socketWR->isValid()) {
//        return QByteArray();
//    }

//    QTime time;
//    time.start();
    QByteArray ba;
//    do {
//        if (m_socketWR->hasPendingDatagrams()) {
//            int nSize = m_socketWR->pendingDatagramSize();
//            ba.resize(nSize);

//            QHostAddress address;
//            quint16 port;
//            m_socketWR->readDatagram(ba.data(), ba.size(), &address, &port);
//            break;
//        }
//        Utility::msleep(1);
//    }
//    while (time.elapsed() < nWaitTime);

//#ifdef PRINT_UDP
//    print("UDP R", ba);
//#endif
    return ba;
}

/*!
 * \brief myUdpControl::writeRead
 * \param ba
 * \param nWaitTime
 * \return
 */
QByteArray myUdpControl::writeRead(QByteArray ba, int nWaitTime)
{
    QMutexLocker _LOCKER(&m_mutex);
    write(ba, true);
    return read(nWaitTime);
}
