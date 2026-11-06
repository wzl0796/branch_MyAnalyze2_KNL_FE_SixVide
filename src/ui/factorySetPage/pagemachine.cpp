#include "pagemachine.h"

pageMachine::pageMachine(QWidget *parent):QWidget(parent)
{
    m_bSetcamNum[0] = false;
    m_bSetcamNum[1] = false;

    createpage();
    laypage();
    connectAndSlt();
}

pageMachine::~pageMachine()
{

}

void pageMachine::createpage()
{
    m_wid = new QWidget();
    m_stackwid = new QStackedWidget();
    m_stackwid->addWidget(m_wid);
    m_stackwid->setCurrentIndex(0);

    //! 机型
    m_machineGb = new QGroupBox(QString("%1").arg(myLan.params_set));
    m_machineGb->setMaximumWidth(600);
    m_machinelb = new QLabel(QString("%1").arg(myLan.machine));
    m_machicomb = new QComboBox();
    QListWidget *machinelistwid = new QListWidget();
    m_machicomb->setModel(machinelistwid->model());
    m_machicomb->setView(machinelistwid);
    m_machicomb->setFixedSize(150,60);
    m_machicomb->insertItem(0,myLan.bottle);
    m_machicomb->insertItem(1,myLan.grains);

    m_bEnAssistlb = new QLabel(myLan.assist_camer);
    m_bEnAssistSw = new mySwitchButton();
    m_bEnAssistSw->setFixedSize(150,60);

    m_camNumlb = new QLabel(myLan.major_camer);
    m_camNumle = new MyLineEdit();
    m_camNumle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_camNumle->setFixedSize(150,60);


    m_bEnMutilablb = new QLabel(QString("%1").arg(myLan.multilab));
    m_bEnMultilabSw = new mySwitchButton();
    m_bEnMultilabSw->setFixedSize(150,60);
    if(struCnfg.nMachineType == nMachine_Bottle){
        m_bEnMutilablb->hide();
        m_bEnMultilabSw->hide();
    }

    m_bEnadjbysorterlb = new QLabel(myLan.sorter_jointdebug);
    m_bEnadjbysortSw = new mySwitchButton();
    m_bEnadjbysortSw->setFixedSize(150,60);

    m_bEnAdjTipslb = new QLabel(myLan.sorter_adj_tips);
    m_bEnAdjTipsSw = new mySwitchButton();
    m_bEnAdjTipsSw->setFixedSize(150,60);

    m_bEnPuritylb = new QLabel(myLan.impurity_sta);
    m_bEnPuritySw = new mySwitchButton();
    m_bEnPuritySw->setFixedSize(150,60);
    if(struCnfg.nMachineType == nMachine_Bottle){
        m_bEnPuritylb->hide();
        m_bEnPuritySw->hide();
    }

    m_trigertypelb = new QLabel(myLan.triger_type);
    m_trigertypecomb = new QComboBox();
    QListWidget *trigertypelstwid = new QListWidget();
    m_trigertypecomb->setModel(trigertypelstwid->model());
    m_trigertypecomb->setView(trigertypelstwid);
    m_trigertypecomb->setFixedSize(150,60);
    m_trigertypecomb->insertItem(0,myLan.internal);
    m_trigertypecomb->insertItem(1,myLan.external);
    m_trigertypecomb->setDisabled(true);

    m_autosamplelb = new QLabel(myLan.auto_sample);
    m_autosampleSw = new mySwitchButton();
    m_autosampleSw->setFixedSize(150,60);

    m_weblb = new QLabel(QString("%1   ").arg(myLan.loT));
    m_webEnSw = new mySwitchButton();
    m_webEnSw->setFixedSize(150,60);

    m_lightsynclb = new QLabel(myLan.light_sync);
    m_lightsyncsw = new mySwitchButton();
    m_lightsyncsw->setFixedSize(150,60);
    if(struCnfg.nMachineType == nMachine_Crop){
        m_lightsynclb->hide();
        m_lightsyncsw->hide();
    }

    m_staticTypelb = new QLabel(QString("%1").arg(myLan.sta_type));
    m_staticTypecob = new QComboBox();
    m_staticTypecob->setFixedSize(150,60);
    QListWidget *statypelstwid = new QListWidget();
    m_staticTypecob->setModel(statypelstwid->model());
    m_staticTypecob->setView(statypelstwid);
    m_staticTypecob->insertItem(0,myLan.statistic_PPM);
    m_staticTypecob->insertItem(1,myLan.statistic_percentage);
    if(struCnfg.nMachineType == nMachine_Crop){
        m_staticTypelb->hide();
        m_staticTypecob->hide();
    }


    m_bSimulatelb = new QLabel(myLan.simulate_en);
    m_simlateSw = new mySwitchButton();
    m_simlateSw->setFixedSize(150,60);
    if(struCnfg.nMachineType == nMachine_Bottle){
        m_bSimulatelb->hide();
        m_simlateSw->hide();
    }

    //! 统计组
    bool bBottle = struCnfg.nMachineType == nMachine_Bottle ? true : false;
    m_camsetGb = new QGroupBox(myLan.statics_gp_set);
    m_camsetGb->setMaximumWidth(500);
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_camIdxlb[i] = new QLabel(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        m_alonechk[i] = new MyCheckBox(myLan.alone);
        if(bBottle){
            m_alonechk[i]->setDisabled(true);
        }
    }

    //! 相机名称
    m_camNamesetGb = new QGroupBox(myLan.camer_name_set);
    m_camNamesetGb->setMaximumWidth(500);
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_camNamebtn[i] = new QPushButton(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
    }


    m_applyBtn = new QPushButton(myLan.apply);
    m_applyBtn->setFixedSize(BTN_WID+30,BTN_HEI);
    m_applyBtn->setStyleSheet(g_enableBtnStyle);
    m_applyBtn->setIcon(QIcon(":/res/png/Action_Apply.png"));
    m_applyBtn->setIconSize(QSize(50,50));

    m_oneKeyparmsBtn = new QPushButton(myLan.onekeyparams);
    m_oneKeyparmsBtn->setFixedSize(140,75);
    m_oneKeyparmsBtn->hide();


    m_paramsBackBtn = new QPushButton(myLan.params_backup);
    m_paramsBackBtn->setFixedSize(BTN_WID+50,BTN_HEI);
    m_paramsBackBtn->setIcon(QIcon(":/res/png/Action_Backup.png"));
    m_paramsBackBtn->setIconSize(QSize(50,50));
    m_paramsBackBtn->setStyleSheet(g_enableBtnStyle);


    m_paramsRestoreBtn = new QPushButton(myLan.params_restore);
    m_paramsRestoreBtn->setFixedSize(BTN_WID+50,BTN_HEI);
    m_paramsRestoreBtn->setIcon(QIcon(":/res/png/Action_Restore.png"));
    m_paramsRestoreBtn->setIconSize(QSize(50,50));
    m_paramsRestoreBtn->setStyleSheet(g_enableBtnStyle);

}

