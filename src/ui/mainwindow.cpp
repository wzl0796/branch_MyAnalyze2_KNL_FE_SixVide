#include "mainwindow.h"
#include <QApplication>
#include "ui/homePage/schmwidget.h"
#include "core/mythread/myprocess.h"
extern QWidget *g_splashwidget;
QWidget     *g_mainWidget;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    resize(WIN_WID, WIN_HEI);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    initParams();

    createMainPage();

    connectSigAndSlot();

    initCommunication();

    //! 检测加密是否正常
    checkEncryptOK();
}

MainWindow::~MainWindow()
{

}

/**
 * @brief MainWindow::初始化参数
 */
void MainWindow::initParams()
{
    m_bFeeder = false;
    m_bProcessStopping = false;  //! 初始化线程停止状态标志

    m_current = -1;

    menulMap = new QSignalMapper();

    createGlobalWidget();
}


/**
 * @brief MainWindow::主界面构造
 */
void MainWindow::createMainPage()
{
    //! 顶部菜单栏
    headerGb = new QGroupBox();
    headerGb->setFixedSize(WIN_WID-160, 90);
    createHeaderGBox();

    //! 底部状态栏
    statusGb = new QGroupBox();
    statusGb->setFixedSize(WIN_WID-160, 75);
    createStatusGBox();

    //! 左侧工具栏
    mainListGb = new QGroupBox("");
    mainListGb->setFixedSize(BTN_WID+10, WIN_HEI-16);
    createListWidget();

    m_stack = new QStackedWidget();
    m_stack->setFixedSize(WIN_WID-160, WIN_HEI-190);

    //! home页面
    mainPage = new homeWidget();
    m_stack->addWidget(mainPage);

    //! 历史数据
    statisticsPage = new StatisticsWidget(0,0);
    m_stack->addWidget(statisticsPage);

    //! 色选联调页面
    sorterPage = new sortercompage();
    m_stack->addWidget(sorterPage);


    identifityPage = new Identifity();
    m_stack->addWidget(identifityPage);

    //! 厂家设置
    facSetPage = new factoryPage();
    m_stack->addWidget(facSetPage);

    systemPage = new SystemPage();
    m_stack->addWidget(systemPage);

    QHBoxLayout *mainLay = new QHBoxLayout(this);
    QVBoxLayout *rightLay = new QVBoxLayout;

    rightLay->addWidget(headerGb);
    rightLay->addWidget(m_stack);
    rightLay->addWidget(statusGb);
    mainLay->addWidget(mainListGb);
    mainLay->addLayout(rightLay);
}

/**
 * @brief MainWindow::顶部菜单栏
 */
void MainWindow::createHeaderGBox()
{
    m_sysHaltBtn = new QToolButton;
    m_sysHaltBtn->setText(myLan.poweroff);
    m_sysHaltBtn->setIcon(QIcon(":/res/png/System_Standby.png"));
    m_sysHaltBtn->setIconSize(QSize(BTN_HEI-25, BTN_HEI-25));
    m_sysHaltBtn->setFixedWidth(BTN_HEI+15);
    m_sysHaltBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_UserBtn = new QToolButton;
    m_UserBtn->setText(myLan.user);
    m_UserBtn->setIcon(QIcon(":/res/png/permission_edit.png"));
    m_UserBtn->setIconSize(QSize(BTN_HEI-25, BTN_HEI-25));
    m_UserBtn->setFixedWidth(BTN_HEI+15);
    m_UserBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_passwordDlg = new PasswordDialog(this);
    m_passwordDlg->hide();

    m_saveBtn = new QToolButton;
    m_saveBtn->setText(myLan.save);
    m_saveBtn->setIcon(QIcon(":/res/png/action_save.png"));
    m_saveBtn->setIconSize(QSize(BTN_HEI-25, BTN_HEI-25));
    m_saveBtn->setFixedWidth(BTN_HEI+15);
    m_saveBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_feederBtn = new QToolButton();
    m_feederBtn->setText(myLan.detect);
    m_feederBtn->setIcon(QIcon(":/res/png/Media_Start.png"));
    m_feederBtn->setFixedSize(BTN_WID+50, BTN_HEI);
    m_feederBtn->setIconSize(QSize(50, 50));
    m_feederBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_feederBtn->setStyleSheet(QString("QToolButton{background-color: %1;border:0px;font: 24px;}")
                                   .arg(g_ColorLightGray3));

    m_autosamplelb = new QLabel("");  //! 自动取样
    m_autosamplelb->setMaximumWidth(250);

    m_schemLbl = new QLabel();
    m_schemLbl->setText(QString("%1  ").arg(myLan.schme));
    m_schemLbl->setStyleSheet("QLabel{width:80;alignment:center;font-size:24px;}");
    m_schemBox = new QComboBox();
    QListWidget *listSchmWidget = new QListWidget();
    m_schemBox->setModel(listSchmWidget->model());
    m_schemBox->setView(listSchmWidget);
    m_schemBox->setStyleSheet("QComboBox{width:300;height:60;font-size:20px;}"
                              "QComboBox QAbstractItemView::item{height:60px;}");
    m_editBtn = new QPushButton();
    m_editBtn->setDisabled(true);
    m_editBtn->setStyleSheet("QPushButton{width:60;height:60;border:0px;margin:0;}"
                              "QPushButton:pressed{margin:2;}"
                             );
    m_editBtn->setIcon(QIcon(":/res/png/action_disable_edit.png"));
    m_editBtn->setIconSize(QSize(60,60));

    refreshSchemBox();

    QHBoxLayout *scemeLayout = new QHBoxLayout();
    scemeLayout->addStretch();
    scemeLayout->addWidget(m_schemLbl);
    scemeLayout->addWidget(m_schemBox);
    scemeLayout->addWidget(m_editBtn);
    scemeLayout->addStretch();

    QHBoxLayout *upLay = new QHBoxLayout(headerGb);
    upLay->setSpacing(5);
    upLay->setContentsMargins(5,0,5,0);
    upLay->addLayout(scemeLayout);
    upLay->addStretch(1);
    upLay->addWidget(m_feederBtn);
    upLay->addStretch(1);
    upLay->addWidget(m_autosamplelb);
    upLay->addStretch(1);
    upLay->addWidget(m_UserBtn);
    upLay->addWidget(m_saveBtn);
    upLay->addWidget(m_sysHaltBtn);
}

/**
 * @brief MainWindow::底部菜单栏
 */
