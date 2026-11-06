#include "aiwidget.h"


AIWidget::AIWidget(QWidget *parent):QWidget(parent),m_uploadTask(nullptr),m_bcaplabEn(false)
{
    createwid();
    pagelayout();
    pagesigandslot();
}

AIWidget::~AIWidget()
{
    //! 清理放大预览窗口
    if (m_enlargedImgWidget) {
        m_enlargedImgWidget->close();
        delete m_enlargedImgWidget;
        m_enlargedImgWidget = nullptr;
    }
    
    if (m_unzipWatcher) {
        if (m_unzipWatcher->isRunning()) {
            m_unzipWatcher->waitForFinished();
        }
        m_unzipWatcher->deleteLater();
        m_unzipWatcher = nullptr;
    }

    if (m_uploadTask) {
        delete m_uploadTask;
        m_uploadTask = nullptr;
    }
}


void AIWidget::createwid()
{
    m_tabBar = new QTabBar();
    m_tabBar->addTab(QString("AI %1").arg(myLan.params_set));
    m_tabBar->addTab(QString("AI %1").arg(myLan.capture));
    m_tabBar->setExpanding(false);
    m_tabBar->setStyleSheet("QTabBar::tab {min-width: 150px; padding: 8px;min-height:45px;margin-right:5px }"
                            "QTabBar::tab:selected { background-color: #007454; color: white; }"
                            "QTabBar::tab:!selected { background-color: #f0f0f0; color: #333333; }"
                            );

    m_viewSwBtn = new QPushButton();
    m_viewSwBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));
    m_viewSwBtn->setIconSize(QSize(50,50));
    m_viewSwBtn->setFixedWidth(BTN_WID+70);

    m_syncSetBtn = new QPushButton(QString("%1").arg(myLan.alone));
    m_syncSetBtn->setIcon(QIcon(":/res/png/tab_auxiliary_normal.png"));
    m_syncSetBtn->setIconSize(QSize(50,50));
    m_syncSetBtn->setFixedWidth(BTN_WID+70);


    m_wid = new QWidget();

    m_aiLabelgb = new QGroupBox(myLan.label_enable);
    m_aiLabelgb->setMinimumWidth(750);
    m_aiLabelgb->setFixedHeight(370);
    for(int i=0; i<MAX_LABELS; i++){
        m_modellbchk[i] = new MyCheckBox(QString(""));
        m_modellbchk[i]->setCheckBtnSize(50,50);
    }

    createAIparamsPage();
    createAIphotoPage();
    createPhotoPreview();
}

/*!
 * \brief 页面布局
 */
void AIWidget::pagelayout(void)
{
    layAIparamsPage();
    layAIphotoPage();
    layPhotoPreview();


    //! 主布局
    QHBoxLayout *hlaytop = new QHBoxLayout();
    hlaytop->addWidget(m_tabBar);
    hlaytop->addStretch();
    hlaytop->addWidget(m_viewSwBtn);
    hlaytop->addWidget(m_syncSetBtn);
    hlaytop->addSpacing(10);

    m_layGb = new QStackedLayout();
    m_layGb->addWidget(m_aiParamssetWid);
    m_layGb->addWidget(m_aiCapgb);

    QVBoxLayout *vlayAIpage = new QVBoxLayout(m_wid);
    vlayAIpage->addLayout(hlaytop);
    vlayAIpage->addLayout(m_layGb);
    vlayAIpage->addSpacing(20);
    vlayAIpage->addWidget(m_aiLabelgb);
    vlayAIpage->addStretch();

    m_laywidstck = new QStackedLayout(this);
    m_laywidstck->addWidget(m_wid);
    m_laywidstck->addWidget(m_imgwid);
}

/*!
 * \brief 信号槽
 */
void AIWidget::pagesigandslot()
{
    connect(m_tabBar, &QTabBar::currentChanged, this, &AIWidget::onTabIndexChg);
    connect(m_viewSwBtn,SIGNAL(pressed()),this,SLOT(onCamViewBtnPressSlt()));
    connect(m_syncSetBtn, SIGNAL(pressed()),this,SLOT(onSyncBtnPressedSlt()));

    //! 标签使能
    QSignalMapper* modellbEnmap = new QSignalMapper();
    for(int i=0; i<MAX_LABELS; i++){
        modellbEnmap->setMapping(m_modellbchk[i], i);
        connect(m_modellbchk[i], SIGNAL(pressed()), modellbEnmap, SLOT(map()));
    }
    connect(modellbEnmap,SIGNAL(mapped(int)),this,SLOT(onlabelEnChanged(int)));

    //! AI参数设置
    QSignalMapper* imgsetmap = new QSignalMapper();
    for(int i=0; i<2; i++){
        imgsetmap->setMapping(m_caple[i], i);
        connect(m_caple[i], SIGNAL(pressed()), imgsetmap, SLOT(map()));
    }
    connect(imgsetmap,     SIGNAL(mapped(int)),             this,SLOT(onimgsetChanged(int)));
    connect(m_modelcomb,   SIGNAL(currentIndexChanged(int)),this,SLOT(onModelIdxChanged(int)));
    connect(m_dataLinecomb,SIGNAL(currentIndexChanged(int)),this,SLOT(onModeldatalineChanged(int)));
    connect(m_modelRescomb,SIGNAL(currentIndexChanged(int)),this,SLOT(onModelResChanged(int)));
    connect(m_coffle,      SIGNAL(pressed()),               this,SLOT(onCoffChanged()));
    connect(m_offsetle,    SIGNAL(pressed()),               this,SLOT(onoffsetChanged()));
    connect(m_areathdle,   SIGNAL(pressed()),               this, SLOT(onAreathdChgSlt()));

    //! 拍照设置
    connect(m_captdle,SIGNAL(pressed()),this,SLOT(oncapThdChanged()));
    connect(m_capcambtngp, SIGNAL(buttonClicked(int)), this, SLOT(onCoCapcamidxChanged(int)));
    connect(m_capTypegp, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &AIWidget::oncapChktypeChanged);

//    QSignalMapper* previewtypemap = new QSignalMapper();
//    for(int i=0; i<2; i++){
//        previewtypemap->setMapping(m_preViewtypebtn[i],i);
//        connect(m_preViewtypebtn[i], &QRadioButton::clicked, previewtypemap, QOverload<>::of(&QSignalMapper::map));
//    }
//    connect(previewtypemap, QOverload<int>::of(&QSignalMapper::mapped), this, QOverload<int>::of(&AIWidget::onPreViewtypeChg));

//    QSignalMapper* capsetmap = new QSignalMapper();
//    for(int i=0; i<3; i++){
//        capsetmap->setMapping(m_capTypebtn[i], i);
//        connect(m_capTypebtn[i], SIGNAL(pressed()), capsetmap, SLOT(map()));
//    }
//    connect(capsetmap,SIGNAL(mapped(int)),this,SLOT(oncapChktypeChanged(int)));

    QSignalMapper* capbtnmap = new QSignalMapper();//! 导出图片
    for(int i=0; i<4; i++){
        capbtnmap->setMapping(m_capbtn[i], i);
        connect(m_capbtn[i], SIGNAL(pressed()), capbtnmap, SLOT(map()));
    }
    connect(capbtnmap,SIGNAL(mapped(int)),this,SLOT(oncapBtnsetChanged(int)));

    connect(m_uploadTask, SIGNAL(sigUpdateTips(QString, int)),this, SLOT(onUpdataUploadTips(QString, int)), Qt::QueuedConnection);
    connect(m_uploadTask, SIGNAL(sigFinish(QString)), this, SLOT(onUploadFinished(QString)), Qt::QueuedConnection);

    //! 图片预览
    connect(m_bckBtn,SIGNAL(pressed()),this,SLOT(onBackbtnSlt()));
    connect(m_imglistwid,SIGNAL(currentTextChanged(QString)),this,SLOT(onRefreshGraphicview(QString)));

    connect(m_labelListWidget, &QListWidget::itemClicked, this, &AIWidget::onLabelListWidgetItemClicked);
    connect(m_imgListWidget, &QListWidget::itemClicked, this, &AIWidget::onImgListWidgetItemClicked);


    QSignalMapper* camworkmodemap = new QSignalMapper();//! 相机工作模式
    for(int i=0; i<MAX_CAM_NUM; i++){
        camworkmodemap->setMapping(m_camworkmodecomb[i], i);
        connect(m_camworkmodecomb[i], SIGNAL(currentIndexChanged(int)), camworkmodemap, SLOT(map()));
    }
    connect(camworkmodemap,SIGNAL(mapped(int)),this,SLOT(oncamworkModelchg(int)));
}