void pageMachine::laypage()
{
    //! 机型
    m_glayMachine = new QGridLayout(m_machineGb);
    m_glayMachine->setContentsMargins(5,40,15,10);

    refreshlayout();

    //! 统计组划分
    QVBoxLayout *vlaycamGb = new QVBoxLayout(m_camsetGb);
    vlaycamGb->setContentsMargins(10,50,10,10);
    QHBoxLayout *hlaycamset[MAX_CAM_NUM];
    for(int i=0; i<MAX_CAM_NUM; i++){
        hlaycamset[i] = new QHBoxLayout();
        hlaycamset[i] ->addWidget(m_camIdxlb[i]);
        hlaycamset[i] ->addWidget(m_alonechk[i]);
        vlaycamGb->addLayout(hlaycamset[i]);
    }

    //! 相机名称修改
    QVBoxLayout *vlaycamnameGb = new QVBoxLayout(m_camNamesetGb);
    vlaycamnameGb->setContentsMargins(10,50,10,10);
    for(int i=0; i<MAX_CAM_NUM; i++){
        vlaycamnameGb->addWidget(m_camNamebtn[i]);
    }


    QGridLayout *glaytop = new QGridLayout();
    glaytop->addWidget(m_machineGb,0,0,1,1);
    glaytop->addWidget(m_camsetGb,0,1,1,1);
    glaytop->addWidget(m_camNamesetGb,0,2,1,1);

    QHBoxLayout *vlaydown = new QHBoxLayout();
    vlaydown->addSpacing(5);
    vlaydown->addWidget(m_applyBtn);
    vlaydown->addSpacing(20);
    vlaydown->addWidget(m_oneKeyparmsBtn);
    vlaydown->addStretch();
    vlaydown->addWidget(m_paramsBackBtn);
    vlaydown->addSpacing(20);
    vlaydown->addWidget(m_paramsRestoreBtn);
    vlaydown->addSpacing(5);

    QVBoxLayout *vlaywid = new QVBoxLayout(m_wid);
    vlaywid->addLayout(glaytop);
    vlaywid->addLayout(vlaydown);


    QHBoxLayout *hlaymain = new QHBoxLayout(this);
    hlaymain->addWidget(m_stackwid);
}

void pageMachine::connectAndSlt()
{
    connect(m_oneKeyparmsBtn,   SIGNAL(pressed()),              this,   SLOT(onOnekeyparmBtnSlt()));
    connect(m_applyBtn,         SIGNAL(pressed()),              this,   SLOT(onApplyBtnPressSlt()));
    connect(m_paramsBackBtn,    SIGNAL(pressed()),              this,   SLOT(onParamsbckBtnPressedSlt()));
    connect(m_paramsRestoreBtn, SIGNAL(pressed()),              this,   SLOT(onParamsRestoreBtnPressedSlt()));
    connect(m_machicomb,        SIGNAL(currentIndexChanged(int)),this,  SLOT(onMachineCombIdxSlt(int)));
    connect(m_bEnAssistSw,      SIGNAL(checkedChanged(bool)),   this,   SLOT(onEnAssistCamChgSlt(bool)));
    connect(m_camNumle,         SIGNAL(pressed()),              this,   SLOT(onCamnumlepressedSlt()));
    connect(m_bEnMultilabSw,    SIGNAL(checkedChanged(bool)),   this,   SLOT(onrationchkpressedSlt(bool)));
    connect(m_bEnadjbysortSw,   SIGNAL(checkedChanged(bool)),   this,   SLOT(onEncosorteradjustChg(bool)));
    connect(m_bEnPuritySw,      SIGNAL(checkedChanged(bool)),   this,   SLOT(onEnPurityStatics(bool)));
    connect(m_trigertypecomb,   SIGNAL(currentIndexChanged(int)),this,  SLOT(onTrigerTypeChgSlt(int)));
    connect(m_autosampleSw,     SIGNAL(checkedChanged(bool)),    this,  SLOT(onautoSampleChgSlt(bool)));
    connect(m_webEnSw,          SIGNAL(checkedChanged(bool)),    this,  SLOT(onEnwebSlt(bool)));
    connect(m_lightsyncsw,      SIGNAL(checkedChanged(bool)),    this,  SLOT(onLightSyncChgSlt(bool)));
    connect(m_staticTypecob,    SIGNAL(currentIndexChanged(int)),this,  SLOT(onStaicTypeChgSlt(int)));
    connect(m_bEnAdjTipsSw,     SIGNAL(checkedChanged(bool)),    this, SLOT(onAdjSorterTipsChgSlt(bool)));
    connect(m_simlateSw,        SIGNAL(checkedChanged(bool)),    this, SLOT(onSimulateSwChanged(bool)));

    //! 统计组
    QSignalMapper* sigModetypemap = new QSignalMapper();
    for(int i=0; i<MAX_CAM_NUM; i++){
        sigModetypemap->setMapping(m_alonechk[i], i);
        connect(m_alonechk[i], SIGNAL(pressed()), sigModetypemap, SLOT(map()));
    }
    connect(sigModetypemap,SIGNAL(mapped(int)),this,SLOT(onModetypeChgSlt(int)));

    //! 相机名称
    QSignalMapper* sigcamnamemap = new QSignalMapper();
    for(int i=0; i<MAX_CAM_NUM; i++){
        sigcamnamemap->setMapping(m_camNamebtn[i], i);
        connect(m_camNamebtn[i], SIGNAL(pressed()), sigcamnamemap, SLOT(map()));
    }
    connect(sigcamnamemap,SIGNAL(mapped(int)),this,SLOT(onCanNameBtnChgSlt(int)));
}

