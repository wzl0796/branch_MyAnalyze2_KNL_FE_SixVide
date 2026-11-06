#include "ticksetting.h"

ticksetting::ticksetting(QWidget *parent) : QWidget(parent)
{
    createpage();
    laypage();
    connectAndSlt();
}

void ticksetting::createpage()
{
    m_stackWidget = new QStackedWidget(this);

    m_ejcCheckBtn = new myPushButton();
    m_ejcCheckBtn->setText(myLan.ejector_selfcheck);
    m_ejcCheckBtn->setFixedHeight(BTN_HEI);

    //! 低压阀设置
    m_wid = new QWidget();
    m_valveGb = new QGroupBox(myLan.Ejector_valve_set);
    m_valveGb->setMaximumHeight(740);
    m_valOnffLb = new QLabel(myLan.ONOFF);
    m_valOnffLb->hide();
    m_valOnffSw = new mySwitchButton();
    m_valOnffSw->setFixedSize(100,60);
    m_valOnffSw->hide();

    m_valSpeedLb = new QLabel(QString("%1(us)").arg(myLan.acceleration_time));
    m_valSpeedLe = new MyLineEdit();
    m_valSpeedLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_valSpeed2Lb = new QLabel(QString("%1%2(ms)").arg(myLan.acceleration_time).arg(2));
    m_valSpeed2Le = new MyLineEdit();
    m_valSpeed2Le->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_valSpantimeLb = new QLabel(QString("%1(ms)").arg(myLan.interval_time));
    m_valSpantimeLe = new MyLineEdit();
    m_valSpantimeLe->setStyleSheet("background-color:rgba(220,220,220,100)");


    m_valFreLb = new QLabel(myLan.frequence);
    m_valFreMut = new myMutex(Qt::Horizontal,2);
    m_valFreMut->setNum(3);
    QStringList lst;
    lst<<"5K"<<"8K"<<"10K";
    m_valFreMut->setLabelText(lst);

    m_valD2Lb = new QLabel(QString("%1D2").arg(myLan.PWM));
    m_valD2Mut = new myMutex(Qt::Horizontal,2);
    m_valD2Mut->setNum(3);
    QStringList lstD2;
    lstD2<<"0.035"<<"0.04"<<"0.045";
    m_valD2Mut->setLabelText(lstD2);

    m_valModeLb = new QLabel(myLan.workmode);
    m_valModeMut = new myMutex(Qt::Horizontal,1);
    m_valModeMut->setNum(2);
    QStringList lstType;
    lstType<<myLan.low_voltage<<myLan.high_voltage;
    m_valModeMut->setLabelText(lstType);

    m_ticktimeGb = new QGroupBox(myLan.tick_setting);
    m_ticktimeGb->setMaximumHeight(550);
    m_delaylb  = new QLabel(QString("%1").arg(myLan.delay_time));
    m_delaylb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_delaylb->setAlignment(Qt::AlignCenter);
    m_delayLe = new MyLineEdit();
    m_delayLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_delayLe->setFixedSize(BTN_WID, BTN_HEI-15);

    m_blowlb  = new QLabel(QString("%1").arg(myLan.blow_time));
    m_blowlb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_blowlb->setAlignment(Qt::AlignCenter);
    m_blowLe = new MyLineEdit();
    m_blowLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_blowLe->setFixedSize(BTN_WID, BTN_HEI-15);

    m_Trunclb  = new QLabel(QString("%1").arg(myLan.trunc_time));
    m_Trunclb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_Trunclb->setAlignment(Qt::AlignCenter);
    m_TruncLe = new MyLineEdit();
    m_TruncLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_TruncLe->setFixedSize(BTN_WID, BTN_HEI-15);

    m_Deglb  = new QLabel(QString("%1").arg(myLan.deg_time));
    m_Deglb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_Deglb->setAlignment(Qt::AlignCenter);
    m_DegLe = new MyLineEdit();
    m_DegLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_DegLe->setFixedSize(BTN_WID, BTN_HEI-15);

    m_Hfplb  = new QLabel(QString("%1").arg(myLan.hig_fre_protect));
    m_Hfplb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_Hfplb->setAlignment(Qt::AlignCenter);
    m_HfpLe = new MyLineEdit();
    m_HfpLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_HfpLe->setFixedSize(BTN_WID, BTN_HEI-15);

    m_ejectorOnoffLb = new QLabel(myLan.ejector_onoff);
    m_ejectorOnoffLb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_ejectorOnoffLb->setAlignment(Qt::AlignCenter);
    m_ejectorOnoffSw = new mySwitchButton();
    m_ejectorOnoffSw->setFixedSize(100, 60);


    m_Cachelb  = new QLabel(QString("%1").arg(myLan.cacheline));
    m_Cachelb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_Cachelb->setAlignment(Qt::AlignCenter);
    m_CacheLe = new MyLineEdit();
    m_CacheLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_CacheLe->setFixedSize(BTN_WID, BTN_HEI-15);

    QString sty = QString("QRadioButton::indicator{width: 40px; height: 40px;}");
    m_PluscacheBtn = new QRadioButton(myLan.positive_cache);
    m_PluscacheBtn->setStyleSheet(sty);
    m_PluscacheBtn->setChecked(true);
    m_NegacacheBtn = new QRadioButton(myLan.negative_cache);
    m_NegacacheBtn->setStyleSheet(sty);
    m_NegacacheBtn->setChecked(false);

    m_mutexBtnGp = new QButtonGroup();
    m_mutexBtnGp->addButton(m_PluscacheBtn,0);
    m_mutexBtnGp->addButton(m_NegacacheBtn,1);
    m_mutexBtnGp->setExclusive(true);

    m_resizeRectGb = new QGroupBox();
    m_resizeRectGb->setMaximumHeight(150);
    m_resizeRectLb  = new QLabel(QString("%1(y)").arg(myLan.Rect_ratio));
    m_resizeRectLb->setFixedSize(BTN_WID+50, BTN_HEI-15);
    m_resizeRectLb->setAlignment(Qt::AlignCenter);
    m_resizeRectLe = new MyLineEdit();
    m_resizeRectLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_resizeRectLe->setFixedSize(BTN_WID, BTN_HEI-15);

    //! 喷阀自检
    m_selfCheckWidget = new ejectorSelfCheckWidget();
    m_stackWidget->addWidget(m_wid);
    m_stackWidget->addWidget(m_selfCheckWidget);
    m_stackWidget->setCurrentIndex(0);
}