/*!
 * \brief 刷新页面
 */
void AIWidget::refreshPage()
{
    if(struCnfg.nMachineType == nMachine_Bottle){
        m_cofflb->hide();
        m_coffle->hide();
        m_offsetlb->hide();
        m_offsetle->hide();
        m_areathdlb->hide();
        m_areathdle->hide();
        m_syncSetBtn->hide();
    }else{
        m_cofflb->show();
        m_coffle->show();
        m_offsetlb->show();
        m_offsetle->show();
        m_areathdlb->show();
        m_areathdle->show();
        m_syncSetBtn->show();
    }

    refreshControlSta();

    m_nCamId = 0;
    m_viewSwBtn->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nCamId]);


    //! 刷新采集行、保留行
    m_caple[0]->setText(QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[m_nCamId]));
    m_caple[1]->setText(QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[m_nCamId]));

    //! 模型线数
    m_dataLinecomb->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelLine);

    //! 分辨率
//    m_modelRescomb->blockSignals(true);
//    m_modelRescomb->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nRes);
//    m_modelRescomb->blockSignals(false)

    //! 重合度系数
    m_coffle->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.ncoffvalue));

    m_offsetle->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.noffsetvalue));

    m_areathdle->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nareathd));

    //! 刷新标签使能
    refreshLabelEnable(m_nCamId);

    //! 模型combox
    refreshModelComb(m_nCamId);

    //! 刷新拍照信息
    refreshCapcaminfo();

    //! 刷新AI工作模式
    refreshCamWorkmodeComb();
}
/*!
 * \brief
 */
void AIWidget::refreshIMGPreviewpage(QStringList imglist)
{
    if(!imglist.isEmpty()){
        while(m_imglistwid->count()){
            QListWidgetItem* pitem = m_imglistwid->takeItem(0);
            delete pitem;
            pitem = nullptr;

        }
        m_imglistwid->addItems(imglist);
    }
}

void AIWidget::refreshControlSta()
{
    m_aiSetsetgb->setDisabled(struGsh.nFeederState);
    m_camworksetGb->setDisabled(struGsh.nFeederState);
}

void AIWidget::onTabIndexChg(int nIdx)
{
    m_layGb->setCurrentIndex(nIdx);
}


/*!
 * \brief 槽：切换模型   （无模型的特殊处理   后市2模型信息要隐藏 切模性的combx 特殊处理）
 * \param nIdx
 */
void AIWidget::onModelIdxChanged(int nIdx)
{
    int modelIdx = m_modelcomb->itemData(nIdx).toInt();

    //! 独立校验
    if((!m_bSync)&&(QString(struGsh.sModelUnitInfo[m_nCamId].model[modelIdx].cnf) == "NULL")){
        infoWidget->setLabelText(myLan.model_empty_or_no_AI);
        infoWidget->raise();
        infoWidget->showSecs();
        return;
    }

    //! 相同校验
    QVector<int> vec;
    if(m_bSync){
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            if((nIdx < 20 && modelIdx < 20) && (QString(struGsh.sModelUnitInfo[i].model[modelIdx].cnf) != "NULL")){
                vec.push_back(i);
            }else{
                infoWidget->setLabelText(QString("%1 %2")
                                         .arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i])
                                         .arg(myLan.no_such_aimodel));
                infoWidget->raise();
                infoWidget->showSecs();
                return;
            }
        }
    }

    myMessageBox msgBox(MSG_QUES, QString("\n%1?").arg(myLan.confirm_switch_model), "", g_mainWidget);
    int ret = msgBox.exec();
    if (ret != QDialog::Accepted) {
        int nOld = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelIdx[m_nCamId];
        //! 恢复原来选择
        m_modelcomb->blockSignals(true);
        for(int i=0; i<m_modelcomb->count(); i++){
            if(m_modelcomb->itemData(i).toInt() == nOld){
                m_modelcomb->setCurrentIndex(i);
                break;
            }
        }
        m_modelcomb->blockSignals(false);
        return;
    }

    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->raise();
    infoWidget->show();


    if(m_bSync){
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelIdx[i] = modelIdx;
        }
    }else{
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelIdx[m_nCamId] = modelIdx;
    }

    //! 1. 发送切换模型命令
    if(m_bSync){
        for(auto ncamId:vec){
            gFlow.SocModelSWitch(struGsh.nAnalyzeIndex, ncamId);
            gFlow.msleep(10);
        }
    }else{
        gFlow.SocModelSWitch(struGsh.nAnalyzeIndex, m_nCamId);
    }

    gFlow.sleep(6);

    //! 2. 查询模型信息
    if(m_bSync){
        for(auto ncamid:vec){
            gFlow.reqSocModelinfo(struGsh.nAnalyzeIndex,ncamid);
            gFlow.msleep(200);
        }
    }else{
        gFlow.reqSocModelinfo(struGsh.nAnalyzeIndex,m_nCamId);
        gFlow.msleep(200);
    }


    //! 3. 获取模型对应的标签
    if(m_bSync){
        for(auto ncamId:vec){
            gFlow.reqSocModelLabelinfo(struGsh.nAnalyzeIndex,ncamId);
            gFlow.msleep(200);
        }
    }else{
        gFlow.reqSocModelLabelinfo(struGsh.nAnalyzeIndex,m_nCamId);
        gFlow.msleep(200);
    }

    //! 查统计类别
    if(m_bSync){
        for(auto ncamId:vec){
            gFlow.reqStainfo(ncamId);
            gFlow.msleep(500);
        }
    }else{
        gFlow.reqStainfo(m_nCamId);
        gFlow.msleep(200);
    }

    gFlow.msleep(500);

    //! 4.刷新页面
    refreshPage();

    bool bflag = true;

    QString caminfo;
    if(m_bSync){
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            if(vec.contains(i)){
                if(struGsh.sModelUnitInfo[i].state == -1){
                    bflag = false;
                    caminfo +=QString("%1:%2")
                                   .arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i])
                                   .arg(myLan.failed);
                }else{
                    caminfo +=QString("%1:%2")
                                   .arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i])
                                   .arg(myLan.sucess);
                }
            }else{
                caminfo +=QString("%1%2")
                               .arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i])
                               .arg(myLan.failed);
            }

            if(i%2 ==1){
                caminfo +="\n";
            }else{
                caminfo +=" ";
            }
        }
    }else{
        if(struGsh.sModelUnitInfo[m_nCamId].state == -1){
            bflag = false;
        }
    }

    QString restr = bflag?myLan.sucess:myLan.failed;

    gFlow.resetSoc();

    infoWidget->setLabelText(QString("%1\n%2").arg(restr).arg(caminfo));
    infoWidget->showSecs();
}
/*!
 * \brief 槽: 采集行、保留行
 * \param nIdx
 */
void AIWidget::onimgsetChanged(int nIdx)
{
    int nline = m_caple[nIdx]->text().toInt();
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 1024, nline, "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_caple[nIdx]->setText(QString("%1").arg(n_sample));
        if(nIdx == 0){
            if(m_bSync){
                for(int i=0; i<gFlow.getMachineCamNum(); i++){
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[i] = n_sample;
                }
            }else{
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[m_nCamId] = n_sample;
            }
        }else{
            if(m_bSync){
                for(int i=0; i<gFlow.getMachineCamNum(); i++){
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[i] = n_sample;
                }
            }else{
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[m_nCamId] = n_sample;
            }
        }
        gFlow.SocAdParamssend(struGsh.nAnalyzeIndex,0);

        if(struCnfg.nMachineType ==1){
            gFlow.sendcapLine2cotrlBoard(struGsh.nAnalyzeIndex);
        }
        infoWidget->setLabelText(myLan.apply_and_wait);
        infoWidget->raise();
        infoWidget->show();

        gFlow.sleep(1);


        //! 获取模型实际的信息
        bool bWaran = false;
        QString camIdstr;
        if(m_bSync){
            for(int i=0; i<gFlow.getMachineCamNum(); i++){
                gFlow.ReqSocInferParams(i);
                gFlow.sleep(1);
            }

            for(int i=0; i<gFlow.getMachineCamNum(); i++){
                int nImgHei = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[i] +
                              struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[i];

                if(struGsh.nSocImgHei != nImgHei){
                    bWaran = true;
                    camIdstr += struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i];
                    camIdstr += " ";
                }
            }
        }else{
            gFlow.ReqSocInferParams(m_nCamId);
            gFlow.sleep(1);

            int nImgHei = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[m_nCamId] +
                          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[m_nCamId];

            if(struGsh.nSocImgHei != nImgHei){
                bWaran = true;
                camIdstr += struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nCamId];
                camIdstr += " ";
            }
        }


        if(bWaran){
            infoWidget->setLabelText(QString("%1! %2\n(%3)").arg(myLan.notice).arg(myLan.pic_size_mismatch).arg(camIdstr));
        }

        infoWidget->showSecs();
    }
}