void MainWindow::createStatusGBox()
{
    m_factoryName = new QPushButton;
    m_factoryName->setText(struCnfg.gCompanyName);
    m_factoryName->setStyleSheet(QString("background-color:#FFFFFF;color:%1;border-width:0px;font: 26px;").arg(g_ColorMYGreen));
    m_factoryName->setFixedHeight(BTN_HEI-40);
    m_factoryName->hide();

    m_blowtimeBtn = new QToolButton;
    m_blowtimeBtn->setText(myLan.tick);
    m_blowtimeBtn->setIcon(QIcon(":/res/png/elecvalve.png"));
    m_blowtimeBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_blowtimeBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_blowtimeBtn->hide();

    m_beltBtn = new QToolButton;
    m_beltBtn->setText(QString("%1-%2").arg(myLan.belt).arg(myLan.open));
    m_beltBtn->setIcon(QIcon(":/res/png/Device_Belt.png"));
    m_beltBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_beltBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_wipeBtn = new QToolButton;
    m_wipeBtn->setText(myLan.clean);
    m_wipeBtn->setIcon(QIcon(":/res/png/action_clean.png"));
    m_wipeBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_wipeBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_setFeederBtn = new QToolButton;
    m_setFeederBtn->setText(myLan.feeder);
    m_setFeederBtn->setIcon(QIcon(":/res/png/action_feed.png"));
    m_setFeederBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_setFeederBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_calibrateBtn = new QToolButton;
    m_calibrateBtn->setText(myLan.sens);
    m_calibrateBtn->setIcon(QIcon(":/res/png/action_calibrate.png"));
    m_calibrateBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_calibrateBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    //! 校正
    m_calWaveBtn = new QToolButton;
    m_calWaveBtn->setText(myLan.auto_adj);
    m_calWaveBtn->setIcon(QIcon(":/res/png/Sorter_Light.png"));
    m_calWaveBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_calWaveBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_labelRatioBtn = new QToolButton;
    m_labelRatioBtn->setText(myLan.calibrate);
    m_labelRatioBtn->setIcon(QIcon(":/res/png/action_calibrate.png"));
    m_labelRatioBtn->setIconSize(QSize(BTN_HEI-40, BTN_HEI-40));
    m_labelRatioBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QHBoxLayout *statusLay = new QHBoxLayout(statusGb);
    statusLay->setMargin(0);
    statusLay->addWidget(m_factoryName);
    statusLay->addStretch(3);
    statusLay->addWidget(m_calWaveBtn);
    statusLay->addWidget(m_blowtimeBtn);
    statusLay->addWidget(m_beltBtn);
    statusLay->addWidget(m_wipeBtn);
    statusLay->addWidget(m_setFeederBtn);
    statusLay->addWidget(m_calibrateBtn);
    statusLay->addWidget(m_labelRatioBtn);

}

/**
 * @brief MainWindow::左侧菜单栏
 */
void MainWindow::createListWidget()
{
    QStringList strlist,iconList;
    strlist << myLan.home << myLan.history <<myLan.sorter_debug
            << myLan.Identifity << myLan.factor << myLan.System;
    iconList << ":/res/png/tab_home_normal.png" << ":res/png/tab_static_normal.png"
             <<":/res/png/Sorter.png"<<":/res/png/Sorter_AI.png"
             << ":/res/png/tab_setting_normal.png"<< ":/res/png/tab_status_normal.png";


    m_dateLbl = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    m_dateLbl->setStyleSheet(QString("font:21px;color: black;"));
    m_dateLbl->setFixedSize(BTN_WID-15, 22);

    m_timeLbl = new QLabel(QDateTime::currentDateTime().toString("hh:mm:ss"));
    m_timeLbl->setStyleSheet(QString("font:32px;color: black;"));
    m_timeLbl->setFixedSize(BTN_WID, 32);

    QVBoxLayout *dateLay = new QVBoxLayout;
    dateLay->setSpacing(3);
    dateLay->addWidget(m_timeLbl, 0, Qt::AlignCenter);
    dateLay->addWidget(m_dateLbl, 0, Qt::AlignCenter);

    typeGb = new QGroupBox("");
    typeGb->setFixedSize(BTN_WID+10, WIN_HEI*0.9);
    typeGb->setStyleSheet("border-width:0px;");
    typeGb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout *btnLay = new QVBoxLayout(typeGb);
    for (int i = 0; i < PAGE_ALL; i++) {
        menuBtn[i] = new QToolButton();
        menuBtn[i]->setText(strlist.at(i));
        menuBtn[i]->setIconSize(QSize(BTN_WID-5, BTN_HEI-15));
        menuBtn[i]->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        menuBtn[i]->setIcon(QIcon(iconList.at(i)));
        menuBtn[i]->setStyleSheet(QString("font:30px;color:%1;").arg(g_ColorGray));
        btnLay->addWidget(menuBtn[i]);
        menulMap->setMapping(menuBtn[i], i);
        connect(menuBtn[i], SIGNAL(released()), menulMap, SLOT(map()));
    }

    if(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        menuBtn[PAGE_SORTERCONTROL]->hide();
    }


    if (struGsh.nAuthority == AUTHORITY_PRIMARY) {
        menuBtn[PAGE_IDENTYFITY]->setEnabled(false);
        menuBtn[PAGE_SET]->setEnabled(false);
    } else {
        menuBtn[PAGE_IDENTYFITY]->setEnabled(true);
        menuBtn[PAGE_SET]->setEnabled(true);
    }

    QVBoxLayout *listLay = new QVBoxLayout(mainListGb);
    listLay->setContentsMargins(0, 20, 0, 20);
    listLay->addLayout(dateLay);
    listLay->addWidget(typeGb);
}

/**
 * @brief MainWindow::createGlobalWidget
 */
