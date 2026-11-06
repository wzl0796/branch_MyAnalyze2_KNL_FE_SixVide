
#include "homewidget.h"

homeWidget::homeWidget(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief homeWidget::初始化参数
 */
void homeWidget::initParams()
{
    vPurityGp.resize(MAX_CAM_NUM);
    memset(m_pEjData,0,MAX_EJECTOR_PACKET_ROW*EJ_SIZE_ROW);
    memset(m_cacheEjData,0,MAX_EJECTOR_PACKET_ROW*EJ_SIZE_ROW);
}

/**
 * @brief homeWidget::界面构造
 */
void homeWidget::createPage()
{
    setFixedSize(WIN_WID-160, WIN_HEI-180);

    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &homeWidget::timeOutSlt);

    m_analyzeStaPage = new AnalyzeTestDlg(struGsh.nAnalyzeIndex);

    QHBoxLayout* g_Lay = new QHBoxLayout(this);
    g_Lay->addWidget(m_analyzeStaPage);
}

/**
 * @brief homeWidget::信号槽绑定
 */
void homeWidget::connectSigAndSlot()
{
    connect(m_timer,    SIGNAL(timeout()),              this,               SLOT(timeOutSlt()));
    connect(this,       SIGNAL(sigRefreshSimRes(QString)),    m_analyzeStaPage,   SLOT(onRefreshSimRes(QString)));
}

/**
 * @brief homeWidget::界面刷新
 */
void homeWidget::refreshDisPlay(bool bflag)
{
    m_analyzeStaPage->refreshStatusItem(bflag);
}


void homeWidget::startorstoptimer(bool bflag)
{
    if(bflag){
        m_timer->start();
    }else{
        m_timer->stop();
    }
}

/*!
 * \brief 刷新页面
 * \param vRes
 */
void homeWidget::onrefreshHomepaheBySocInfer(std::vector<BBox> vBox, int nCamId, uint nRow)
{
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample && (!struGsh.bStatics)){
        return;
    }

    //! 以行号为索引，记录相机序号为nCamId的矩形框（裁剪小图）
    if(struGsh.bCapimg && vBox.size() > 0){
        if(!struGsh.lineBoxMap.contains(nRow)){
            struGsh.lineBoxMap[nRow] = QMap<int, QList<BBox>>();
        }

        QList<BBox> boxes;
        for(size_t i = 0; i < vBox.size(); ++i){
            BBox box = vBox[i];
            boxes.append(box);
        }

        struGsh.lineBoxMap[nRow][nCamId] = boxes;

        //! 清理旧的行号记录，只保留最近的行号
        // clearLineBoxMapBeforeRow(nRow - 100); // 保留最近100行的记录
    }

    //! 清空当前相机旧的映射记录，避免显示上帧残留数据（仿真）
    if(struGsh.bEnSimulate &&
        nCamId < struGsh.vCamLabelMapping.size()){
        struGsh.vCamLabelMapping[nCamId].clear();
    }

    std::vector<BBox> tmpBox = _preProcessBox(vBox, nCamId);
    struGsh.vSocInfer[nCamId] = tmpBox;


    //! 首页统计
    if(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[nCamId]){//! 需要合并结果
        QList<int> camidLst = struGsh.socLineinfomap.keys();
        struGsh.socLineinfomap[nCamId] = nRow;

        if(bRsvallcamRes()){
            std::vector<myfvKA1Box> vecfvKA1Boxes;
            vecfvKA1Boxes = preProBox(camidLst,false);
            _statisticRes(vecfvKA1Boxes,camidLst.at(0));

            if(struGsh.bCalibrateSoc){ //! 标定时缓存框
                for (int cameraIndex = 0; cameraIndex < struGsh.vSocInfer.size(); ++cameraIndex) {//! 满足全相机条件后累计保存识别框
                    if(struGsh.vSocInfer[cameraIndex].size() > 0){
                        std::vector<BBox> &readyBoxes = struGsh.vSocInferReady[cameraIndex];
                        const std::vector<BBox> &currentBoxes = struGsh.vSocInfer[cameraIndex];
                        const size_t remaining = readyBoxes.size() < 500 ? 500 - readyBoxes.size() : 0;
                        const size_t copyCount = qMin(remaining, currentBoxes.size());
                        readyBoxes.insert(readyBoxes.end(), currentBoxes.begin(), currentBoxes.begin() + copyCount);
                    }
                }
            }

            //! 喷阀数据处理
            if(!struGsh.bCapimg && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal){
                _disposeEjData(vecfvKA1Boxes,nRow);
            }

            if(struGsh.bEnSimulate){//! 仿真
                _staticCamLabels(vecfvKA1Boxes);
            }
        }
    }else{
        std::vector<myfvKA1Box> vecfvKA1Boxes;
        QList<int> camlst;
        camlst<<nCamId;

        vecfvKA1Boxes = preProBox(camlst,false);
        _statisticRes(vecfvKA1Boxes,nCamId);
    }
}

/*!
 * \brief 刷新SOC统计结果
 * \param pdata
 * \param nCamId
 * \param nRow
 */
void homeWidget::onrefreshpagebySocBottleInfer(QByteArray parray, int nCamId, uint nRow)
{
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample && (!struGsh.bStatics)){
        return;
    }

    _calDen(parray, nCamId);
    _staMat(parray, nCamId);
    _staLab(parray, nCamId, nRow);
}

/*!
 * \brief 槽: 定时器定时刷新检测结果
 */
void homeWidget::timeOutSlt()
{
    if(struGsh.bEnSimulate){
//        emit sigRefreshSimRes(struGsh.simRes);
//        QMutexLocker lock(&m_mutex);
//        struGsh.simRes.clear();
        return;
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample && struGsh.bStatics){
        m_analyzeStaPage->refreshInferResult();
    }else{
        m_analyzeStaPage->refreshInferResult();
    }
}


/*!
 * \brief 自动取样时，刷新统计
 * \param nCount
 * \param bRest
 */
void homeWidget::onResetStaticsResult(int nCount, bool bRest)
{
    Q_UNUSED(nCount);
    if(bRest){
        m_analyzeStaPage->refreshInferResult();
    }
}