/*!
 * \brief 槽: 图像导出
 * \param nIdx
 */
void AIWidget::oncapBtnsetChanged(int nIdx)
{
    switch(nIdx){
    case 0:   //! 图片导出
        onHandleImgExport();
        break;
    case 1: //! 图片预览
        handleIMGpreview();
        break;
    case 2: //! 裁剪小图
         onHandleImg();
         break;
    case 3://! 参数清除
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            gFlow.SocImgClear(struGsh.nAnalyzeIndex, i);
        }
        break;
    default:
        break;
    }
}
/*!
 * \brief 槽: 拍照设置
 * \param nIdx: 拍照类型 0：不保存 1：原始 2：标记
 */
void AIWidget::oncapChktypeChanged(int nIdx)
{
    struGsh.bCapimg = m_capTypebtn[1]->isChecked()||m_capTypebtn[2]->isChecked(); //! 拍图不写数据库

    struGsh.nSoccaptype = nIdx;
    gFlow.SocAdParamssend(struGsh.nAnalyzeIndex,nIdx);   //! 发送拍图类型
    gFlow.msleep(5);

    m_bcaplabEn = m_capTypebtn[2]->isChecked(); //! 标记图和原始图标签使能分开
    refreshLabelEnable(m_nCamId);

    if(m_capTypebtn[2]->isChecked()){//! 拍标记图,发送使能标签
        gFlow.SocCaplabEnParamsSend();
        gFlow.msleep(5);
    }

    gFlow.SocParamssend(struGsh.nAnalyzeIndex, 0);  //! 拍图时，按照实际灵敏度下发
}

/*!
 * \brief 查询模型信息 ===================待删除============
 */
void AIWidget::onrequireModelinfo()
{
//    for(int i=1; i<gFlow.getMachineCamNum()+1; i++){
//        m_modelinfoitem[2*i-1]->setText(QString("-------"));
//    }

//    myMessageBox msgBox(MSG_INFO, QString("\n\n%1").arg(myLan.apply_and_wait), "", g_mainWidget);
//    msgBox.setautoClose(false);
//    msgBox.show();

//    for(int i=0; i<gFlow.getMachineCamNum(); i++){
//        gFlow.reqSocModelinfo(struGsh.nAnalyzeIndex,i); //! 查询模型信息
//        gFlow.msleep(50);
//    }

//    QEventLoop loop;
//    QTimer timer;
//    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
//    timer.start(4000);
//    loop.exec();

//    for(int i=1; i<gFlow.getMachineCamNum()+1; i++){
//        QString strsModel = QString("%1").arg(QString::fromLocal8Bit(struGsh.sModelUnitInfo[i-1].
//                                                                     model[struGsh.sModelUnitInfo[i-1].currentId].cnf));
//        m_modelinfoitem[2*i-1]->setText(strsModel);
//    }

//    msgBox.close();
}

/*!
 * \brief 标签使能
 * \param nIdx
 */
void AIWidget::onlabelEnChanged(int nIdx)
{
    if(m_bcaplabEn){//! 拍标记图--按标签
        struGsh.bEnCaplabs[m_nCamId][nIdx] = m_modellbchk[nIdx]->isChecked()?true:false;
        gFlow.SocCaplabEnParamsSend();
    }else{
        auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
        bool bChecked = m_modellbchk[nIdx]->isChecked();

        if(m_bSync){//! 同步修改
            for(int i=0; i<gFlow.getMachineCamNum(); i++){
                pProfile->strctAI.bEnlabs[i][nIdx] = bChecked;
            }
            gFlow.SocParamssend(struGsh.nAnalyzeIndex,0);
        }else{//! 独立修改
            pProfile->strctAI.bEnlabs[m_nCamId][nIdx] = bChecked;
            gFlow.SocParamssend(struGsh.nAnalyzeIndex,0);
        }

        //! 将标签的使能关联到标签的显示与隐藏
        pProfile->blabshowmap[m_modellbchk[nIdx]->text()] = bChecked ? false : true;
    }
}

/*!
 * \brief 采样阈值改变
 */
void AIWidget::oncapThdChanged()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 100000, m_captdle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_captdle->setText(QString("%1").arg(n_sample));
        struGsh.nSoccapthd = n_sample;
        gFlow.SocAdParamssend(struGsh.nAnalyzeIndex, 0);
    }
}

/*!
 * \brief 槽：同步拍照相机序号
 */
void AIWidget::onCoCapcamidxChanged(int nIdx)
{
//    auto *AnalyParams = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
//    int nCamid = -1;

    struGsh.nCamidvc.clear();
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        if(m_capCamidxbtn[i]->isChecked()){
            struGsh.nCamidvc.push_back(i);
        }

//        if( m_capCamidxbtn[i]->isChecked() &&
//            AnalyParams->struCaParams->sensorType[i] !=SENSOR_IM04){
//            struGsh.nCamidvc.push_back(i);
//        }

//        if(AnalyParams->bHasAssist &&
//            AnalyParams->struCaParams->nWorkMode[i] == 2){
//            nCamid = i;
//        }
    }

//    //! 做相机拍照关联
//    if(nCamid != -1){
//        int nId = gFlow.getMachineCamNum();
//        if(m_capCamidxbtn[nCamid]->isChecked()){
//             m_capCamidxbtn[nId-1]->setChecked(true);
//        }else{
//            m_capCamidxbtn[nId-1]->setChecked(false);
//        }
//    }
    gFlow.SocAdParamssend(struGsh.nAnalyzeIndex,nIdx);
}

/*!
 * \brief 槽：切换模型线数
 * \param nIdx
 */
void AIWidget::onModeldatalineChanged(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelLine = nIdx;
    gFlow.SocModelLine();
}

/*!
 * \brief 槽: AI模型分辨率切换
 * \param nIdx:0-512 1-1024 2-2048
 */
void AIWidget::onModelResChanged(int nIdx)
{
    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->raise();
    infoWidget->show();

    //! 下发切换命令
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nRes = nIdx;
    gFlow.SocAdParamssend(struGsh.nAnalyzeIndex,0);
    gFlow.sleep(1);

    //! 获取模型实际的信息
    gFlow.ReqSocInferParams(m_nCamId);
    gFlow.sleep(1);

    //! 提示
    int nRes = 1024;
    switch(nIdx){
    case 0:
        nRes = 512;
        break;
    case 1:
        nRes = 1024;
        break;
    case 2:
        nRes = 2048;
        break;
    default:
        nRes = 1024;
        break;
    }

    if(struGsh.nSocImgWid != nRes){
        infoWidget->setLabelText(QString("%1: %2").arg(myLan.notice).arg(myLan.pic_size_mismatch));
    }

    infoWidget->showSecs();
}

/*!
 * \brief 重叠系数改变
 */
void AIWidget::onCoffChanged()
{
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 1000.0, m_coffle->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        m_coffle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.ncoffvalue = n_sample;
    }
}

/*!
 * \brief 偏置系数改变
 */
void AIWidget::onoffsetChanged()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 1000, m_offsetle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_offsetle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.noffsetvalue = n_sample;
    }
}

/*!
 * \brief AIWidget::onBackbtnSlt
 */
void AIWidget::onBackbtnSlt()
{
    m_laywidstck->setCurrentIndex(0);
}

/*!
 * \brief AIWidget::onRefreshGraphicview
 * \param imgpath
 */
void AIWidget::onRefreshGraphicview(QString imgpath)
{
    if(!imgpath.isEmpty()){
        m_pixitem->setPixmap(QPixmap(QString("%1Imgs/%2").arg(MOUNT_UDISK_PATH).arg(imgpath)));
    }
}

/*!
 * \brief 槽: viewBtn
 */
void AIWidget::onCamViewBtnPressSlt()
{
    m_nCamId +=1;
    if(m_nCamId > gFlow.getMachineCamNum() -1){
        m_nCamId =0;
    }

    m_viewSwBtn->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nCamId]));

    //! 刷新标签使能
    refreshLabelEnable(m_nCamId);

    //! 模型combox
    refreshModelComb(m_nCamId);

    //! 刷新采集行和保留行
    m_caple[0]->setText(QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCapLine[m_nCamId]));
    m_caple[1]->setText(QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nCashLine[m_nCamId]));
}



