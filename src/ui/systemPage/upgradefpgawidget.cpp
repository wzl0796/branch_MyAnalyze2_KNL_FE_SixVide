#include "upgradefpgawidget.h"
#include "common/selectfiledialog.h"
#include "common/mymessagebox.h"
#include "core/io/udp/myudp.h"
#include <QScrollArea>

upgradeFPGAwidget::upgradeFPGAwidget(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief upgradeFPGAwidget::初始化参数
 */
void upgradeFPGAwidget::initParams()
{
    initLanParams();

    initUpgradeInfo();

    m_copyFilethread = new FileCopyThread();

    upgradeTimer = new QTimer();
    connect(upgradeTimer, SIGNAL(timeout()), this, SLOT(upgradeTimeOutSlt()));

    burnTimer = new QTimer();
    connect(burnTimer,    SIGNAL(timeout()), this, SLOT(burnTimeOutSlt()));

    m_bClear = false;
}

/**
 * @brief upgradeFPGAwidget::初始化语言参数
 */
void upgradeFPGAwidget::initLanParams()
{
    curCamId = "";
    CtrlStepInfoList << myLan.switch_board_CL_to_factor
                     << myLan.erase_board_CL
                     << myLan.transmit_and_burn
                     << myLan.switch_board_CL_to_user;

    CamStepInfoList << myLan.switch_board_CL_to_factor
                    << myLan.switch_board_Cam_to_factor
                    << myLan.erase_board_Cam
                    << myLan.transmit_and_burn
                    << myLan.switch_board_Cam_to_user
                    << myLan.switch_board_CL_to_user;

    SocStepInfoList << myLan.switch_board_CL_to_factor
                    << myLan.switch_board_Cam_to_factor
                    << myLan.transmit_and_burn
                    << myLan.switch_board_Cam_to_user
                    << myLan.switch_board_CL_to_user;

    EjectorStepInfoList << myLan.switch_board_CL_to_factor
                        << myLan.switch_ejector_to_factor
                        << myLan.erase_board_Ejector
                        << myLan.transmit_and_burn
                        << myLan.switch_ejector_to_user
                        << myLan.switch_board_CL_to_user;

    m_selBtnSigMap  = new QSignalMapper();
    m_boardComboBoxSigMap = new QSignalMapper();
    m_fileSigMap    = new QSignalMapper();
}

/**
 * @brief upgradeFPGAwidget::初始化升级板信息
 */
void upgradeFPGAwidget::initUpgradeInfo()
{
    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_struInfo[i].bEn = false;
        m_struInfo[i].nBoard = 0; //! 板序号

        m_struInfo[i].nBoardTotal = struCnfg.analyzeNum;

        for (int j = 0; j < struCnfg.analyzeNum; j++) {
            m_struInfo[i].qvControlUnit << j;
            m_struInfo[i].nStatus[j] = UPGRADE_UNSTARTED;
            m_struInfo[i].sUpgradeInfo[j] = "";
        }
    }

    upgrade_type    = UPGRADE_UDISK;
    m_socmodelIdx = 0;

    m_bClear = false;

    //! 更新下板名称
    m_strList.clear();
    m_strList << myLan.arm_linux;
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_strList<<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i];
    }
    m_strList<< myLan.control_board << myLan.low_valve << myLan.board_AI
             << QString("%1").arg(myLan.AImodel);

}

/**
 * @brief upgradeFPGAwidget::搭建界面
 */
void upgradeFPGAwidget::createPage()
{
    QString BoxStyle = QString("QComboBox {border: 1px solid lightgray; border-radius: 10px; padding: 1px 1px 1px 1px;background-color: #FFFFFF;}");
    QString BoxArrowStyle = QString("QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 50px; "
                                    "border-left-width: 0px; border-left-color:darkgray; border-left-style: solid; "
                                    "border-top-right-radius: 3px; border-bottom-right-radius: 3px;}");
    QString BoxArrowImage = QString("QComboBox::down-arrow {image:url(:/res/png/icon_down_arrow); width:20px; height: 20px;}");


    QVBoxLayout *mainLayout = new QVBoxLayout(this);


//    //本地升级/U盘升级
//    m_updateTab = new QTabBar;
//    m_updateTab->setStyleSheet(QString("QTabBar::tab{border:0px;width:180;height:55;outline:0px;}")
//                             +QString("QTabBar::tab:first{border-top-left-radius: 25px;border-bottom-left-radius: 25px;}")
//                             +QString("QTabBar::tab:last{border-top-right-radius: 25px;border-bottom-right-radius: 25px;}")
//                             +QString("QTabBar::tab:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen)
//                             +QString("QTabBar::tab:!selected{background-color:%1;color:%2;}").arg(g_ColorLightGray3).arg(g_ColorGray));
//    m_updateTab->setDrawBase(false);
//    m_updateTab->addTab(myLan.local);
//    m_updateTab->addTab(myLan.Udisk);
//    m_updateTab->setTabEnabled(0,false);
//    mainLayout->addWidget(m_updateTab, 0, Qt::AlignCenter);

    //! 升级模型序号
    m_labelIdxlb = new QLabel(myLan.modelIdx);
    m_labelIdxlb->setFixedSize(200,BTN_HEI);
    m_socmodelIdxcmb = new QComboBox();
    modelidxlstwid   = new QListWidget();
    m_socmodelIdxcmb->setFixedSize(6*BTN_WID + BTN_WID+70, BTN_HEI);
    m_socmodelIdxcmb->setIconSize(QSize(BTN_HEI-5,BTN_HEI-5));
    m_socmodelIdxcmb->setStyleSheet(QString("%1 %2 %3").arg(BoxStyle).arg(BoxArrowStyle).arg(BoxArrowImage));
    m_socmodelIdxcmb->setModel(modelidxlstwid->model());
    m_socmodelIdxcmb->setView(modelidxlstwid);
    QStringList modelidxlst;
    for(int i=0; i<20; i++){
        modelidxlst<<QString::number(i);
    }
    m_socmodelIdxcmb->insertItems(0,modelidxlst);

    m_clearParamschk = new MyCheckBox(myLan.clear_params);
    m_clearParamschk->setCheckBtnSize(BTN_HEI, BTN_HEI);

    QWidget     *boardWidget = new QWidget();
    QVBoxLayout *vBoardLay = new QVBoxLayout(boardWidget);
    QHBoxLayout *hlayModelidx = new QHBoxLayout();
    QHBoxLayout *boardLayout[BOARD_TOTAL];
    QListWidget *listWidget[BOARD_TOTAL];
    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_selBtns[i] = new QPushButton(); //! 板使能
        m_selBtns[i]->setIconSize(QSize(BTN_HEI-5, BTN_HEI-5));
        m_selBtns[i]->setFixedSize(BTN_HEI, BTN_HEI);
        connect(m_selBtns[i], SIGNAL(pressed()), m_selBtnSigMap, SLOT(map()));
        m_selBtnSigMap->setMapping(m_selBtns[i], i);

        m_boardType[i] = new QLabel(m_strList.at(i)); //! 板名称
        m_boardType[i]->setFixedSize(BTN_WID, BTN_HEI);
        m_boardType[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        listWidget[i] = new QListWidget(); //! 版序号选择下拉框
        m_boardNum[i] = new QComboBox();
        m_boardNum[i]->setCurrentIndex(-1);
        m_boardNum[i]->setFixedSize(BTN_WID+60, BTN_HEI);
        m_boardNum[i]->setIconSize(QSize(BTN_HEI-5,BTN_HEI-5));
        m_boardNum[i]->setStyleSheet(QString("%1 %2 %3").arg(BoxStyle).arg(BoxArrowStyle).arg(BoxArrowImage));
        m_boardNum[i]->setModel(listWidget[i]->model());
        m_boardNum[i]->setView(listWidget[i]);
        connect(m_boardNum[i], SIGNAL(currentIndexChanged(int)), m_boardComboBoxSigMap, SLOT(map()));
        m_boardComboBoxSigMap->setMapping(m_boardNum[i], i);

        m_boardFile[i] = new QPushButton(myLan.select_file); //! 文件选择
        m_boardFile[i]->setFixedSize(BTN_WID*6, BTN_HEI);
        connect(m_boardFile[i], SIGNAL(pressed()), m_fileSigMap, SLOT(map()));
        m_fileSigMap->setMapping(m_boardFile[i], i);

        boardLayout[i] = new QHBoxLayout();
        boardLayout[i]->addSpacing(15);
        boardLayout[i]->addWidget(m_selBtns[i]);       
        boardLayout[i]->addWidget(m_boardType[i]);
        boardLayout[i]->addSpacing(10);
        boardLayout[i]->addWidget(m_boardNum[i]);
        boardLayout[i]->addSpacing(10);
        boardLayout[i]->addWidget(m_boardFile[i]);
        boardLayout[i]->addStretch();
        if (i == BOARD_LINUX) {
            boardLayout[i]->addSpacing(10);
            boardLayout[i]->addWidget(m_clearParamschk);
            // allow the file button to shrink, keep the checkbox fixed
            m_boardFile[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            m_clearParamschk->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        }
        vBoardLay->addLayout(boardLayout[i]);
    }
    hlayModelidx->addSpacing(15);
    hlayModelidx->addWidget(m_labelIdxlb);
    hlayModelidx->addSpacing(20);
    hlayModelidx->addWidget(m_socmodelIdxcmb);
    hlayModelidx->addStretch();
    vBoardLay->addLayout(hlayModelidx);

    m_boardNum[BOARD_LINUX]->hide();
    m_boardNum[BOARD_LINUX]->setDisabled(true);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setMinimumHeight(550);
    scrollArea->setWidget(boardWidget);
    mainLayout->addWidget(scrollArea);


    //! 下侧结果显示控件
    m_sourceView = new QTreeView();
    m_sourceView->setRootIsDecorated(false);
    m_sourceView->setAlternatingRowColors(true);
    m_sourceView->header()->hide();
    m_sourceView->header()->setDefaultAlignment(Qt::AlignLeft);
    m_sourceView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_sourceView->setStyleSheet(QString("QTreeView::item{height: 35px;}")
                              +QString("QTreeView::Item:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen));
    m_sourceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sourceView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_sourceView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_sourceView->setMinimumWidth(800);
    
    mainLayout->addWidget(m_sourceView, 0, Qt::AlignLeft);

    onTabIndexChangedSlt(UPGRADE_UDISK);

}

/**
 * @brief upgradeFPGAwidget::信号槽绑定
 */
void upgradeFPGAwidget::connectSigAndSlot()
{
    connect(m_selBtnSigMap,        SIGNAL(mapped(int)),         this, SLOT(onSelBtnsPressed(int)));
    connect(m_boardComboBoxSigMap, SIGNAL(mapped(int)),         this, SLOT(onBoardComboBoxesPressed(int)));
    connect(m_fileSigMap,          SIGNAL(mapped(int)),         this, SLOT(onFileBtnsPressed(int)));
//    connect(m_updateTab,           SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChangedSlt(int)));
    connect(m_socmodelIdxcmb,      SIGNAL(currentIndexChanged(int)),this,SLOT(onModelIdxChgedSlt(int)));
    connect(m_clearParamschk,SIGNAL(toggled(bool)), this, SLOT(onClearParamsChgSlt(bool)));
}

/**
 * @brief upgradeFPGAwidget::刷新界面显示
 */
void upgradeFPGAwidget::refreshPage()
{
    initUpgradeInfo();
    refreshSelsBtnDisplay();
    refreshFileBtnDisplay();
    refreshBoardComboBoxDisplay();
    refreshResultDisplay();

    m_clearParamschk->setChecked(m_bClear);

//    m_updateTab->setCurrentIndex(upgrade_type);
    onTabIndexChangedSlt(UPGRADE_UDISK);
    refreshModelindexComb();
}

/**
 * @brief upgradeFPGAwidget::刷新板使能的显示
 */
void upgradeFPGAwidget::refreshSelsBtnDisplay()
{
    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_selBtns[i]->setIcon(QIcon());
    }

    const bool bValveEnabled = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal;
    m_selBtns[BOARD_VALVE]->setVisible(bValveEnabled);
    m_boardType[BOARD_VALVE]->setVisible(bValveEnabled);
    m_boardNum[BOARD_VALVE]->setVisible(bValveEnabled);
    m_boardFile[BOARD_VALVE]->setVisible(bValveEnabled);
    if (!bValveEnabled) {
        m_struInfo[BOARD_VALVE].bEn = false;
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_selBtns[BOARD_FRONT+i]->show();
            m_boardType[BOARD_FRONT+i]->show();
            m_boardType[BOARD_FRONT+i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        }else{
            m_selBtns[BOARD_FRONT+i]->hide();
            m_boardType[BOARD_FRONT+i]->hide();
        }
    }
}

/**
 * @brief upgradeFPGAwidget::刷新选择文件的显示
 */
void upgradeFPGAwidget::refreshFileBtnDisplay()
{
    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_boardFile[i]->setText(QString("%1").arg(myLan.select_file));
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i < gFlow.getMachineCamNum()){
            m_boardFile[BOARD_FRONT+i]->show();
        }else{
            m_boardFile[BOARD_FRONT+i]->hide();
        }
    }
}