/*!
 * \brief 刷新页面
 */
void pageMachine::refreshPagemachine()
{
    //! 机型参数刷新
    m_machicomb->blockSignals(true);

    m_machicomb->setCurrentIndex(struCnfg.nMachineType);
    m_bEnAssistSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist);
    m_camNumle->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum));
    m_bEnadjbysortSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass);
    m_bEnPuritySw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPurityStats);
    m_autosampleSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample);
    m_bEnMultilabSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnMultilab);
    m_trigertypecomb->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamTrigerType);
    m_webEnSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb);
    m_staticTypecob->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaisticType);
    m_lightsyncsw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnlightSync);
    m_bEnAdjTipsSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAdjSorterTips);
    m_simlateSw->setChecked(struGsh.bEnSimulate);

    bool bBottle = struCnfg.nMachineType == nMachine_Bottle ? true: false;
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_alonechk[i]->setDisabled(bBottle);
    }

    //! 刷新统计组
    refreshStatisGb();

    //! 刷新相机名称
    refreshcamNameGb();

    m_machicomb->blockSignals(false);
}

/*!
 * \brief pageMachine::backupParams
 * \param tooltype: 0:本地 1:u盘  2:云端
 * \param paramsType 0:屏参数 1：联调参数
 * \return
 */
bool pageMachine::backupParams(int tooltype, int paramsType)
{
    bool ret = true;
    QString fileName;
    fileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");

    QString sDirUdisk;


    QString dircnf("/ssd/cnf"); //! 默认保存屏参数
    if(paramsType ==1){
        dircnf = "/userdata/cnf";
        fileName = QString("Adj_%1").arg(fileName);
    }else{
        fileName = QString("Screen_%1").arg(fileName);
    }

    QDir dir;
    switch(tooltype) {
    case 2: //! 上传服务器
        if(paramsType==1){
            QString tmpdir = "/userdata/SorterAdjcnf";
            if(gFlow.copyDir(dircnf, tmpdir)){
                emit siguploadparmas(tmpdir,2);
            }else{
                ret = false;
            }
        }else{
            emit siguploadparmas(dircnf,2);
        }
        break;
    case 1:
        if (gFlow.mountUDisk()){ //! 拷贝到优盘
            sDirUdisk = MOUNT_UDISK_PATH;

            ret = gFlow.zipWiter(QString("%1cnf_manual_%2.zip").arg(sDirUdisk).arg(fileName), dircnf);
            gFlow.sleep(1);
            gFlow.SystemCmd("sync");
            gFlow.umountUDisk(MOUNT_UDISK_PATH);
        }
        break;
    case 0:
        if(paramsType ==1){
            sDirUdisk = "/userdata/cnf_bck";
            fileName = QString("Adj");
        }else{
            sDirUdisk = "/ssd/cnf_bck";
            fileName = QString("Screen");
        }

        dir.setPath(sDirUdisk);
        if (!dir.exists()) {
            dir.mkpath(sDirUdisk);
        }else{
            if(!dir.isEmpty()){
                dir.removeRecursively();
                dir.mkpath(sDirUdisk);
            }
        }

        ret = gFlow.zipWiter(QString("%1/cnf_manual_%2.zip").arg(sDirUdisk).arg(fileName), dircnf);
        gFlow.sleep(1);
        gFlow.SystemCmd("sync");
        break;
    default:
        break;
    }

    return ret;
}

/*!
 * \brief 槽：下发整机参数
 */
void pageMachine::onOnekeyparmBtnSlt()
{
    gFlow.sendAll(true);
}

/*!
 * \brief 槽：应用机器
 */
void pageMachine::onApplyBtnPressSlt()
{
    gFlow.sendAll(true);
    // setMachinedefaultParm();
    emit sigApplyMachine();

}

/*!
 * \brief 槽：机型
 * nidx: 0:代表瓶片  1：杂粮
 */
void pageMachine::onMachineCombIdxSlt(int nIdx)
{
    if(!m_bSetcamNum[0] || !m_bSetcamNum[1]){
        m_machicomb->blockSignals(true);
        if(nIdx ==1){
            m_machicomb->setCurrentIndex(0);
        }else{
            m_machicomb->setCurrentIndex(1);
        }
        m_machicomb->blockSignals(false);

        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.set_cameranum_first));
        infoWidget->showSecs(5);
        return;
    }

    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_set_cam), myLan.notice, g_mainWidget);
    msgBox.setOKBtnText(myLan.yes);
    msgBox.setCancelBtnText(myLan.no);
    if(msgBox.exec() == QDialog::Rejected){
        m_machicomb->blockSignals(true);
        if(nIdx ==1){
            m_machicomb->setCurrentIndex(0);
        }else{
            m_machicomb->setCurrentIndex(1);
        }
        m_machicomb->blockSignals(false);
        return;
    }else{
        msgBox.close();
    }

    infoWidget->raise();
    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->show();

    //! 设置机型
    struCnfg.nMachineType = nIdx;
    gFlow.sendMachineType();

    if(struCnfg.nMachineType == nMachine_Bottle){
        m_bEnMutilablb->hide();
        m_bEnMultilabSw->hide();

        m_bEnPuritylb->hide();
        m_bEnPuritySw->hide();

        m_lightsynclb->show();
        m_lightsyncsw->show();

        m_staticTypelb->show();
        m_staticTypecob->show();

        m_bSimulatelb->hide();
        m_simlateSw->hide();
    }else{
        m_bEnMutilablb->show();
        m_bEnMultilabSw->show();

        m_bEnPuritylb->show();
        m_bEnPuritySw->show();

        m_lightsynclb->hide();
        m_lightsyncsw->hide();

        m_staticTypelb->hide();
        m_staticTypecob->hide();

        m_bSimulatelb->show();
        m_simlateSw->show();
    }

    refreshlayout();

    emit sigMachineTypechg(nIdx==nMachine_Bottle?true:false); //!  标定页面

    //! 刷新统计组
    for(int i=0;i<MAX_CAM_NUM; i++){
        if(struCnfg.nMachineType == nMachine_Bottle){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i] =1;
        }else{
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i] =0;
        }
    }
    refreshStatisGb(true);

    setMachinedefaultParm();

    infoWidget->setLabelText(myLan.operate_sucess_system_reboot);
    infoWidget->showSecs(3);

     system("killall mystart.sh; killall myrestart.sh; cd /app && ./myrestart.sh &");

}