/*!
 * \brief 相机工作模式切换
 * \param nIdx
 */
void AIWidget::oncamworkModelchg(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[nIdx] = m_camworkmodecomb[nIdx]->currentIndex();

    gFlow.sendSocWorkMode(nIdx);

    if(m_camworkmodecomb[nIdx]->currentIndex() == 2){
        struGsh.nAssisttomainCamId = nIdx;
    }
}


/*!
 * \brief 同步或者独立设置SOC参数
 */
void AIWidget::onSyncBtnPressedSlt()
{
    m_bSync = !m_bSync;
    if(m_bSync){
        m_syncSetBtn->setText(myLan.same);
    }else{
        m_syncSetBtn->setText(myLan.alone);
    }
}

/*!
 * \brief 面积阈值
 */
void AIWidget::onAreathdChgSlt()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 50000, m_areathdle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_areathdle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nareathd = n_sample;
    }
}


/*!
 * \brief 处理图片导出
 */
void AIWidget::onHandleImgExport()
{
    QMap<int,bool> resmap;
    QString msgStr;
    if (struGsh.nFeederState == 1) {
        msgStr =  QString("%1\n\n %2？")
                     .arg(myLan.affect_detect_res)
                     .arg(myLan.confirm_export_picture);
    }else{
        msgStr = QString("%1?").arg(myLan.confirm_export_picture);
    }

    myMessageBox msgBox(MSG_QUES, msgStr, "", g_mainWidget);

    if (msgBox.exec() == QDialog::Accepted) {
        infoWidget->setLabelText(myLan.apply_and_wait);
        infoWidget->raise();
        infoWidget->show();

        if(!gFlow.isUdiskExist()){
            infoWidget->setLabelText(QString("%1!").arg(myLan.no_found_udisk));
            infoWidget->showSecs();
            return;
        }

        if(!gFlow.mountUDisk()){
            infoWidget->setLabelText(myLan.mount_udisk_failed);
            infoWidget->showSecs();
        }else{
            char data[2] = {0,0};
            data[0]= 1;
            data[1]= 1;

            //! 做下特殊处理
            auto *AnalyParams = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
            QVector<int> vCamIdx = struGsh.nCamidvc; 
            int nMajor = -1, nAssist = -1;
            for(const int &n : struGsh.nCamidvc){
                if(AnalyParams->struCaParams->sensorType[n] ==SENSOR_IM04){
                    nAssist = n;
                }                                  
                
                if(AnalyParams->struCaParams->nWorkMode[n] == 2){
                    nMajor = n;
                }  
            }

            if(nMajor!=-1 && nAssist!=-1){
                vCamIdx.removeOne(nAssist);
            }
            
            //! 1.下发导出图片指令(转换和压缩)
            QVector<QPair<int, QString> > todoTask;
            for (const int &nId : vCamIdx) {
                QByteArray ba = gFlow.packet_TypeA_NetData(CAMERA_CK,MODULE_SOC, nId, CMD_SOC_IMG_EXPORT, data, 2);
                g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
                todoTask.append({nId, ""});
            }
            gFlow.msleep(3000);

            //! 2.图片转换&压缩
            m_uploadTask->setTask(todoTask);
            m_uploadTask->start();
        }
    }
}
/*!
 * \brief 处理日志导出
 */
void AIWidget::onHandleLogExport()
{
    QMap<int,bool> resmap;
    QString msgStr;
    if (struGsh.nFeederState == 1) {
        msgStr =  "色选过程中导出日志，会影响色选效果。\n\n 确认导出图片？";
    }else{
        msgStr = "确认导出图片?";
    }

    myMessageBox msgBox(MSG_QUES, msgStr, "", g_mainWidget);

    if (msgBox.exec() == QDialog::Accepted) {
        infoWidget->setLabelText("日志导出处理中,请稍后.....");
        infoWidget->raise();
        infoWidget->show();

        if(!gFlow.mountUDisk()){
            gFlow.umountUDisk();
            infoWidget->setLabelText("未检测到U盘");
            infoWidget->showSecs();
        }else if(struGsh.nCamidvc.isEmpty()){
            infoWidget->setLabelText("请勾选要导出的相机序号\n ");
            infoWidget->showSecs();
        }else{
        }
    }

    int num =0;
    QString resstr;
    QMap<int, bool>::const_iterator itor = resmap.constBegin();
      while (itor != resmap.constEnd()) {
          resstr += itor.value() ? QString("相机%1 成功  ").arg(itor.key()):
                                   QString("相机%1 失败  ").arg(itor.key());

          ++itor;
          num++;
          if(num%2 ==0){
              resstr += "\n";
          }
      }
    infoWidget->setLabelText(resstr);
    infoWidget->showSecs();
}

/*!
 * \brief 异步解压文件
 * \param zipFile 压缩文件名
 * \param unzipPath 解压路径
 * \return 解压是否成功
 */
bool AIWidget::unzipFileAsync(const QString &zipFile, const QString &unzipPath)
{
    QDir dir(unzipPath);
    if (!dir.exists()) {
        if (!dir.mkdir(unzipPath)) {
            qDebug() << "创建解压目录失败:" << unzipPath;
            return false;
        }
    } else {
        if (!dir.isEmpty()) {
            // 清空目录
            system(qPrintable(QString("rm -rf %1/*").arg(unzipPath)));
        }
    }

    QString fullPath = QString("%1%2").arg(MOUNT_UDISK_PATH).arg(zipFile);
//    qDebug() << "开始异步解压:" << fullPath << "到:" << unzipPath;

    QProcess process;
    process.start(QString("unzip -o %1 -d %2").arg(fullPath).arg(unzipPath));
    
    if (process.waitForFinished(60000)) {
        if (process.exitCode() == 0) {
            QProcess::execute("sync"); // 解压后立即同步
//            qDebug() << "异步解压完成:" << zipFile;
            return true;
        } else {
//            qDebug() << "异步解压失败，退出码:" << process.exitCode();
//            qDebug() << "错误输出:" << process.readAllStandardError();
            return false;
        }
    } else {
        process.kill();
//        qDebug() << "异步解压超时";
        return false;
    }
}

/*!
 * \brief 处理图片预览
 */
void AIWidget::handleIMGpreview()
{
    int nType = m_preViewtypegp->checkedId();
    if(nType == 1){
        onPreVireCropImg();
        m_laywidstck->setCurrentIndex(1);
        m_staclay->setCurrentIndex(1);
    }else{
        if(!gFlow.isUdiskExist()){
            infoWidget->setLabelText(myLan.no_found_udisk);
            infoWidget->showSecs();
        }else{
            if(gFlow.mountUDisk()){
                m_staclay->setCurrentIndex(0);

                SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget);
                dialog->setDir(MOUNT_UDISK_PATH, QStringList("*.zip"));
                if(dialog->exec() == QDialog::Accepted){
                    dialog->hide(); // 优化：解压前先隐藏dialog
                    infoWidget->raise();
                    infoWidget->setLabelText(myLan.apply_and_wait);
                    infoWidget->delayShow();
                    
                    QString zipFile = dialog->getFileName();
                    QString unzippath = QString("%1Imgs").arg(MOUNT_UDISK_PATH);
                    
                    // 检查是否已经有解压任务在进行
                    if (m_unzipWatcher && m_unzipWatcher->isRunning()) {
                        infoWidget->setLabelText("正在解压中，请稍候...");
                        infoWidget->showSecs();
                        delete dialog;
                        return;
                    }
                    
                    // 创建异步解压任务
                    m_unzipWatcher = new QFutureWatcher<bool>(this);
                    
                    // 连接完成信号
                    connect(m_unzipWatcher, &QFutureWatcher<bool>::finished, this, [=]() {
                        bool success = m_unzipWatcher->result();
                        if (success) {
                            // 解压成功，处理图片列表
                            QDir dir(unzippath);
                            QStringList namelist;
                            QStringList filters;
                            filters << QString("*.png") << QString("*.bmp");
                            QFileInfoList infolist = dir.entryInfoList(filters, QDir::Files|QDir::NoSymLinks);
                            for(const QFileInfo &fileinfo : infolist) {
                                namelist << fileinfo.fileName();
                            }
                            refreshIMGPreviewpage(namelist);
                            m_laywidstck->setCurrentIndex(1);
                            infoWidget->hide();
                        } else {
                            // 解压失败
                            infoWidget->setLabelText(myLan.unzip_file_failed);
                            infoWidget->showSecs();
                        }
                        m_unzipWatcher->deleteLater();
                        m_unzipWatcher = nullptr;
                    });
                    
                    // 启动异步解压任务
                    QFuture<bool> future = QtConcurrent::run([=]() {
                        return unzipFileAsync(zipFile, unzippath);
                    });
                    m_unzipWatcher->setFuture(future);
                }
                delete dialog;
            }else{
                infoWidget->setLabelText(myLan.mount_udisk_failed);
                infoWidget->showSecs();
            }
        }
    }    
}