/**
 * @brief upgradeFPGAwidget::刷新板数量组合框
 */
void upgradeFPGAwidget::refreshBoardComboBoxDisplay()
{
    //! 隐藏无用项
    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i < gFlow.getMachineCamNum()){
            m_boardNum[BOARD_FRONT+i]->show();
        }else{
            m_boardNum[BOARD_FRONT+i]->hide();
        }
    }

    //! 填充升级combox项
    m_boardComboBoxSigMap->blockSignals(true);

    QPixmap pixmap(1, 45);
    pixmap.fill(Qt::transparent);
    QIcon icon(pixmap);


    //! 增加myLan.all选项
    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_boardNum[i]->clear();
        m_boardNum[i]->addItem(myLan.all);
    }

    //! 上位机
    m_boardNum[BOARD_LINUX]->setCurrentIndex(-1);

    //! 前视相机板
    for (int i = 0; i < m_struInfo[BOARD_FRONT].nBoardTotal; i++) {
        m_boardNum[BOARD_FRONT]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_FRONT]->setCurrentIndex(-1);

    //! 前视相机板2
    for (int i = 0; i < m_struInfo[BOARD_FRONT2].nBoardTotal; i++) {
        m_boardNum[BOARD_FRONT2]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_FRONT2]->setCurrentIndex(-1);

    //! 后视相机板
    for (int i = 0; i < m_struInfo[BOARD_REAR].nBoardTotal; i++) {
        m_boardNum[BOARD_REAR]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_REAR]->setCurrentIndex(-1);

    //! 后视相机板2
    for (int i = 0; i < m_struInfo[BOARD_REAR2].nBoardTotal; i++) {
        m_boardNum[BOARD_REAR2]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_REAR2]->setCurrentIndex(-1);

    //! 额外相机板1
    for (int i = 0; i < m_struInfo[BOARD_CAM_EXTRA1].nBoardTotal; i++) {
        m_boardNum[BOARD_CAM_EXTRA1]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_CAM_EXTRA1]->setCurrentIndex(-1);

    //! 额外相机板2
    for (int i = 0; i < m_struInfo[BOARD_CAM_EXTRA2].nBoardTotal; i++) {
        m_boardNum[BOARD_CAM_EXTRA2]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_CAM_EXTRA2]->setCurrentIndex(-1);

    //! 控制板
    for (int i = 0; i < m_struInfo[BOARD_CONTROL].nBoardTotal; i++) {
        m_boardNum[BOARD_CONTROL]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_CONTROL]->setCurrentIndex(-1);


    for (int i = 0; i < m_struInfo[BOARD_VALVE].nBoardTotal; i++) {
        m_boardNum[BOARD_VALVE]->addItem(icon, QString("%1").arg(i+1));
    }
    m_boardNum[BOARD_VALVE]->setCurrentIndex(-1);


    //! SOC软件及模型
    for (int i = 0; i < m_struInfo[BOARD_SOC].nBoardTotal; i++) {
        for(int j=0; j<gFlow.getMachineCamNum(); j++){
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[j] !=SENSOR_IM04){
                m_boardNum[BOARD_SOC]->addItem(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[j]);
            }
            m_boardNum[BOARD_SOC_MODEL]->addItem(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[j]);
        }
    }
    m_boardNum[BOARD_SOC]->setCurrentIndex(-1);
    m_boardNum[BOARD_SOC_MODEL]->setCurrentIndex(-1);

    m_boardComboBoxSigMap->blockSignals(false);
}

/**
 * @brief upgradeFPGAwidget::升级方式切换
 * @param nIndex
 */
void upgradeFPGAwidget::onTabIndexChangedSlt(int nIndex)
{
    upgrade_type = nIndex;
    if (upgrade_type == UPGRADE_LOCAL) {
        if (m_struInfo[BOARD_LINUX].bEn) {
            m_struInfo[BOARD_LINUX].bEn = false;
            m_selBtns[BOARD_LINUX]->setIcon(QIcon());
            for (int i = BOARD_FRONT; i < BOARD_TOTAL; i++) {
                m_selBtns[i]->setEnabled(true);
                m_boardNum[i]->setEnabled(true);
                m_boardFile[i]->setEnabled(true);
            }
        }
        m_selBtns[BOARD_LINUX]->hide();
        m_boardType[BOARD_LINUX]->hide();
        m_boardFile[BOARD_LINUX]->hide();
        m_boardNum[BOARD_LINUX]->hide();
        m_clearParamschk->hide();
    } else {
        m_selBtns[BOARD_LINUX]->show();
        m_boardType[BOARD_LINUX]->show();
        m_boardFile[BOARD_LINUX]->show();
        m_boardNum[BOARD_LINUX]->hide();
        m_clearParamschk->show();
    }
}

/**
 * @brief upgradeFPGAwidget::使能按钮
 * @param nIndex
 */