void MainWindow::createGlobalWidget()
{
    g_mainWidget = this;

    g_BootPage = new pageBoot(this);
    g_BootPage->setWindowFlags((g_BootPage->windowFlags() & ~Qt::WindowType_Mask) | Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

#ifdef Q_OS_UNIX
    g_BootPage->raise();
    g_BootPage->show();
    g_BootPage->setBootloader();
#endif

    infoWidget =new myInfoWidget(this);
    infoWidget->hide();
}

/**
 * @brief MainWindow::信号槽绑定
 */
void MainWindow::connectSigAndSlot()
{
    connect(menulMap,       SIGNAL(mapped(int)),                   this,  SLOT(onMenuBtnPressedSlt(int)));

    connect(m_sysHaltBtn,   SIGNAL(pressed()),                     this,  SLOT(onHaltBtnPressedSlt()));
    connect(m_saveBtn,      SIGNAL(pressed()),                     this,  SLOT(onSaveBtnPressedSlt()));
    connect(m_UserBtn,      SIGNAL(pressed()),                     this,  SLOT(onUserBtnPressedSlt()));
    connect(m_feederBtn,    SIGNAL(pressed()),                     this,  SLOT(onFeederBtnClickedSlt()));
    connect(m_editBtn,      SIGNAL(pressed()),                     this,  SLOT(onSchemEditBtnClicked()));
    connect(m_schemBox,     SIGNAL(currentIndexChanged(int)),      this,  SLOT(onSchmBoxChangedSlt(int)));
    connect(m_setFeederBtn, SIGNAL(pressed()),                     this,  SLOT(onFeederSetBtnSlt()));
    connect(m_blowtimeBtn,  SIGNAL(pressed()),                      this, SLOT(onblowTimeSetBtnSlt()));
    connect(m_calibrateBtn, SIGNAL(clicked(bool)),                 this,  SLOT(onCaliParaBtnClicked()));
    connect(m_wipeBtn,      SIGNAL(pressed()),                     this,  SLOT(onWipeSetBtnSlt()));
    connect(m_beltBtn,      SIGNAL(pressed()),                     this,  SLOT(onBeltBtnSlt()));
    connect(m_calWaveBtn,   SIGNAL(pressed()),                     this, SLOT(onCalWaveBtnPressedSlt()));
    connect(m_labelRatioBtn, SIGNAL(pressed()),                    this, SLOT(onLabelRatioBtnPressedSlt()));

    connect(this,           SIGNAL(sigChangeScheme()),             mainPage,    SLOT(onChangeScheme()));
    connect(this,           SIGNAL(sigChangeScheme()),             facSetPage, SIGNAL(sigChangeScheme()));
    connect(this,           SIGNAL(sigIM01Tempture(int)),          facSetPage, SIGNAL(sigIM01SenTempture(int)));
    connect(this,           SIGNAL(sigPermissionChg()),            systemPage, SLOT(onPermissionChg()));

    connect(facSetPage, SIGNAL(sigrefreshBeltSta()), this,     SLOT(onrefreshBeltSta()));
    connect(facSetPage, SIGNAL(sigautosampleEnstatus(int,bool)), this, SLOT(onrefreshautosamplesta(int,bool)));
    connect(facSetPage, SIGNAL(sigMachineTypeChg(bool)), identifityPage, SLOT(onMachineTypeChgSlt(bool)));
    connect(facSetPage, SIGNAL(sigSimulateEnChg(bool)), mainPage,  SLOT(onSimulateEnchg(bool)));
    connect(facSetPage, &factoryPage::sigEnfeeder, this, [this](bool bEn){
            m_feederBtn->setEnabled(bEn);
    });
    //! udp接口
    connect(g_udpSocket[struGsh.nAnalyzeIndex],SIGNAL(sigSocinferFinished(std::vector<BBox>,int,uint)),
            mainPage,SLOT(onrefreshHomepaheBySocInfer(std::vector<BBox>,int,uint)),Qt::DirectConnection);
    connect(g_udpSocket[struGsh.nAnalyzeIndex],SIGNAL(sigSocinferBottleFinished(QByteArray,int,uint)),
            mainPage,SLOT(onrefreshpagebySocBottleInfer(QByteArray,int,uint)));
    connect(g_udpSocket[struGsh.nAnalyzeIndex], SIGNAL(sigIM01Tempure(int)), this, SIGNAL(sigIM01Tempture(int)));

    //! 物联网信号槽
    connect(this,        SIGNAL(sigInitWebinfo()),          g_Lotsocket, SLOT(onInitLocalSocket()));
    connect(g_Lotsocket, SIGNAL(sigsetWindow(int)),         systemPage,    SIGNAL(sigUpdateIotWid(int)));
    connect(g_Lotsocket, SIGNAL(sigrefreshVPNStatus(bool)), systemPage,    SIGNAL(sigUpdateVpnSta(bool)));
    connect(facSetPage,  SIGNAL(sigInitwebSocket(bool)),    this,        SLOT(onInitWebSlt(bool)));
    connect(g_Lotsocket, SIGNAL(sigScanCodeRet(bool)),      this,        SLOT(onHandleScanCodeRes(bool)));
    connect(facSetPage,  SIGNAL(sigUploadParamsbyCloud(QString,int)),   g_Lotsocket, SLOT(uploadParams(QString,int)));

    //! 色选联调
    connect(g_Process[struGsh.nAnalyzeIndex], SIGNAL(sigUploadWeb()), g_Lotsocket, SLOT(onuploadstaticRes()));
    connect(g_Process[struGsh.nAnalyzeIndex], SIGNAL(sigautosamplesta(int,bool)), this, SLOT(onrefreshautosamplesta(int,bool)));
    connect(g_Process[struGsh.nAnalyzeIndex], SIGNAL(sigautosamplesta(int,bool)), mainPage, SLOT(onResetStaticsResult(int,bool)));
    connect(g_localSocket, SIGNAL(siglocalsocketfeedback(int)), sorterPage, SLOT(onupdateLocalsocketstatus(int)));
    connect(&gFlow, SIGNAL(sigNotifyAdjTips(QStringList)),this, SLOT(onRefreshAdjTips(QStringList)));

    //!
    connect(this,        SIGNAL(sigInitRemoteSorter()),    g_localSocket, SLOT(onInitLocalSocket()));
    connect(facSetPage,  SIGNAL(sigInitRemoteSorter()),    g_localSocket, SLOT(onInitLocalSocket()));
    connect(facSetPage,  SIGNAL(sigInitRemoteSorter()),    this, SLOT(onRefreshSorterpage()));


    connect(this,        SIGNAL(sigInitEncryTimer()),      &g_delagCode, SLOT(onStartTimer()));

    //! 刷新系统状态
    connect(g_netThread,          SIGNAL(sigUpdateSysSta(int)),     this, SLOT(onRefreshSystemSta(int)));
}

/**
 * @brief MainWindow::初始化通信
 */
void MainWindow::initCommunication()
{
#ifdef Q_OS_LINUX
    gFlow.sleep(40); //! 增加等待时间，等soc版启动

// if (g_splashwidget) {
//     g_splashwidget->hide();
//     delete g_splashwidget;
//     g_splashwidget = nullptr;
// }

    g_BootPage->refreshPage();

    
    gFlow.sendAll(true);
    g_serial->InitMotor();


    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
        emit sigInitWebinfo();
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
       emit sigInitRemoteSorter();
    }

     emit sigInitEncryTimer();
#endif

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOutSlt()));
    timer->start(1000);

#ifdef Q_OS_LINUX
    g_netThread->start();  //! 这个地方有线程冲突
#endif
    onMenuBtnPressedSlt(PAGE_HOME);
}

/**
 * @brief MainWindow::定时器响应
 */
void MainWindow::timeOutSlt()
{
    m_dateLbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    m_timeLbl->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));

     struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRuntime +=1;
     if(struGsh.nFeederState){
         struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nFeedertime +=1;
         struGsh.nfeederTime +=1;
         struGsh.nRuningTime +=1;
     }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
        QJsonObject staicsjson;
        staicsjson["telemetry"] = true;
        g_Lotsocket->uploadInfo(staicsjson);
    }
}

/**
 * @brief MainWindow::开始检测
 */