/*!
 * \brief AIWidget::refreshModelComb
 * \param nIdx: 相机序号
 */
void AIWidget::refreshModelComb(int nIdx)
{
    if((nIdx < 0) || (nIdx > gFlow.getMachineCamNum() -1))
        return;

    m_modelcomb->blockSignals(true);

    for(int i=0;i<m_modelcomb->count();i++){
        m_modelcomb->removeItem(i);
    }
    m_modelcomb->clear();
    int nItem =0;
    for(int j=0; j<20; j++){
        if(QString(struGsh.sModelUnitInfo[nIdx].model[j].cnf) != "NULL"){
            m_modelcomb->insertItem(nItem++,QString("%1  %2").arg(j).arg(struGsh.sModelUnitInfo[nIdx].model[j].cnf), j);
        }
    }

    int nmodelIdx = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.nModelIdx[nIdx];
    if(m_modelcomb->count()>0&&m_modelcomb->count()>=nmodelIdx){
        m_modelcomb->setCurrentIndex(nmodelIdx);
    }

    m_modelcomb->blockSignals(false);
}

/*!
 * \brief 刷新标签使能
 * \param nIdx: 相机编号
 */
void AIWidget::refreshLabelEnable(int nCamId)
{
    if((nCamId<0) || (nCamId > gFlow.getMachineCamNum()-1))
        return;

    //! gobox名称
    if(m_bcaplabEn){
        m_aiLabelgb->setTitle(myLan.selct_enable_lab);
    }else{
        m_aiLabelgb->setTitle(myLan.label_enable);
    }

   //! 刷新名字 使能
   QList<QString> namelist = struGsh.vlabNamemap[nCamId].values();
   int ntotal = namelist.size();
   for(int i=0; i<MAX_LABELS; i++){
       if(i < ntotal){
           m_modellbchk[i]->setText(namelist.at(i));
           if(m_bcaplabEn){
               m_modellbchk[i]->setChecked(struGsh.bEnCaplabs[nCamId][i]);
           }else{
               m_modellbchk[i]->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.bEnlabs[nCamId][i]);
           }
           m_modellbchk[i]->show();
       }else{
           m_modellbchk[i]->hide();
       }
   }
}

/*!
 * \brief 刷新相机的工作模式
 * \param nCamId
 */
void AIWidget::refreshCamWorkmodeComb()
{
    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_camIdlb[i]->show();
            m_camworkmodecomb[i]->show();
            m_camIdlb[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
            m_camworkmodecomb[i]->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[i]);
        }else{
            m_camIdlb[i]->hide();
            m_camworkmodecomb[i]->hide();
        }
    }
}

/*!
 * \brief 刷新拍照相机信息
 */
void AIWidget::refreshCapcaminfo()
{
    m_aiCapgb->setDisabled(struGsh.nFeederState);

    for(int i=0; i <MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_capCamidxbtn[i]->show();
            m_capCamidxbtn[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        }else{
            m_capCamidxbtn[i]->hide();
        }

        // if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist &&
        //     struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[i] == SENSOR_IM04){
        //     m_capCamidxbtn[i]->setDisabled(true);
        // }
    }
}


bool AIWidget::getFilesandUnzip()
{

    //! 清空原有目录文件
    QString unzippath = QString("%1Imgs").arg(MOUNT_UDISK_PATH);
    QDir unzipDir(unzippath);
    if(!unzipDir.exists()) {
        unzipDir.mkdir(unzippath);
    } else if(!unzipDir.isEmpty()) {
        system(QString("rm -rf %1/*").arg(unzippath).toLocal8Bit().data());
    }


    //! 解压文件
    bool success = true;
    QProcess process;
    process.setWorkingDirectory(MOUNT_UDISK_PATH);

    QString cmd = "cd " + QString("%1").arg(MOUNT_UDISK_PATH) + " && ";
    for(const QString &fileName : m_fileList) {
        cmd += QString("unzip -o -q \"%1\" -d \"%2\" && ")
                .arg(fileName)
                .arg(unzippath);
    }
    cmd.chop(4);

    process.start("/bin/sh", QStringList() << "-c" << cmd);

    if(!process.waitForFinished(300000)) { // 5分钟超时
        process.kill();
        qDebug() << "Unzip timeout";
        success = false;
    } else if(process.exitCode() != 0) {
        qDebug() << "Unzip failed:" << process.readAllStandardError();
        success = false;
    }

    // if(success) {
    //     QProcess::execute("sync");
    // }

    return success;
}

bool AIWidget::findImgs()
{
    m_lineGroups.clear();
    m_lineGroups.reserve(1500);

    static const QRegExp filePattern("\\d+_\\d+_(\\d+)\\.");
    static const QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff"};
    
    //! 使用QDirIterator直接遍历,避免获取全部文件列表
    int processedFiles = 0;
    int invalidFiles = 0;
    QString imgDirPath = QString("%1Imgs").arg(MOUNT_UDISK_PATH);
    QDirIterator it(imgDirPath, filters, QDir::Files | QDir::NoSymLinks | QDir::AllDirs | QDir::NoDotAndDotDot);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = it.fileName();

        if (filePattern.indexIn(fileName) != -1) {
            bool ok;
            int lineNumber = filePattern.cap(1).toInt(&ok);
            if (ok && lineNumber > 0) {
                if (!m_lineGroups.contains(lineNumber)) {
                    m_lineGroups[lineNumber] = QStringList();
                }
                m_lineGroups[lineNumber].append(filePath);
                processedFiles++;
            } else {
                invalidFiles++;
            }
        } else {
            invalidFiles++;
        }
    }

//    // 7. 添加调试信息
//    qDebug() << "处理文件:" << processedFiles
//             << "无效文件:" << invalidFiles
//             << "分组数:" << m_lineGroups.size();

    return m_lineGroups.size() > 0;
}

/*!
 * \brief 计算目录大小
 * \param dirPath 目录路径
 * \return 目录大小（字节）
 */
qint64 AIWidget::calculateDirectorySize(const QString& dirPath)
{
    qint64 totalSize = 0;
    QDir dir(dirPath);
    
    if (!dir.exists()) {
        return 0;
    }
    
    QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo(it.fileInfo());
        totalSize += fileInfo.size();
    }
    
    return totalSize;
}

/*!
 * \brief 裁剪小图
 * \return
 */
bool AIWidget::cropImg()
{
    const QString saveRoot = "/userdata/CropImgs/";
    const qint64 maxSize = 1024 * 1024 * 1024;
    QAtomicInt   totalSize(0);
    
    //! 创建输出目录
    QDir dir(saveRoot);
    if(!dir.exists()) {
        if(!dir.mkpath(saveRoot)) {
            qDebug() << "Failed to create output directory";
            return false;
        }
    } else {
        system(QString("rm -rf %1/*").arg(saveRoot).toLocal8Bit().data());
    }

    //! 配置线程池
    QThreadPool threadPool;
    int threadCount = QThread::idealThreadCount();
    threadPool.setMaxThreadCount(threadCount > 0 ? threadCount : 4);
    
    QSet<QString> createdDirs;
    QMutex mutex;

    //! 遍历每行图片
    for(auto itRow = struGsh.lineBoxMap.constBegin(); 
        itRow != struGsh.lineBoxMap.constEnd(); ++itRow) {//! 遍历行号
        
        uint row = itRow.key();
        if(!m_lineGroups.contains(row)) continue;

        const QStringList& imgList = m_lineGroups[row];
        const QMap<int, QList<BBox>>& camBoxMap = itRow.value();

        for(auto itCam = camBoxMap.constBegin(); 
            itCam != camBoxMap.constEnd(); ++itCam) {//! 遍历相机
            
            int camId = itCam.key();
            const QList<BBox>& boxList = itCam.value();
            const auto& labelMap = struGsh.vlabNamemap[camId];

            //! 处理每张图片
            for(const QString& imgPath : imgList) {
                QStringList parts = imgPath.split('_');
                if(parts.size() < 3) continue;

                bool ok;
                int imgCamId = parts[1].toInt(&ok);
                if(!ok || imgCamId != camId) continue;

                // 处理图片中的每个目标框
                for(const BBox& box : boxList) {
                    QString labelName = labelMap.value(box.label, "other");
                    QString labelFolder = QString("%1/%2_%3")
                                        .arg(saveRoot)
                                        .arg(box.label)
                                        .arg(labelName);

                    // 创建标签目录
                    {
                        QMutexLocker locker(&mutex);
                        if(!createdDirs.contains(labelFolder)) {
                            QDir().mkpath(labelFolder);
                            createdDirs.insert(labelFolder);
                        }
                    }

                    // 提交图片处理任务
                    auto task = new ImageProcessTask(
                        imgPath, box, labelFolder, labelName,
                        totalSize, maxSize);
                        
                    threadPool.start(task);
                }
            }
        }
    }

    //! 等待所有任务完成
    threadPool.waitForDone();

    //! 同步文件系统
//  QProcess::execute("sync");

    return true;
}

