#include "perpheralpage.h"

perpheralPage::perpheralPage(QWidget *parent) : QWidget(parent)
{
    createpage();
    laypage();
    connectAndSlt();
}

void perpheralPage::createpage()
{
    m_wid = new QWidget();

    //! 卤素灯
    m_HalogenGb = new QGroupBox(myLan.halogen_set);
//    m_HalogenGb->setFixedSize(360,370);
    m_HalogenGb->setFixedWidth(400);
    m_HalogenGb->setMinimumHeight(480);
    m_Halogenthdlb = new QLabel(myLan.current_thd);
    m_Halogenle = new MyLineEdit();
    m_Halogenle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_Enhalogenlb = new QLabel(myLan.halogen_enable);
    m_EnHalogenSw = new mySwitchButton();
    m_EnHalogenSw->setFixedSize(100,60);
    m_autoCalhalogenlb = new QLabel(myLan.auto_adj);
    m_autoCalHalogenSw = new mySwitchButton();
    m_autoCalHalogenSw->setFixedSize(100,60);

    m_InfraTempthdlb = new QLabel(myLan.tempture_thd);
    m_InfraTemple = new MyLineEdit();
    m_InfraTemple->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_InfraTempCtrlb = new QLabel(myLan.temp_const_ctrl);
    m_InfraTempCtrSw = new mySwitchButton();
    m_InfraTempCtrSw->setFixedSize(100,60);
    m_infraTempureEnlb = new QLabel(myLan.cam_tempure_alarm);
    m_infraTempureEnsw = new mySwitchButton();
    m_infraTempureEnsw->setFixedSize(100,60);


    //! 伺服
    m_motorGb = new QGroupBox(myLan.servo_set);
//    m_motorGb->setFixedSize(360,370);
    m_motorGb->setFixedWidth(400);
    m_motorOnofflb = new QLabel(myLan.ONOFF);
    m_motorSw = new mySwitchButton();
    m_motorSw->setFixedSize(100,60);
    m_speedlb = new QLabel(QString("%1(m/s)").arg(myLan.speed));
    m_motorSpeedle = new MyLineEdit();
    m_motorSpeedle->setFixedWidth(100);
    m_motorSpeedle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_motorInitBtn = new QPushButton(myLan.servo_Init);
    m_motorInitBtn->setFixedSize(160,70);
    m_motorInitBtn->setStyleSheet(g_enableBtnStyle);


    //! 其他
    m_otherGb = new QGroupBox(myLan.other_periph_device);
//    m_otherGb->setFixedSize(450,800);
    m_otherGb->setFixedWidth(440);
    m_pluslightEnlb = new QLabel(myLan.plus_light);
    m_pluslightEnsw = new mySwitchButton();
    m_pluslightEnsw->setFixedSize(100,60);

    m_backgdrodEnlb = new QLabel(myLan.background_bar);
    m_backgdrodEnsw = new mySwitchButton();
    m_backgdrodEnsw->setFixedSize(100,60);

    m_clenjarEnlb = new QLabel(myLan.clean_tank_alarm_enable);
    m_cleanjarEnsw = new mySwitchButton();
    m_cleanjarEnsw->setFixedSize(100,60);

    m_AiralarmEnlb = new QLabel(myLan.pressure_alarm_enable);
    m_AiralarmEnsw = new mySwitchButton();
    m_AiralarmEnsw->setFixedSize(100,60);
    m_AiralarmEnlb->hide();
    m_AiralarmEnsw->hide();

    m_SNVEnlb = new QLabel(myLan.elec_valve_enable);
    m_SNVsw = new mySwitchButton();
    m_SNVsw->setFixedSize(100,60);

    m_ValEnlb = new QLabel(myLan.low_valve);
    m_Valsw = new mySwitchButton();
    m_Valsw->setFixedSize(100,60);


    m_MetalEnlb = new QLabel(myLan.metal_detect);
    m_Metalsw = new mySwitchButton();
    m_Metalsw->setFixedSize(100,60);


    m_multiVibratorlb = new QLabel(myLan.multi_vibrator);
    m_multiVibratorsw = new mySwitchButton();
    m_multiVibratorsw->setFixedSize(100,60);


    //! 电磁阀
    m_SNVGb = new QGroupBox(myLan.elec_valve_set);
//    m_SNVGb->setFixedSize(400,800);
    m_SNVGb->setFixedWidth(400);
    for(int i=0; i<MAX_SNV_CHANNEL; i++){
        m_SNVChannellb[i] = new QLabel(QString("%1%2").arg(myLan.channel).arg(i+1));
        m_SVNEnsw[i] = new mySwitchButton();
        m_SVNEnsw[i]->setFixedSize(100,60);
    }
}