void MainWindow::onFeederBtnClickedSlt()
{
    //! 检查是否正在异步停止线程进程
    if (m_bProcessStopping) {
        qWarning() << "Process is still stopping, ignoring button click";
        
        if(infoWidget->isHidden()) {
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1.....").arg(myLan.auto_sample_abnormal));
            infoWidget->showSecs();
        }
        return;
    }

    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        if (gFlow.getSpecifyAnalyzeShare(i)->bWipe) {
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1.....").arg(myLan.clean));
            infoWidget->showSecs();
            return;
        }

        if (struGsh.bCapimg && !m_bFeeder) {
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1").arg(myLan.warn_capture_Mode));
            infoWidget->showSecs(3);
        }
    }

    if(!m_bFeeder) {
        myMessageBox msgBox(MSG_QUES, myLan.input_batchname, myLan.start_detect, g_mainWidget);
        msgBox.setBatchBtntext(struGsh.batchname);
        msgBox.setBatchBtnshow(false);
        msgBox.setShiftBtntext(struGsh.duty_shift);
        msgBox.setShiftBtnshow(false);
        msgBox.settextNotCenter();
        msgBox.setYieldInfovisible(true);
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass&&
            (!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)){
            QStringList lst;
            for(int i=0; i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nmaxNum; i++){
                lst<<QString::fromLocal8Bit(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.sortclassname[i]);
            }
            msgBox.setadjcontetshow(false,lst);
        }

        int ret = msgBox.exec();
        if(ret == QDialog::Rejected){
            return;
        }
        struGsh.batchname = msgBox.getBathBtntext();
        struGsh.duty_shift = msgBox.getshiftBtntext();
        struGsh.detect_yield = msgBox.getyield();
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass&&
            (!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)){
            struGsh.nSorterAdjIdx = msgBox.getsorterschmeidx() < 0 ? 0 : msgBox.getsorterschmeidx();

            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nsourceIdx[struGsh.nSorterAdjIdx] = msgBox.getsortersourceidx()<0 ? 0:msgBox.getsortersourceidx();
        }
    }

    m_bFeeder = !m_bFeeder;
    if(m_bFeeder) {
        m_feederBtn->setText(myLan.stop);
        m_feederBtn->setIcon(QIcon(":/res/png/Media_Stop.png"));
        menuBtn[PAGE_SET]->setEnabled(false);
        menuBtn[PAGE_IDENTYFITY]->setEnabled(false);
    } else {
        m_feederBtn->setText(myLan.detect);
        m_feederBtn->setIcon(QIcon(":/res/png/Media_Start.png"));
        if(struGsh.nAuthority != AUTHORITY_PRIMARY){
            menuBtn[PAGE_SET]->setEnabled(true);
            menuBtn[PAGE_IDENTYFITY]->setEnabled(true);
        }
      
    }

    struGsh.nFeederState = m_bFeeder;

    if((!struGsh.bCapimg) && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        gFlow.sendFeederstatoSorter();
    }

    //! 一键所有分析仪开始检测时，之前在检测的分析仪不重新下发参数
    for(int i=0; i< struCnfg.analyzeNum; i++){
        if(m_bFeeder) {
            if(infoWidget->isHidden()){
                infoWidget->show();
            }
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1......").arg(myLan.start_detect));

            g_udpSocket[struGsh.nAnalyzeIndex]->clearudpbuff(); //! 清空统计参数
            gFlow.resetStats();
            gFlow.sendAll(false); //! 下发所有参数
            mainPage->startorstoptimer(true); //! 启动刷新统计计时器

            if(!gFlow.getSpecifyAnalyzeShare(i)->bFeeder){
                auto st = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]; //! 添加清灰操作
                if ((m_nDetectTimes%st->nWipeInterval == 0)&&
                        (!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)){
                    gFlow.sendWipePara(1, struGsh.nAnalyzeIndex);
                    gFlow.msleep(50);
                    quint8 data = 0;
                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel != 0){
                        data = (1<<(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel-1));
                    }
                    gFlow.snvchannelonoffctrl(data);
                    gFlow.msleep(st->nWipeDelay);
                    gFlow.snvchannelonoffctrl(0);
                    m_nDetectTimes = 0;
                }
                m_nDetectTimes++;

                //! 自动校正
                if(struCnfg.nMachineType == nMachine_Bottle &&
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAutocalIMSensor){
                    calIMserialsensors();
                }

                //! 开卤素灯及紫外
                if(struCnfg.nMachineType == nMachine_Bottle &&
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnlightSync){
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bHologenLampOn = true;
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnlamp[LAMP_BACK_DOWN] = true;
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnlamp[LAMP_MAT_DOWN] = true;
                    gFlow.sendLampPara(struGsh.nAnalyzeIndex);
                }

                if(st->bEnBackdgrod){//! 背景棍控制
                    gFlow.sendbackgdrodOnoffcmd(true);
                }

                gFlow.getSpecifyAnalyzeShare(i)->bFeeder = m_bFeeder; //! 开关供料命令下发
                gFlow.sendSocOnOff(i);
                gFlow.msleep(3000);
                gFlow.sendOnOff(i);

                g_serial->SwMotorOnOff(true);  //! 先开伺服
                gFlow.msleep(1000);
                if(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample){
                    gFlow.sendFeederPara(i);
                }

                struGsh.bBeltOnff = true;
                onrefreshBeltSta();

                gFlow.msleep(1000);
                if((!struGsh.bCapimg)&&struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample){//! 自动取样
                    g_Process[i]->startTimerSlt();

                    int nchanel = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nChannel;//! 长吸电磁阀
                    if(nchanel !=0){
                        quint8 data = 0;
                        data = data | (1<<(nchanel-1));
                        gFlow.snvchannelonoffctrl(data);
                        gFlow.msleep(500);
                    }

                    g_Process[i]->start();
                }

                if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){ //! 发送物联网
                    QJsonObject staicsjson;
                    staicsjson["telemetry"] = true;
                    g_Lotsocket->uploadInfo(staicsjson);
                }

                infoWidget->hide();
            }
        } else {
            mainPage->startorstoptimer(false); //! 停止刷新首页统计


            if(infoWidget->isHidden()){
                infoWidget->show();
            }
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1.....").arg(myLan.stop));

            if((!struGsh.bCapimg)&&struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample){//! 自动取样停止
                g_Process[i]->stopTimerSlt();

                //! 设置线程停止状态标志，防止用户重复点击
                m_bProcessStopping = true;
                
                //! 使用异步方式等待线程退出，避免阻塞主线程
                QTimer *waitTimer = new QTimer();
                int processIndex = i;
                
                //! 使用Timer的property来存储计数器，避免static变量问题
                waitTimer->setProperty("timeoutCounter", 0);
                
                connect(waitTimer, &QTimer::timeout, [this, waitTimer, processIndex]() {
                    int timeoutCounter = waitTimer->property("timeoutCounter").toInt();
                    
                    if(!g_Process[processIndex]->isRunning() || timeoutCounter > 6) { // 10秒超时
                        waitTimer->stop();
                        waitTimer->deleteLater();
                        
                        //! 清除线程停止状态标志，允许下次操作
                        m_bProcessStopping = false;
                        
                        //! 线程正常退出或超时后执行原有逻辑
                        gFlow.snvchannelonoffctrl(0);
                        gFlow.sendFeederPara(0,false,false,false);  //!先关振动器

                        
                        if(timeoutCounter > 6) {
                            //! 超时强制终止线程
                            qWarning() << "Process thread timeout, forcing termination for index:" << processIndex;
                            g_Process[processIndex]->terminate();
                            g_Process[processIndex]->wait(3000);
                        }
                        return;
                    }
                    
                    // 更新计数器
                    waitTimer->setProperty("timeoutCounter", timeoutCounter + 1);
                });
                
                waitTimer->start(1000); // 每秒检查一次
            }


            gFlow.getSpecifyAnalyzeShare(i)->bFeeder = m_bFeeder;

            gFlow.sendFeederPara(i);  //!先关振动器

            gFlow.sendOnOff(i);
            gFlow.sendSocOnOff(i);


//            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass&&
//                (!struGsh.bCapimg)&&(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)){//! 统计结果下发色选机
//                gFlow.sendStaticsrestoSorter();
//            }

            //! 统计数据记录数据库
            if((!struGsh.bCapimg)&&(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)){
                gFlow.recordStatusToDB();
            }

            gFlow.msleep(2000);
            g_serial->SwMotorOnOff(false);  //!关伺服

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnBackdgrod){
                gFlow.msleep(3000);
                gFlow.sendbackgdrodOnoffcmd(false);
            }

            struGsh.bBeltOnff = false;
            onrefreshBeltSta();

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample){
                onrefreshautosamplesta(0, false);
            }

            if(struCnfg.nMachineType == nMachine_Bottle &&
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnlightSync){
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bHologenLampOn = false;
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnlamp[LAMP_BACK_DOWN] = false;
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnlamp[LAMP_MAT_DOWN] = false;
                gFlow.sendLampPara(struGsh.nAnalyzeIndex);
            }

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){ //! 发送物联网
                QJsonObject staicsjson;
                staicsjson["telemetry"] = true;
                g_Lotsocket->uploadInfo(staicsjson);
            }
     
            gFlow.msleep(1000);

            infoWidget->hide();

            //! 统计结果下发色选机
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass&&
                (!struGsh.bCapimg)&&(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)){
                gFlow.sendStaticsrestoSorter();
            }
        }
    }

//    emit sigUploadRunningState();    //! 临时注释 by wzl
//    mqttFlow->uploadRunningState();
//    if(m_bFeeder){
//       mainPage->refreshDisPlay(true);
//    }else{
//       mainPage->refreshDisPlay(false);
//    }