void ticksetting::laypage()
{
    auto layitem =[this](QBoxLayout *hlay, QWidget* wid, auto&&... widgets){
        hlay->addWidget(wid);
        (hlay->addWidget(widgets), ...);
    };


    //! 低压阀
    QHBoxLayout *hlayEnval = new QHBoxLayout();
    layitem(hlayEnval,m_valOnffLb,m_valOnffSw);

    QHBoxLayout *hlayvalspeed = new QHBoxLayout();
    layitem(hlayvalspeed,m_valSpeedLb,m_valSpeedLe);

    QHBoxLayout *hlayvalspeed2 = new QHBoxLayout();
    layitem(hlayvalspeed2,m_valSpeed2Lb,m_valSpeed2Le);

    QHBoxLayout *hlayvalspan = new QHBoxLayout();
    layitem(hlayvalspan,m_valSpantimeLb,m_valSpantimeLe);

    QVBoxLayout *vlayvalfre = new QVBoxLayout();
    layitem(vlayvalfre,m_valFreLb,m_valFreMut);

    QVBoxLayout *vlayvalD2 = new QVBoxLayout();
    layitem(vlayvalD2,m_valD2Lb,m_valD2Mut);

    QVBoxLayout *vlayvalMod = new QVBoxLayout();
    layitem(vlayvalMod,m_valModeLb,m_valModeMut);

    QVBoxLayout *vlayval = new QVBoxLayout(m_valveGb);
    vlayval->setContentsMargins(10,50,10,10);
    vlayval->addSpacing(20);
    vlayval->addLayout(hlayEnval);
    vlayval->addSpacing(15);
    vlayval->addLayout(hlayvalspeed);
    vlayval->addSpacing(15);
    vlayval->addLayout(hlayvalspeed2);
    vlayval->addSpacing(15);
    vlayval->addLayout(hlayvalspan);
    vlayval->addSpacing(20);
    vlayval->addLayout(vlayvalfre);
    vlayval->addLayout(vlayvalD2);
    vlayval->addLayout(vlayvalMod);


    //! 剔除时间
    QHBoxLayout *hlaydelay = new QHBoxLayout();
    layitem(hlaydelay,m_delaylb,m_delayLe);

    QHBoxLayout *hlayblow = new QHBoxLayout();
    layitem(hlayblow,m_blowlb,m_blowLe);

    QHBoxLayout *hlayhfp = new QHBoxLayout();
    layitem(hlayhfp,m_Hfplb,m_HfpLe);

    QVBoxLayout *vlay = new QVBoxLayout();
    vlay->addLayout(hlaydelay);
    vlay->addSpacing(20);
    vlay->addLayout(hlayblow);
    vlay->addSpacing(20);
    vlay->addLayout(hlayhfp);

    QHBoxLayout *hlayEjectorOnoff = new QHBoxLayout();
    layitem(hlayEjectorOnoff, m_ejectorOnoffLb, m_ejectorOnoffSw);

    QHBoxLayout *hlaytrunc = new QHBoxLayout();
    layitem(hlaytrunc,m_Trunclb,m_TruncLe);

    QHBoxLayout *hlaydeg = new QHBoxLayout();
    layitem(hlaydeg,m_Deglb,m_DegLe);

    QVBoxLayout *vlayejc = new QVBoxLayout();
    vlayejc->addLayout(hlaytrunc);
    vlayejc->addSpacing(20);
    vlayejc->addLayout(hlaydeg);
    vlayejc->addSpacing(20);
    vlayejc->addLayout(hlayEjectorOnoff);


    QHBoxLayout *hlaytop = new QHBoxLayout();
    hlaytop->addLayout(vlay);
    hlaytop->addLayout(vlayejc);

    QHBoxLayout *hlaycache = new QHBoxLayout();
    hlaycache->addSpacing(5);
    layitem(hlaycache,m_Cachelb,m_CacheLe);
    hlaycache->addStretch();

    QHBoxLayout *hlaycachebtn = new QHBoxLayout();
    hlaycachebtn->addSpacing(40);
    hlaycachebtn->addWidget(m_PluscacheBtn);
    hlaycachebtn->addSpacing(20);
    hlaycachebtn->addWidget(m_NegacacheBtn);

    QVBoxLayout *vlaybottom = new QVBoxLayout();
    vlaybottom->addLayout(hlaycache);
    vlaybottom->addLayout(hlaycachebtn);


    QVBoxLayout *vlayticktime = new QVBoxLayout(m_ticktimeGb);
    vlayticktime->addLayout(hlaytop);
    vlayticktime->addLayout(vlaybottom);


    QHBoxLayout *hlayresize = new QHBoxLayout();
    layitem(hlayresize,m_resizeRectLb,m_resizeRectLe);
    QVBoxLayout *vlayresize = new QVBoxLayout(m_resizeRectGb);
    vlayresize->addLayout(hlayresize);

    QVBoxLayout *vlayleft = new QVBoxLayout();
    vlayleft->addWidget(m_ticktimeGb);
    vlayleft->addWidget(m_resizeRectGb);

    QHBoxLayout *hlaywid = new QHBoxLayout(m_wid);
    hlaywid->addLayout(vlayleft);
    hlaywid->addSpacing(60);
    hlaywid->addWidget(m_valveGb);

    QVBoxLayout *vlaypage = new QVBoxLayout();
    vlaypage->addWidget(m_stackWidget);
    vlaypage->addWidget(m_ejcCheckBtn, 0, Qt::AlignCenter);
    vlaypage->addSpacing(6);
    this->setLayout(vlaypage);

}

