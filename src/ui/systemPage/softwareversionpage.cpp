#include "softwareversionpage.h"
#include "core/io/modbus/modbusctlclass.h"
#include <QScrollArea>

analyzeVersion::analyzeVersion(int analyze, QWidget *parent): QWidget(parent),m_analyzeIndex(analyze)
{
    initParams();
    createPage();
}

/**
 * @brief analyzeVersion::初始化参数
 */
void analyzeVersion::initParams()
{
    movie = new QMovie(":/res/png/2.gif");
    movie->setScaledSize(QSize(40,40));
    movie->start();
    checkItemStrList.clear();
    checkItemStrList << myLan.control_board
                     << QString("%1%2").arg(myLan.camer).arg(1)
                     << QString("%1%2").arg(myLan.camer).arg(2)
                     << QString("%1%2").arg(myLan.camer).arg(3)
                     << QString("%1%2").arg(myLan.camer).arg(4)
                     << QString("%1%2").arg(myLan.camer).arg(5)
                     << QString("%1%2").arg(myLan.camer).arg(6)
                     << "SOC1" << "SOC2" << "SOC3" << "SOC4"
                     << "SOC5" << "SOC6"
                     << myLan.GPU
                     << myLan.plus_light
                     << myLan.halogen_board
                     << myLan.elec_valve
                     << myLan.low_valve; //! 微阀
}

/**
 * @brief analyzeVersion::界面搭建
 */
void analyzeVersion::createPage()
{
    QString gbstyle = QString("QGroupBox{"
                              "background:transparent;"
                              "border:0px solid lightgray;"
                              "margin-top:10px; outline:0px;}"
                              "QLabel{font-size: 24px;}");
    //! 版本
    m_verInfogb = new QGroupBox();
    for (int i = 0; i < ITEM_ALL; i++) {
        itemGroup[i] = new QGroupBox();
        itemGroup[i]->setStyleSheet(gbstyle);

        //! 版本查询图标
        headIconLbl[i] = new QLabel("");
        headIconLbl[i]->setFixedSize(40,40);
        headIconLbl[i]->setScaledContents(true);
        headIconLbl[i]->setPixmap(QPixmap(":/res/png/model_parse.png"));

        //! 版本查询标题
        headInfoLbl[i] = new QLabel(checkItemStrList.at(i));
        headInfoLbl[i]->setFixedHeight(BTN_HEI-30);

        //! 查询结果显示
        simpleInfoBtn[i] = new QLabel();
        simpleInfoBtn[i]->setText(myLan.normal);
        simpleInfoBtn[i]->setFixedSize(BTN_WID*2, BTN_HEI-30);
        simpleInfoBtn[i]->setAlignment(Qt::AlignRight | Qt::AlignBottom);

        //! 加载进度显示
        movieLbl[i] = new QLabel("");
        movieLbl[i]->setFixedSize(40,BTN_HEI-30);
        movieLbl[i]->setMovie(movie);
        movieLbl[i]->hide();

        //! 分界线
        headLine[i] = new QFrame();
        headLine[i]->setFrameShape(QFrame::HLine);
        headLine[i]->setFrameShadow(QFrame::Raised);
        headLine[i]->setStyleSheet(QString("background-color: %1;").arg(g_ColorLightGray2));
        headLine[i]->setLineWidth(2);
    }

    m_glayPage = new QGridLayout(m_verInfogb);
    m_glayPage->setHorizontalSpacing(80);
    m_glayPage->setContentsMargins(40,20,40,20);
    for(int i = 0; i < ITEM_ALL; i++) {
        m_headVLayout[i] = new QVBoxLayout(itemGroup[i]);
        m_glayPage->addWidget(itemGroup[i], i/2, i%2, 1, 1);

        m_headHLayout[i] = new QHBoxLayout();
        m_headHLayout[i]->addWidget(headIconLbl[i]);
        m_headHLayout[i]->addSpacing(6);
        m_headHLayout[i]->addWidget(headInfoLbl[i]);
        m_headHLayout[i]->addStretch(0);
        m_headHLayout[i]->addWidget(simpleInfoBtn[i]);
        m_headHLayout[i]->addSpacing(10);
        m_headHLayout[i]->addWidget(movieLbl[i]);

        m_lineHLayout[i] = new QHBoxLayout();
        m_lineHLayout[i]->addWidget(headLine[i]);

        m_headVLayout[i]->addLayout(m_headHLayout[i]);
        m_headVLayout[i]->addLayout(m_lineHLayout[i]);
    }


    //! 模型
    m_modelInfogb = new QGroupBox();
    m_modelInfogb->hide();

    QGridLayout *m_glaymodel = new QGridLayout(m_modelInfogb);
    m_glaymodel->setHorizontalSpacing(80);
    m_glaymodel->setContentsMargins(40,20,40,20);

    for(int i=0; i<MAX_CAM_NUM; i++){
        m_modelitemgp[i] = new QGroupBox();
        m_modelitemgp[i]->setStyleSheet(gbstyle);

        m_modelIconlb[i] = new QLabel();
        m_modelIconlb[i]->setPixmap(QPixmap(":/res/png/model_parse.png"));
        m_modelIconlb[i]->setFixedSize(40,40);
        m_modelIconlb[i]->setScaledContents(true);

        m_modelInfolb[i] = new QLabel();
        m_modelInfolb[i]->setFixedHeight(BTN_HEI-30);

        m_modelInfoReslb[i] = new QLabel();
        m_modelInfoReslb[i]->setFixedSize(BTN_WID*3+20, BTN_HEI-30);
        m_modelInfoReslb[i]->setAlignment(Qt::AlignRight | Qt::AlignBottom);


        m_modelmovielb[i] = new QLabel();
        m_modelmovielb[i]->setFixedSize(40,BTN_HEI-30);
        m_modelmovielb[i]->setMovie(movie);
        m_modelmovielb[i]->hide();

        m_modelframe[i] = new QFrame();
        m_modelframe[i]->setFrameShape(QFrame::HLine);
        m_modelframe[i]->setFrameShadow(QFrame::Raised);
        m_modelframe[i]->setStyleSheet(QString("background-color: %1;").arg(g_ColorLightGray2));
        m_modelframe[i]->setLineWidth(2);

        QHBoxLayout *hlaymodelitem = new QHBoxLayout();
        hlaymodelitem->addWidget(m_modelIconlb[i]);
        hlaymodelitem->addWidget(m_modelInfolb[i]);
        hlaymodelitem->addWidget(m_modelInfoReslb[i]);
        hlaymodelitem->addWidget(m_modelmovielb[i]);

        QVBoxLayout* vlayitem = new QVBoxLayout(m_modelitemgp[i]);
        vlayitem->addLayout(hlaymodelitem);
        vlayitem->addWidget(m_modelframe[i]);

        m_glaymodel->addWidget(m_modelitemgp[i],i/2,i%2);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_verInfogb);
    mainLayout->addWidget(m_modelInfogb);
}