/*!
 * \brief 相机合并结果设置
 * \param nIdx
 */
void pageMachine::onModetypeChgSlt(int nIdx)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;


    int ntye = m_alonechk[nIdx]->isChecked()?1:0;  //! 0:相同  1：独立
    pProfile->nType[nIdx] = ntye;

    QStringList samelist;
    pProfile->nStaticGroup = 0;
    pProfile->vUnitaddrlist.clear();
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        if(pProfile->nType[i]){ //! 独立
            QStringList list;
            list<<QString::number(i);
            pProfile->nStaticGroup +=1;
            pProfile->vUnitaddrlist.push_back(list);
        }else{
            samelist<<QString::number(i);
        }
    }

    if(!samelist.isEmpty()){
        pProfile->nStaticGroup +=1;
        pProfile->vUnitaddrlist.push_back(samelist);
    }

    gFlow.ResetGlobalStaParams();
    
    infoWidget->raise();
    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->show();
    for(int n=0; n<gFlow.getMachineCamNum(); n++){
        gFlow.reqStainfo(n);
        gFlow.msleep(20);
    }
    gFlow.msleep(2000);
    infoWidget->hide();

}

/*!
 * \brief 是否使能辅配相机
 * \param bEn
 */
void pageMachine::onEnAssistCamChgSlt(bool bEn)
{
    infoWidget->raise();
    if(bEn&&struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum >5){
        m_bEnAssistSw->setChecked(false);
        infoWidget->setLabelText(QString("%1!").arg(myLan.over_max_camer_num));
        infoWidget->showSecs();
    }else{
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist = bEn;

        //! 重置相机标签vec
        struGsh.vlabNamemap.resize(gFlow.getMachineCamNum());

        //! 刷新统计组
        for(int i=0;i<MAX_CAM_NUM; i++){
            if(struCnfg.nMachineType == nMachine_Bottle){
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i] =1;
            }else{
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i] =0;
            }
        }
        refreshStatisGb(true);

        //! 刷新相机名称修改
        refreshcamNameGb();

        m_bSetcamNum[1] = true;

        myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_set_machineAndmajor), myLan.notice, g_mainWidget);
        msgBox.setOKBtnText(myLan.yes);
        msgBox.setCancelBtnText(myLan.no);
        if(msgBox.exec() == QDialog::Accepted){
            msgBox.close();

            infoWidget->raise();
            infoWidget->setLabelText(myLan.apply_and_wait);
            infoWidget->show();
            setMachinedefaultParm();

            infoWidget->setLabelText(myLan.operate_sucess_system_reboot);
            infoWidget->showSecs(3);

            system("killall mystart.sh; killall myrestart.sh; cd /app && ./myrestart.sh &");
        }
    }
}

/*!
 * \brief 修改相机名称
 * \param nIdx
 */
void pageMachine::onCanNameBtnChgSlt(int nIdx)
{
    myInputMethod input(myLan.camera_name, m_camNamebtn[nIdx]->text(), g_mainWidget);

    int ret = input.exec();
    if (ret == QDialog::Accepted) {
        QString tmp = input.getText().trimmed();

        m_camNamebtn[nIdx]->setText(tmp);
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nIdx] = tmp;

        m_camIdxlb[nIdx]->setText(tmp); //! 同步更新统计组的标签名称
    }
}

/*!
 * \brief 相机个数改变
 */
void pageMachine::onCamnumlepressedSlt()
{
    int max = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist?5:6;
    myInputPanel *inputPanel = new myInputPanel(intType, 1, max, m_camNumle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_camNumle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum = n_sample;

        //! 重置相机标签vec
        struGsh.vlabNamemap.resize(gFlow.getMachineCamNum());

        //! 刷新统计组
        for(int i=0;i<MAX_CAM_NUM; i++){
            if(struCnfg.nMachineType == nMachine_Bottle){
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i] =1;
            }else{
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i] =0;
            }
        }
        refreshStatisGb(true);

        //! 刷新相机名称修改
        refreshcamNameGb();


        m_bSetcamNum[0] = true;


        myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_set_machineAndassist), myLan.notice, g_mainWidget);
        msgBox.setOKBtnText(myLan.yes);
        msgBox.setCancelBtnText(myLan.no);
        if(msgBox.exec() == QDialog::Accepted){
            msgBox.close();

            infoWidget->raise();
            infoWidget->setLabelText(myLan.apply_and_wait);
            infoWidget->show();
            setMachinedefaultParm();

            infoWidget->setLabelText(myLan.operate_sucess_system_reboot);
            infoWidget->showSecs(3);

            system("killall mystart.sh; killall myrestart.sh; cd /app && ./myrestart.sh &");
        }
    }
}

/*!
 * \brief 占比
 */
void pageMachine::onrationchkpressedSlt(bool bsta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnMultilab = bsta;
    gFlow.sendMachineType();
}
/*!
 * \brief 槽: 使能联调色选机
 * \param bEn
 */
void pageMachine::onEncosorteradjustChg(bool bEn)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass  = bEn;
    emit sigInitRemoteSorter();
}


/*!
 * \brief 使能杂质统计
 * \param bSta
 */
void pageMachine::onEnPurityStatics(bool bSta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPurityStats = bSta;
}

/*!
 * \brief 触发方式改变
 * \param nIdx
 */
void pageMachine::onTrigerTypeChgSlt(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamTrigerType = nIdx;
    gFlow.sendcamtrigerType(struGsh.nAnalyzeIndex);
}

/*!
 * \brief 自动取样使能
 * \param bsta
 */
void pageMachine::onautoSampleChgSlt(bool bsta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample = bsta;
    emit sigautosampleEnsta((int)bsta,false);
}

/*!
 * \brief 是否使能物联网
 * \param bSta
 */
void pageMachine::onEnwebSlt(bool bSta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb = bSta;
    emit sigInitWeb(bSta);
}