void perpheralPage::laypage()
{
    //! 卤素灯
    QVBoxLayout *vlayHalogen = new QVBoxLayout(m_HalogenGb);
    QHBoxLayout *hlaythd = new QHBoxLayout();
    QHBoxLayout *hlayEn = new QHBoxLayout();
    QHBoxLayout *hlayautocal = new QHBoxLayout();
    QHBoxLayout *hlayInfractrl = new QHBoxLayout();
    QHBoxLayout *hlayInfratemthd = new QHBoxLayout();
    QHBoxLayout *hlaycamtempure = new QHBoxLayout();

    vlayHalogen->setContentsMargins(20,40,20,0);
    hlaythd->addWidget(m_Halogenthdlb);
    hlaythd->addWidget(m_Halogenle);
    hlayEn->addWidget(m_Enhalogenlb);
    hlayEn->addWidget(m_EnHalogenSw);
    hlayautocal->addWidget(m_autoCalhalogenlb);
    hlayautocal->addWidget(m_autoCalHalogenSw);
    hlayInfractrl->addWidget(m_InfraTempthdlb);
    hlayInfractrl->addWidget(m_InfraTemple);
    hlayInfratemthd->addWidget(m_InfraTempCtrlb);
    hlayInfratemthd->addWidget(m_InfraTempCtrSw);
    hlaycamtempure->addWidget(m_infraTempureEnlb);
    hlaycamtempure->addWidget(m_infraTempureEnsw);

    vlayHalogen->addLayout(hlaythd);
    vlayHalogen->addLayout(hlayEn);
    vlayHalogen->addLayout(hlayInfratemthd);
    vlayHalogen->addLayout(hlayInfractrl);
    vlayHalogen->addLayout(hlaycamtempure);
    vlayHalogen->addLayout(hlayautocal);


    //! 伺服
    QHBoxLayout *hlaymotorsw = new QHBoxLayout();
    hlaymotorsw->addWidget(m_motorOnofflb);
    hlaymotorsw->addWidget(m_motorSw);

    QHBoxLayout *hlaymotorsp = new QHBoxLayout();
    hlaymotorsp->addWidget(m_speedlb);
    hlaymotorsp->addWidget(m_motorSpeedle);

    QVBoxLayout *vlaymotor = new QVBoxLayout(m_motorGb);
    vlaymotor->setContentsMargins(10,50,10,10);
    vlaymotor->addLayout(hlaymotorsw);
    vlaymotor->addLayout(hlaymotorsp);
    vlaymotor->addWidget(m_motorInitBtn);

    //! 其他
    QVBoxLayout *vlayotherGb = new QVBoxLayout(m_otherGb);
    vlayotherGb->setContentsMargins(10,50,10,10);
    QHBoxLayout *hlayplus = new QHBoxLayout();
    QHBoxLayout *hlayjaralarm = new QHBoxLayout();
    QHBoxLayout *hlayrod = new QHBoxLayout();
    QHBoxLayout *hlayair = new QHBoxLayout();
    QHBoxLayout *hlaysnv = new QHBoxLayout();
    QHBoxLayout *hlayval = new QHBoxLayout();
    QHBoxLayout *hlaymetal = new QHBoxLayout();
    QHBoxLayout *hlaymulvib = new QHBoxLayout();
    hlayplus->addWidget(m_pluslightEnlb);
    hlayplus->addWidget(m_pluslightEnsw);
    hlayrod->addWidget(m_backgdrodEnlb);
    hlayrod->addWidget(m_backgdrodEnsw);
    hlayjaralarm->addWidget(m_clenjarEnlb);
    hlayjaralarm->addWidget(m_cleanjarEnsw);
    hlayair->addWidget(m_AiralarmEnlb);
    hlayair->addWidget(m_AiralarmEnsw);
    hlaysnv->addWidget(m_SNVEnlb);
    hlaysnv->addWidget(m_SNVsw);
    hlayval->addWidget(m_ValEnlb);
    hlayval->addWidget(m_Valsw);
    hlaymetal->addWidget(m_MetalEnlb);
    hlaymetal->addWidget(m_Metalsw);
    hlaymulvib->addWidget(m_multiVibratorlb);
    hlaymulvib->addWidget(m_multiVibratorsw);

    vlayotherGb->addLayout(hlayplus);
    vlayotherGb->addLayout(hlayrod);
    vlayotherGb->addLayout(hlayval);
    vlayotherGb->addLayout(hlaymetal);
    vlayotherGb->addLayout(hlaysnv);
    vlayotherGb->addLayout(hlayair);
    vlayotherGb->addLayout(hlaymulvib);
    vlayotherGb->addLayout(hlayjaralarm);

    //! 电磁阀
    QGridLayout *glaySNV = new QGridLayout(m_SNVGb);
    glaySNV->setContentsMargins(10,50,10,10);
    for(int i=0; i<MAX_SNV_CHANNEL; i++){
        glaySNV->addWidget(m_SNVChannellb[i],i,0);
        glaySNV->addWidget(m_SVNEnsw[i],i,1);

    }

    QHBoxLayout *hlaywid = new QHBoxLayout(m_wid);
    QVBoxLayout *vlayleft = new QVBoxLayout();
    vlayleft->addWidget(m_HalogenGb);
    vlayleft->addWidget(m_motorGb);

    hlaywid->addLayout(vlayleft);
    hlaywid->addWidget(m_otherGb);
    hlaywid->addWidget(m_SNVGb);

    QVBoxLayout *vlaymain =new QVBoxLayout(this);
    vlaymain->addWidget(m_wid);
}