/**
 * @brief analyzeVersion::界面刷新
 */
void analyzeVersion::refreshPage()
{
    //! 刷新版本
    refreshItemShowSta();
    clearAndrelayout();
    refreshItemContens();

    //! 刷新模型信息
    if(!m_modelInfogb->isHidden()){
        refreshModelInfo();
    }
}

/**
 * @brief analyzeVersion::开始版本查询
 */
void analyzeVersion::startVersionCheck()
{
    for (int i = 0; i < ITEM_ALL; i++) {
        simpleInfoBtn[i]->hide();
        movieLbl[i]->show();
    }

    if(!m_modelInfogb->isHidden()){
        for(int i=0; i<gFlow.getMachineCamNum(); i++){
            m_modelmovielb[i]->show();
            m_modelInfoReslb[i]->hide();
        }
    }

}

void analyzeVersion::setModelInfoVisable()
{
    m_modelInfogb->show();
}

void analyzeVersion::clearAndrelayout()
{

    // Remove existing items from layout without deleting the widgets
    QLayoutItem *item = nullptr;
    while ((item = m_glayPage->takeAt(0)) != nullptr) {
        QWidget *w = item->widget();
        if (w) {
            m_glayPage->removeWidget(w);
        }
        delete item;
    }

    QList<QWidget*> visibleItems;
    QList<QWidget*> hiddenItems;

    auto appendItem = [&](QWidget *w) {
        if (!w) return;
        if (w->isHidden()) hiddenItems.append(w);
        else visibleItems.append(w);
    };

    // Cameras
    int camNum = gFlow.getMachineCamNum();
    for (int i = 0; i < camNum; ++i) {
        appendItem(itemGroup[ITEM_CAM1 + i]);
    }

    // SOC units (respect bHasAssist)
    int nSocnum = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist ? camNum - 1 : camNum;
    for (int i = 0; i < nSocnum; ++i) {
        appendItem(itemGroup[ITEM_SOC1 + i]);
    }

    // Control and optional items (order preserved)
    appendItem(itemGroup[ITEM_CONTROL]);
    auto *pAnalyzegb = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
    if (pAnalyzegb->bHasAssist) appendItem(itemGroup[ITEM_HALOGEN]);
    if (pAnalyzegb->bEnPulseLight) appendItem(itemGroup[ITEM_PLUS_LIGHT]);
    if (pAnalyzegb->bEnSNV) appendItem(itemGroup[ITEM_VALVE]);
    if(pAnalyzegb->bEnVal) appendItem(itemGroup[ITEM_EJECTOR]);
    appendItem(itemGroup[ITEM_MODEL]);

    // Remaining camera placeholders
    for (int i = camNum; i < MAX_CAM_NUM; ++i) {
        appendItem(itemGroup[ITEM_CAM1 + i]);
    }

    // Remaining soc placeholders
    for (int i = nSocnum; i < MAX_CAM_NUM; ++i) {
        appendItem(itemGroup[ITEM_SOC1 + i]);
    }

    // Final ordered list: visible first, then hidden
    QList<QWidget*> ordered = visibleItems + hiddenItems;

    // Add widgets into grid sequentially (2 columns)
    for (int idx = 0; idx < ordered.size(); ++idx) {
        int row = idx / 2;
        int col = idx % 2;
        m_glayPage->addWidget(ordered.at(idx), row, col, 1, 1);
    } 
}