/*!
 * \brief 处理裁剪小图
 */
void AIWidget::onHandleImg()
{
    QString msgStr = myLan.confirm_export_picture;
    myMessageBox msgBox(MSG_QUES, msgStr, "", g_mainWidget);

    if (msgBox.exec() == QDialog::Accepted) {
        if(!gFlow.isUdiskExist()){
            infoWidget->setLabelText(myLan.no_found_udisk);
            infoWidget->showSecs();
        }else{
            if(gFlow.mountUDisk()){
                m_fileList.clear();

                SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget,true);
                dialog->setDir(MOUNT_UDISK_PATH, QStringList("*.zip"));
                if(dialog->exec() == QDialog::Accepted){
                    m_fileList= dialog->getFileNames();
                }else{
                    gFlow.umountUDisk();
                    delete dialog;
                    return;
                }

                infoWidget->setLabelText(myLan.apply_and_wait);
                infoWidget->raise();
                infoWidget->show();
        
                // 创建watcher监控异步任务
                QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
        
                // 连接完成信号
                connect(watcher, &QFutureWatcher<bool>::finished, this, [=]() {
                    bool bRes = watcher->result();
                    if(!bRes){
                        infoWidget->setLabelText(myLan.operate_failed);
                    }else{
                        infoWidget->setLabelText(myLan.operate_sucess);
                    }
                    infoWidget->showSecs();
                    watcher->deleteLater();
                });
        
                // 启动异步任务
                QFuture<bool> future = QtConcurrent::run([=]() {
                    if(getFilesandUnzip()){
                        if(findImgs()){
                            if(cropImg()){
                                return true;
                            }
                        }
                    }
                    return false;
                });
                watcher->setFuture(future);
                delete dialog;
            }
        }
    }
}

/*!
 * \brief 预览裁剪图片
 */
void AIWidget::onPreVireCropImg()
{
    // 1. 遍历/userdata/CropImgs下所有label文件夹
    QString cropRoot = "/userdata/CropImgs";
    QDir rootDir(cropRoot);
    if (!rootDir.exists()) {
        infoWidget->setLabelText(myLan.no_find_imgdir);
        infoWidget->showSecs();
        return;
    }

    // 获取所有label文件夹
    QStringList labelDirs = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    qDebug()<<labelDirs;

    if (labelDirs.isEmpty()) {
        infoWidget->setLabelText(myLan.no_imgfile);
        infoWidget->showSecs();
        return;
    }

    m_labelListWidget->clear();
    m_labelListWidget->addItems(labelDirs);      
    m_labelListWidget->setCurrentRow(0);
    onLabelListWidgetItemClicked(m_labelListWidget->item(0));
}

/*!
 * \brief 标签文件夹列表项点击
 */
void AIWidget::onLabelListWidgetItemClicked(QListWidgetItem *item)
{
    m_currentImgFiles.clear();

    //! 递归查找所有图片
    QString labelDirPath = "/userdata/CropImgs/" + item->text();
    QDir labelDir(labelDirPath);

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QStringList imgFiles = labelDir.entryList(filters, QDir::Files | QDir::NoSymLinks | QDir::AllDirs | QDir::NoDotAndDotDot);
   
    for(const QString &fileName : imgFiles){
        QString absolutePath = labelDirPath + "/" + fileName;
        m_currentImgFiles.append(absolutePath);   
    }
 
    Refreshimglstwid();
}

/*!
 * \brief 裁剪图片列表项点击
 */
void AIWidget::onImgListWidgetItemClicked(QListWidgetItem *item)
{
    if (!item) return;
    
    //! 从item的存储数据中获取图片路径
    QString selectedImagePath = item->data(Qt::UserRole).toString();
    if (!selectedImagePath.isEmpty()) {
        QPixmap selectedImagePixmap(selectedImagePath);
        onShowEnlargedImage(selectedImagePixmap);
    }
}

/*!
 * \brief 刷新图片列表控件
 */
void AIWidget::Refreshimglstwid()
{
    m_imgListWidget->clear();
    
    //! 设置图标大小和网格大小
    m_imgListWidget->setIconSize(QSize(150, 150));
    m_imgListWidget->setGridSize(QSize(170, 170));
    
    for (int i = 0; i < m_currentImgFiles.size(); ++i) {
        QString imgPath = m_currentImgFiles[i];
        QFileInfo fileInfo(imgPath);
        QString fileName = fileInfo.fileName();
        
        //! 创建列表项 - 不显示文件名
        QListWidgetItem *item = new QListWidgetItem();
        
        //! 加载图片并创建缩略图
        QPixmap originalPixmap(imgPath);
        if (!originalPixmap.isNull()) {
            //! 创建缩略图
            QPixmap thumbnail = originalPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            item->setIcon(QIcon(thumbnail));    
        }
        
        //! 将文件路径存储在item的data中，以便点击时能获取
        item->setData(Qt::UserRole, imgPath);
        
        //! 设置项的大小提示
        item->setSizeHint(QSize(170, 170));
        
        //! 添加到列表控件
        m_imgListWidget->addItem(item);
    }
}

/*!
 * \brief 事件过滤器，处理图片视图的点击事件
 */
bool AIWidget::eventFilter(QObject *obj, QEvent *event)
{
   if (obj == m_enlargedImgView && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            onCloseEnlargedImage();
            return true;
        }
    }

   return QWidget::eventFilter(obj, event);
}

/*!
 * \brief AI参数设置
 */