//    displayPage->refreshDisplay(); //! 临时注释 by wzl

    mainPage->refreshDisPlay(false);
    if(m_stack->currentIndex() == PAGE_IDENTYFITY){
        identifityPage->refreshPage();
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb && (!m_bFeeder)){ //! 发送物联网
        QJsonObject staicsjson;
        staicsjson["Statics"] = true;
        g_Lotsocket->uploadInfo(staicsjson);
    }

}

/**
 * @brief MainWindow::菜单按钮
 * @param index
 */
void MainWindow::onMenuBtnPressedSlt(int index)
{
    if(struGsh.bUpgrading){
        return;
    }

    if(struCnfg.nMachineType == nMachine_Bottle){
        m_calWaveBtn->show();
        m_labelRatioBtn->hide();
    }else{
        m_calWaveBtn->hide();
        m_labelRatioBtn->show();
    }


    if (index == m_current) return;

    if(index>PAGE_SORTERCONTROL && index != PAGE_SYSTEM){
        if(struGsh.nAuthority == AUTHORITY_PRIMARY){
            infoWidget->raise();
            infoWidget->setLabelText(myLan.switch_to_advance);
            infoWidget->showSecs();
            return;
        }
    }

    if(index == PAGE_HOME){
        m_schemBox->setDisabled(false);
        if(struGsh.nAuthority != AUTHORITY_PRIMARY){
            m_editBtn->setDisabled(false);
            m_editBtn->setIcon(QIcon(":/res/png/action_edit.png"));
        }else{
            m_editBtn->setDisabled(true);
            m_editBtn->setIcon(QIcon(":/res/png/action_disable_edit.png"));
        }
    }else{
        m_schemBox->setDisabled(true);
        m_editBtn->setDisabled(true);
        m_editBtn->setIcon(QIcon(":/res/png/action_disable_edit.png"));
    }


    m_current = index;
    m_stack->setCurrentIndex(m_current);

    for (int i = 0; i < PAGE_ALL; i++) {
        if(i == PAGE_SYSTEM && m_nSystemAlarmSta != 0){
            //! 如果系统页面有报警，保持红色背景
            menuBtn[i]->setStyleSheet(QString("font:30px;color:%1;background-color:red;").arg(g_ColorGray));
        }else{
            menuBtn[i]->setStyleSheet(QString("font:30px;color:%1;").arg(g_ColorGray));
        }
    }


    //! 设置当前选中按钮的样式
    if(m_current == PAGE_SYSTEM && m_nSystemAlarmSta != 0){
        //! 如果切换到系统页面且有报警，保持红色背景
        menuBtn[m_current]->setStyleSheet(QString("font:30px;color:%1;background-color:red;").arg(g_ColorMYGreen));
    }else{
        menuBtn[m_current]->setStyleSheet(QString("font:30px;color:%1;background-color:%2;").arg(g_ColorMYGreen).arg(g_ColorLightGreen));
    }
    g_curveDataThread->stopWave();

    //! 将信号切换到校正信号
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        for(int j = 0; j < MAX_CAM_NUM; j++)
            gFlow.sendCamSignalMode(i, j, 1);
    }

    m_feederBtn->setEnabled(true);
    m_calWaveBtn->setEnabled(true);

    if(index !=PAGE_SYSTEM){
        systemPage->stopRefreshSysta();
    }

    switch (m_current) {
    case PAGE_HOME:
        mainPage->refreshDisPlay(false);
        break;
    case PAGE_HISTORY:
        statisticsPage->refreshPage();
        break;
    case PAGE_SORTERCONTROL:
        sorterPage->refreshpage();
        break;
    case PAGE_IDENTYFITY:
        identifityPage->refreshPage();
        break;
    case PAGE_SET:
        m_feederBtn->setEnabled(false);
        m_calWaveBtn->setEnabled(false);
        facSetPage->refreshDisplay();
        break;
    case PAGE_SYSTEM:
        systemPage->refreshPage();
        break;
    default:
        break;
    }
}

/**
 * @brief MainWindow::权限切换
 */
void MainWindow::onUserBtnPressedSlt()
{
    QString primary = struGsh.nAuthority == AUTHORITY_PRIMARY ?
                          myLan.switch_to_factorper : myLan.switch_to_userPer;

    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(primary), myLan.notice, this);

    if (msgBox.exec() == QDialog::Accepted) {
        if (struGsh.nAuthority == AUTHORITY_ENGINEER){
            struGsh.nAuthority = AUTHORITY_PRIMARY;
            if(m_stack->currentIndex() == PAGE_IDENTYFITY ||
                m_stack->currentIndex() == PAGE_SET ){
                onMenuBtnPressedSlt(PAGE_HOME);
            }
        } else {
            m_passwordDlg->setQRCodeData(struCnfg.master_device_code);
            m_passwordDlg->clearPassWord();
            m_passwordDlg->raise();
            m_passwordDlg->show();
            int ret = m_passwordDlg->exec();
            if (ret == QDialog::Accepted){
                QString pwd = gFlow.getAdminPwd();
                QString facpwd = gFlow.getCheckCode(2);
                QString higpwd = gFlow.getCheckCode(1);
                if(m_passwordDlg->getPassword().compare(pwd) != 0 &&
                   m_passwordDlg->getPassword().compare(facpwd) != 0 &&
                   m_passwordDlg->getPassword().compare(higpwd) != 0) {
                    infoWidget->raise();
                    infoWidget->setLabelText(QString("%1!").arg(myLan.password_error));
                    infoWidget->showSecs();
                    return;
                } else {
                    struGsh.nAuthority = AUTHORITY_ENGINEER;
                }
            }else if(ret == QDialog::Rejected){
                m_passwordDlg->hide();
            }
        }

        if (struGsh.nAuthority == AUTHORITY_PRIMARY) {
            menuBtn[PAGE_SET]->setEnabled(false);
            menuBtn[PAGE_IDENTYFITY]->setEnabled(false);
            m_UserBtn->setText(myLan.user);
            m_editBtn->setDisabled(true);
            m_editBtn->setIcon(QIcon(":/res/png/action_disable_edit.png"));
        } else {
            if(!m_bFeeder) {
                menuBtn[PAGE_SET]->setEnabled(true);
                menuBtn[PAGE_IDENTYFITY]->setEnabled(true);
                m_editBtn->setDisabled(false);
                m_editBtn->setIcon(QIcon(":/res/png/action_edit.png"));
            }
            m_UserBtn->setText(myLan.factor);
        }
        emit sigPermissionChg();
    } 
}

/**
 * @brief MainWindow::保存按钮
 */
void MainWindow::onSaveBtnPressedSlt()
{
    myMessageBox msgBox(MSG_QUES,QString("%1?").arg(myLan.confirm_save_params), myLan.notice, this);
    int ret = msgBox.exec();
    if (ret == QDialog::Accepted) {
        infoWidget->raise();
        infoWidget->setLabelText(myLan.apply_and_wait);
        infoWidget->show();
        gFlow.sleep(1);
        if (!gFlow.saveGlobalParams(CNF_GLOBAL)) {
            infoWidget->setLabelText(QString("%1!").arg(myLan.save_params_failed));
        } else {
            infoWidget->setLabelText(QString("%1!").arg(myLan.save_params_sucess));
        }
        system("sync");
        gFlow.sleep(2);
        infoWidget->hide();
    }
}

/**
 * @brief MainWindow::关机按钮槽函数
 */