void analyzeVersion::refreshItemShowSta()
{
    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            itemGroup[ITEM_CAM1+i]->show();
            if(gFlow.bIsIMSerialCam(i)){
                itemGroup[ITEM_SOC1+i]->hide();
            }else{
                itemGroup[ITEM_SOC1+i]->show();
            }
           
        }else{
            itemGroup[ITEM_CAM1+i]->hide();
            itemGroup[ITEM_SOC1+i]->hide();
        }
    }

    auto *pAnalyzegb =  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];

    if(pAnalyzegb->bHasAssist){
        itemGroup[ITEM_HALOGEN]->show();
    }else{
        itemGroup[ITEM_HALOGEN]->hide();
    }

    if(pAnalyzegb->bEnSNV){
        itemGroup[ITEM_VALVE]->show();
    }else{
        itemGroup[ITEM_VALVE]->hide();
    }

    if(pAnalyzegb->bEnPulseLight){
        itemGroup[ITEM_PLUS_LIGHT]->show();
    }else{
        itemGroup[ITEM_PLUS_LIGHT]->hide();
    }

    if(pAnalyzegb->bEnVal){
        itemGroup[ITEM_EJECTOR]->show();
    }else{
        itemGroup[ITEM_EJECTOR]->hide();
    }

    for (int i = 0; i < ITEM_ALL; i++) {
        movieLbl[i]->hide();
        simpleInfoBtn[i]->show();
    }
}

void analyzeVersion::refreshItemContens()
{
    auto *pAnalyzeShare = struGsh.analyzeInfo[m_analyzeIndex];
    QStringList verlst, camlst, soclst;
    bool bAbnormal = false;
    verlst <<QString(pAnalyzeShare->sCtrlVersion);
    for(int i=0; i<MAX_CAM_NUM; i++){
        camlst<<QString(pAnalyzeShare->sCamVersion[i]);
        soclst<<QString(pAnalyzeShare->sSocVersion[i]);

        if(i<gFlow.getMachineCamNum()){
            if(!struGsh.bSocModelUnitResult[i]){
                bAbnormal = true;
            }
        }
    }
    verlst<<camlst<<soclst;

    verlst<<QString(bAbnormal?"NULL":myLan.normal);
    verlst<<QString(pAnalyzeShare->sPulseVersion);
    verlst<<QString(pAnalyzeShare->sHalogenVersion);
    verlst<<QString(pAnalyzeShare->sSVBoardVersion);
    verlst<<QString(pAnalyzeShare->sValveBoardVersion);

    for(int i=0; i<ITEM_ALL && i<verlst.size(); i++){
        simpleInfoBtn[i]->setText(QString("%1").arg(verlst.at(i)));
        simpleInfoBtn[i]->setStyleSheet("color:black");
        if(verlst.at(i).compare("NULL") == 0){
            simpleInfoBtn[i]->setStyleSheet("color:red");
        }
    }
}

/*!
 * \brief 刷新模型信息
 */