void homeWidget::onChangeScheme()
{
    m_analyzeStaPage->refreshStatusItem(true);
}

void homeWidget::onSimulateEnchg(bool bEn)
{
    Q_UNUSED(bEn)
    m_analyzeStaPage->refreshStatusItem(true);
}

/*!
 * \brief 判断是否同时收到所有相机的推理结果
 * \return
 */
bool homeWidget::bRsvallcamRes()
{
    bool bReady = true;
    QList<uint> nRowlist = struGsh.socLineinfomap.values();
    for(int i=0; i<nRowlist.size()-1; i++){
        if(nRowlist.at(i) != nRowlist.at(i+1)){
            bReady = false;
            break;
        }
    }

    return bReady;
}

/*!
 * \brief homeWidget::preProBox
 * \param camIdxlst
 * \param bSingle
 * \return
 */
std::vector<myfvKA1Box> homeWidget::preProBox(QList<int> camIdxlst, bool bSingle)
{
    auto *pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    std::vector<myfvKA1Box> vFvBox;
    std::vector<float> vsens;

    for(auto camIdx: camIdxlst){
        vsens.clear();
        struGsh.PriorityForKFD.clear();
        struGsh.vlabMutex.clear();

        QList<QString> valulst = struGsh.vlabNamemap[camIdx].values();
        for(auto labName : valulst){
            if(pProfileparams->strctAI.vlabelidxmap[camIdx].contains(labName)){//! 塞标签优先级
                struGsh.PriorityForKFD.push_back(pProfileparams->strctAI.vlabelidxmap[camIdx][labName]);
            }

            if(pProfileparams->strctAI.vlabMutexmap[camIdx].contains(labName)){
                struGsh.vlabMutex.push_back(pProfileparams->strctAI.vlabMutexmap[camIdx][labName]);
            }

            float sens = (100 - pProfileparams->strctAI.sens[camIdx][valulst.indexOf(labName) + struGsh.nSorterAdjIdx*MAX_LABELS])*0.01; //! 塞灵敏度
            vsens.push_back(sens);
        }
        gFlow.CurBoxMerge(vFvBox, struGsh.vSocInfer[camIdx], camIdx, pProfileparams->ncoffvalue, vsens);

    }


    if(vFvBox.size() > 0 ){
        gFlow.CatchedBoxMerge(vFvBox,
                              bSingle ? struGsh.vCrossBoxesOldSingleNum[camIdxlst.at(0)]
                                      : struGsh.vCrossBoxesOld[camIdxlst.at(0)],
                              pProfileparams->ncoffvalue,
                              pProfileparams->strctAI.nCapLine[0]);
                 

        if(struGsh.nMergertype == 0){
            gFlow.NormalMergedBoxProcess(vFvBox, struGsh.PriorityForKFD, camIdxlst.size());
        }else if(struGsh.nMergertype == 1){
            gFlow.KHZMergedBoxProcess(vFvBox,struGsh.PriorityForKFD,struGsh.vlabMutex,camIdxlst.size());
        }else if(struGsh.nMergertype == 2){
            gFlow.KxgLHMergedBoxProcess(vFvBox,struGsh.PriorityForKFD, camIdxlst.size(),vsens);
        }else if(struGsh.nMergertype == 3){
            gFlow.LvDouLHMergedBoxProcess(vFvBox,struGsh.PriorityForKFD, camIdxlst.size(),vsens);
        }else if(struGsh.nMergertype == 4){
            gFlow.ZiRanMergedBoxProcess(vFvBox,struGsh.PriorityForKFD, camIdxlst.size());     
        }else if(struGsh.nMergertype == 5){
            gFlow.YuMiLHMergedBoxProcess(vFvBox,struGsh.PriorityForKFD, camIdxlst.size(),vsens);
        }else if(struGsh.nMergertype == 6){
            gFlow.ShanZhaPianLHMergedBoxProcess(vFvBox,struGsh.PriorityForKFD, camIdxlst.size(),vsens);
        }else if(struGsh.nMergertype == 7){
            gFlow.STMergedBoxProcess(vFvBox,struGsh.PriorityForKFD, camIdxlst.size(),vsens);
        }
    }

    return vFvBox;
}

/*!
 * \brief 框的前处理
 * \param vBox
 * \return
 */
QVector<QRect> homeWidget::_predisBBox(std::vector<myfvKA1Box> vBox)
{
    auto *pAIstu = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
    int ymax = pAIstu->nCashLine[0] + pAIstu->nCapLine[0];

    QVector<QRect> vRect;
    for(int i=0; i<vBox.size(); i++){
        if(vBox[i].label.size()>0 &&
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[0][vBox[i].label[0]]){
            int x1 = vBox[i].topx;
            int y1 = vBox[i].topy;
            int x2 = vBox[i].topx + vBox[i].bw;
            int y2 = vBox[i].topy + vBox[i].bh;

            int topx1 =  qBound(0,x1,1023);
            int topy1 =  qBound(0,y1,ymax-1);
            int botmx2 = qBound(0,x2,1023);
            int botmy2 = qBound(0,y2,ymax-1);

            QRect Rect(QPoint(topx1,topy1),QPoint(botmx2,botmy2));

            _adjustRect(Rect);

            vRect.push_back(Rect);
        }
    }
    return vRect;
}

/*!
 * \brief 处理喷阀数据
 */