void MainWindow::onHaltBtnPressedSlt()
{
    if (struGsh.nFeederState) {
        infoWidget->setLabelText(QString("%1!").arg(myLan.please_stop_detect));
        infoWidget->raise();
        infoWidget->show();
        gFlow.sleep(1);
        infoWidget->hide();
        return;
    }

    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_shutdown), myLan.notice, this);

    int ret = msgBox.exec();
    if (ret == QDialog::Accepted) {
        emit sigClosedMqtt();

        gFlow.msleep(50);

        if (!gFlow.saveGlobalParams(CNF_GLOBAL)) {
            infoWidget->setLabelText(QString("%1!").arg(myLan.save_params_failed));
        } else {
            infoWidget->setLabelText(QString("%1!").arg(myLan.save_params_sucess));
        }

        this->close();
        gFlow.msleep(200);
#ifdef Q_OS_UNIX
        gFlow.SocShutdown();
        system("poweroff");
#endif
    }
}

/**
 * @brief MainWindow::振动量调节界面
 */
void MainWindow::onFeederSetBtnSlt()
{
    setFeederPage* feederDlg = new setFeederPage(this);
    feederDlg->refreshDisplay();
    feederDlg->exec();
}

/*!
 * \brief AI灵敏度调节
 */
void MainWindow::onCaliParaBtnClicked()
{
//    if(AUTHORITY_PRIMARY == struGsh.nAuthority) {
//        infoWidget->setLabelText("请升级权限!");
//        infoWidget->raise();
//        infoWidget->showSecs();
//        return;
//    }

    calibrateWidget pageCalibrate(this);
//    pageCalibrate.sltRefreshUI();
    pageCalibrate.exec();
}

/*!
 * \brief 槽: 吹气时间和延迟时间
 */
void MainWindow::onblowTimeSetBtnSlt()
{
    setblowtimepage blowpage(this);
    blowpage.refreshpage();
    blowpage.exec();
}

/**
 * @brief MainWindow::清灰设置界面
 */
void MainWindow::onWipeSetBtnSlt()
{
    setWipePage *wipeSetDlg = new setWipePage(this);
    wipeSetDlg->refreshDisplay();
    wipeSetDlg->exec();
}

/*!
 * \brief  槽：伺服驱动开关
 */
void MainWindow::onBeltBtnSlt()
{
    struGsh.bBeltOnff = !struGsh.bBeltOnff;
    QString str = struGsh.bBeltOnff ? QString("%1-%2").arg(myLan.belt).arg(myLan.close)
                                    : QString("%1-%2").arg(myLan.belt).arg(myLan.open);
    m_beltBtn->setText(str);
    g_serial->SwMotorOnOff(struGsh.bBeltOnff);
}


/*!
 * \brief 高光谱校正
 */
void MainWindow::onCalWaveBtnPressedSlt()
{
 if(m_bFeeder){
        infoWidget->setLabelText(myLan.please_stop_detect);
        infoWidget->delayShow();
    }else{
     myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_calibrate), myLan.notice, g_mainWidget);

        int ret = msgBox.exec();
        if (ret != QDialog::Accepted) {
            return;
        }

        infoWidget->raise();
        infoWidget->show();
        infoWidget->setLabelText(myLan.apply_and_wait);

        calIMserialsensors();
    }


    infoWidget->hide();
}

void MainWindow::onLabelRatioBtnPressedSlt()
{
    if (m_bFeeder || struGsh.nFeederState) {
        infoWidget->raise();
        infoWidget->setLabelText(QString::fromUtf8("请关闭供料后再进行定标操作"));
        infoWidget->showSecs();
        return;
    }

    myMessageBox warningBox(MSG_QUES,
                            QString::fromUtf8("定标操作会影响检测性能，是否继续？"),
                            QString::fromUtf8("定标"), g_mainWidget);
    if (warningBox.exec() != QDialog::Accepted) {
        return;
    }

    QStringList labels;
    if (!struGsh.vlabNamemap.isEmpty()) {
        const QMap<int, QString> &labelMap = struGsh.vlabNamemap.first();
        for (const QString &label : labelMap.values()) {
            if (!label.isEmpty() && !labels.contains(label)) {
                labels.append(label);
            }
        }
    }

    if (labels.isEmpty()) {
        infoWidget->raise();
        infoWidget->setLabelText(QString::fromUtf8("未找到AI标签"));
        infoWidget->showSecs();
        return;
    }

    QMap<QString, double> currentRatios;
    for (const QString &label : labels) {
        currentRatios[label] = struGsh.aiLabelRatio.value(label, 0.0);
    }

    LabelRatioDialog ratioDialog(labels, currentRatios, g_mainWidget);
    if (ratioDialog.exec() != QDialog::Accepted) {
        return;
    }

    const int cameraCount = gFlow.getMachineCamNum();
    if (cameraCount <= 0) {
        return;
    }

    struGsh.bCalibrateSoc = true;
    struGsh.aiLabelRatio = ratioDialog.ratios();
    QVector<QMap<QString, double>> calibrationRatios;
    QVector<std::vector<std::vector<BBox>>> calibrationSamples;

    for (int round = 0; round < 3; ++round) {
        for (std::vector<BBox> &boxes : struGsh.vSocInferReady) {
            boxes.clear();
        }

        onFeederBtnClickedSlt();
        if (!m_bFeeder) {
            return;
        }

        QDialog collectDialog(g_mainWidget);
        collectDialog.setWindowTitle(QString::fromUtf8("采集定标数据"));
        collectDialog.setModal(true);
        collectDialog.setMinimumSize(420, 180);
        auto *collectLayout = new QVBoxLayout(&collectDialog);
        auto *collectLabel = new QLabel(
            QString::fromUtf8("正在采集第 %1 轮数据，请点击关闭供料").arg(round + 1),
            &collectDialog);
        collectLabel->setAlignment(Qt::AlignCenter);
        collectLayout->addWidget(collectLabel);
        auto *stopButton = new QPushButton(QString::fromUtf8("关闭供料"), &collectDialog);
        collectLayout->addWidget(stopButton);
        connect(stopButton, &QPushButton::clicked, &collectDialog, [this, &collectDialog]() {
            onFeederBtnClickedSlt();
            collectDialog.accept();
        });
        collectDialog.exec();

        if (m_bFeeder) {
            return;
        }

        calibrationSamples.append(struGsh.vSocInferReady);
        calibrationRatios.append(struGsh.aiLabelRatio);

        if (round == 2) {
            break;
        }

        myMessageBox continueBox(MSG_QUES,
                                 QString::fromUtf8("本轮数据采集完成，是否继续采集？"),
                                 QString::fromUtf8("定标"), g_mainWidget);
        if (continueBox.exec() != QDialog::Accepted) {
            break;
        }
    }

    QDialog waitDialog(g_mainWidget);
    waitDialog.setWindowTitle(QString::fromUtf8("正在定标"));
    waitDialog.setModal(true);
    waitDialog.setMinimumSize(420, 160);
    auto *waitLayout = new QVBoxLayout(&waitDialog);
    auto *waitLabel = new QLabel(QString::fromUtf8("正在等待定标结果，请稍候..."), &waitDialog);
    waitLabel->setAlignment(Qt::AlignCenter);
    waitLayout->addWidget(waitLabel);
    waitDialog.show();
    QApplication::processEvents();

    const bool calibrationSuccess = gFlow.calibrate(calibrationRatios, calibrationSamples);
    waitDialog.close();
    struGsh.bCalibrateSoc = false;
    if (!calibrationSuccess) {
        infoWidget->raise();
        infoWidget->setLabelText(QString::fromUtf8("定标失败"));
        infoWidget->showSecs();
    }
}