void analyzeVersion::refreshModelInfo()
{
    auto *pAnalyzerGb = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_modelitemgp[i]->show();
            m_modelmovielb[i]->hide();
            m_modelIconlb[i]->show();
            m_modelInfolb[i]->show();
            m_modelInfoReslb[i]->show();
            m_modelframe[i]->show();

            m_modelInfolb[i]->setText(QString("%1_SOC").arg(pAnalyzerGb->CamName[i]));

            QString strsModel = QString("%1").
                                arg(QString::fromLocal8Bit(struGsh.sModelUnitInfo[i].model[struGsh.sModelUnitInfo[i].currentId].cnf));
            m_modelInfoReslb[i]->setText(strsModel);
            if(strsModel.compare("NULL") == 0){
                m_modelInfoReslb[i]->setStyleSheet("color:red");
            }else{
                m_modelInfoReslb[i]->setStyleSheet("color:black");
            }
        }else{
            m_modelIconlb[i]->hide();
            m_modelInfolb[i]->hide();
            m_modelInfoReslb[i]->hide();
            m_modelframe[i]->hide();
            m_modelmovielb[i]->hide();
            m_modelitemgp[i]->hide();
        }
    }
}

softwareVersionPage::softwareVersionPage(QWidget *parent) : QWidget(parent)
{
    createPage();

    connecctSigAndSlot();
}

/**
 * @brief softwareVersionPage::搭建界面
 */
void softwareVersionPage::createPage()
{
    m_versionWid = new QWidget();
    m_versionWid->setStyleSheet("background-color:white; border-radius:10px;");

    QHBoxLayout *upLayout = new QHBoxLayout();

    QFont font;
    machineLogoLbl = new QLabel("");
    machineLogoLbl->setFixedSize(BTN_HEI, BTN_HEI);
    machineLogoLbl->setScaledContents(true);
    machineLogoLbl->setPixmap(QPixmap(":/res/png/softversion.png").scaled(machineLogoLbl->width(),
                                                                           machineLogoLbl->height(),
                                                                           Qt::IgnoreAspectRatio,
                                                                           Qt::SmoothTransformation));
    machineLogoLbl->hide();

    softwareDetailLbl = new QLabel("");
    softwareDetailLbl->setFixedHeight(BTN_HEI);
    softwareDetailLbl->setScaledContents(true);
    font.setPixelSize(15);
    softwareDetailLbl->setFont(font);
    softwareDetailLbl->hide();

#ifdef Q_OS_UNIX
    softwareDetailLbl->setText(QString("%1 V：%2 R:%3 %4 %5").arg(ARM_CODE)
                                .arg(ARM_VERSION).arg(SVN_VERSION)
                                .arg(QString(__DATE__)).arg(QString(__TIME__)));
#else
    softwareDetailLbl->setText(QString("%1 R：%2 %3 %4").arg("80030214")
                                                        .arg(SVN_VERSION)
                                                        .arg(QString(__DATE__))
                                                        .arg(QString(__TIME__)));
#endif

    upLayout->addSpacing(20);
    upLayout->addWidget(machineLogoLbl , 0, Qt::AlignLeft);
    upLayout->addWidget(softwareDetailLbl);
    upLayout->addStretch();

    //! Other
    QWidget *downWidget = new QWidget();
    QGridLayout *downLayout = new QGridLayout(downWidget);
    for (int i = 0; i < MAX_ANALYZE_NUM ; i++) {
        analyzePage[i] = new analyzeVersion(i);
        if(i!=0){
            analyzePage[i]->hide();
        }
        downLayout->addWidget(analyzePage[i] ,i/2, i%2, 1, 1);
    }

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(downWidget);
    scrollArea->setWidgetResizable(true);


    QVBoxLayout *vlayversion = new QVBoxLayout(m_versionWid);
    vlayversion->addLayout(upLayout);
    vlayversion->addWidget(scrollArea);


    m_stackWid = new QStackedWidget();
    m_stackWid->addWidget(m_versionWid);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_stackWid);
}

/**
 * @brief softwareVersionPage::信号槽绑定
 */
void softwareVersionPage::connecctSigAndSlot()
{
}

/**
 * @brief softwareVersionPage::开始版本查询
 */
void softwareVersionPage::startVersionCheck()
{
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        analyzePage[i]->startVersionCheck();
    }
}

/**
 * @brief softwareVersionPage::刷新界面
 */
void softwareVersionPage::refreshPage()
{
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        analyzePage[i]->show();
        analyzePage[i]->refreshPage();
    }
    for (int i = struCnfg.analyzeNum; i < MAX_ANALYZE_NUM; i++) {
        analyzePage[i]->hide();
    }
}

void softwareVersionPage::setScreenVerLabShow()
{
    softwareDetailLbl->show();
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        analyzePage[i]->setModelInfoVisable();
    }
}

/**
 * @brief softwareVersionPage::刷新版本
 */
void softwareVersionPage::refreshVerison()
{
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        analyzePage[i]->refreshPage();
    }
}