void homeWidget::_disposeEjData(std::vector<myfvKA1Box> vBox, uint nRow)
{
    QVector<QRect> vRect = _predisBBox(vBox);

    foreach (QRect rect, vRect) {
        int nBegin = _getEjIndex(0, rect.left()+1024); //! 相机1看的是右一半
        int nEnd   = _getEjIndex(0, rect.right()+1024);

        if (nBegin < 0 || nEnd < 0) {//! 喷阀异常处理
            if (nBegin == EJ_BEGIN_OUT && nEnd >= 0) {
                nBegin = 0;
            } else if (nBegin >= 0 && nEnd == EJ_END_OUT) {
                nEnd = EJ_NUM-1;
            } else {
                continue;
            }
        }

        for (int y = rect.top(); y <= rect.bottom(); y++) {
            for (int x = nBegin; x <= nEnd; x++) {
                m_pEjData[y*EJ_SIZE_ROW + EJ_SIZE_ROW - 1 - x/8] |= 1<<(x%8);
            }
        }
    }


    //! 将上一幅图像保留的喷阀数据
    auto *pAIstu = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
    for(int r=0; r<pAIstu->nCashLine[0]; r++){
        for(int j=0; j<EJ_SIZE_ROW;j++){
            m_pEjData[r*EJ_SIZE_ROW+j]= m_pEjData[r*EJ_SIZE_ROW+j]|m_cacheEjData[r*EJ_SIZE_ROW+j];
        }
    }

    _sendEjData(0, m_pEjData, nRow);

    //! 拷贝当前图像采集行后保留行
    int nCpSize = pAIstu->nCashLine[0] * EJ_SIZE_ROW;
    int nOffset = pAIstu->nCapLine[0] * EJ_SIZE_ROW;
    memcpy(m_cacheEjData, m_pEjData+nOffset, nCpSize);
    memset(m_pEjData, 0, MAX_EJECTOR_PACKET_ROW*EJ_SIZE_ROW);

}
/*!
 * \brief 获取喷阀序号
 * \param nCamId
 * \param nPx
 * \return
 */
int homeWidget::_getEjIndex(int nCamId, int nPx)
{
    auto *p = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;
    if (nPx < p->nPixelBegin[nCamId]) {
        return EJ_BEGIN_OUT;
    } else if (nPx > p->nPixelEnd[nCamId]) {
        return EJ_END_OUT;
    } else {
        for (int i = 0; i < EJ_NUM; i++) {
            if (nPx >= p->npEjBegin[nCamId][i] && nPx <= p->npEjEnd[nCamId][i]) {
                return i;
            }
        }
    }
    return EJ_NOT_FOUND;
}
/*!
 * \brief 发送喷阀数据
 * \param nCamId
 */
void homeWidget::_sendEjData(int nCamId, char *pData, uint nRow)
{
    Q_UNUSED(nCamId);

    auto *pAIstu = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;

    QByteArray data;
    int nEjDataLen = pAIstu->nCapLine[0];
    data.resize(nEjDataLen + 4);
    data.fill(0);

    data[0] = nRow/(256*256*256);
    data[1] = nRow/(256*256);
    data[2] = nRow/256;
    data[3] = nRow%256;
    for (int i = 0; i < nEjDataLen; i++) {
        for(int j=0; j<EJ_SIZE_ROW; j++){
            data[i*EJ_SIZE_ROW + j + 4] = pData[i*EJ_SIZE_ROW+j];
        }
    }

    gFlow.sendEjtpacket(data, data.size());
}

/*!
 * \brief 缩框操作
 * \param rect
 */
void homeWidget::_adjustRect(QRect &rect)
{
    int nHei = rect.height();

    float nSize = 1.0;
    int ytop, ybottom;
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize < 100){
        nSize = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize/100.0;
        if(nSize > 0.8){
            nSize = 0.8;
        }
    }else if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize >100){
        nSize = (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize - 100)/100.0;
    }else{
        return;
    }

    nHei = (nSize*nHei)/2;

    auto *pAIstu = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
    int ymax = pAIstu->nCashLine[0] + pAIstu->nCapLine[0];

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize < 100){
        ytop =  qBound(0, rect.top() + nHei, ymax-1);
        ybottom = qBound(0, rect.bottom() - nHei,ymax-1);
    }else if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize >100){
        ytop =  qBound(0, rect.top() - nHei, ymax-1);
        ybottom = qBound(0, rect.bottom() +nHei, ymax-1);
    }

    rect.setTop(ytop);
    rect.setBottom(ybottom);
}

void homeWidget::_staticCamLabels(std::vector<myfvKA1Box> &mFvBox)
{

    auto* pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    QString simRes;
    //! 遍历vCamLabelMapping下每个相机, 逐条显示原始标签及映射后标签信息
    for(int nCam=0; nCam<struGsh.vCamLabelMapping.size() && nCam < gFlow.getMachineCamNum(); nCam++){
        if(struGsh.vCamLabelMapping[nCam].isEmpty()) continue;

        //! 获取vMegerlabmap的索引: 若包含当前相机id则取id，否则取0
        int nMergeIdx = (nCam < pProfileparams->vMegerlabmap.size()) ? nCam : 0;
        QString oriRes;
        for(const BBoxLabelMapping& mapping : struGsh.vCamLabelMapping[nCam]){
            QString oriLab = (struGsh.vlabNamemap.size()>nCam && struGsh.vlabNamemap[nCam].contains(mapping.oriLabel))
                                ? struGsh.vlabNamemap[nCam][mapping.oriLabel] : QString("?");
            if(mapping.mappedLabel == -1){
                oriRes +=QString("%1->%2: %3  ").arg(oriLab).arg(myLan.empty).arg(QString::number(mapping.prob,'f',2));
            }else if(mapping.mappedLabel == mapping.oriLabel){
                oriRes +=QString("%1: %2  ").arg(oriLab).arg(QString::number(mapping.prob,'f',2));
            }else{
                QString mappedLab = (struGsh.vlabNamemap.size()>nCam && struGsh.vlabNamemap[nCam].contains(mapping.mappedLabel))
                                    ? struGsh.vlabNamemap[nCam][mapping.mappedLabel] : QString("?");
                //! 判断映射后标签是否为vMegerlabmap的子标签，若是则替换为父标签名称
                if(nMergeIdx < pProfileparams->vMegerlabmap.size()){
                    const auto& mergeMap = pProfileparams->vMegerlabmap[nMergeIdx];
                    for(auto it = mergeMap.begin(); it != mergeMap.end(); ++it){
                        if(it.value() == mappedLab){
                            mappedLab = it.key(); //! 替换为父标签名称
                            break;
                        }
                    }
                }
                oriRes +=QString("%1->%2: %3  ").arg(oriLab).arg(mappedLab).arg(QString::number(mapping.prob,'f',2));
            }
        }
        simRes +=QString("%1:    %2\n").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nCam])
                            .arg(oriRes);
    }

    //! 统计合并标签后的结果
    QString merRes;
    for(int i=0; i<mFvBox.size(); i++){
        myfvKA1Box box = mFvBox[i];
        if ((!box.label.empty() && box.label[0] == -1)) {
            break;
        }
        for (int k = 0; k <box.label.size(); ++k) {
            if (box.count_flag != 1 || box.ex_info.size() < 3 ||
                box.ex_info[2] < pProfileparams->nareathd || box.label.size() ==0){
                continue;
            }

            if(struGsh.vlabNamemap.size() == 0 || (!struGsh.vlabNamemap[0].contains(box.label[k]))){
                qDebug("_staLabRes: Error not contains label");
                continue;
            }

            QString orilab = struGsh.vlabNamemap[0][box.label[k]];
            merRes +=QString("%1  ").arg(orilab);
        }
    }
    if(!merRes.isEmpty()){
        simRes +=QString("%1   %2\n").arg(myLan.merger).arg(merRes);
    }


    if(!simRes.isEmpty()){
        emit sigRefreshSimRes(simRes);
    }
}
/*!
 * \brief 统计检测结果
 * \param vBox
 * \param nCamId
 */