/*!
 * \brief 参数备份
 */
void pageMachine::onParamsbckBtnPressedSlt()
{
    QStringList lst,lsttype;
    lst <<myLan.local <<myLan.Udisk <<myLan.cloud;
    lsttype << myLan.screen_params << myLan.sorteradj_params;

    setExportTypeWidget dlg(g_mainWidget);
    dlg.settitleText(myLan.params_backup);
    dlg.setcontentText(lst);
    dlg.setextraSelshow();
    dlg.setarrayBtntext(lsttype);
    dlg.setarrayBtnshow();
    int ret = dlg.exec();
    if(ret == QDialog::Rejected) {
        return;
    }

    int nRes = -1;
    int ntype = dlg.getSelIndex();
    int nparamstype = dlg.getArraytype();
    bool bFlag = true;

    if(ntype == 1){
        if(!gFlow.isUdiskExist()){
            nRes = -1;
            bFlag = false;
        }
    }else if(ntype ==2){
        if(struGsh.nNetwortStat != NETWORK_NORMAL){
            nRes = -3;
            bFlag = false;
        }
    }

    if(infoWidget->isHidden()){
        infoWidget->show();
    }
    infoWidget->raise();

    if(bFlag){
        infoWidget->setLabelText(QString("%1.....").arg(myLan.params_backup));

        //! 异步执行
        QFuture<bool> future = QtConcurrent::run(this, &pageMachine::backupParams, ntype, nparamstype);
        while(!future.isFinished()) {
            gFlow.msleep(500);
        }
        nRes = future.result();

        if(nRes){
            nRes = 1;
        }else{
            nRes =-2;
        }
    }

//    if(ntype == 0){
//        nRes = _backParamstolocal();
//    }else if(ntype == 1){
//        nRes = _backParamstoUdisk();
//    }else if(ntype == 2){
//        if(struGsh.nNetwortStat == NETWORK_NORMAL){
//            emit sig
//        }else{
//            nRes = -3;
//        }
//    }

    if(nRes == -2){
        infoWidget->setLabelText(QString("%1!").arg(myLan.params_backup_failed));
    }else if(nRes == -1){
        infoWidget->setLabelText(QString("%1").arg(myLan.udisk_or_file_not_found));
    }else if(nRes == -3){
        infoWidget->setLabelText(QString("%1").arg(myLan.network_abnormal));
    }else{
        infoWidget->setLabelText(QString("%1!").arg(myLan.params_backup_sucess));
    }
    gFlow.msleep(2000);
    infoWidget->hide();
}

/*!
 * \brief 参数恢复
 */
void pageMachine::onParamsRestoreBtnPressedSlt()
{
    QStringList lst,lsttype;
    lst <<myLan.local <<myLan.Udisk;
    lsttype << myLan.screen_params << myLan.sorteradj_params;

    setExportTypeWidget dlg(g_mainWidget);
    dlg.settitleText(myLan.params_restore);
    dlg.setcontentText(lst);
    dlg.setarrayBtntext(lsttype);
    dlg.setarrayBtnshow();
    int ret = dlg.exec();
    if(ret == QDialog::Rejected) {
        return;
    }

    if(infoWidget->isHidden()){
        infoWidget->show();
    }
    infoWidget->raise();
    infoWidget->setLabelText(QString("%1....").arg(myLan.params_restore));


    int ntype = dlg.getSelIndex();
    int nparamstype = dlg.getArraytype();

    int nRes = -1;
    if(ntype ==0){
       nRes = _restoreParamsBylocal(nparamstype);
    }else if(ntype ==1){
       nRes = _restoreParamsByUdisk(nparamstype);
    }

    if(nRes < 0){
        infoWidget->setLabelText(QString("%1!").arg(myLan.restore_params_failed));
    }else{
        infoWidget->setLabelText(QString("%1!").arg(myLan.restore_params_sucess));
    }
    gFlow.msleep(2000);
    infoWidget->hide();

    if(nRes >= 0){
      gFlow.SystemCmd("reboot");
    }
}

/*!
 * \brief 灯光跟随使能
 * \param bSta
 */
void pageMachine::onLightSyncChgSlt(bool bSta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnlightSync = bSta;
}

/*!
 * \brief 统计类型
 * \param bSta
 */
void pageMachine::onStaicTypeChgSlt(int bSta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaisticType = bSta;
}

void pageMachine::onAdjSorterTipsChgSlt(bool bSta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAdjSorterTips = bSta;
}

void pageMachine::onSimulateSwChanged(bool bEn)
{
    struGsh.bEnSimulate = bEn;
    emit sigSimulateEn(bEn);
}


/*!
 * \brief  刷新统计组设置
 * \param  bReset:是否需要重置统计组
 */
void pageMachine::refreshStatisGb(bool bReset)
{
    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_camIdxlb[i]->show();
            m_alonechk[i]->show();
            m_camIdxlb[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
            m_alonechk[i]->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i]);
        }else{
            m_camIdxlb[i]->hide();
            m_alonechk[i]->hide();
            if(bReset){
                m_alonechk[i]->setChecked(false);
                onModetypeChgSlt(i);
            }
        }
    }
}

/*!
 * \brief 刷新相机名称
 */
void pageMachine::refreshcamNameGb()
{
    for(int i=0 ;i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_camNamebtn[i]->show();
        }else{
            m_camNamebtn[i]->hide();
        }

    }
}
/*!
 * \brief 备份参数到本地
 */
int pageMachine::_backParamstolocal()
{
    const QString sDirUdisk = QString("/ssd/cnfbak");
    QDir dir("/ssd/cnf");
    if (!dir.exists()) {
        return -1;
    }

    int ret = gFlow.compressDir("/ssd/cnf", QString("%1.tar").arg(sDirUdisk));
    gFlow.msleep(1000);
    gFlow.SystemCmd("sync");
    return ret;
}

/*!
 * \brief 备份参数到U盘
 */