void upgradeFPGAwidget::onSelBtnsPressed(int nIndex)
{
    m_struInfo[nIndex].bEn = !m_struInfo[nIndex].bEn;

    if (m_struInfo[nIndex].bEn) {
        m_selBtns[nIndex]->setIcon(QIcon(":/res/png/action_select.png"));
    } else {
        m_selBtns[nIndex]->setIcon(QIcon());
    }

    switch (nIndex) {
    case BOARD_FRONT:
    case BOARD_SOC:
    case BOARD_SOC_MODEL:
        m_struInfo[nIndex].nBoardUnit = 0;
        break;
    case BOARD_FRONT2:
        m_struInfo[nIndex].nBoardUnit = 1;
        break;
    case BOARD_REAR:
        m_struInfo[nIndex].nBoardUnit = 2;
        break;
    case BOARD_REAR2:
        m_struInfo[nIndex].nBoardUnit = 3;
        break;
    case BOARD_CAM_EXTRA1:
        m_struInfo[nIndex].nBoardUnit = 4;
        break;
    case BOARD_CAM_EXTRA2:
        m_struInfo[nIndex].nBoardUnit = 5;
        break;
    case BOARD_VALVE:
        m_struInfo[nIndex].nBoardUnit = 0;
        break;
    default:
        break;
    }

    if (nIndex == BOARD_LINUX) {
        if (m_struInfo[nIndex].bEn) {
            for (int i = BOARD_FRONT; i < BOARD_TOTAL; i++) {
                m_struInfo[i].bEn = false;
                m_selBtns[i]->setIcon(QIcon());
                m_selBtns[i]->setEnabled(false);
                m_boardNum[i]->setEnabled(false);
                m_boardFile[i]->setEnabled(false);
            }
        } else {
            for (int i = BOARD_FRONT; i < BOARD_TOTAL; i++) {
                m_selBtns[i]->setEnabled(true);
                m_boardNum[i]->setEnabled(true);
                m_boardFile[i]->setEnabled(true);
            }
        }
    }
}

/**
 * @brief 槽：升级的板地址选择
 * @param nIndex
 */
void upgradeFPGAwidget::onBoardComboBoxesPressed(int nIndex)
{

    m_struInfo[nIndex].nBoard = m_boardNum[nIndex]->currentIndex();  //! 记录需要升级的分析仪的地址下标

//  int nBoardCount = m_struInfo[nIndex].nBoardTotal;

    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_struInfo[i].qvControlUnit.clear();
        m_struInfo[i].qvControlUnit<<0;    //! 控制板板地址固定0
    }

//    if (m_struInfo[nIndex].nBoard == 0) {
//        for (int i = 0; i < nBoardCount; i++) {
//            m_struInfo[nIndex].qvControlUnit << i;
//        }
//    } else {
//        m_struInfo[nIndex].qvControlUnit << (m_struInfo[nIndex].nBoard-1);
//    }

    switch (nIndex) {
    case BOARD_FRONT:
        m_struInfo[nIndex].nBoardUnit = 0;
        break;
    case BOARD_FRONT2:
        m_struInfo[nIndex].nBoardUnit = 1;
        break;
    case BOARD_REAR:
        m_struInfo[nIndex].nBoardUnit = 2;
        break;
    case BOARD_REAR2:
        m_struInfo[nIndex].nBoardUnit = 3;
        break;
    case BOARD_CAM_EXTRA1:
        m_struInfo[nIndex].nBoardUnit = 4;
        break;
    case BOARD_CAM_EXTRA2:
        m_struInfo[nIndex].nBoardUnit = 5;
        break;
    case BOARD_SOC:
        bSocAllUpd[0] = false;
        if(m_boardNum[nIndex]->currentIndex() == 0){
            bSocAllUpd[0] = true;
        }else{
            m_struInfo[nIndex].nBoardUnit = m_boardNum[nIndex]->currentIndex();
        }
        m_struInfo[nIndex].nBoard = m_boardNum[nIndex]->currentIndex() > 1 ? 1: m_boardNum[nIndex]->currentIndex();
        break;
    case BOARD_SOC_MODEL:
        bSocAllUpd[1] = false;
        if(m_boardNum[nIndex]->currentIndex() == 0){
            bSocAllUpd[1] = true;
        }else{
            m_struInfo[nIndex].nBoardUnit = m_boardNum[nIndex]->currentIndex();
        }
        m_struInfo[nIndex].nBoard = m_boardNum[nIndex]->currentIndex() > 1 ? 1 : m_boardNum[nIndex]->currentIndex();
        refreshModelindexComb();
        break;
    default:
        m_struInfo[nIndex].nBoardUnit = 0;
        break;
    }

}

/**
 * @brief upgradeFPGAwidget::文件选择
 * @param nIndex
 */
void upgradeFPGAwidget::onFileBtnsPressed(int nIndex)
{
    if (!m_struInfo[nIndex].bEn) {
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_upgrade_board));
        infoWidget->showSecs();
        return;
    }
    //! 检查U盘
#ifdef Q_OS_UNIX
    if(upgrade_type == UPGRADE_UDISK) {
        if (!gFlow.mountUDisk()) {
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.no_find_udisk));
            infoWidget->showSecs();
            return;
        }
    }
#endif
//    QStringList strList;
//    strList << "*.rpd" << "*.bin" << "*.pg";

    QStringList strList;
    if(BOARD_LINUX == nIndex){
        strList << "*356X*.bin";
    }else if(BOARD_SOC == nIndex){
        strList << "*AI*.bin";
    }else if(BOARD_FRONT == nIndex || BOARD_FRONT2 == nIndex
            || BOARD_REAR == nIndex||BOARD_REAR2 == nIndex
            ||BOARD_CAM_EXTRA1 == nIndex||BOARD_CAM_EXTRA2 == nIndex
            || BOARD_CONTROL == nIndex || BOARD_VALVE == nIndex){
        if((nIndex < BOARD_CONTROL) &&
            (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nIndex-1] == SENSOR_IM01 ||
             struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nIndex-1] == SENSOR_IM04)){
            strList <<"*.rpd";
        }else{
            strList << "*.pg";
        }
    }else if(BOARD_SOC_MODEL == nIndex){
        strList << "*.zip";
    }

    QString sPath;
    if(upgrade_type == UPGRADE_UDISK) {
        sPath = MOUNT_UDISK_PATH;
    } else if(upgrade_type == UPGRADE_SSD) {
        sPath = "/ssd";
    } else {
        sPath = ".";
    }

    QDir dir(sPath);
    if(dir.entryList(strList).isEmpty() == true) {
        if(upgrade_type == UPGRADE_UDISK)
            gFlow.umountUDisk(MOUNT_UDISK_PATH);
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_invalid_file));
        infoWidget->showSecs();
        return ;
    }

    SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget);
    dialog->setDir(sPath, strList);
    if (dialog->exec() == QDialog::Accepted) {
        QString sFileName = dialog->getFileName();
        m_boardFile[nIndex]->setText(sFileName);

        //! 若升级文件更换则清空升级状态
        if (m_struInfo[nIndex].sFileName != sFileName) {
            clearUpgradeStatus(nIndex);
            m_struInfo[nIndex].sFileName = sFileName;
        }

        //! 根据升级文件大小确定包长
        QFile file(QString("%1%2").arg(sPath).arg(m_struInfo[nIndex].sFileName));
        if (!file.open(QIODevice::ReadOnly)) {
            if(upgrade_type == UPGRADE_UDISK)
                gFlow.umountUDisk();
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.open_file_failed));
            infoWidget->showSecs();
            return;
        }

        if (nIndex != BOARD_LINUX) {

            //! 读取升级文件
            m_struInfo[nIndex].fileBuf = file.readAll();

            if (nIndex == BOARD_SOC || nIndex == BOARD_SOC_MODEL) {
                m_struInfo[nIndex].nWriteLen = file.size();
            } else {
                if (file.size() >= FILE_SIZE_8M) {
                    m_struInfo[nIndex].nWriteLen = PACKET_COUNT_BIG*2;
                } else if (file.size() == FILE_SIZE_4M) {
                    m_struInfo[nIndex].nWriteLen = PACKET_COUNT_BIG;
                } else if (file.size() == FILE_SIZE_2M) {
                    m_struInfo[nIndex].nWriteLen = PACKET_COUNT;
                } else if (file.size() == FILE_SIZE_1M) {
                    m_struInfo[nIndex].nWriteLen = PACKET_COUNT_1M;
                } else if (file.size() == FILE_SIZE_512K) {
                    m_struInfo[nIndex].nWriteLen = PACKET_COUNT_SMALL;
                } else if (file.size() == FILE_SIZE_128K) {
                    m_struInfo[nIndex].nWriteLen = PACKET_COUNT_TINY;
                } else {
                    m_struInfo[nIndex].nWriteLen = 0;
                }
            }
        }

        file.close();
    }

    if(upgrade_type == UPGRADE_UDISK)
        gFlow.umountUDisk();
}

/*!
 * \brief 刷新模型升级序号
 * \param nIdx
 */
void upgradeFPGAwidget::onModelIdxChgedSlt(int nIdx)
{
    m_socmodelIdx = nIdx;
}

void upgradeFPGAwidget::onClearParamsChgSlt(bool bCheck)
{
    m_bClear = bCheck;
}

/**
 * @brief upgradeFPGAwidget::清空对应下位机的升级状态
 * @param nType
 */
void upgradeFPGAwidget::clearUpgradeStatus(int nType)
{
    for (int i = 0; i < MAX_ANALYZE_NUM; i++) {
        m_struInfo[nType].nStatus[i] = UPGRADE_UNSTARTED;
    }
}

/*!
 * \brief 校验模型序号，避免跨序号升级模型，导致序号对不齐
 */