void homeWidget::_statisticRes(std::vector<myfvKA1Box> vBox, int nCamId)
{
    auto *pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    for(int i=0; i<vBox.size(); i++){
        bool bPurity = true;
        if((struGsh.nMergertype == 1) && (!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPurityStats)){
            for(int k=0; k<vBox[i].label.size(); k++){
                if(!struGsh.vlabNamemap[nCamId].contains(vBox[i].label[k])){
                    qDebug("error: not contais lab!");
                    continue;
                }
                QString labPurity = struGsh.vlabNamemap[nCamId][vBox[i].label[k]];
                if(labPurity.contains("杂质")
                    ||labPurity.contains("zz")
                    ||labPurity.contains("impurity")
                    ||labPurity.contains("壳")
                    ||labPurity.contains("shell")
                    ){
                    bPurity = false;
                }
            }
        }

        if(vBox[i].count_flag==1 && bPurity
            &&vBox[i].ex_info[2] >= pProfileparams->nareathd){
            _calLabDen(vBox[i], nCamId);
            _staMaterialRes(vBox[i], nCamId);
        }

        _staLabRes(vBox[i],nCamId);
    }
    struGsh.vCrossBoxesOld[nCamId].clear();
    struGsh.vCrossBoxesOld[nCamId].assign(vBox.begin(),vBox.end());
}

/*!
 * \brief 统计物料结果
 */