void AIWidget::createAIparamsPage()
{
    m_aiParamssetWid = new QWidget();

    m_aiSetsetgb = new QGroupBox();
    m_aiSetsetgb->setMinimumWidth(600);
    m_aiSetsetgb->setMaximumHeight(470);

    m_modelswlb = new QLabel(myLan.AImodel);
    m_modelswlb->setFixedWidth(140);
    m_modelcomb = new QComboBox();
    QListWidget *modelinfolistwid = new QListWidget();
    m_modelcomb->setModel(modelinfolistwid->model());
    m_modelcomb->setView(modelinfolistwid);
    m_modelcomb->setMinimumWidth(650);
    m_modelcomb->setMinimumHeight(BTN_HEI);

    QStringList imglist;                        //! 采集行 保留行
    imglist <<myLan.cap_line<<myLan.retain_line;
    for(int i=0;i<2;i++){
        m_caplb[i] = new QLabel(imglist.at(i));
        m_caplb[i]->setFixedWidth(140);
        m_caple[i] = new MyLineEdit();
        m_caple[i]->setFixedSize(200,BTN_HEI);
    }

    m_cofflb = new QLabel(myLan.overlap_degree_coeff); //! 杂粮
    m_cofflb->setFixedWidth(140);
    m_coffle = new MyLineEdit();
    m_coffle->setFixedSize(200,BTN_HEI);

    m_offsetlb = new QLabel(myLan.center_rect);       //! 外接矩
    m_offsetlb->setFixedWidth(140);
    m_offsetle = new MyLineEdit();
    m_offsetle->setFixedSize(200,BTN_HEI);

    m_areathdlb = new QLabel(myLan.area_threshold);   //! 过滤小面积
    m_areathdlb->setFixedWidth(140);
    m_areathdle = new MyLineEdit();
    m_areathdle->setFixedSize(200,BTN_HEI);

    //! 暂时用不到
    m_dataLinelb = new QLabel(myLan.AImodel_type);  //! 传感器线数
    m_dataLinelb->setFixedWidth(140);
    m_dataLinelb->hide();
    m_dataLinecomb = new QComboBox();
    m_dataLinecomb->hide();
    QListWidget *datalinelstwid = new QListWidget();
    m_dataLinecomb->setModel(datalinelstwid->model());
    m_dataLinecomb->setView(datalinelstwid);
    m_dataLinecomb->setFixedSize(200,BTN_HEI);
    QStringList datalinestr;
    datalinestr<<"NRG"<<"NRB"<<"NGB"<<"RGB"<<"NRGB";
    for(int i=0; i<datalinestr.size(); i++){
        m_dataLinecomb->insertItem(i,datalinestr.at(i));
    }

    m_modelReslb = new QLabel(myLan.resolution);
    m_modelReslb->setFixedWidth(140);
    m_modelReslb->hide();
    m_modelRescomb = new QComboBox();
    m_modelRescomb->hide();
    QListWidget *reslstwid = new QListWidget();
    m_modelRescomb->setModel(reslstwid->model());
    m_modelRescomb->setView(reslstwid);
    m_modelRescomb->setFixedSize(200,BTN_HEI);
    QStringList reslinestr;
    reslinestr<<myLan.low_res<<myLan.hig_res<<myLan.ultra_hig_res;   //! 512 1024 2048
    for(int i=0; i<reslinestr.size(); i++){
        m_modelRescomb->insertItem(i,reslinestr.at(i));
    }

    //! AI工作模式设置
    QStringList worktypelst;
    worktypelst <<myLan.standard <<myLan.tradition
                <<myLan.std_hyperspectral << myLan.ultraviolet;

    m_camworksetGb = new QGroupBox();
    m_camworksetGb->setMinimumWidth(450);

    for(int i=0; i<MAX_CAM_NUM; i++){
        m_camIdlb[i] = new QLabel(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        m_camworklstwid[i] = new QListWidget();
        m_camworkmodecomb[i] = new QComboBox();
        m_camworkmodecomb[i]->setModel(m_camworklstwid[i]->model());
        m_camworkmodecomb[i]->setView(m_camworklstwid[i]);
        m_camworkmodecomb[i]->addItems(worktypelst);
        m_camworkmodecomb[i]->setFixedSize(320,BTN_HEI);
    }
}

/*!
 * \brief UI: 拍照设置
 */
void AIWidget::createAIphotoPage()
{
    QString radiosty = QString("QRadioButton::indicator{width:70px;height:70px;border:none;}"
                               "QRadioButton::indicator::unchecked{image:url(:/res/png/action-unchecked.png);}"
                               "QRadioButton::indicator::checked{image:url(:/res/png/action-checked.png);}");


    QStringList capsetlist,btnlist,previewtypelst,iconlst;
    capsetlist<<myLan.do_not_save<<myLan.raw_pic
               <<myLan.mark_pic;

    btnlist <<myLan.pic_export<<myLan.pic_preview
            <<myLan.CropImg_export<<myLan.pic_clear;

    previewtypelst<<myLan.ori_labimg<<myLan.small_labimg;

    iconlst <<":/res/png/img_export.png"
            <<":/res/png/img_preview.png"
            <<":/res/png/Edit_Cut.png"
            <<":/res/png/del.png";

    m_aiCapgb = new QGroupBox();
    m_aiCapgb->setMinimumWidth(750);
    m_aiCapgb->setMaximumHeight(470);

    m_captdlb = new QLabel(myLan.triger_lowlimit);
    m_captdlb->setFixedWidth(150);
    m_captdle = new MyLineEdit();
    m_captdle->setFixedWidth(200);
    m_captdle->setText(QString("%1").arg(20));

    m_preViewtypelb = new QLabel(myLan.previewtype);
    m_preViewtypelb->setFixedWidth(150);
    m_preViewtypegp = new QButtonGroup();
    m_preViewtypegp->setExclusive(true);
    for(int i=0; i<previewtypelst.size(); i++){
        m_preViewtypebtn[i] = new QRadioButton(previewtypelst.at(i));
        m_preViewtypebtn[i]->setFixedWidth(220);
        m_preViewtypebtn[i]->setStyleSheet(radiosty);
        m_preViewtypegp->addButton(m_preViewtypebtn[i],i);
    }
    m_preViewtypebtn[0]->setChecked(true);

    m_captypelb = new QLabel(myLan.capture_type);
    m_captypelb->setFixedWidth(150);
    m_capTypegp = new QButtonGroup();
    m_capTypegp->setExclusive(true);
    for(int i=0; i<capsetlist.size(); i++){
        m_capTypebtn[i] = new QRadioButton(capsetlist.at(i));
        m_capTypebtn[i]->setStyleSheet(radiosty);
        m_capTypebtn[i]->setFixedWidth(220);
        m_capTypegp->addButton(m_capTypebtn[i],i);
    }
    m_capTypebtn[0]->setChecked(true);

    m_camselectlb = new QLabel(myLan.camer);
    m_camselectlb->setFixedWidth(150);
    m_capcambtngp = new QButtonGroup();
    m_capcambtngp->setExclusive(false);
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_capCamidxbtn[i] = new QRadioButton(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        m_capCamidxbtn[i]->setStyleSheet(radiosty);
        m_capCamidxbtn[i]->setFixedWidth(220);
        m_capcambtngp->addButton(m_capCamidxbtn[i],i);
    }

    for(int i=0; i<btnlist.size(); i++){
       m_capbtn[i] = new QPushButton(btnlist.at(i));
       m_capbtn[i]->setFixedSize(BTN_WID+60, BTN_HEI);
       m_capbtn[i]->setIconSize(QSize(50,50));
       m_capbtn[i]->setIcon(QIcon(iconlst.at(i)));
    }

    //! 导图线程
    m_uploadTask = new AiImageSaveTask();
}

void AIWidget::createPhotoPreview()
{
    m_imgwid = new QWidget();   //! 图片预览页面

    //!  图片预览
    m_imgpreGb = new QGroupBox();
    m_imgview  = new QGraphicsView();
    m_imgscene = new QGraphicsScene();
    m_pixitem = new QGraphicsPixmapItem();
    m_imgview->setScene(m_imgscene);
    m_imgscene->addItem(m_pixitem);
    m_imgview->setRenderHint(QPainter::SmoothPixmapTransform);
    m_imgview->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_imgview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_imgview->installEventFilter(this);
    m_imglistwid = new QListWidget();
    m_imglistwid->setViewMode(QListView::ListMode);
    m_imglistwid->setFixedWidth(450);

    //! 预览裁剪图片
    m_preVireCropImgGb = new QGroupBox();
    m_labelListWidget = new QListWidget();
    m_labelListWidget->setViewMode(QListView::ListMode);
    m_labelListWidget->setFixedWidth(200);
    m_imgListWidget = new QListWidget();
    m_imgListWidget->setViewMode(QListView::IconMode);
    m_imgListWidget->setFlow(QListView::LeftToRight);
    m_imgListWidget->setFixedWidth(1400);

    m_enlargedImgWidget = new QWidget(this);
    m_enlargedImgWidget->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    m_enlargedImgWidget->resize(800, 800);
    m_enlargedImgWidget->hide();

    m_enlargedImgView = new QGraphicsView(m_enlargedImgWidget);
    m_enlargedImgScene = new QGraphicsScene(m_enlargedImgWidget);
    m_enlargedPixItem = new QGraphicsPixmapItem();
    m_enlargedImgView->setScene(m_enlargedImgScene);
    m_enlargedImgScene->addItem(m_enlargedPixItem);
    m_enlargedImgView->setRenderHint(QPainter::SmoothPixmapTransform);
    m_enlargedImgView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_enlargedImgView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_enlargedImgView->installEventFilter(this);

    //! 设置放大窗口的布局
    QVBoxLayout *enlargedLayout = new QVBoxLayout(m_enlargedImgWidget);
    enlargedLayout->setContentsMargins(0, 0, 0, 0);
    enlargedLayout->addWidget(m_enlargedImgView);


    m_bckBtn = new QPushButton(myLan.back);
    m_bckBtn->setFixedSize(BTN_WID,BTN_HEI);
    m_bckBtn->setStyleSheet(g_enableBtnStyle);
}

void AIWidget::layAIparamsPage()
{
    //! 标签使能
    QGridLayout *glaylb = new QGridLayout(m_aiLabelgb);
    glaylb->setContentsMargins(20,50,20,20);
    for(int i=0; i<MAX_LABELS; i++){
        glaylb->addWidget(m_modellbchk[i],i/8,i%8,1,1);
    }

    //! AI参数设置
    QHBoxLayout *hlayRow = new QHBoxLayout();
    hlayRow->addSpacing(60);
    hlayRow->addWidget(m_caplb[0]); //! 采集行和保留行
    hlayRow->addSpacing(10);
    hlayRow->addWidget(m_caple[0]);
    hlayRow->addStretch(1);
    hlayRow->addWidget(m_caplb[1]);
    hlayRow->addWidget(m_caple[1]);
    hlayRow->addStretch(3);

    QHBoxLayout *hlaycoef = new QHBoxLayout();
    hlaycoef->addSpacing(60);
    hlaycoef->addWidget(m_cofflb);  //! 重合度系数
    hlaycoef->addSpacing(10);
    hlaycoef->addWidget(m_coffle);
    hlaycoef->addStretch(1);
    hlaycoef->addWidget(m_offsetlb);  //! 偏置系数
    hlaycoef->addSpacing(10);
    hlaycoef->addWidget(m_offsetle);
    hlaycoef->addStretch(3);

    QHBoxLayout *hlayarea = new QHBoxLayout();
    hlayarea->addSpacing(60);
    hlayarea->addWidget(m_areathdlb); //! 面积阈值
    hlayarea->addSpacing(10);
    hlayarea->addWidget(m_areathdle);
    hlayarea->addStretch();

    QHBoxLayout *hlayline = new QHBoxLayout();
    hlayline->addSpacing(60);
    hlayline->addWidget(m_dataLinelb);  //! 模型线数和分辨率
    hlayline->addWidget(m_dataLinecomb);
    hlayline->addStretch(1);
    hlayline->addWidget(m_modelReslb);
    hlayline->addWidget(m_modelRescomb);
    hlayline->addStretch(3);

    QHBoxLayout *hlaymodel = new QHBoxLayout();//! 切模型
    hlaymodel->addSpacing(60);
    hlaymodel->addWidget(m_modelswlb);
    hlaymodel->addSpacing(10);
    hlaymodel->addWidget(m_modelcomb);
    hlaymodel->addStretch();

    QVBoxLayout *vlayAI= new QVBoxLayout(m_aiSetsetgb);
    vlayAI->addLayout(hlayRow);
    vlayAI->addLayout(hlaycoef);
    vlayAI->addLayout(hlayarea);
    vlayAI->addLayout(hlayline);
    vlayAI->addLayout(hlaymodel);


    //! SOC工作模式
    QVBoxLayout *vlaywork = new QVBoxLayout(m_camworksetGb);
    for(int i=0; i<MAX_CAM_NUM; i++){
        QHBoxLayout *layitem = new QHBoxLayout();
        layitem->addWidget(m_camIdlb[i]);
        layitem->addWidget(m_camworkmodecomb[i]);
        vlaywork->addLayout(layitem);
    }


    QHBoxLayout *hlayAIparams = new QHBoxLayout(m_aiParamssetWid);
    hlayAIparams->addWidget(m_aiSetsetgb);
    hlayAIparams->addSpacing(20);
    hlayAIparams->addWidget(m_camworksetGb);

}

void AIWidget::layAIphotoPage()
{
    //! 采样阈值
    QHBoxLayout *hlaycaptd = new QHBoxLayout();
    hlaycaptd->addSpacing(60);
    hlaycaptd->addWidget(m_captdlb);
    hlaycaptd->addSpacing(30);
    hlaycaptd->addWidget(m_captdle);
    hlaycaptd->addStretch();


    QGridLayout *glaypre = new QGridLayout();
    glaypre->setHorizontalSpacing(60);
    glaypre->addWidget(m_preViewtypebtn[0],0,0);
    glaypre->addWidget(m_preViewtypebtn[1],0,1);

    QHBoxLayout *hlaypreview = new QHBoxLayout();
    hlaypreview->addSpacing(60);
    hlaypreview->addWidget(m_preViewtypelb);
    hlaypreview->addSpacing(30);

    hlaypreview->addLayout(glaypre);
    hlaypreview->addStretch();

    //! 采图模式
    QHBoxLayout *hlaycapmode= new QHBoxLayout();
    hlaycapmode->addSpacing(60);
    hlaycapmode->addWidget(m_captypelb);
    hlaycapmode->addSpacing(30);

    QGridLayout *glaycapmode = new QGridLayout();
    glaycapmode->setHorizontalSpacing(60);
    for(int i=0; i<3; i++){
        glaycapmode->addWidget(m_capTypebtn[i],0,i);
    }
    hlaycapmode->addLayout(glaycapmode);
    hlaycapmode->addStretch();

    //! 采图相机
    QHBoxLayout *hlaycocap = new QHBoxLayout();
    hlaycocap->addSpacing(60);
    hlaycocap->addWidget(m_camselectlb);
    hlaycocap->addSpacing(30);
    QGridLayout *glaycocap= new QGridLayout();
    glaycocap->setHorizontalSpacing(60);
    for(int i=0; i<MAX_CAM_NUM; i++){
        glaycocap->addWidget(m_capCamidxbtn[i],0,i);
    }
    hlaycocap->addLayout(glaycocap);
    hlaycocap->addStretch();

    //! 图片操作
    QHBoxLayout *hlaycapbtn = new QHBoxLayout();
    hlaycapbtn->addSpacing(100);
    for(int i=0; i<4; i++){
        hlaycapbtn->addWidget(m_capbtn[i]);
        hlaycapbtn->addSpacing(100);
    }
    hlaycapbtn->addStretch();

    QVBoxLayout *vlaycapset = new QVBoxLayout(m_aiCapgb);
    vlaycapset->addLayout(hlaycaptd);
    vlaycapset->addLayout(hlaypreview);
    vlaycapset->addLayout(hlaycapmode);
    vlaycapset->addLayout(hlaycocap);
    vlaycapset->addSpacing(10);
    vlaycapset->addLayout(hlaycapbtn);
}

void AIWidget::layPhotoPreview()
{
    //! 标记图片预览
    QHBoxLayout *hlayimgtop = new QHBoxLayout(m_imgpreGb);
    hlayimgtop->addWidget(m_imglistwid);
    hlayimgtop->addWidget(m_imgview);

    //! 裁剪图片预览
    QHBoxLayout *hlaypreVireCropImg = new QHBoxLayout(m_preVireCropImgGb);
    hlaypreVireCropImg->addWidget(m_labelListWidget);
    hlaypreVireCropImg->addWidget(m_imgListWidget);

    m_staclay = new QStackedLayout();
    m_staclay->addWidget(m_imgpreGb);
    m_staclay->addWidget(m_preVireCropImgGb);

    QHBoxLayout *hlayimgdown = new QHBoxLayout();
    hlayimgdown->addStretch();
    hlayimgdown->addWidget(m_bckBtn);
    hlayimgdown->addSpacing(10);

    QVBoxLayout *vlayimg = new QVBoxLayout(m_imgwid);
    vlayimg->addLayout(m_staclay);
    vlayimg->addLayout(hlayimgdown);
    vlayimg->addSpacing(10);
}

/*!
 * \brief 显示放大图片
 */
void AIWidget::onShowEnlargedImage(QPixmap pixmap)
{
    if (m_bImageEnlarged) return;
    
    //! 获取当前显示的图片
    QPixmap currentPixmap = pixmap;
    if (currentPixmap.isNull()) return;
    
    //! 在放大窗口中显示图片
    m_enlargedPixItem->setPixmap(currentPixmap);
    m_enlargedImgView->fitInView(m_enlargedPixItem, Qt::KeepAspectRatio);
    
    //! 将放大窗口设置到父窗口中心
    QWidget *parentWidget = this->window();
    if (parentWidget) {
        QRect parentGeometry = parentWidget->geometry();
        int centerX = parentGeometry.x() + (parentGeometry.width() - m_enlargedImgWidget->width()) / 2;
        int centerY = parentGeometry.y() + (parentGeometry.height() - m_enlargedImgWidget->height()) / 2;
        m_enlargedImgWidget->move(centerX, centerY);
    }
    
    //! 显示放大窗口
    m_enlargedImgWidget->show();
    m_enlargedImgWidget->raise();
    m_enlargedImgWidget->activateWindow();
    
    m_bImageEnlarged = true;
}

/*!
 * \brief 关闭放大图片
 */
void AIWidget::onCloseEnlargedImage()
{
    if (!m_bImageEnlarged) return;
    
    m_enlargedImgWidget->hide();
    m_bImageEnlarged = false;
}

/*!
 * \brief 槽: 图片上传结束
 * \param str
 */
void AIWidget::onUploadFinished(QString str)
{
    gFlow.umountUDisk(MOUNT_UDISK_PATH);
    gFlow.msleep(1000);

    infoWidget->setLabelText(str);
    infoWidget->showSecs();    
}

/*!
 * \brief 槽: 图片上传反馈
 * \param str
 * \param type
 */
void AIWidget::onUpdataUploadTips(QString str, int type)
{
    Q_UNUSED(type);
    infoWidget->setLabelText(str);
}