bool upgradeFPGAwidget::checkUpgradeModelIdx(int nIdx)
{
    bool bOk = true;
    int ncurID = m_boardNum[BOARD_SOC_MODEL]->currentIndex();
    if(ncurID == 0){
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            if(gFlow.bIsSupportAI(i)){
                if(nIdx > struGsh.nAIModel[i]){
                    bOk = false;
                    break;
                }
            }
        }
    }else{
        if(nIdx > struGsh.nAIModel[ncurID - 1 < 0 ? 0 : ncurID - 1]){
            bOk = false;
        }
    }

    return bOk;
}

/*!
 * \brief 刷新模型升级下表
 * \param nCamid
 * \return
 */
void upgradeFPGAwidget::refreshModelindexComb()
{

    m_socmodelIdxcmb->clear();

    int nidx = m_boardNum[BOARD_SOC_MODEL]->currentIndex();
    if(nidx == -1){
        return;
    }

    int nCamIdx = 0;
    if(nidx>0){
        nCamIdx = nidx - 1;
    }

    disconnect(m_socmodelIdxcmb,SIGNAL(currentIndexChanged(int)),this,SLOT(onModelIdxChgedSlt(int)));

    int nItem =0;
    for(int j=0; j<20; j++){
        m_socmodelIdxcmb->insertItem(nItem++,QString("%1  %2").arg(j)
                                                  .arg(QString(struGsh.sModelUnitInfo[nCamIdx].model[j].cnf)));
    }
    connect(m_socmodelIdxcmb,SIGNAL(currentIndexChanged(int)),this,SLOT(onModelIdxChgedSlt(int)));

    m_socmodelIdxcmb->setCurrentIndex(0);
    m_socmodelIdx = 0;
}

/**
 * @brief upgradeFPGAwidget::上位机升级定时器
 */
void upgradeFPGAwidget::upgradeTimeOutSlt()
{
    int percent = 0;
    int fileSize = 0;

    QString sFileName = QString("%1%2").arg(MEYER_BIN_DIR).arg(FILE_APP_BIN);
    QFile file(sFileName);
    system("sync");
    fileSize = file.size();

    QStandardItemModel *model = new QStandardItemModel(0, 2, this);
    int nModelRow = 0;
    if(m_struInfo[BOARD_LINUX].nWriteLen != 0) {
        percent = (fileSize*100.0) / m_struInfo[BOARD_LINUX].nWriteLen;
        model->insertRow(nModelRow);
        model->setData(model->index(nModelRow, 0), QString("%1").arg(myLan.upgrade));
        model->setData(model->index(nModelRow, 1), QString("%1%").arg(percent));

        model->item(nModelRow,0)->setTextAlignment(Qt::AlignCenter);
        model->item(nModelRow,1)->setTextAlignment(Qt::AlignCenter);
    }

    bool bReboot = false;
    if(fileSize == m_struInfo[BOARD_LINUX].nWriteLen) {
        upgradeTimer->stop();
        QString chmodCmd = QString("chmod +x %1").arg(sFileName);
        //printf("%s\n", qPrintable(chmodCmd));
        system(qPrintable(chmodCmd));
        fflush(0);
        system("sync");
        model->setData(model->index(nModelRow, 0), QString("%1").arg(myLan.upgrade_sucess));

        bReboot = true;
    }
    m_sourceView->setModel(model);
    m_sourceView->setColumnWidth(0, BTN_WID+120);
    m_sourceView->setColumnWidth(1, BTN_WID+150);

    if(bReboot){
        gFlow.msleep(3000);
        system("reboot");
    }
}

/**
 * @brief upgradeFPGAwidget::下发并烧写定时器
 */
void upgradeFPGAwidget::burnTimeOutSlt()
{
    for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
        int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
        if (checkBoardStatus(curBoardMode, nIndex)){
            m_struInfo[curBoardMode].sUpgradeInfo[nIndex] = getTextUpgradeInfo(curBoardMode, step);
        }
    }

    refreshCurrentDisplay(curBoardMode);
}

/**
 * @brief upgradeFPGAwidget::开始升级流程
 */
void upgradeFPGAwidget::startUpgradeProcess()
{
    //! 清空提示字符
    curCamId = "";

    //! 开始升级流程
    if(!checkUpgradeBoardEn()) {
        enableBtn(true);
        return;
    }

    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_selBtns[i]->setEnabled(false);
        m_boardNum[i]->setEnabled(false);
        m_boardFile[i]->setEnabled(false);
    }

    if (m_struInfo[BOARD_LINUX].bEn) {
        if (!gFlow.mountUDisk()) {
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1%!").arg(myLan.no_find_udisk));
            infoWidget->showSecs();
            enableBtn(true);
            return;
        }
        if(m_bClear){//! 清除参数后再执行升级
            QString bakCmd = QString("rm -rf %1*").arg("/userdata/meyer/cnf/");
            system(qPrintable(bakCmd));
            gFlow.sleep(2);
        }
        upgradeLinux(MOUNT_UDISK_PATH, m_struInfo[BOARD_LINUX].sFileName);
    } else {//! 根据使能升级对应下位机
        if (m_struInfo[BOARD_CONTROL].bEn) {
            curBoardMode = BOARD_CONTROL;
            upgradeCtrlBoard();
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_VALVE].bEn) {
            curBoardMode = BOARD_VALVE;
            upgradeValveBoard();
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_FRONT].bEn) {
            curBoardMode = BOARD_FRONT;
            upgradeCamBoard(BOARD_FRONT);
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_FRONT2].bEn) {
            curBoardMode = BOARD_FRONT2;
            upgradeCamBoard(BOARD_FRONT2);
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_REAR].bEn) {
            curBoardMode = BOARD_REAR;
            upgradeCamBoard(BOARD_REAR);
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_REAR2].bEn) {
            curBoardMode = BOARD_REAR2;
            upgradeCamBoard(BOARD_REAR2);
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_CAM_EXTRA1].bEn) {
            curBoardMode = BOARD_CAM_EXTRA1;
            upgradeCamBoard(BOARD_CAM_EXTRA1);
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_CAM_EXTRA2].bEn) {
            curBoardMode = BOARD_CAM_EXTRA2;
            upgradeCamBoard(BOARD_CAM_EXTRA2);
            gFlow.sleep(1);
        }

        if (m_struInfo[BOARD_SOC].bEn){
            if(bSocAllUpd[0]){//! 全部升级
                SocUpgdStamap.clear();
                SocUpgdInfomap.clear();
                for(int i=0; i<gFlow.getMachineCamNum(); i++){
                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist && gFlow.bIsIMSerialCam(i)){//! 辅配相机无需升级AI
                        continue;
                    }
                    curBoardMode = BOARD_SOC;
                    m_struInfo[BOARD_SOC].nBoardUnit = i + 1;
                    curCamId = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i];
                    upgradeAIBoard(curBoardMode, m_struInfo[BOARD_SOC].nBoardUnit);
                    if(checkBoardStatus(BOARD_SOC, struGsh.nAnalyzeIndex)) {
                        m_struInfo[BOARD_SOC].nStatus[struGsh.nAnalyzeIndex] = UPGRADE_OK;
                        m_struInfo[BOARD_SOC].sUpgradeInfo[struGsh.nAnalyzeIndex] = "";
                    }
                    SocUpgdStamap[i]= getTextByStatus(BOARD_SOC, m_struInfo[BOARD_SOC].nStatus[struGsh.nAnalyzeIndex], struGsh.nAnalyzeIndex);
                    SocUpgdInfomap[i] =  m_struInfo[BOARD_SOC].sUpgradeInfo[struGsh.nAnalyzeIndex]; //! 获取升级信息
                    gFlow.sleep(1);
                }
            }else{
                curBoardMode = BOARD_SOC;
                if(gFlow.bIsIMSerialCam(m_struInfo[BOARD_SOC].nBoardUnit -1) && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
                    return;
                }
                upgradeAIBoard(curBoardMode, m_struInfo[BOARD_SOC].nBoardUnit);
                gFlow.sleep(1);
            }
        }

        if (m_struInfo[BOARD_SOC_MODEL].bEn) {
            if(!checkUpgradeModelIdx(m_socmodelIdxcmb->currentIndex())){
                infoWidget->raise();
                infoWidget->setLabelText(myLan.forbid_cross_version);
                infoWidget->showSecs();
                return;
            }
            if(bSocAllUpd[1]){
                SocModelUpgdStamap.clear();
                SocModelUpgdInfomap.clear();
                for(int i=0; i<gFlow.getMachineCamNum(); i++){
                    curBoardMode = BOARD_SOC_MODEL;
                    m_struInfo[BOARD_SOC_MODEL].nBoardUnit = i+1;
                    curCamId = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i];
                    upgradeAIBoard(curBoardMode, m_struInfo[BOARD_SOC_MODEL].nBoardUnit);
                    if(checkBoardStatus(BOARD_SOC_MODEL, struGsh.nAnalyzeIndex)) {
                        m_struInfo[BOARD_SOC_MODEL].nStatus[struGsh.nAnalyzeIndex] = UPGRADE_OK;
                        m_struInfo[BOARD_SOC_MODEL].sUpgradeInfo[struGsh.nAnalyzeIndex] = "";
                    }
                    SocModelUpgdStamap[i]= getTextByStatus(BOARD_SOC_MODEL, m_struInfo[BOARD_SOC_MODEL].nStatus[struGsh.nAnalyzeIndex], struGsh.nAnalyzeIndex);
                    SocModelUpgdInfomap[i] =  m_struInfo[BOARD_SOC_MODEL].sUpgradeInfo[struGsh.nAnalyzeIndex]; //! 获取升级信息
                    gFlow.sleep(1);
                }
            }else{
                curBoardMode = BOARD_SOC_MODEL;
                upgradeAIBoard(curBoardMode, m_struInfo[BOARD_SOC_MODEL].nBoardUnit);
                gFlow.sleep(1);
            }
        }

        getResults();

        enableBtn(true);
    }
}