void homeWidget::_staMaterialRes(myfvKA1Box &box, int nCamId)
{
    int nGbid = gFlow.getStaticIdfromCamid(nCamId);
    auto* pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    //! 统计物料统计类别的分母
    auto& vmatTotalmap = struGsh.vmatTotalmap[nGbid];
    for (auto it = vmatTotalmap.keyBegin(); it != vmatTotalmap.keyEnd(); ++it) {
        const QString& var = *it;
        QStringList lst = var.split("/");
        if (lst.size() < 2) continue;
        int nstatype = lst[1].mid(1).toInt();
        switch (nstatype) {
        case STATISTIC_A4:
        case STATISTIC_A5:
        case STATISTIC_A6:
        case STATISTIC_A8:
            vmatTotalmap[var] += 1;
            break;
        case STATISTIC_A10:
        {
            if(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape){
                vmatTotalmap[var] += 1;
            }else{
                for(int val : box.label){
                    if(val < 30 && pProfileparams->strctAI.bShapelabEn[0][val]){
                        vmatTotalmap[var] += 1;
                        break;
                    }
                }
            }
        }
        break;
        default:
            break;
        }
    }


    //! 统计物料统计类别
    auto& vsMatinfo = struGsh.vsMatinfo[nGbid];
    auto& staInfo = vsMatinfo.staInfo;
    auto& staRes = vsMatinfo.staRes;

    for (auto it = staRes.keyBegin(); it != staRes.keyEnd(); ++it) {
        const QString& var = *it;
        const QString& statype = staInfo[var];
        QStringList lst = statype.split("/");
        if (lst.size() < 2) continue;
        int noffset = lst[0].mid(1).toInt();
        int nstatype = lst[1].mid(1).toInt();

        switch (nstatype) {
        case STATISTIC_A1:
            if (noffset == 0) {
                staRes[var][0] += 1;
            } else if (box.ex_info.size() > noffset - 1) {
                staRes[var][0] += box.ex_info[noffset - 1];
            }
            break;
        case STATISTIC_A2:
            if (box.label.size() > 0 && noffset > 0 && box.ex_info.size() > noffset - 1) {
                if(!struGsh.vlabNamemap[nCamId].contains(box.label[0])){
                    qDebug("Error: not contais box.label!");
                    continue;
                }
                const QString& orilab = struGsh.vlabNamemap[nCamId][box.label[0]];
                staRes[var][0] += box.ex_info[noffset - 1] * pProfileparams->Area2weightmap.value(orilab, 1.0f);
            }
            break;
        case STATISTIC_A4:
        case STATISTIC_A5:
            if (noffset > 0 && box.ex_info.size() > noffset - 1) {
                staRes[var][0] += box.ex_info[noffset - 1];
            }else{
                staRes[var][0] +=1;
            }
            break;
        case STATISTIC_A6:
            if (noffset == 1 && box.ex_info.size() > 0) { //! 长
                staRes[var][0] += box.ex_info[noffset - 1] * struGsh.nlegthRatio;
            } else if (noffset == 2 && box.ex_info.size() > 1) { //! 宽
                staRes[var][0] += box.ex_info[noffset - 1] * struGsh.nwidRatio;
            } else if (noffset == 3 && box.ex_info.size() > 2) { //! 面积
                staRes[var][0] += box.ex_info[noffset - 1] * (struGsh.nlegthRatio * struGsh.nwidRatio);
            }
            break;
        case STATISTIC_A7:
            if(noffset >=1){
                float nVal = box.ex_info[noffset - 1];
                if(nVal < pProfileparams->nMatclassifythd[0]){
                    staRes[var][0] +=1;
                }else if(nVal < pProfileparams->nMatclassifythd[1]){
                    staRes[var][1] +=1;
                }else if(nVal < pProfileparams->nMatclassifythd[2]){
                    staRes[var][2] +=1;
                }else{
                    staRes[var][3] +=1;
                }
            }
            break;
        case STATISTIC_A9:
            if (noffset == 1 && box.ex_info.size() > 0) { //! 长
                int nlength = box.ex_info[noffset - 1] * struGsh.nlegthRatio;
                if (nlength < pProfileparams->strctAI.nshapelegthlow[nCamId]) {
                    staRes[var][0] += 1;
                } else if (nlength < pProfileparams->strctAI.nshapelegthhig[nCamId]) {
                    staRes[var][1] += 1;
                } else {
                    staRes[var][2] += 1;
                }
            } else if (noffset == 2 && box.ex_info.size() > 1) { //! 宽
                int nwid = box.ex_info[noffset - 1] * struGsh.nwidRatio;
                if (nwid < pProfileparams->strctAI.nshapewidthlow[nCamId]) {
                    staRes[var][0] += 1;
                } else if (nwid < pProfileparams->strctAI.nshapewidthhig[nCamId]) {
                    staRes[var][1] += 1;
                } else {
                    staRes[var][2] += 1;
                }
            } else if (noffset == 3 && box.ex_info.size() > 2) { //! 面积
                int narea = box.ex_info[noffset - 1] * (struGsh.nlegthRatio * struGsh.nwidRatio);
                if (narea < pProfileparams->strctAI.nshapearealow[nCamId]) {
                    staRes[var][0] += 1;
                } else if (narea < pProfileparams->strctAI.nshapeareahig[nCamId]) {
                    staRes[var][1] += 1;
                } else {
                    staRes[var][2] += 1;
                }
            }
            break;
        case STATISTIC_A10:
        {
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape){
                bool bRcord = false;
                for(int val : box.label){
                    if(val <30 && pProfileparams->strctAI.bShapelabEn[0][val]){
                        bRcord = true;
                        break;
                    }
                }
                if(!bRcord){
                    continue;
                }
            }

            //! 获取使能状态
            bool bLengthEn = pProfileparams->strctAI.bshapeEn[nCamId];
            bool bWidthEn = pProfileparams->strctAI.bshapewidEn[nCamId];
            bool bAreaEn = pProfileparams->strctAI.bshapeAreaEn[nCamId];

            //! 获取测量值
            int nlength = (box.ex_info.size() > 0) ? box.ex_info[0] * struGsh.nlegthRatio : 0;
            int nwid = (box.ex_info.size() > 1) ? box.ex_info[1] * struGsh.nwidRatio : 0;
            int narea = (box.ex_info.size() > 2) ? box.ex_info[2] * (struGsh.nlegthRatio * struGsh.nwidRatio) : 0;


            //! 根据使能条件进行分类
            bool bIsSmall = false;
            bool bIsLarge = false;
            bool bIsMiddle = false;

            if (bLengthEn && !bWidthEn && !bAreaEn) {
                //! 只有长度使能
                if (nlength < pProfileparams->strctAI.nshapelegthlow[nCamId]) {
                    bIsSmall = true;
                } else if (nlength > pProfileparams->strctAI.nshapelegthhig[nCamId]) {
                    bIsLarge = true;
                } else {
                    bIsMiddle = true;
                }
            } else if (!bLengthEn && bWidthEn && !bAreaEn) {
                //! 只有宽度使能
                if (nwid < pProfileparams->strctAI.nshapewidthlow[nCamId]) {
                    bIsSmall = true;
                } else if (nwid > pProfileparams->strctAI.nshapewidthhig[nCamId]) {
                    bIsLarge = true;
                } else {
                    bIsMiddle = true;
                }
            } else if (!bLengthEn && !bWidthEn && bAreaEn) {
                //! 只有面积使能
                if (narea < pProfileparams->strctAI.nshapearealow[nCamId]) {
                    bIsSmall = true;
                } else if (narea > pProfileparams->strctAI.nshapeareahig[nCamId]) {
                    bIsLarge = true;
                } else {
                    bIsMiddle = true;
                }
            } else if (bLengthEn && bWidthEn && !bAreaEn) {
                //! 长度和宽度都使能
                bool bLengthSmall = (nlength < pProfileparams->strctAI.nshapelegthlow[nCamId]);
                bool bLengthLarge = (nlength > pProfileparams->strctAI.nshapelegthhig[nCamId]);
                bool bWidthSmall = (nwid < pProfileparams->strctAI.nshapewidthlow[nCamId]);
                bool bWidthLarge = (nwid > pProfileparams->strctAI.nshapewidthhig[nCamId]);

                bIsSmall = bLengthSmall && bWidthSmall;
                bIsLarge = bLengthLarge && bWidthLarge;
                bIsMiddle = !bIsSmall && !bIsLarge;
            } else if (bLengthEn && !bWidthEn && bAreaEn) {
                //! 长度和面积都使能
                bool bLengthSmall = (nlength < pProfileparams->strctAI.nshapelegthlow[nCamId]);
                bool bLengthLarge = (nlength > pProfileparams->strctAI.nshapelegthhig[nCamId]);
                bool bAreaSmall = (narea < pProfileparams->strctAI.nshapearealow[nCamId]);
                bool bAreaLarge = (narea > pProfileparams->strctAI.nshapeareahig[nCamId]);

                bIsSmall = bLengthSmall && bAreaSmall;
                bIsLarge = bLengthLarge && bAreaLarge;
                bIsMiddle = !bIsSmall && !bIsLarge;
            } else if (!bLengthEn && bWidthEn && bAreaEn) {
                //! 宽度和面积都使能
                bool bWidthSmall = (nwid < pProfileparams->strctAI.nshapewidthlow[nCamId]);
                bool bWidthLarge = (nwid > pProfileparams->strctAI.nshapewidthhig[nCamId]);
                bool bAreaSmall = (narea < pProfileparams->strctAI.nshapearealow[nCamId]);
                bool bAreaLarge = (narea > pProfileparams->strctAI.nshapeareahig[nCamId]);

                bIsSmall = bWidthSmall && bAreaSmall;
                bIsLarge = bWidthLarge && bAreaLarge;
                bIsMiddle = !bIsSmall && !bIsLarge;
            } else if (bLengthEn && bWidthEn && bAreaEn) {
                //! 长度、宽度和面积都使能
                bool bLengthSmall = (nlength < pProfileparams->strctAI.nshapelegthlow[nCamId]);
                bool bLengthLarge = (nlength > pProfileparams->strctAI.nshapelegthhig[nCamId]);
                bool bWidthSmall = (nwid < pProfileparams->strctAI.nshapewidthlow[nCamId]);
                bool bWidthLarge = (nwid > pProfileparams->strctAI.nshapewidthhig[nCamId]);
                bool bAreaSmall = (narea < pProfileparams->strctAI.nshapearealow[nCamId]);
                bool bAreaLarge = (narea > pProfileparams->strctAI.nshapeareahig[nCamId]);

                bIsSmall = bLengthSmall && bWidthSmall && bAreaSmall;
                bIsLarge = bLengthLarge && bWidthLarge && bAreaLarge;
                bIsMiddle = !bIsSmall && !bIsLarge;
            }

            //! 根据分类结果更新统计
            if (bIsSmall) {
                staRes[var][0] += 1;
            } else if (bIsLarge) {
                staRes[var][2] += 1;
            } else if (bIsMiddle) {
                staRes[var][1] += 1;
            }
            break;
        }
        default:
            break;
        }
    }
}