void MainWindow::onFactoryNameClicked()
{
INPUT_NAME1:
        QString str;
        myInputMethod input("公司名称", m_factoryName->text(), g_mainWidget);
        int ret = input.exec();
        if (ret == QDialog::Accepted) {
            str = input.getText().trimmed();
            if(str.compare("") == 0) {
                infoWidget->raise();
                infoWidget->setLabelText("不能为空");
                infoWidget->showSecs();
                goto INPUT_NAME1;
            }
            if(!gFlow.inputStrIsValid(str)) {
                infoWidget->raise();
                infoWidget->setLabelText("不能输入特殊字符");
                infoWidget->showSecs();
                goto INPUT_NAME1;
            }
            struCnfg.gCompanyName = str;
            m_factoryName->setText(str);
        }
}

/*!
 * \brief 开机检查加密信息
 */
void MainWindow::checkEncryptOK()
{
#ifdef Q_OS_UNIX
    const int REMIND_DAYS = 7;
    int nRemindDays = 0;
    int nRemindTimes = DelayCode::MAX_ERROR_TIMES;

    int nResult = g_delagCode.detectDelayCode(nRemindDays, nRemindTimes);//! 获取加密信息

    QString sInfo;
    if (nResult == DelayCode::DETECT_NORMAL) {      //! 1.正常进入系统
        if (g_delagCode.getEnable() && nRemindDays <= REMIND_DAYS) {    //! 加密到期提醒
            sInfo = QString("%1: %2").arg(myLan.limit_remain).arg(nRemindDays);
            myMessageBox msgBox(MSG_WARN,QString("%1").arg(sInfo),"");
            msgBox.exec();
        }
    } else if (nResult == DelayCode::DETECT_EXPIRE) {   //! 2.到期锁屏
        while (1) {
            sInfo = QString("%1(%2)").arg(myLan.input_code)
                            .arg(g_delagCode.getMachineId());

            myInputMethod *kb = new myInputMethod(sInfo,"" );
            if (kb->exec() == QDialog::Accepted) {
                QString sMachineId;
                QDate date;
                int nRet = g_delagCode.checkDelayCode(kb->getText(), sMachineId, date);
                if (nRet == DelayCode::DC_SUCCESS) {
                    QString sInfo = QString("%1: %2\n%3: %4?")
                                        .arg(myLan.serial_num).arg(sMachineId)
                                        .arg(myLan.dc_limit).arg(date.toString("yyyy-MM-dd"));
                    myMessageBox msgBox(MSG_QUES, sInfo, "");
                    if (msgBox.exec() == QDialog::Accepted) {
                        nRet = g_delagCode.encryptAndDecrypt(kb->getText());
                        if (nRet == DelayCode::DC_SUCCESS) {
                            break;
                        } else {
                            myMessageBox msgBox(MSG_WARN,QString("%1").arg(g_delagCode.getText(nRet)),"");
                            msgBox.exec();
                        }
                    }
                } else {
                    myMessageBox msgBox(MSG_WARN,QString("%1").arg(g_delagCode.getText(nRet)),"");
                    msgBox.exec();
                }
            }
        }
    } else if (nResult == DelayCode::DETECT_TIME_ABNORMAL) {    //! 3.系统时间异常
        QString sText = QString("%1(%2%3/%4)").arg(myLan.dc_input_time_decode)
                            .arg(g_delagCode.getMachineId())
                            .arg(g_delagCode.getExpireDate().toString("yyyyMMdd"))
                            .arg(g_delagCode.getLocalTime().toString("yyyyMMdd"));
        if (nRemindTimes >= 0) {    //! 时间异常开机仍有剩余次数
            while (1) {
                sInfo = QString("%1\n%2: %3\n\n%4")
                            .arg(myLan.msg_time_abnormal)
                            .arg(myLan.remind_times)
                            .arg(nRemindTimes + 1)
                            .arg(myLan.cfm_input_time_code);

                myMessageBox msgBox(MSG_QUES, sInfo, myLan.notice, g_mainWidget);

                if (msgBox.exec()==QDialog::Accepted) {
                    myInputMethod *kb = new myInputMethod(sText, "");
                    if (kb->exec() == QDialog::Accepted) {
                        int nRet = g_delagCode.calibrateTime(kb->getText());
                        if (nRet == DelayCode::DC_SUCCESS) {
                            break;
                        } else {
                            myMessageBox msgBox(MSG_WARN,QString("%1").arg(g_delagCode.getText(nRet)),"");
                            msgBox.exec();
                        }
                    }
                } else {
                    break;
                }
            }
        } else {    //! 时间异常开机超过5次则强制输入时间校准码
            while (1) {
                myInputMethod *kb = new myInputMethod(sText,"");
                if (kb->exec() == QDialog::Accepted) {
                    int nRet = g_delagCode.calibrateTime(kb->getText());
                    if (nRet == DelayCode::DC_SUCCESS) {
                        break;
                    } else {
                        myMessageBox msgBox(MSG_WARN,QString("%1").arg(g_delagCode.getText(nRet)),"");
                        msgBox.exec();
                    }
                }
            }
        }
    }
#endif
}

/**
 * @brief MainWindow::refreshSchemBox
 */
void MainWindow::refreshSchemBox()
{
    m_schemBox->blockSignals(true);
    m_schemBox->clear();
    for(int i = 0; i < struCnfg.nSchemTotal; i++){
        m_schemBox->addItem(QIcon(), gFlow.getSchemeName(i));
    }
    m_schemBox->setCurrentIndex(struCnfg.nCntScheme);
    m_schemBox->blockSignals(false);
}

/*!
 * \brief 校正IM系列传感器
 */
void MainWindow::calIMserialsensors()
{
    int ncamid = -1;
    for(int i=0; i<gFlow.getMachineCamNum(); i++){
        if(gFlow.bIsIMSerialCam(i)){
            ncamid = i;
            break;
        }
    }

    if(ncamid == -1){
        return;
    }

    //! 切换原始信号
    gFlow.sendCamSignalMode(struGsh.nAnalyzeIndex, ncamid, 0);
    gFlow.sleep(2);

    //! 关灯
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bHologenLampOn = false;
    gFlow.sendLampPara(struGsh.nAnalyzeIndex);
    gFlow.sleep(3);

    //! 校正系数获取
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, 2, 0, ncamid);

    gFlow.sleep(1);

    //! 校正系数保存
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, 2, 2, ncamid);

    gFlow.sleep(1);

    //! 开灯
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bHologenLampOn = true;
    gFlow.sendLampPara(struGsh.nAnalyzeIndex);
    gFlow.sleep(3);


    //! 明校正
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, 2, 1, ncamid);

    gFlow.sleep(1);

    //! 2、校正系数保存
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, 2, 3, ncamid);
    gFlow.sleep(2);


    //! 切换到校正波形
    gFlow.sendCamSignalMode(struGsh.nAnalyzeIndex, ncamid, 1);
    gFlow.sleep(2);
}


/**
 * @brief MainWindow::onSchmBoxChangedSlt
 * @param schemeIndex
 */