/*!
 * \brief 获取升级结果
 */
bool upgradeFPGAwidget::getResults()
{
    for (int i = 0; i < BOARD_TOTAL; i++) {
        if (m_struInfo[i].bEn) {
            for (int j = 0; j < m_struInfo[i].qvControlUnit.size(); j++) {
                int nIndex = m_struInfo[i].qvControlUnit.at(j);
                if(checkBoardStatus(i, nIndex)) {
                    m_struInfo[i].nStatus[nIndex] = UPGRADE_OK;
                    m_struInfo[i].sUpgradeInfo[nIndex] = "";
                    if(i == BOARD_SOC_MODEL){//! 模型升级成功后查询模型及标签信息
                        for(int i=0; i<gFlow.getMachineCamNum(); i++){
                            if(gFlow.bIsSupportAI(i)){
                                gFlow.reqSocModelinfo(struGsh.nAnalyzeIndex,i);
                                gFlow.msleep(5);
                            }
                        }

                        for(int i=0; i<gFlow.getMachineCamNum(); i++){
                            if(gFlow.bIsSupportAI(i)){
                                gFlow.reqSocModelLabelinfo(struGsh.nAnalyzeIndex,i);
                                gFlow.msleep(5);
                            }
                        }
                    }
                }
            }
        }
    }

    refreshResultDisplay();

    return true;
}

/*!
 * \brief 刷新升级完成后总的结果显示
 */
void upgradeFPGAwidget::refreshResultDisplay()
{
    QStandardItemModel *model = new QStandardItemModel(0, 3, this);
    int nModelRow = 0;

    QString boardType = "";
    for (int i = 0; i < BOARD_TOTAL; i++) {
        int nType = i;
        if (m_struInfo[i].bEn) {
            boardType = m_strList[nType];
            for(int j = 0; j < m_struInfo[nType].qvControlUnit.size(); j++) {
                int nUnit = m_struInfo[nType].qvControlUnit.at(j);
                if((i!= BOARD_SOC) && (i!= BOARD_SOC_MODEL)){
                    model->insertRow(nModelRow);
                    model->setData(model->index(nModelRow, 0),boardType);
                }
//              model->setData(model->index(nModelRow, 0), QString("%1(分析仪%2)").arg(boardType).arg(nUnit+1));

                if(i == BOARD_SOC){
                    if(bSocAllUpd[0]){
                        QString strname,strstaus,strinfo;
                        QList<int> lst = SocUpgdStamap.keys();
                        for(int n: lst){
                            strname = QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[n]);
                            strstaus = SocUpgdStamap.value(n);
                            strinfo =  SocUpgdInfomap.value(n);

                            model->insertRow(nModelRow);
                            model->setData(model->index(nModelRow, 0), QString("%1(%2)").arg(boardType).arg(strname));
                            model->setData(model->index(nModelRow, 1), strstaus);
                            model->setData(model->index(nModelRow, 2), strinfo);
                            nModelRow++;
                        }
                        nModelRow--;
                    }else{
                        model->insertRow(nModelRow);
                        model->setData(model->index(nModelRow, 0),boardType);
                        model->setData(model->index(nModelRow, 1), getTextByStatus(nType, m_struInfo[nType].nStatus[nUnit], nUnit));
                        model->setData(model->index(nModelRow, 2), m_struInfo[nType].sUpgradeInfo[nUnit]);
                    }
                }else if(i ==BOARD_SOC_MODEL){
                    if(bSocAllUpd[1]){
                        QString strname,strstausmodel,strinfomodel;
                        for(int n=0; n<SocModelUpgdStamap.size(); n++){
                            strname = QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[n]);
                            strstausmodel = SocModelUpgdStamap.value(n);
                            strinfomodel = SocModelUpgdInfomap.value(n);


                            model->insertRow(nModelRow);
                            model->setData(model->index(nModelRow, 0), QString("%1(%2)").arg(boardType).arg(strname));
                            model->setData(model->index(nModelRow, 1), strstausmodel);
                            model->setData(model->index(nModelRow, 2), strinfomodel);
                            nModelRow++;
                        }
                        nModelRow--;
                    }else{
                        model->insertRow(nModelRow);
                        model->setData(model->index(nModelRow, 0),boardType);
                        model->setData(model->index(nModelRow, 1), getTextByStatus(nType, m_struInfo[nType].nStatus[nUnit], nUnit));
                        model->setData(model->index(nModelRow, 2), m_struInfo[nType].sUpgradeInfo[nUnit]);
                    }
                }else{
                    model->setData(model->index(nModelRow, 1), getTextByStatus(nType, m_struInfo[nType].nStatus[nUnit], nUnit));
                    model->setData(model->index(nModelRow, 2), m_struInfo[nType].sUpgradeInfo[nUnit]);
                }

//                model->item(nModelRow,0)->setTextAlignment(Qt::AlignCenter);
//                model->item(nModelRow,1)->setTextAlignment(Qt::AlignCenter);
//                model->item(nModelRow,2)->setTextAlignment(Qt::AlignCenter);
                nModelRow++;
            }
        }
    }

    m_sourceView->setModel(model);
    m_sourceView->setColumnWidth(0, BTN_WID+120);
    m_sourceView->setColumnWidth(1, BTN_WID+150);
    m_sourceView->setColumnWidth(2, BTN_WID-30);
}

void upgradeFPGAwidget::enableBtn(bool bEnable)
{
    for (int i = 0; i < BOARD_TOTAL; i++) {
        m_selBtns[i]->setEnabled(bEnable);
        m_boardNum[i]->setEnabled(bEnable);
        m_boardFile[i]->setEnabled(bEnable);
    }
}

bool upgradeFPGAwidget::getModuleType(int nBoardType, char &module_type, int &nUnit)
{
    bool bRet = true;
    switch(nBoardType){
    case BOARD_CONTROL:
        module_type = MODULE_CTRL;
        nUnit = 0;
        break;
    case BOARD_VALVE:
        module_type = MODULE_VAL;
        nUnit = 0;
        break;
    case BOARD_FRONT:
        module_type = MODULE_CAM;
        nUnit = 0;
        break;
    case BOARD_FRONT2:
        module_type = MODULE_CAM;
        nUnit = 1;
        break;
    case BOARD_REAR:
        module_type = MODULE_CAM;
        nUnit = 2;
        break;
    case BOARD_REAR2:
        module_type = MODULE_CAM;
        nUnit = 3;
        break;
    case BOARD_CAM_EXTRA1:
        module_type = MODULE_CAM;
        nUnit = 4;
        break;
    case BOARD_CAM_EXTRA2:
        module_type = MODULE_CAM;
        nUnit = 5;
        break;
    case BOARD_SOC:
    case BOARD_SOC_MODEL:
        module_type = MODULE_SOC;
        nUnit = m_struInfo[curBoardMode].nBoardUnit - 1;
        break;
    default:
        bRet = false;
        break;
    }

    return bRet;
}

void upgradeFPGAwidget::switchMode(BOARD_MODE mode)
{
    char module_type = 0;
    int nUnit = 0;
    int cmd = MODE_FACTORY == mode ? CMD_UPDATE_FACTORY : CMD_UPDATE_USER;
    char data = 1;

    QVector<int> vctModel = {BOARD_CONTROL, BOARD_VALVE, BOARD_FRONT, BOARD_REAR,BOARD_FRONT2,
                             BOARD_REAR2,  BOARD_CAM_EXTRA1, BOARD_CAM_EXTRA2};

    bool bRet = false;
    for(int i = 0; i < vctModel.size(); i++){
        int curBoard = vctModel[i];

        bRet = getModuleType(curBoard, module_type, nUnit);

        if(bRet && m_struInfo[curBoard].bEn){
            for (int i = 0; i < m_struInfo[curBoard].qvControlUnit.size(); i++) {
                int nIndex = m_struInfo[curBoard].qvControlUnit.at(i);
                gFlow.sendUpgradeInfo(nIndex, cmd, module_type, nUnit, &data, 1);
            }
        }
    }
}

/**
 * @brief upgradeFPGAwidget::根据状态获取文本
 * @param nType
 * @param nStat
 * @param nBoard
 * @return
 */