void ticksetting::connectAndSlt()
{
    //! 低压阀
    connect(m_valOnffSw, SIGNAL(checkedChanged(bool)), this ,SLOT(onValEnchgSlt(bool)));
    connect(m_valSpeedLe, SIGNAL(pressed()), this, SLOT(onAcctimelechgslt()));
    connect(m_valSpeed2Le, SIGNAL(pressed()), this, SLOT(onAcctime2lechgslt()));
    connect(m_valFreMut, SIGNAL(indexChanged(int)), this, SLOT(onValfretypechgslt(int)));
    connect(m_valD2Mut, SIGNAL(indexChanged(int)), this, SLOT(onD2typechgslt(int)));
    connect(m_valModeMut, SIGNAL(indexChanged(int)), this, SLOT(onVoltypechgslt(int)));
    connect(m_valSpantimeLe, SIGNAL(pressed()), this, SLOT(onValspantimechgslt()));

    connect(m_delayLe, &MyLineEdit::pressed, this, &ticksetting::onDelayBtnpressedSlt);
    connect(m_blowLe, &MyLineEdit::pressed, this, &ticksetting::onBlowBtnpressedSlt);
    connect(m_TruncLe, &MyLineEdit::pressed, this, &ticksetting::onTruncBtnpressedSlt);
    connect(m_DegLe, &MyLineEdit::pressed, this, &ticksetting::onDegBtnpressedSlt);
    connect(m_CacheLe, &MyLineEdit::pressed, this, &ticksetting::onCacheBtnpressedSlt);
    connect(m_mutexBtnGp, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &ticksetting::onmutexBtnchagedSlt);
    connect(m_HfpLe, &MyLineEdit::pressed, this, &ticksetting::onHfpBtnpressedSlt);
    connect(m_ejectorOnoffSw, &mySwitchButton::checkedChanged, this, [this](bool bOnOff) {
        auto *analyzeInfo = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
        analyzeInfo->bEjectorOnoff = bOnOff;
        gFlow.sendEjectorOnOff(struGsh.nAnalyzeIndex, bOnOff);
    });

    connect(m_resizeRectLe, SIGNAL(pressed()), this ,SLOT(onRectResizeChgSlt()));

    connect(m_ejcCheckBtn, &QPushButton::clicked, this, [this]() {
        m_stackWidget->setCurrentIndex(1);
        m_ejcCheckBtn->hide();
    });
    connect(m_selfCheckWidget, &ejectorSelfCheckWidget::backRequested, this, [this]() {
        m_stackWidget->setCurrentIndex(0);
        m_ejcCheckBtn->show();
    });
}