/*!
 * \brief 统计标签结果
 * \fn   统计标签结果(合框函数)
 * \param box
 * \param nCamId
 */
void homeWidget::_staLabRes(myfvKA1Box &box, int nCamId)
{
    int nGbid = gFlow.getStaticIdfromCamid(nCamId);
    auto* pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    if ((box.label.empty() && pProfileparams->slabSecondary[nCamId].isEmpty()) ||
        (!box.label.empty() && box.label[0] == -1)) {
        return;
    }


    if(box.label.empty() && (!pProfileparams->slabSecondary[nCamId].isEmpty())){
        QString tarval = pProfileparams->slabSecondary[nCamId];
        QMap<int,QString>::const_iterator it = struGsh.vlabNamemap[nCamId].constBegin();
        while(it != struGsh.vlabNamemap[nCamId].end()){
            if(it.value() == tarval){
                int key = it.key();
                box.label.push_back(key);
                break;
            }
            it++;
        }
    }

    int nLopTime = pProfileparams->bEnMultilab ? box.label.size() : 1;

    for (int k = 0; k < nLopTime; ++k) {
        if (box.count_flag != 1 || box.ex_info.size() < 3 ||
            box.ex_info[2] < pProfileparams->nareathd || box.label.size() ==0){
            continue;
        }

        if(!struGsh.vlabNamemap[nCamId].contains(box.label[k])){
            qDebug("_staLabRes: Error not contains label");
            continue;
        }

        const QString& orilab = struGsh.vlabNamemap[nCamId][box.label[k]];
        auto& staInfo = struGsh.vlabStamap[nGbid][orilab].staInfo;
        auto& staRes = struGsh.vlabStamap[nGbid][orilab].staRes;

        for (auto it = staInfo.keyBegin(); it != staInfo.keyEnd(); ++it) {
            const QString& var = *it;
            const QString& strtype = staInfo[var];
            QStringList stalst = strtype.split("/");
            if (stalst.size() < 2) continue;
            int type = stalst[1].mid(1).toInt();
            int n = stalst[0].mid(1).toInt();

            switch (type) {
            case STATISTIC_A1:
            case STATISTIC_A4:
                if (n == 0) {
                    staRes[var][0] += 1;
                } else if (box.ex_info.size() > n - 1) {
                    staRes[var][0] += box.ex_info[n - 1];
                }
                break;
            case STATISTIC_A2:
            case STATISTIC_A3:
                if(pProfileparams->Area2weightmap.contains(orilab)){
                    if (n == 0) {
                        staRes[var][0] += 1*pProfileparams->Area2weightmap[orilab];
                    } else if (box.ex_info.size() > n - 1) {
                        staRes[var][0] += box.ex_info[n - 1]*pProfileparams->Area2weightmap[orilab];
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}

/*!
 * \brief homeWidget::_calLabDen
 * \fn   计算标签统计结果所需的分母
 * \param box
 */
void homeWidget::_calLabDen(myfvKA1Box &box, int nCamId)
{
    int nGbid = gFlow.getStaticIdfromCamid(nCamId);
    auto* pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    auto& totalMap = struGsh.vlabTotalmap[nGbid];

    //! 预先缓存label和ex_info
    int label0 = (box.label.size() > 0) ? box.label[0] : -1;
    const auto& ex_info = box.ex_info;

    //! 遍历所有统计项
    for (auto it = totalMap.begin(); it != totalMap.end(); ++it) {
        const QString& var = it.key();
        //! 只做一次split和解析
        QStringList lst = var.split("/");
        if (lst.size() < 2) continue;
        int noffset = lst[0].mid(1).toInt();
        int nstatype = lst[1].mid(1).toInt();

        switch (nstatype) {
        case STATISTIC_A3:{
            QString orilab;
            float weight =1.0;
            if(label0!=-1){
                if(!struGsh.vlabNamemap[nCamId].contains(label0)){
                    qDebug("Error: not contais label0!");
                    continue;
                }
                orilab = struGsh.vlabNamemap[nCamId][label0];
                weight = pProfileparams->Area2weightmap.value(orilab, 1.0f);
            }
            if(label0==-1 && !pProfileparams->slabSecondary[nCamId].isEmpty()){
                int key = -1;
                QString tarval = pProfileparams->slabSecondary[nCamId];
                QMap<int,QString>::const_iterator it = struGsh.vlabNamemap[nCamId].constBegin();
                while(it != struGsh.vlabNamemap[nCamId].end()){
                    if(it.value() == tarval){
                        key = it.key();
                        break;
                    }
                    it++;
                }
                if(key != -1){
                    if(!struGsh.vlabNamemap[nCamId].contains(key)){
                        qDebug("Error: not contais key!");
                        continue;
                    }
                    orilab = struGsh.vlabNamemap[nCamId][key];
                    weight = pProfileparams->Area2weightmap.value(orilab, 1.0f);
                }
            }
            if(noffset == 0){
                it.value() += 1 * weight;
            }else{
                if (label0 >= 0 && ex_info.size() > noffset - 1) {
                    it.value() += ex_info[noffset - 1] * weight;
                }
            }
        }
        break;
        case STATISTIC_A4:{
            if (noffset == 0) {
                it.value() += 1;
            } else if (ex_info.size() > noffset - 1) {
                it.value() += ex_info[noffset - 1];
            }
        }
        break;
        case STATISTIC_A5:
        case STATISTIC_A6:
        case STATISTIC_A8:
        case STATISTIC_A10:
            it.value() += 1;
            break;
        default:
            break;
        }
    }
}

/*!
 * \brief 计算分母
 * \param ary
 * \param nCamId
 */
void homeWidget::_calDen(QByteArray ary, int nCamId)
{
    int nGbid = gFlow.getStaticIdfromCamid(nCamId);
    auto* pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const auto& lablst = struGsh.vlabNamemap[nGbid].values();
    auto& totalMap = struGsh.vlabTotalmap[nGbid];
    const auto& area2weightmap = pProfileparams->Area2weightmap;

    for (auto it = totalMap.begin(); it != totalMap.end(); ++it) {
        const QString& var = it.key();
        QStringList lst = var.split("/");
        if (lst.size() < 2) continue;
        int noffset = lst[0].mid(1).toInt(); // 假设格式如"A3"取3
        int nstatype = lst[1].mid(1).toInt();

        if (nstatype != STATISTIC_A3) continue;

        for (int n = 0; n < lablst.size(); ++n) {
            const QString& lab = lablst.at(n);
            auto weightIt = area2weightmap.find(lab);
            if (weightIt == area2weightmap.end()) continue;

            int noff = noffset * 120 + n * 4;
            if (noff + 3 >= ary.size()) continue; // 防止越界

            uint nval = ((ary[noff] & 0xff) << 24) |
                        ((ary[noff + 1] & 0xff) << 16) |
                        ((ary[noff + 2] & 0xff) << 8) |
                        (ary[noff + 3] & 0xff);

            it.value() += nval * (*weightIt);
        }
    }
}

/*!
 * \brief homeWidget::_staLab
 * \fn  统计标签结果(非合框函数)
 * \param parray
 * \param nCamId
 * \param nRow
 */
void homeWidget::_staLab(QByteArray parray, int nCamId, uint nRow)
{
    Q_UNUSED(nRow);

    int nGbid = gFlow.getStaticIdfromCamid(nCamId);
    if (nGbid == -1) return;

    const auto& area2weightmap = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.Area2weightmap;
    const auto& vlabNamemap = struGsh.vlabNamemap[nCamId];
    auto& vlabStamap = struGsh.vlabStamap[nGbid];

    int nlabIdx = 0;
    for (auto keyIt = vlabNamemap.keyBegin(); keyIt != vlabNamemap.keyEnd(); ++keyIt, ++nlabIdx) {
        const QString& labname = vlabNamemap[*keyIt];
        auto& staInfo = vlabStamap[labname].staInfo;
        auto& staRes = vlabStamap[labname].staRes;

        for (auto varIt = staInfo.keyBegin(); varIt != staInfo.keyEnd(); ++varIt) {
            const QString& var = *varIt;
            const QString& strtype = staInfo[var];
            QStringList stalst = strtype.split("/");
            if (stalst.size() < 2) continue;
            int type = stalst[1].mid(1).toInt();
            int noff = stalst[0].mid(1).toInt();

            int noffset = noff * 120 + nlabIdx * 4;
            if (noffset + 3 >= parray.size()) continue; //! 防止越界

            uint nval = ((parray[noffset] & 0xff) << 24) |
                        ((parray[noffset + 1] & 0xff) << 16) |
                        ((parray[noffset + 2] & 0xff) << 8) |
                        (parray[noffset + 3] & 0xff);
            switch (type) {
            case STATISTIC_A1:
                staRes[var][0] += nval;
                break;
            case STATISTIC_A3:
            {
                auto weightIt = area2weightmap.find(labname);
                if (weightIt != area2weightmap.end())
                    staRes[var][0] += nval * (*weightIt);
            }
            break;
            default:
                break;
            }
        }
    }
}

/*!
 * \brief 统计物料信息
 * \param parray
 * \param nCamId
 */
void homeWidget::_staMat(QByteArray parray, int nCamId)
{
    int nGbid = gFlow.getStaticIdfromCamid(nCamId);
    if (nGbid == -1) return;


    auto* pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    //! 统计物料统计类别的分母
    auto& vmatTotalmap = struGsh.vmatTotalmap[nGbid];
    for (auto it = vmatTotalmap.keyBegin(); it != vmatTotalmap.keyEnd(); ++it) {
        const QString& var = *it;
        QStringList lst = var.split("/");
        if (lst.size() < 2) continue;
        int nstatype = lst[1].mid(1).toInt();
        switch (nstatype) {
        case STATISTIC_A4:
        case STATISTIC_A5:
        case STATISTIC_A6:
        case STATISTIC_A8:
            if(parray.size() > 246){
                vmatTotalmap[var] += ((parray[240]&0xFF)<<8) + (parray[241]&0xFF);
            }
            break;
        case STATISTIC_A10:
            break;
        default:
            break;
        }
    }


    //! 计算统计类别
    auto& vsMatinfo = struGsh.vsMatinfo[nGbid];
    auto& staInfo = vsMatinfo.staInfo;
    auto& staRes = vsMatinfo.staRes;


    int nlabsta = 2 /*gFlow.getLabstacatNum(nCamId)*/; //! 瓶片现在默认有2个标签统计类别
    int nstaIdx = 0;

    for (auto it = staInfo.keyBegin(); it != staInfo.keyEnd(); ++it, ++nstaIdx) {
        const QString& var = *it;
        const QString& strtype = staInfo[var];
        QStringList stalst = strtype.split("/");
        if (stalst.size() < 2) continue;
        int type = stalst[1].mid(1).toInt();
        int nPos = stalst[0].mid(1).toInt();

        if(type == STATISTIC_A8){
            if(parray.size() <246 ) continue;
            int nDomain = ((parray[240]&0xFF)<<8) + (parray[241]&0xFF);

            for(int k=0; k<nDomain; k++){
                int noffset = nlabsta * 120 + 2 + k*12 + nPos*4;
                if (noffset + 3 >= parray.size()) continue; //! 防止越界

                uint nval = ((parray[noffset] & 0xff) << 24) |
                            ((parray[noffset + 1] & 0xff) << 16) |
                            ((parray[noffset + 2] & 0xff) << 8) |
                            (parray[noffset + 3] & 0xff);



                int nlowthd=0, nhigthd=0;
                if(nPos == 0){//! 长(高)
                    nval = nval*struGsh.nlegthRatio;
                    nlowthd = pProfileparams->strctAI.nshapelegthlow[nCamId]*100;
                    nhigthd = pProfileparams->strctAI.nshapelegthhig[nCamId]*100;
                }else if(nPos == 1){//! 宽
                    nval = nval*struGsh.nwidRatio;
                    nlowthd = pProfileparams->strctAI.nshapewidthlow[nCamId]*100;
                    nhigthd = pProfileparams->strctAI.nshapewidthhig[nCamId]*100;
                }else{//! 面积
                    nval = nval * 0.1;
                    nlowthd = pProfileparams->strctAI.nshapearealow[nCamId];
                    nhigthd = pProfileparams->strctAI.nshapeareahig[nCamId];
                }

                if(nval <= nlowthd){
                    staRes[var][0] +=1;
                }else if(nval <=nhigthd){
                    staRes[var][1] +=1;
                }else{
                    staRes[var][2] +=1;
                }
            }
        }
    }
}

/*!
 * \brief 框预处理：当vBox.prob小于sens时替换为映射标签，无映射标签则置为-1
 * \param vBox
 * \param nCamId
 * \return 预处理后的框集合
 */
std::vector<BBox> homeWidget::_preProcessBox(const std::vector<BBox> &vBox, int nCamId)
{
    std::vector<BBox> tmpBox = vBox;
    auto *pProfileparams = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    //! 确保vCamLabelMapping容量足够
    if(struGsh.vCamLabelMapping.size() <= nCamId){
        return tmpBox;
    }

    if(nCamId >= 0 && nCamId < struGsh.vlabNamemap.size()){
        QList<QString> valulst = struGsh.vlabNamemap[nCamId].values();
        for(size_t i = 0; i < tmpBox.size(); ++i){
            int labelKey = tmpBox[i].label;
            if(struGsh.vlabNamemap[nCamId].contains(labelKey)){
                QString origLab = struGsh.vlabNamemap[nCamId][labelKey];
                int labIdx = valulst.indexOf(origLab);
                if(labIdx >= 0){
                    int sensIdx = labIdx + struGsh.nSorterAdjIdx * MAX_LABELS;
                    float sensVal = (100 - pProfileparams->strctAI.sens[nCamId][sensIdx]) * 0.01f;
                    if(tmpBox[i].prob < sensVal){
                        QString mappedLab = (nCamId < pProfileparams->vlabMixmap.size()) ? pProfileparams->vlabMixmap[nCamId].value(origLab, "") : "";
                        if(!mappedLab.isEmpty() && valulst.contains(mappedLab)){
                            int mappedKey = struGsh.vlabNamemap[nCamId].key(mappedLab, -1);
                            //! 记录标签映射: 原始标签 -> 映射后标签
                            BBoxLabelMapping mapping;
                            mapping.prob = tmpBox[i].prob;
                            mapping.oriLabel = labelKey;
                            mapping.mappedLabel = mappedKey;
                            struGsh.vCamLabelMapping[nCamId].append(mapping);
                            tmpBox[i].label = mappedKey;
                        }else{
                            //! 记录标签映射: 无有效映射，标签被丢弃
                            BBoxLabelMapping mapping;
                            mapping.prob = tmpBox[i].prob;
                            mapping.oriLabel = labelKey;
                            mapping.mappedLabel = -1;
                            struGsh.vCamLabelMapping[nCamId].append(mapping);
                            tmpBox[i].label = -1;
                        }
                    }else{
                        //! 记录: 置信度达标，标签保持不变
                        BBoxLabelMapping mapping;
                        mapping.prob = tmpBox[i].prob;
                        mapping.oriLabel = labelKey;
                        mapping.mappedLabel = labelKey;
                        struGsh.vCamLabelMapping[nCamId].append(mapping);
                    }
                }
            }
        }
    }
    return tmpBox;
}

// /*!
//  * \brief 清理指定行号之前的矩形框记录
//  * \param nRow 指定行号，清理该行号之前的所有记录
//  */
// void homeWidget::clearLineBoxMapBeforeRow(uint nRow)
// {
//     QList<uint> keysToRemove;

//     // 找到需要删除的行号
//     QMapIterator<uint, QMap<int, QList<BBox>>> it(m_lineBoxMap);
//     while(it.hasNext()){
//         it.next();
//         uint rowNumber = it.key();
//         if(rowNumber < nRow){
//             keysToRemove.append(rowNumber);
//         }
//     }

//     // 删除旧的行号记录
//     for(uint key : keysToRemove){
//         m_lineBoxMap.remove(key);
//     }

//     if(!keysToRemove.isEmpty()){
//         qDebug() << "清理了" << keysToRemove.size() << "个旧行号的矩形框记录";
//     }
// }

// /*!
//  * \brief 获取指定行号指定相机的矩形框列表
//  * \param nRow 行号
//  * \param nCamId 相机ID
//  * \return 矩形框列表
//  */
// QList<BBox> homeWidget::getBoxesByRowAndCamera(uint nRow, int nCamId) const
// {
//     if(m_lineBoxMap.contains(nRow)){
//         if(m_lineBoxMap[nRow].contains(nCamId)){
//             return m_lineBoxMap[nRow][nCamId];
//         }
//     }
//     return QList<BBox>();
// }