QString upgradeFPGAwidget::getTextByStatus(int nType, int nStat, int nBoard)
{
    int step_sum;
    switch(nType) {
    case BOARD_CONTROL:
        step_sum = 4;
        break;
    case BOARD_FRONT:
    case BOARD_REAR:
    case BOARD_FRONT2:
    case BOARD_REAR2:
    case BOARD_CAM_EXTRA1:
    case BOARD_CAM_EXTRA2:
    case BOARD_VALVE:
        step_sum = 6;
        break;
    case BOARD_SOC:
    case BOARD_SOC_MODEL:
        step_sum = 5;
        break;
    default:
        break;
    }

    QString tmp = QString("%1").arg(myLan.download_file_failed);
    if(BOARD_SOC == nType || BOARD_SOC_MODEL == nType){
        auto m_pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(nBoard);
        switch(m_pAnalyzeShare->bAIUpdStatus)
        {
        case UPGRADE_NULL:
            tmp = QString("%1").arg(myLan.reset_upgrade_status);
            break;
        case UPGRADE_OK:
            tmp = myLan.sucess;
            break;
        case UPGRADE_FILE_REPLACE_FAILED:
            tmp = QString("%1").arg(myLan.burn_file_failed);
            break;
        case UPGRADE_FILE_LACK:
            tmp = QString("%1").arg(myLan.loss_file);
            break;
        case UPGRADE_FILE_VERTIFY_FAILED:
            tmp =  QString("%1").arg(myLan.check_file_failed);
            break;
        default:
            tmp = QString("%1").arg(myLan.download_file_failed);
            break;
        }
    }

    QString str;
    switch (nStat) {
    case UPGRADE_UNSTARTED:
        str = QString("%1!").arg(myLan.no_start_upgrade);
        break;
    case UPGRADE_DOING:
        str = myLan.upgrade+QString("...(%1/%2)").arg(step+1).arg(step_sum);
        break;
    case UPGRADE_ERR_FACTORY:
        str = QString("%1").arg(myLan.switch_factory_failed)
              + QString::number(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_SEND:
        str = tmp+QString::number(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_BURN:
        str = QString("%1").arg(myLan.burn_file_failed) + QString::number(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_USER:
        str = QString("%1").arg(myLan.switch_user_failed)
              + QString::number(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_ERR_ERASE:
        str = QString("%1").arg(myLan.erase_failed) + QString::number(m_struInfo[nType].nstep[nBoard]);
        break;
    case UPGRADE_OK:
        str = QString("%1").arg(myLan.sucess);
        break;
    default:
        break;
    }

    return str;
}

/**
 * @brief upgradeFPGAwidget::根据状态获取文本
 * @param nType
 * @param step
 * @return
 */
QString upgradeFPGAwidget::getTextUpgradeInfo(int nType, int step)
{
    QString str;
    switch(nType) {
    case BOARD_CONTROL:
        str = CtrlStepInfoList.at(step);
        break;
    case BOARD_FRONT:
    case BOARD_REAR:
    case BOARD_FRONT2:
    case BOARD_REAR2:
    case BOARD_CAM_EXTRA1:
    case BOARD_CAM_EXTRA2:
        str = CamStepInfoList.at(step);
        break;
    case BOARD_VALVE:
        str = EjectorStepInfoList.at(step);
        break;
    case BOARD_SOC:
    case BOARD_SOC_MODEL:
        str = SocStepInfoList.at(step);
        break;
    }
    if((nType == BOARD_CONTROL && step == 2) ||
        ((nType == BOARD_FRONT || nType == BOARD_REAR || nType == BOARD_FRONT2 ||
          nType == BOARD_REAR2 || nType == BOARD_CAM_EXTRA1 || nType == BOARD_CAM_EXTRA2 ||
          nType == BOARD_VALVE) && step == 3))
    {
        str += QString("%1%").arg(nBurnPages*400/m_struInfo[nType].nWriteLen);
    }

    if ((nType == BOARD_SOC || nType == BOARD_SOC_MODEL) && step == 2) {
        int nPacketNum = qCeil(m_struInfo[nType].nWriteLen*1.0 / LEN_GENERAL_FILE_PACKET_DATA ) + 1;
        str += QString("%1%").arg(nBurnPages*100/nPacketNum);
    }
    return str;
}

/**
 * @brief upgradeFPGAwidget::刷新升级板的初始化状态
 */
void upgradeFPGAwidget::refreshInitDisplay()
{
    nBurnPages = 0;

    int nBoardTotal = m_struInfo[curBoardMode].nBoardTotal;
    int nIndex = m_struInfo[curBoardMode].nBoard;
    QString info;
    switch (curBoardMode) {
    case BOARD_CONTROL:
        info = CtrlStepInfoList[step];
        break;
    case BOARD_VALVE:
        info = EjectorStepInfoList[step];
        break;
    case BOARD_FRONT:
    case BOARD_REAR:
    case BOARD_FRONT2:
    case BOARD_REAR2:
    case BOARD_CAM_EXTRA1:
    case BOARD_CAM_EXTRA2:
        info = CamStepInfoList[step];
        break;
    case BOARD_SOC:
    case BOARD_SOC_MODEL:
        info = SocStepInfoList[step];
        break;
    default:
        break;
    }

    for(int i=0; i<BOARD_TOTAL; i++){
        for (int j = 0; j < nBoardTotal; j++) {
            m_struInfo[i].nStatus[j] = UPGRADE_DOING;
            m_struInfo[i].sUpgradeInfo[j] = info;
        }
    }

    refreshCurrentDisplay(curBoardMode);
}

/**
 * @brief upgradeFPGAwidget::刷新显示当前升级板的升级信息
 * @param nType
 * @return
 */
void upgradeFPGAwidget::refreshCurrentDisplay(int nType)
{
    int nModelRow = 0;
    QStandardItemModel *model = new QStandardItemModel(0, 3, this);

    QString boardType = m_strList[nType];

    for(int i = 0; i < m_struInfo[nType].qvControlUnit.size(); i++) {
        int nUnit = m_struInfo[nType].qvControlUnit.at(i);

        model->insertRow(nModelRow);
        model->setData(model->index(nModelRow, 0), curCamId.isEmpty()?boardType : QString("%1(%2)").arg(boardType).arg(curCamId));
        model->setData(model->index(nModelRow, 1), getTextByStatus(nType, m_struInfo[nType].nStatus[nUnit], nUnit));
        model->setData(model->index(nModelRow, 2), m_struInfo[nType].sUpgradeInfo[nUnit]);

        model->item(nModelRow,0)->setTextAlignment(Qt::AlignCenter);
        model->item(nModelRow,1)->setTextAlignment(Qt::AlignCenter);
        model->item(nModelRow,2)->setTextAlignment(Qt::AlignCenter);
        nModelRow++;
    }

    m_sourceView->setModel(model);
    m_sourceView->setColumnWidth(0, BTN_WID+120);
    m_sourceView->setColumnWidth(1, BTN_WID+150);
    m_sourceView->setColumnWidth(2, BTN_WID-30);

}

/**
 * @brief upgradeFPGAwidget::检查板级升级状态
 * @param nBoardType
 * @param nStatType
 * @return
 */
bool upgradeFPGAwidget::checkStatus(int nBoardType, int nStatType)
{
    bool bStat = false;

    char module_type = 0;
    int nUnit = 0;
    getModuleType(nBoardType, module_type, nUnit);

    for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
        int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
        if (!checkBoardStatus(curBoardMode, nIndex)) {
            continue;
        }

        if(!checkUpdateStatus(nIndex, nBoardType, nUnit, nStatType)) { //! 发送1001命令，查询状态
            m_struInfo[curBoardMode].nStatus[nIndex] = nStatType+1;
            m_struInfo[curBoardMode].nstep[nIndex] = step;
        } else {
            bStat = true;
            if((curBoardMode == BOARD_CONTROL && step < 4) ||
                 ((curBoardMode == BOARD_FRONT || curBoardMode == BOARD_REAR ||
                 curBoardMode == BOARD_FRONT2 || curBoardMode == BOARD_REAR2 ||
                 curBoardMode == BOARD_CAM_EXTRA1  || curBoardMode == BOARD_CAM_EXTRA2 ||
                 curBoardMode == BOARD_VALVE)&& step < 6) ||
                 ((curBoardMode == BOARD_SOC || curBoardMode == BOARD_SOC_MODEL)&& step < 5))
            {
                m_struInfo[curBoardMode].sUpgradeInfo[nIndex] = getTextUpgradeInfo(curBoardMode, m_bchgstep?step-1:step);
            }
        }
    }

    if ((curBoardMode == BOARD_CONTROL && step < 4)
            || ((curBoardMode == BOARD_FRONT || curBoardMode == BOARD_REAR
            || curBoardMode == BOARD_FRONT2 || curBoardMode == BOARD_REAR2
            || curBoardMode == BOARD_CAM_EXTRA1  || curBoardMode == BOARD_CAM_EXTRA1 || curBoardMode == BOARD_VALVE)&& step < 6)
            || ((curBoardMode == BOARD_SOC || curBoardMode == BOARD_SOC_MODEL)&& step < 5)){
        refreshCurrentDisplay(curBoardMode);
    }

    return bStat;
}

/**
 * @brief upgradeFPGAwidget::检查对应下位机的升级状态
 * @param nType
 * @param nUnit
 * @return
 */
bool upgradeFPGAwidget::checkBoardStatus(int nType, int nUnit)
{
    int nStat = m_struInfo[nType].nStatus[nUnit];
    if (nStat == UPGRADE_ERR_FACTORY
            || nStat == UPGRADE_ERR_SEND
            || nStat == UPGRADE_ERR_BURN
            || nStat == UPGRADE_ERR_USER
            || nStat == UPGRADE_ERR_ERASE) {
        return false;
    }
    return true;
}

/**
 * @brief upgradeFPGAwidget::检查下位机状态
 * @param nBoardType
 * @param nUnit
 * @param nStatType
 * @return
 */
bool upgradeFPGAwidget::checkUpdateStatus(int nIndex, int nBoardType, int nUnit, int nStatType)
{
    int nTimes = 0;
    int moudel_type = 0;
    auto m_pAnalyzeShare = gFlow.getSpecifyAnalyzeShare(nIndex);
    char data = 0;

RECHECK:
    if (nBoardType == BOARD_CONTROL || nBoardType == BOARD_VALVE) {
        int nwaittime = nBoardType == BOARD_VALVE ? 2000 : 20;
        m_pAnalyzeShare->nUpdateType = nStatType;
        m_pAnalyzeShare->nUpgradeStatusInfo = false;
        moudel_type = nBoardType == BOARD_VALVE ? MODULE_VAL : MODULE_CTRL;
        gFlow.sendUpgradeInfo(nIndex, CMD_CHECK_MODE, moudel_type, nUnit, &data, 1);
        gFlow.msleep(nwaittime);
        if(m_pAnalyzeShare->nUpgradeStatusInfo) {
            return true;
        }
    }

    if (nBoardType == BOARD_FRONT || nBoardType == BOARD_REAR
            || nBoardType == BOARD_FRONT2 || nBoardType == BOARD_REAR2
            ||nBoardType == BOARD_CAM_EXTRA1 || nBoardType == BOARD_CAM_EXTRA2) {
        m_pAnalyzeShare->nUpdateType = nStatType;
        m_pAnalyzeShare->nUpgradeStatusInfo = false;
        moudel_type = MODULE_CAM;
        gFlow.sendUpgradeInfo(nIndex, CMD_CHECK_MODE, moudel_type, nUnit, &data, 1);
        gFlow.msleep(20);
        if(m_pAnalyzeShare->nUpgradeStatusInfo) {
            return true;
        }
    }

    if (nBoardType == BOARD_SOC || nBoardType == BOARD_SOC_MODEL) {
        m_pAnalyzeShare->bAIUpdStatus = -1;
        moudel_type = MODULE_SOC;
        data = 1;
        gFlow.sendUpgradeInfo(nIndex, CMD_CHECK_MODE, moudel_type, nUnit, &data, 1);
        gFlow.msleep(50);

        if(m_pAnalyzeShare->bAIUpdStatus == UPGRADE_SOC_OK) {
            return true;
        }
    }

    if (nTimes++ < 5) {
        gFlow.sleep(1);
        goto RECHECK;
    }

    return false;
}

/**
 * @brief upgradeFPGAwidget::检查要升级板级使能
 */
bool upgradeFPGAwidget::checkUpgradeBoardEn()
{
    bool stat = false;
    for (int i = 0; i < BOARD_TOTAL; i++) {
        if(m_struInfo[i].bEn) {
            stat = true;
            break;
        }
    }
    if (!stat) {
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_upgrade_board));
        infoWidget->showSecs();
        return false;
    }

    for (int i = 0; i < BOARD_TOTAL; i++) {
        if (m_struInfo[i].bEn && checkFileIsInValid(i)) {
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.file_type_error));
            infoWidget->showSecs();
            return false;
        }
    }

    return true;
}