int pageMachine::_backParamstoUdisk()
{
    if(!gFlow.mountUDisk()){
        return -1;
    }

    QString datestr =  QDateTime::currentDateTime().toString("yyyyMMddhhmm");
    const QString sDirUdisk = QString("%1%2_cnfbak").arg(MOUNT_UDISK_PATH).arg(datestr);
    QDir dir("/ssd/cnf");
    if (!dir.exists()) {
        return -1;
    }

    int ret = gFlow.compressDir("/ssd/cnf", QString("%1.tar").arg(sDirUdisk));
    gFlow.msleep(1000);
    gFlow.SystemCmd("sync");

    gFlow.umountUDisk(MOUNT_UDISK_PATH);

    return ret;
}

/*!
 * \brief 从本地恢复参数
 */
int pageMachine::_restoreParamsBylocal(int nType)
{
    int nRes = -1;

    QString tmpdir, desdir;
    QString filename;
    if(nType == 0){//! 屏参数
        filename = "/ssd/cnf_bck/cnf_manual_Screen.zip";
        tmpdir = "/ssd/tmp";
        desdir = "/ssd/cnf";
    }else{//! 联调参数
        filename = "/userdata/cnf_bck/cnf_manual_Adj.zip";
        tmpdir = "/userdata/tmp";
        desdir = "/userdata/cnf";
    }

    QFile file(filename);
    if(!file.exists()){
        return nRes;
    }

    gFlow.uncompressDir(filename, tmpdir);

    nRes = gFlow.SystemCmd(QString("rm -rf %1").arg(desdir));
    gFlow.sleep(1);
    nRes = gFlow.SystemCmd(QString("mv %1 %2").arg(tmpdir).arg(desdir));
    nRes = gFlow.SystemCmd("sync");
    gFlow.sleep(1);

    return nRes;
}
/*!
 * \brief 从U盘恢复参数
 */
int pageMachine::_restoreParamsByUdisk(int nType)
{
    if(!gFlow.mountUDisk()){
        qDebug()<<"U mount failed";
        return -1;
    }else{
        QStringList strList;
        strList << "*.zip";

        QDir dir(MOUNT_UDISK_PATH);
        if(dir.entryList(strList).isEmpty()) {
            gFlow.umountUDisk(MOUNT_UDISK_PATH);
            return -2;
        }

        SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget);
        dialog->setDir(MOUNT_UDISK_PATH, strList);
        if (dialog->exec() == QDialog::Accepted) {
            QString sFileName = dialog->getFileName();
            if(nType ==0 && !sFileName.contains("Screen")){//! 屏参数
                return -4;
            }else if(nType ==1 && !sFileName.contains("Adj")){//! 联调参数
                return -4;
            }

            QString tmpdir,desdir;
            if(nType ==0){
                tmpdir ="/ssd/tmp";
                desdir ="/ssd/cnf";
            }else{
                tmpdir ="/userdata/tmp";
                desdir ="/userdata/cnf";
            }

            gFlow.uncompressDir(QString("%1%2").arg(MOUNT_UDISK_PATH).arg(sFileName), tmpdir);
            gFlow.SystemCmd(QString("rm -rf %1").arg(desdir));
            gFlow.sleep(1);
            gFlow.SystemCmd(QString("mv %1 %2").arg(tmpdir).arg(desdir));
            gFlow.SystemCmd("sync");
            gFlow.sleep(1);

            return 1;
        }else{
            gFlow.umountUDisk(MOUNT_UDISK_PATH);
            return -3;
        }
    }
}

void pageMachine::refreshlayout()
{
    QLayoutItem *item;
    while((item = m_glayMachine->takeAt(0)) != 0){
        m_glayMachine->removeItem(item);
    }


    m_glayMachine->addWidget(m_machinelb,0,0,1,1);  //! 机型
    m_glayMachine->addWidget(m_machicomb,0,1,1,1);

    m_glayMachine->addWidget(m_trigertypelb,0,2,1,1); //! 触发方式
    m_glayMachine->addWidget(m_trigertypecomb,0,3,1,1);


    if(struCnfg.nMachineType ==nMachine_Bottle){
        m_glayMachine->addWidget(m_staticTypelb,1,0,1,1); //! 统计类型
        m_glayMachine->addWidget(m_staticTypecob,1,1,1,1);

        m_glayMachine->addWidget(m_camNumlb,1,2,1,1); //! 主配相机
        m_glayMachine->addWidget(m_camNumle,1,3,1,1);

        m_glayMachine->addWidget(m_bEnAssistlb,2,0,1,1); //! 辅配相机
        m_glayMachine->addWidget(m_bEnAssistSw,2,1,1,1);

        m_glayMachine->addWidget(m_bEnadjbysorterlb,2,2,1,1); //! 色选联调
        m_glayMachine->addWidget(m_bEnadjbysortSw,2,3,1,1);

        m_glayMachine->addWidget(m_bEnAdjTipslb,3,0,1,1); //! 联调提示
        m_glayMachine->addWidget(m_bEnAdjTipsSw,3,1,1,1);

        m_glayMachine->addWidget(m_autosamplelb,3,2,1,1); //! 自动取样
        m_glayMachine->addWidget(m_autosampleSw,3,3,1,1);

        m_glayMachine->addWidget(m_weblb,4,0,1,1); //! 物联网
        m_glayMachine->addWidget(m_webEnSw,4,1,1,1);

        m_glayMachine->addWidget(m_lightsynclb,4,2,1,1); //! 灯光跟随
        m_glayMachine->addWidget(m_lightsyncsw,4,3,1,1);

        m_glayMachine->addWidget(m_bEnMutilablb,5,0,1,1); //! 多标签使能
        m_glayMachine->addWidget(m_bEnMultilabSw,5,1,1,1);

        m_glayMachine->addWidget(m_bEnPuritylb,5,2,1,1); //! 杂质统计
        m_glayMachine->addWidget(m_bEnPuritySw,5,3,1,1);

        m_glayMachine->addWidget(m_bSimulatelb,6,0,1,1); //! 仿真使能
        m_glayMachine->addWidget(m_simlateSw,6,1,1,1);
    }else{
        m_glayMachine->addWidget(m_camNumlb,1,0,1,1); //! 主配相机
        m_glayMachine->addWidget(m_camNumle,1,1,1,1);

        m_glayMachine->addWidget(m_bEnAssistlb,1,2,1,1); //! 辅配相机
        m_glayMachine->addWidget(m_bEnAssistSw,1,3,1,1);

        m_glayMachine->addWidget(m_bEnadjbysorterlb,2,0,1,1); //! 色选联调
        m_glayMachine->addWidget(m_bEnadjbysortSw,2,1,1,1);

        m_glayMachine->addWidget(m_bEnAdjTipslb,2,2,1,1); //! 联调提示
        m_glayMachine->addWidget(m_bEnAdjTipsSw,2,3,1,1);

        m_glayMachine->addWidget(m_autosamplelb,3,0,1,1); //! 自动取样
        m_glayMachine->addWidget(m_autosampleSw,3,1,1,1);

        m_glayMachine->addWidget(m_weblb,3,2,1,1); //! 物联网
        m_glayMachine->addWidget(m_webEnSw,3,3,1,1);

        m_glayMachine->addWidget(m_bEnMutilablb,4,0,1,1); //! 多标签使能
        m_glayMachine->addWidget(m_bEnMultilabSw,4,1,1,1);

        m_glayMachine->addWidget(m_bEnPuritylb,4,2,1,1); //! 杂质统计
        m_glayMachine->addWidget(m_bEnPuritySw,4,3,1,1);

        m_glayMachine->addWidget(m_bSimulatelb,5,0,1,1); //! 仿真使能
        m_glayMachine->addWidget(m_simlateSw,5,1,1,1);

        m_glayMachine->addWidget(m_staticTypelb,5,2,1,1); //! 统计类型
        m_glayMachine->addWidget(m_staticTypecob,5,3,1,1);

        m_glayMachine->addWidget(m_lightsynclb,6,0,1,1); //! 灯光跟随
        m_glayMachine->addWidget(m_lightsyncsw,6,1,1,1);
    }
}