void ticksetting::refreshPage()
{
    auto *analyzeInfo = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
    m_valOnffSw->setChecked(analyzeInfo->svalParms.bEn);
    m_valSpeedLe->setText(QString("%1").arg(analyzeInfo->svalParms.nacctime));
    m_valSpeed2Le->setText(QString::number(analyzeInfo->svalParms.nacctime2,'f',1));
    m_valFreMut->setCurrentIndex(analyzeInfo->svalParms.nfre);
    m_valD2Mut->setCurrentIndex(analyzeInfo->svalParms.D2type);
    m_valModeMut->setCurrentIndex(analyzeInfo->svalParms.voltype);
    m_valSpantimeLe->setText(QString("%1").arg(analyzeInfo->svalParms.nspan));

    m_delayLe->setText(QString("%1").arg(analyzeInfo->nBlowDelay));
    m_blowLe->setText(QString("%1").arg(analyzeInfo->nBlowTime));
    m_TruncLe->setText(QString("%1").arg(analyzeInfo->nTruncTime));
    m_DegLe->setText(QString("%1").arg(analyzeInfo->nDegTime));
    m_CacheLe->setText(QString("%1").arg(analyzeInfo->nCacheLine));
    m_HfpLe->setText(QString("%1").arg(analyzeInfo->nhfpTime));
    m_ejectorOnoffSw->setChecked(analyzeInfo->bEjectorOnoff);
    int nType =  analyzeInfo->nCacheType;
    m_mutexBtnGp->blockSignals(true);
    if(nType == 0){
        m_PluscacheBtn->setChecked(true);
        m_NegacacheBtn->setChecked(false);
    }else{
        m_PluscacheBtn->setChecked(false);
        m_NegacacheBtn->setChecked(true);
    }
    m_mutexBtnGp->blockSignals(false);

    m_resizeRectLe->setText(QString("%1%").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize));
}