/**
 * @brief upgradeFPGAwidget::检查升级文件是否合理
 * @param index
 * @return
 */
bool upgradeFPGAwidget::checkFileIsInValid(int index)
{
    if (index == BOARD_LINUX) {
        if(m_struInfo[index].sFileName.isEmpty() || !m_struInfo[index].sFileName.endsWith(".bin")){
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.file_type_error));
            infoWidget->showSecs();
            return true;
        }
        return false;
    } else {
        //! 文件长度为0
        if(m_struInfo[index].nWriteLen == 0)
            return true;

        if(m_struInfo[index].sFileName.isEmpty()){
           return true;
        }
        //! U盘升级不校验文件名
        if(upgrade_type == UPGRADE_UDISK)
            return false;

//        switch (index) {
//        case BOARD_FRONT:
//        case BOARD_REAR:
//            if (m_struInfo[index].sFileName.contains("")) {

//            }
//            break;
//        default:
//            break;
//        }
        return false;
    }

}

/**
 * @brief upgradeFPGAwidget::升级边缘服务器程序
 */
void upgradeFPGAwidget::upgradeLinux(QString dir, QString file_name)
{
    QFile file(QString("%1%2").arg(dir).arg(file_name));

    m_struInfo[BOARD_LINUX].nWriteLen = file.size();
    QString bakCmd = QString("mv -f %1%2 %3%4").arg(MEYER_BIN_DIR).arg(FILE_APP_BIN).arg(MEYER_BIN_DIR).arg(FILE_APP_BIN_BAK);
    system(qPrintable(bakCmd));
    gFlow.sleep(2);

    m_copyFilethread->setAddr(dir, file_name);
    m_copyFilethread->setDestAddr(MEYER_BIN_DIR,FILE_APP_BIN);
    m_copyFilethread->start();
    upgradeTimer->start(1000);
}

/**
 * @brief upgradeFPGAwidget::控制板升级
 */
bool upgradeFPGAwidget::upgradeCtrlBoard()
{
    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1.切换控制板至工厂模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_CONTROL)) {
        return false;
    }

    //! 2.擦除控制板
    step = 2;
    if(!ctrlEraseFlash()) {
        return false;
    }

    //! 3.下发并烧写升级文件
    step = 3;
    if(!ctrlSendFileByNet()) {
        return false;
    }

    //! 4.切换控制板至用户模式
    if (!m_struInfo[BOARD_FRONT].bEn && !m_struInfo[BOARD_REAR].bEn
            && !m_struInfo[BOARD_FRONT2].bEn && !m_struInfo[BOARD_REAR2].bEn) {
        step = 4;
        if(!stepSwitchToUser(BOARD_CONTROL)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief upgradeFPGAwidget::微阀板升级
 */
bool upgradeFPGAwidget::upgradeValveBoard()
{
    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1、切换控制板至工厂模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_CONTROL)) {
        return false;
    }

    //! 2、切换微阀板至工厂模式
    step = 2;
    if(!stepSwitchToFactory(BOARD_VALVE)) {
        return false;
    }

    //! 3、下载文件到微阀板
    step = 3;
    if(!ctrlEraseFlash()) {
        return false;
    }

    //! 4、烧写文件到flash
    step = 4;
    if(!ctrlSendFileByNet()) {
        return false;
    }

    //! 5、微阀板切回用户模式
    step = 5;
    if(!stepSwitchToUser(BOARD_VALVE)) {
        return false;
    }

    //! 6、切换控制板至用户模式
    step = 6;
    if(!stepSwitchToUser(BOARD_CONTROL)) {
        return false;
    }

    return true;
}

/**
 * @brief upgradeFPGAwidget::相机板升级
 * @return
 */
bool upgradeFPGAwidget::upgradeCamBoard(int nType)
{
    m_bchgstep = false;

    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1、切换控制板至工厂模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_CONTROL)) {
        return false;
    }

    //! 2、切换相机板至工厂模式（辅配升级需要先将对应的主配切到工厂）
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist && gFlow.bIsIMSerialCam(nType-1)){
        curBoardMode = struGsh.nAssisttomainCamId + 1;
        if(!stepSwitchToFactory(struGsh.nAssisttomainCamId + 1)){
            m_struInfo[nType].nStatus[0] = m_struInfo[curBoardMode].nStatus[0];
            curBoardMode = nType;
            return false;
        }else{
            curBoardMode = nType;
        }
    }

    step = 2;
    if(!stepSwitchToFactory(nType)) {
        return false;
    }

    //! 3、下载文件到相机板
    step = 3;
    if(!ctrlEraseFlash()) {
        return false;
    }

    //! 4、烧写文件到flash
    step = 4;
    if(!ctrlSendFileByNet()) {
        return false;
    }

    //! 5、相机板切回用户模式
    step = 5;
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist && gFlow.bIsIMSerialCam(nType-1)){
        m_bchgstep = true;
    }
    if(!stepSwitchToUser(nType)) {
        return false;
    }
    m_bchgstep = false;

    //! 辅配关联的主配相机切用户
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist && gFlow.bIsIMSerialCam(nType-1)){
        curBoardMode = struGsh.nAssisttomainCamId + 1;
        if(!stepSwitchToUser(struGsh.nAssisttomainCamId+1)){
            m_struInfo[nType].nStatus[0] = m_struInfo[curBoardMode].nStatus[0];
            curBoardMode = nType;
            return false;
        }else{
            curBoardMode = nType;
        }
    }

    //! 6、切换控制板至用户模式
    step = 6;
    if(!stepSwitchToUser(BOARD_CONTROL)) {
        return false;
    }

    return true;
}

/**
 * @brief upgradeFPGAwidget::AI板升级
 * @param nType
 * @return
 */
bool upgradeFPGAwidget::upgradeAIBoard(int nType, int BoardUnit)
{
    nBurnPages = 0;
    //! 初始化升级的状态
    step = 0;
    refreshInitDisplay();

    //! 1、切换工厂模式
    step = 1;
    if(!stepSwitchToFactory(BOARD_CONTROL)){
        return false;
    }

    //! 辅配无SOC，需要将辅配地址改成相应的主配地址
    if(gFlow.bIsIMSerialCam(BoardUnit-1) && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist){
        BoardUnit = struGsh.nAssisttomainCamId + 1;
    }

    //! 2、切换相机板至工厂模式
    step = 2;
    if(!stepSwitchToFactory(BoardUnit)) {
        return false;
    }

    //! 下发传输方式类型  SG板子是USB
    setfacType(1, BoardUnit -1);
    gFlow.msleep(500);

    //! 3、下发升级文件
    step = 3;
    if(!ctrlSendFileByNet()) {
        return false;
    }

    //! 4、相机板切回用户模式
    step = 4;
    if(!stepSwitchToUser(BoardUnit)) {
        return false;
    }

    //! 5、切回用户模式
    step = 5;
    if(!stepSwitchToUser(BOARD_CONTROL)){
        return false;
    }

    return true;
}