/*!
 * \brief 设置机型的默认参数
 */
void pageMachine::setMachinedefaultParm()
{
    auto *pAnalyergb =struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;


    QList<int> wavelst;
    wavelst<<57<<61<<69<<73<<141<<205<<249<<329<<345<<373<<377<<381;

    if(struCnfg.nMachineType == nMachine_Bottle){
        //! 相机触发方式
        pAnalyergb->nCamTrigerType = 0;

        if(struCnfg.gDeviceNmae.compare("分析仪(KA1)") ==0){
            struCnfg.gDeviceNmae=QString("分析仪(KB1)");
        }

        //! 恒流源使能
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPulseLight =false;

        //! 相机统计方式 及 AI工作方式方式
        for(int i=0; i<MAX_CAM_NUM; i++){
            pProfile->nType[i] = 1;
            pAnalyergb->struCaParams->exposeValue[i] =18000;
            pAnalyergb->struCaParams->NoneexposeValue[i] =3000;
            for(int k=0; k<4; k++){
                pAnalyergb->struCaParams->infraBias[i][k] = 7;
            }

            for(int j=0; j<wavelst.size(); j++){
                pAnalyergb->struCaParams->nWavesSix[i][j] = wavelst.at(j);
            }
        }

        pProfile->strctAI.nModelLine = 3;
        pProfile->strctAI.nRes = 2;

        //! 相机个数
        bool bAssist = pAnalyergb->bHasAssist;

        if(bAssist && gFlow.getMachineCamNum() ==2){
            pAnalyergb->struCaParams->nWorkMode[1] = 0;
            pAnalyergb->struCaParams->sensorType[1] = 3;
            pAnalyergb->struCaParams->nFreQuence[1] = 24000;
            pAnalyergb->struCaParams->nPixelBegin[1] = 100;
            pAnalyergb->struCaParams->nPixelEnd[1] = 560;

            
            pProfile->nGainFast[1][COLOR_TYPE_R] =355;
            pProfile->nGainFast[1][COLOR_TYPE_G] =355;
            pProfile->nGainFast[1][COLOR_TYPE_B] =355;
            pProfile->nGainSlow[1][COLOR_TYPE_R] =3;
            pProfile->nGainSlow[1][COLOR_TYPE_G] =3;
            pProfile->nGainSlow[1][COLOR_TYPE_B] =3;

            pProfile->strctAI.nCapLine[1] = 512;
            pProfile->strctAI.nCashLine[1] = 0;

            pAnalyergb->CamName[1] = myLan.hyperspec_camer;


        }else if(bAssist && gFlow.getMachineCamNum() ==3){
            pAnalyergb->struCaParams->nWorkMode[1] = 3;
            pAnalyergb->struCaParams->sensorType[1] = 2;
            pAnalyergb->struCaParams->nFreQuence[1] = 11000;
            pAnalyergb->struCaParams->nPixelBegin[1] = 150;
            pAnalyergb->struCaParams->nPixelEnd[1] = 1700;

            
            pProfile->nGainFast[1][COLOR_TYPE_R] =0;
            pProfile->nGainFast[1][COLOR_TYPE_G] =5;
            pProfile->nGainFast[1][COLOR_TYPE_B] =3;
            pProfile->nGainSlow[1][COLOR_TYPE_R] =0;
            pProfile->nGainSlow[1][COLOR_TYPE_G] =55;
            pProfile->nGainSlow[1][COLOR_TYPE_B] =45;
            pProfile->strctAI.nCapLine[1] = 512;
            pProfile->strctAI.nCashLine[1] = 0;

            pAnalyergb->struCaParams->nWorkMode[2] = 0;
            pAnalyergb->struCaParams->sensorType[2] = 3;
            pAnalyergb->struCaParams->nFreQuence[2] = 24000;
            pAnalyergb->struCaParams->nPixelBegin[2] = 100;
            pAnalyergb->struCaParams->nPixelEnd[2] = 560;

            
            pProfile->nGainFast[2][COLOR_TYPE_R] =355;
            pProfile->nGainFast[2][COLOR_TYPE_G] =355;
            pProfile->nGainFast[2][COLOR_TYPE_B] =355;
            pProfile->nGainSlow[2][COLOR_TYPE_R] =3;
            pProfile->nGainSlow[2][COLOR_TYPE_G] =3;
            pProfile->nGainSlow[2][COLOR_TYPE_B] =3;


            pProfile->strctAI.nCapLine[2] = 512;
            pProfile->strctAI.nCashLine[2] = 0;

            pAnalyergb->CamName[1] = myLan.ultra_camer;
            pAnalyergb->CamName[2] = myLan.hyperspec_camer;
            
        }else if(bAssist && gFlow.getMachineCamNum() ==4){
            pAnalyergb->struCaParams->nWorkMode[1] = 3;
            pAnalyergb->struCaParams->sensorType[1] = 2;
            pAnalyergb->struCaParams->nFreQuence[1] = 11000;
             pAnalyergb->struCaParams->nPixelBegin[1] = 150;
            pAnalyergb->struCaParams->nPixelEnd[1] = 1700;

            
            pProfile->nGainFast[1][COLOR_TYPE_R] =0;
            pProfile->nGainFast[1][COLOR_TYPE_G] =5;
            pProfile->nGainFast[1][COLOR_TYPE_B] =3;
            pProfile->nGainSlow[1][COLOR_TYPE_R] =0;
            pProfile->nGainSlow[1][COLOR_TYPE_G] =55;
            pProfile->nGainSlow[1][COLOR_TYPE_B] =45;
            pProfile->strctAI.nCapLine[1] = 512;
            pProfile->strctAI.nCashLine[1] = 0;

            pAnalyergb->struCaParams->nWorkMode[2] = 1;
            pAnalyergb->struCaParams->sensorType[2] = 0;
            pAnalyergb->struCaParams->nFreQuence[2] = 11000;
             pAnalyergb->struCaParams->nPixelBegin[2] = 150;
            pAnalyergb->struCaParams->nPixelEnd[1] = 1934;

            
            pProfile->nGainFast[2][COLOR_TYPE_R] =1;
            pProfile->nGainFast[2][COLOR_TYPE_G] =1;
            pProfile->nGainFast[2][COLOR_TYPE_B] =2;
            pProfile->nGainSlow[2][COLOR_TYPE_R] =10;
            pProfile->nGainSlow[2][COLOR_TYPE_G] =20;
            pProfile->nGainSlow[2][COLOR_TYPE_B] =20;
            pProfile->strctAI.nCapLine[2] = 640;
            pProfile->strctAI.nCashLine[2] = 0;

            pAnalyergb->struCaParams->nWorkMode[3] = 0;
            pAnalyergb->struCaParams->sensorType[3] = 3;
            pAnalyergb->struCaParams->nFreQuence[3] = 24000;
            pAnalyergb->struCaParams->nPixelBegin[3] = 100;
            pAnalyergb->struCaParams->nPixelEnd[3] = 560;

            
            pProfile->nGainFast[3][COLOR_TYPE_R] =355;
            pProfile->nGainFast[3][COLOR_TYPE_G] =355;
            pProfile->nGainFast[3][COLOR_TYPE_B] =355;
            pProfile->nGainSlow[3][COLOR_TYPE_R] =3;
            pProfile->nGainSlow[3][COLOR_TYPE_G] =3;
            pProfile->nGainSlow[3][COLOR_TYPE_B] =3;
            pProfile->strctAI.nCapLine[3] = 512;
            pProfile->strctAI.nCashLine[3] = 0;

            pAnalyergb->CamName[1] = myLan.ultra_camer;
            pAnalyergb->CamName[2] = myLan.down_camer;
            pAnalyergb->CamName[3] = myLan.hyperspec_camer;
        }

        pAnalyergb->struCaParams->nWorkMode[0] =2;
        pAnalyergb->struCaParams->sensorType[0] = 0;
        pAnalyergb->struCaParams->nFreQuence[0] = 11000;
        pProfile->strctAI.nCapLine[0] = 640;
        pProfile->strctAI.nCashLine[0] = 0;
        pAnalyergb->CamName[0] = myLan.up_camer;

    }else{
        pAnalyergb->nCamTrigerType = 1;

        if(struCnfg.gDeviceNmae.compare("分析仪(KB1)") ==0){
            struCnfg.gDeviceNmae=QString("分析仪(KA1)");
        }

        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnPulseLight =true;

        //! 相机统计方式 及 AI工作方式方式  传感器类型
        for(int i=0; i<MAX_CAM_NUM; i++){
            pProfile->nType[i] = 0;
            pAnalyergb->struCaParams->nWorkMode[i] =0;
            pAnalyergb->struCaParams->sensorType[i] = 0;
            pAnalyergb->struCaParams->nFreQuence[i] = 11000;
            pProfile->strctAI.nCapLine[i] = 384;
            pProfile->strctAI.nCashLine[i] = 256;

            pProfile->nGainFast[i][COLOR_TYPE_R] =1;
            pProfile->nGainFast[i][COLOR_TYPE_G] =1;
            pProfile->nGainFast[i][COLOR_TYPE_B] =1;
            pProfile->nGainSlow[i][COLOR_TYPE_R] =0;
            pProfile->nGainSlow[i][COLOR_TYPE_G] =0;
            pProfile->nGainSlow[i][COLOR_TYPE_B] =0;

            pAnalyergb->CamName[i] = QString("%1%2").arg(myLan.camer).arg(i+1);
        }

        pAnalyergb->struCaParams->nPixelBegin[0] = 1025;
        pAnalyergb->struCaParams->nPixelEnd[0] = 1934;

        pAnalyergb->struCaParams->nPixelBegin[1] = 115;
        pAnalyergb->struCaParams->nPixelEnd[1] = 1024;

        pAnalyergb->struCaParams->nPixelBegin[2] = 115;
        pAnalyergb->struCaParams->nPixelEnd[2] = 1024;

        pAnalyergb->struCaParams->nPixelBegin[3] = 1025;
        pAnalyergb->struCaParams->nPixelEnd[3] = 1934;

        //! AI的相关参数
        pProfile->strctAI.nModelLine = 3;
        pProfile->strctAI.nRes = 1;
    }

    //! 辅配相机关联--》卤素灯  红外相机的控制
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnHalogen = true; //!卤素灯报警
        pProfile->bHologenLampOn = true;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnCamTempureAlarm = true;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEninfratempCtrl = true;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAutocalIMSensor = false;
    }else{
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnHalogen = false; //!卤素灯报警
        pProfile->bHologenLampOn = false;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnCamTempureAlarm = false;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEninfratempCtrl = false;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAutocalIMSensor = false;
    }


    gFlow.saveGlobalParams(CNF_GLOBAL);

    system("sync");
}