void perpheralPage::connectAndSlt()
{
    connect(m_Halogenle,SIGNAL(pressed()),this,SLOT(onHalogenthdChg()));
    connect(m_motorInitBtn,SIGNAL(pressed()),this,SLOT(onInitMotoSlt()));
    connect(m_motorSpeedle,SIGNAL(pressed()),this,SLOT(onFixMotoreSpeedSlt()));
    connect(m_motorSw,SIGNAL(checkedChanged(bool)),this,SLOT(onSwitchMotorOnoroffSlt(bool)));
    connect(m_SNVsw,SIGNAL(checkedChanged(bool)),this,SLOT(onSwSnvEnchgSlt(bool)));


    auto bindSw =[this](mySwitchButton *sw, bool analyzeGlobal::*mem){
        connect(sw, &mySwitchButton::checkedChanged, this, [mem](bool bSta){
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->*mem = bSta;
        });
    };
    bindSw(m_multiVibratorsw,&analyzeGlobal::bEnMultivibrator);
    bindSw(m_EnHalogenSw,&analyzeGlobal::bEnHalogen);
    bindSw(m_autoCalHalogenSw,&analyzeGlobal::bEnAutocalIMSensor);
    bindSw(m_pluslightEnsw,&analyzeGlobal::bEnPulseLight);
    bindSw(m_cleanjarEnsw,&analyzeGlobal::bEncleanjar);
    bindSw(m_backgdrodEnsw,&analyzeGlobal::bEnBackdgrod);
    bindSw(m_AiralarmEnsw,&analyzeGlobal::bEnAirAlarm);
    bindSw(m_Metalsw,&analyzeGlobal::bEnmetalDetect);
    bindSw(m_infraTempureEnsw,&analyzeGlobal::bEnCamTempureAlarm);


    connect(m_Valsw, &mySwitchButton::checkedChanged, this, [this](bool bSta){
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal = bSta;
        emit sigEjectorEnChg(bSta);
    });

    connect(m_InfraTempCtrSw, SIGNAL(checkedChanged(bool)), this, SLOT(onInfratempCtrlChgSlt(bool)));
    connect(m_InfraTemple,    SIGNAL(pressed()),         this, SLOT(onInfratempthdChgSlt()));

    QSignalMapper *snvsigmap = new QSignalMapper();
    for(int i=0; i<MAX_SNV_CHANNEL; i++){
        snvsigmap->setMapping(m_SVNEnsw[i], i);
        connect(m_SVNEnsw[i], SIGNAL(checkedChanged(bool)), snvsigmap, SLOT(map()));
    }
    connect(snvsigmap,SIGNAL(mapped(int)),this,SLOT(onSNVChannelOnOff(int)));
}