/*!
 * \brief 低压阀使能
 * \param bsta
 */
void ticksetting::onValEnchgSlt(bool bsta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.bEn = bsta;
    gFlow.sendvalBoardOnff();
}

/*!
 * \brief 加速时间
 */
void ticksetting::onAcctimelechgslt()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 10000, m_valSpeedLe->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_valSpeedLe->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.nacctime = n_sample;
        gFlow.sendvalBoardparams();
    }
}

/*!
 * \brief 加速时间2
 */
void ticksetting::onAcctime2lechgslt()
{
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 10.0, m_valSpeed2Le->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        m_valSpeed2Le->setText(QString::number(n_sample,'f',1));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.nacctime2 = n_sample;
        gFlow.sendvalBoardparams();
    }
}


/*!
 * \brief 低压阀电压类型
 */
void ticksetting::onVoltypechgslt(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.voltype = nIdx;
    gFlow.sendvalBoardparams();
}

/*!
 * \brief 低压阀D2
 */
void ticksetting::onD2typechgslt(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.D2type = nIdx;
    gFlow.sendvalBoardparams();
}

/*!
 * \brief 低压阀频率
 */
void ticksetting::onValfretypechgslt(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.nfre = nIdx;
    gFlow.sendvalBoardparams();
}

/*!
 * \brief 低压阀间隔时间
 */
void ticksetting::onValspantimechgslt()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 2, m_valSpantimeLe->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_valSpantimeLe->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->svalParms.nspan = n_sample;
        gFlow.sendvalBoardparams();
    }
}

void ticksetting::onDelayBtnpressedSlt()
{
    int nWipeInterval = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 5000, nWipeInterval, myLan.delay_time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay = inputPanel->getValue();
        m_delayLe->setText(QString("%1").arg(inputPanel->getValue()));
        gFlow.sendDelayTime();
    }
}

void ticksetting::onBlowBtnpressedSlt()
{
    int nWipeInterval = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowTime;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 60, nWipeInterval, myLan.blow_time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowTime = inputPanel->getValue();
        m_blowLe->setText(QString("%1").arg(inputPanel->getValue()));
        gFlow.sendBlowTime();
    }
}

void ticksetting::onTruncBtnpressedSlt()
{
    int nTrunc = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nTruncTime;
    myInputPanel *inputPanel = new myInputPanel(intType, 4, 100, nTrunc, myLan.trunc_time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int nVal = inputPanel->getValue();
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nTruncTime = nVal;
        m_TruncLe->setText(QString("%1").arg(nVal));
        gFlow.sendBlowTime();
    }
}

void ticksetting::onDegBtnpressedSlt()
{
    int ndeg = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nDegTime;
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 12, ndeg, myLan.deg_time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int nVal = inputPanel->getValue();
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nDegTime = nVal;
        m_DegLe->setText(QString("%1").arg(nVal));
        gFlow.sendBlowTime();
    }
}

void ticksetting::onCacheBtnpressedSlt()
{
    int ncache = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCacheLine;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 255, ncache, myLan.cacheline, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCacheLine = inputPanel->getValue();
        m_CacheLe->setText(QString("%1").arg(inputPanel->getValue()));
        gFlow.sendcapLine2cotrlBoard(struGsh.nAnalyzeIndex);
    }
}

void ticksetting::onmutexBtnchagedSlt(int nIdx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCacheType = nIdx == 0 ? 0 : 1;
    gFlow.sendcapLine2cotrlBoard(struGsh.nAnalyzeIndex);
}

void ticksetting::onHfpBtnpressedSlt()
{
    int nhfp = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nhfpTime;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 60000, nhfp, myLan.hig_fre_protect, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nhfpTime = inputPanel->getValue();
        m_HfpLe->setText(QString("%1").arg(inputPanel->getValue()));
        gFlow.sendHigfreProtect(struGsh.nAnalyzeIndex);
    }
}

void ticksetting::onRectResizeChgSlt()
{
    int nSize = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 200, nSize,"", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int nval = inputPanel->getValue();
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize = nval;
        m_resizeRectLe->setText(QString("%1%").arg(nval));
    }
}