void MainWindow::onSchmBoxChangedSlt(int schemeIndex)
{
    if(m_bFeeder){
        infoWidget->setLabelText(QString("%1.....").arg(myLan.please_stop_detect));
        infoWidget->raise();
        infoWidget->show();
        infoWidget->showSecs();
    }else{
        QString file_name;
        infoWidget->setLabelText(QString("%1.....").arg(myLan.switch_scheme));
        infoWidget->raise();
        infoWidget->show();
        gFlow.msleep(200);

        auto *pprofile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

        //! 先保存参数
        gFlow.saveGlobalParams(CNF_GLOBAL);

        int nCamid = 3;
        QStringList nWavelst, nNewWavelst;
        if(struCnfg.nMachineType == nMachine_Bottle){
            for(int idx = 0; idx<gFlow.getMachineCamNum(); idx++){
                if(gFlow.bIsIMSerialCam(idx)){
                    nCamid = idx;
                    for(int n=0; n<MAX_WAVE_LINE; n++){
                        nWavelst<<QString("%1").arg(pprofile->n12WavesVal[idx][n]);
                    }
                }
            }
        }


        //! 切换方案序号
        struCnfg.nCntScheme = schemeIndex;

        //! 清空方案统计参数
        pprofile->labprotymap.clear();
        pprofile->blabshowmap.clear();
        pprofile->Area2weightmap.clear();
        pprofile->strctAI.vlabelidxmap.clear();
        pprofile->strctAI.vlabMutexmap.clear();
        pprofile->strctAI.vlabthdmap.clear();
        pprofile->vMegerlabmap.clear();
        pprofile->staclassEnmap.clear();
        pprofile->vlabMixmap.clear();

        //! 重新加载方案参数
        for(int i=0; i< struCnfg.analyzeNum; i++){
            file_name = struCnfg.analyzeInfo[i]->anlyze_schemeList[struCnfg.nCntScheme].code;
            gFlow.getAnalyzeProfileParams(&struCnfg.analyzeInfo[i]->analyzeProfile, file_name);
        }

        //! Soc切换模型
        for(int i =0; i<gFlow.getMachineCamNum(); i++){
            gFlow.SocModelSWitch(struGsh.nAnalyzeIndex,i);
            gFlow.msleep(5);
        }

        gFlow.sleep(2);

        //! 查询模型信息
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            gFlow.reqSocModelinfo(struGsh.nAnalyzeIndex,i);
            gFlow.msleep(300);
        }

        //! 查询标签信息
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            struGsh.vlabNamemap[i].clear();
            gFlow.reqSocModelLabelinfo(struGsh.nAnalyzeIndex,i);
            gFlow.msleep(500);
        }


        //! 重置统计组信息
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaticGroup = 0;
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist.clear();
        QStringList colist;
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nType[i]){//! 独立
                QStringList tmplist;
                tmplist<<QString::number(i);
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaticGroup += 1;
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist.push_back(tmplist);
            }else{
                colist<<QString::number(i);
            }
        }
        if(!colist.isEmpty()){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaticGroup += 1;
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vUnitaddrlist.push_back(colist);
        }

        gFlow.ResetGlobalStaParams();

        //! 查统计类别
        for(int k=0; k<gFlow.getMachineCamNum(); k++){
            gFlow.reqStainfo(k);
            gFlow.msleep(500);
        }

        //! 创建方案统计数据表
        g_sqliteInter->createTable(struCnfg.schemeList[struCnfg.nCntScheme].code);

        emit sigChangeScheme();  //! 刷新

        gFlow.sendAll(false);

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
           gFlow.sendSorterlabparams(false);
        }

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
            QJsonObject staicsjson;
            staicsjson["Scheme"] = true;
            g_Lotsocket->uploadInfo(staicsjson);
        }

        if(struCnfg.nMachineType == nMachine_Bottle){
            for(int n=0; n<MAX_WAVE_LINE; n++){
                nNewWavelst<<QString("%1").arg(pprofile->n12WavesVal[nCamid][n]);
            }

            for(QString var : nWavelst){
                if(!nNewWavelst.contains(var)){
                    infoWidget->setLabelText(myLan.calibrate_GGP_wave);
                    gFlow.msleep(1000);
                }
            }
        }

        //! 刷新页面
        onRefreshSorterpage();
        onrefreshautosamplesta(1,false);

        infoWidget->hide();
    }
}

/**
 * @brief MainWindow::onSchemEditBtnClicked
 */
void MainWindow::onSchemEditBtnClicked()
{
    if(m_bFeeder){
        infoWidget->setLabelText(QString("%1.....").arg(myLan.please_stop_detect));
        infoWidget->raise();
        infoWidget->show();
        infoWidget->showSecs();
    }else{
        schmWidget shemeDlg(0, g_mainWidget);
        int ret = shemeDlg.exec();

        if(ret == QDialog::Accepted) {
            refreshSchemBox();
        }
    }
}

/*!
 * \brief 刷新皮带状态
 */
void MainWindow::onrefreshBeltSta()
{
    QString str = struGsh.bBeltOnff ? QString("%1-%2").arg(myLan.belt).arg(myLan.close)
                                    : QString("%1-%2").arg(myLan.belt).arg(myLan.open);
    m_beltBtn->setText(str);

}

/*!
 * \brief MainWindow::onInitWebSlt
 * \param bsta
 */
void MainWindow::onInitWebSlt(bool bsta)
{
    if(bsta)
        emit sigInitWebinfo();
}

/*!
 * \brief MainWindow::onrefreshautosamplesta
 * \param nIdx
 */
void MainWindow::onrefreshautosamplesta(int nIdx, bool breset)
{
    Q_UNUSED(breset);
    if(m_bFeeder){
        m_autosamplelb->setText(QString("%1:  %2").arg(myLan.auto_sample_channel)
                                .arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.sortclassname[nIdx]));
    }else{
        m_autosamplelb->setText(QString("%1").arg(myLan.stop_auto_sample));
    }

    m_autosamplelb->setHidden(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample);
}

/*!
 * \brief 刷新系统状态
 * \param nIdx
 */
void MainWindow::onRefreshSystemSta(int Sta)
{
    m_nSystemAlarmSta = Sta;  //! 保存系统状态
    
    if(Sta == 0){
        menuBtn[PAGE_SYSTEM]->setStyleSheet("font:30px;background-color:white;");
    }else{
        menuBtn[PAGE_SYSTEM]->setStyleSheet("font:30px;background-color:red;");
    }   
}

void MainWindow::onHandleScanCodeRes(bool bSta)
{
    if(bSta){
        if(!m_passwordDlg->isHidden()){
            m_passwordDlg->hide();
        }

        if(struGsh.nAuthority == AUTHORITY_ENGINEER){
            return;
        }
        
        struGsh.nAuthority = AUTHORITY_ENGINEER;
        if(!m_bFeeder) {
            menuBtn[PAGE_SET]->setEnabled(true);
            menuBtn[PAGE_IDENTYFITY]->setEnabled(true);
            m_editBtn->setEnabled(true);
        }
        m_UserBtn->setText(myLan.factor);
    }else{
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.operate_failed));
        infoWidget->showSecs();
    }
}

/*!
 * \brief 槽： 刷新色选联调
 */
void MainWindow::onRefreshSorterpage()
{
    if(m_stack->currentIndex() ==PAGE_SORTERCONTROL){
        onMenuBtnPressedSlt(PAGE_HOME);
    }

    if(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        menuBtn[PAGE_SORTERCONTROL]->hide();
    }else{
        menuBtn[PAGE_SORTERCONTROL]->show();
    }
}

void MainWindow::onRefreshAdjTips(QStringList lablst)
{
    if(!infoWidget->isHidden())
        return;

    QString tips;
    tips+= QString("%1%2: %3\n").arg(myLan.abnormal).arg(myLan.category).arg(lablst.join(", "));
    tips+= myLan.adjusting_sorter;
    infoWidget->raise();
    infoWidget->setLabelText(tips);
    infoWidget->showSecs(5);
}