void perpheralPage::refreshPage()
{
    auto *analyzeInfo = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];
    m_Halogenle->setText(QString("%1").arg(analyzeInfo->nHalogenThd));
    m_motorSpeedle->setText(QString("%1").arg(analyzeInfo->nMotorSpeed));
    m_EnHalogenSw->setChecked(analyzeInfo->bEnHalogen);
    m_pluslightEnsw->setChecked(analyzeInfo->bEnPulseLight);
    m_cleanjarEnsw->setChecked(analyzeInfo->bEncleanjar);
    m_backgdrodEnsw->setChecked(analyzeInfo->bEnBackdgrod);
    m_AiralarmEnsw->setChecked(analyzeInfo->bEnAirAlarm);
    m_SNVsw->setChecked(analyzeInfo->bEnSNV);
    m_motorSw->setChecked(struGsh.bBeltOnff);

    m_Valsw->setChecked(analyzeInfo->bEnVal);
    m_Metalsw->setChecked(analyzeInfo->bEnmetalDetect);
    m_multiVibratorsw->setChecked(analyzeInfo->bEnMultivibrator);

    m_infraTempureEnsw->setChecked(analyzeInfo->bEnCamTempureAlarm);
    m_InfraTemple->setText(QString("%1").arg(analyzeInfo->nInfratempthd));
    m_InfraTempCtrSw->setChecked(analyzeInfo->bEninfratempCtrl);

    if(struCnfg.nMachineType == nMachine_Bottle){
        m_pluslightEnlb->hide();
        m_pluslightEnsw->hide();
        m_ValEnlb->hide();
        m_Valsw->hide();

        m_Metalsw->show();
        m_MetalEnlb->show();
        m_backgdrodEnlb->show();
        m_backgdrodEnsw->show();
    }else{
        m_pluslightEnlb->show();
        m_pluslightEnsw->show();
        m_ValEnlb->show();
        m_Valsw->show();

        m_Metalsw->hide();
        m_MetalEnlb->hide();
        m_backgdrodEnlb->hide();
        m_backgdrodEnsw->hide();
    }

    if(analyzeInfo->bHasAssist){
        m_HalogenGb->show();
    }else{
        m_HalogenGb->hide();
    }
}

/*!
 * \brief perpheralPage::onHalogenthdChg
 */
void perpheralPage::onHalogenthdChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 1000, m_Halogenle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_Halogenle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd = n_sample;
    }
}

void perpheralPage::onInitMotoSlt()
{
    g_serial->ResetMotorParams();
}

void perpheralPage::onFixMotoreSpeedSlt()
{
    float nspeed = m_motorSpeedle->text().toFloat();
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 3.0, nspeed, "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        m_motorSpeedle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMotorSpeed = n_sample;
        int nspeed = (int)(n_sample*607);
        g_serial->SendMotorSpeed(nspeed*10);


        struGsh.nlegthRatio = 1000*struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMotorSpeed/struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nFreQuence[0];

    }
}

void perpheralPage::onSwitchMotorOnoroffSlt(bool status)
{
    struGsh.bBeltOnff = status;
    g_serial->SwMotorOnOff(status);
    emit sigrefreshBeltStatus();
}



/*!
 * \brief 电磁阀使能
 * \param sta
 */
void perpheralPage::onSwSnvEnchgSlt(bool sta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnSNV = sta;
    gFlow.sendSNVEnstatus(struGsh.nAnalyzeIndex);
}

/*!
 * \brief SNV通道开关使能
 * \param nIdx
 */
void perpheralPage::onSNVChannelOnOff(int nIdx)
{
    Q_UNUSED(nIdx);
    quint8 data =0;
    for(int i=0; i<MAX_SNV_CHANNEL; i++){
        if(m_SVNEnsw[i]->getChecked()){
            data = data | (1<<i);
        }
    }
    gFlow.snvchannelonoffctrl(data);
}


void perpheralPage::onInfratempCtrlChgSlt(bool bSta)
{
   struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEninfratempCtrl = bSta;
   gFlow.setHSTempCtrl(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd,bSta);
}

void perpheralPage::onInfratempthdChgSlt()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 100, m_InfraTemple->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_InfraTemple->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd = n_sample;
        gFlow.setHSTempCtrl(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd,
                            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEninfratempCtrl);
    }
}