/**
 * @brief upgradeFPGAwidget::1-切换至工厂模式
 * @param nType
 * @return
 */
bool upgradeFPGAwidget::stepSwitchToFactory(int nType)
{
    char module_type = 0;
    int nUnit = 0;

    bool bRet = getModuleType(nType, module_type, nUnit);
    if(bRet){
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            char data = 1;
            gFlow.sendUpgradeInfo(nIndex, CMD_UPDATE_FACTORY, module_type, nUnit, &data, 1);
        }
        gFlow.msleep(1500);

        if(!checkStatus(nType, UPGRADE_FACTORY)) {
            return false;
        }else
            return true;
    }

    return false;
}

/**
 * @brief upgradeFPGAwidget::2-擦除控制板的用户空间
 * @return
 */
bool upgradeFPGAwidget::ctrlEraseFlash()
{
    char module_type = 0;
    int nUnit = 0;

    bool bRet = getModuleType(curBoardMode, module_type, nUnit);
    if(bRet){
        int nCmd = CMD_START_BURN;
        char data = 1;

        auto &qvControlUnit = m_struInfo[curBoardMode].qvControlUnit;
        for (int i = 0; i < qvControlUnit.size(); i++) {
            int nIndex = qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;
            gFlow.sendUpgradeInfo(nIndex, nCmd, module_type, nUnit, &data, 1);
        }

        gFlow.sleep(15);

        if(!checkStatus(curBoardMode,UPGRADE_ERASE)) {
            return false;
        }else
            return true;
    }

    return false;
}

/**
 * @brief upgradeFPGAwidget::3-向控制板发送文件
 * @return
 */
bool upgradeFPGAwidget::ctrlSendFileByNet()
{
    bool ret = false;
    int writeLen = m_struInfo[curBoardMode].nWriteLen;
    char data[2];
    data[0] = writeLen/256;
    data[1] = writeLen%256;
    char module_type = 0;
    int module_unit = 0;
    int nwaittime = 2;
    nBurnPages = 0;
    --step;
    burnTimer->start(2000);
    if (curBoardMode == BOARD_CONTROL) {
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            gFlow.sendUpgradeInfo(nIndex, CMD_FILE_LEN, MODULE_CTRL, 0, data, 2);
        }

        gFlow.msleep(10);

        QVector<int> vctControl;
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;
            else
                vctControl.push_back(nIndex);
        }

        for(int packetId = 0; packetId < writeLen/4; packetId++) {
            nBurnPages = packetId;

            QMap<int, bool> mapRet;
            for(int m = 0; m < vctControl.size(); m++){
                int nIndex = vctControl[m];
                ret = true;
                for(int j = 0; j < 2; ++j) {
                    gFlow.sendFpgaUpgradeData(nIndex, MODULE_CTRL, 0, 0, packetId, m_struInfo[curBoardMode].fileBuf);
                    gFlow.msleep(20);
                    //! 判断页文件写入SDRAM是否成功
                    if (packetId < writeLen/4-1) {
                        ret = checkUpdateStatus(nIndex, curBoardMode, 0, UPGRADE_BURN);
                        if(ret)
                            break;
                    } else
                        break;
                }
                mapRet[nIndex] = ret;
            }

            QMapIterator<int, bool> inter(mapRet);
            while (inter.hasNext()) {
                inter.next();
                if(false == inter.value()){
                    vctControl.removeOne(inter.key());
                }
            }
        }

    } else if (curBoardMode == BOARD_FRONT || curBoardMode == BOARD_REAR || curBoardMode == BOARD_FRONT2
               || curBoardMode == BOARD_REAR2 || curBoardMode == BOARD_CAM_EXTRA1
               || curBoardMode == BOARD_CAM_EXTRA2 || curBoardMode == BOARD_VALVE) {
        if(curBoardMode == BOARD_VALVE){
            module_type = MODULE_VAL;
            module_unit = 0;
            nwaittime = 20;
        }else{
            module_type = MODULE_CAM;
            module_unit = m_struInfo[curBoardMode].nBoardUnit;
        }
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            gFlow.sendUpgradeInfo(nIndex, CMD_FILE_LEN, module_type, module_unit, data, 2);
        }

        gFlow.msleep(10);

        QVector<int> vctControl;
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;
            else
                vctControl.push_back(nIndex);
        }

        for(int packetId = 0; packetId < writeLen/4; packetId++){
            nBurnPages = packetId;

            for(int m = 0; m < vctControl.size(); m++){
                int nIndex = vctControl[m];

                gFlow.sendFpgaUpgradeData(nIndex, module_type, module_unit, 0, packetId, m_struInfo[curBoardMode].fileBuf);
                gFlow.msleep(nwaittime);
            }
        }

//        gFlow.msleep((writeLen/4)*3);
//        gFlow.sleep(15);

    } else if (curBoardMode == BOARD_SOC || curBoardMode == BOARD_SOC_MODEL) {
        QByteArray md5Result;
        QCryptographicHash md(QCryptographicHash::Md5);
        md.addData(m_struInfo[curBoardMode].fileBuf);
        md5Result = md.result();

        writeLen = qCeil(m_struInfo[curBoardMode].nWriteLen*1.0 / LEN_GENERAL_FILE_PACKET_DATA ) + 1;
        data[0] = writeLen/256;
        data[1] = writeLen%256;

        //! step 1: 下发升级文件总包个数
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            gFlow.sendUpgradeInfo(nIndex, CMD_SOC_UPGRADE_PACKAGES, MODULE_SOC, m_struInfo[curBoardMode].nBoardUnit - 1, data, 2);
        }

        gFlow.sleep(2);

        QVector<int> vctControl;
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;
            else
                vctControl.push_back(nIndex);
        }

        //! step2: 下发升级文件
        for(int n = 0; n < writeLen - 1 ; n++){
            int nValidDataLen  = LEN_GENERAL_FILE_PACKET_DATA;
            if(n == (writeLen-2)){
                nValidDataLen = m_struInfo[curBoardMode].nWriteLen - (LEN_GENERAL_FILE_PACKET_DATA*n);
            }

            for(int m = 0; m < vctControl.size(); m++){
                int nIndex = vctControl[m];
                gFlow.writeUpdateDataForSoc(nIndex, MODULE_SOC, m_struInfo[curBoardMode].nBoardUnit - 1, 0x1006, n, m_struInfo[curBoardMode].fileBuf, nValidDataLen, false);
            }

            nBurnPages++;
            gFlow.msleep(10);
        }

        //! step3: 下发md5校验包
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            gFlow.writeUpdateDataForSoc(nIndex, MODULE_SOC, m_struInfo[curBoardMode].nBoardUnit - 1, 0x1006, writeLen - 1, md5Result, md5Result.length(), true);
        }

        nBurnPages++;

        gFlow.sleep(3);

        //! step4: 下发升级类型 升级SOC软件or模型
        data[0] = (curBoardMode == BOARD_SOC) ? 0 : 1;
        data[1] = m_socmodelIdx;
        for (int i = 0; i < m_struInfo[curBoardMode].qvControlUnit.size(); i++) {
            int nIndex = m_struInfo[curBoardMode].qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            gFlow.sendUpgradeInfo(nIndex, CMD_SOC_UPGRADE_TYPE, MODULE_SOC, m_struInfo[curBoardMode].nBoardUnit - 1, data, 2);
        }

        gFlow.msleep(200);
    }

    gFlow.sleep(15);

    burnTimer->stop();
    ++step;
    if(!checkStatus(curBoardMode, UPGRADE_SEND)) {
        return false;
    }
    return true;
}

/**
 * @brief upgradeFPGAwidget::升级步骤4: 切换至用户模式
 * @param nType
 * @return
 */
bool upgradeFPGAwidget::stepSwitchToUser(int nType)
{   
    char module_type = 0;
    int nUnit = 0;
    bool bRet = getModuleType(nType, module_type, nUnit);

    if(bRet){
        auto &qvControlUnit = m_struInfo[curBoardMode].qvControlUnit;
        for (int i = 0; i < qvControlUnit.size();i++) {
            int nIndex = qvControlUnit.at(i);
            if(!checkBoardStatus(curBoardMode, nIndex))
                continue;

            char data = 1;
            gFlow.sendUpgradeInfo(nIndex, CMD_UPDATE_USER, module_type, nUnit, &data, 1);
        }
        gFlow.msleep(500);

        if(!checkStatus(nType,UPGRADE_USER)) {
            return false;
        }else
            return true;
    }

    return false;
}
/*!
 * \brief 板载深度学习通讯方式
 * \param nType： 0：网口 1:usb （SG板子是usb）
 * \param nUint： 相机地址
 * \return
 */
bool upgradeFPGAwidget::setfacType(int nType, int nUint)
{
    gFlow.sendFpgaFacType(struGsh.nAnalyzeIndex, nType, nUint);
    return true;
}
