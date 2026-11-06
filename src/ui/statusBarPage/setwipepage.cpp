#include "setwipepage.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"
#include "common/myinputpanel.h"

setWipePage::setWipePage(QWidget *parent) : QDialog(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief setWipePage::初始化参数
 */
void setWipePage::initParams()
{
    nAnalyzeNum = struCnfg.analyzeNum;
}

/**
 * @brief setWipePage::搭建界面
 */
void setWipePage::createPage()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    m_widget = new QWidget(this);
    m_widget->setWindowFlags(Qt::FramelessWindowHint);
    m_widget->setGeometry((WIN_WID+30 - 850)/2, (WIN_HEI - 600)/2, 850, 650);
    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    m_groupBox = new QGroupBox(myLan.clean_set);
    m_groupBox->setFixedSize(830,550);

    //! 清灰间隔
    QHBoxLayout *upLayout = new QHBoxLayout();
    m_WipeLbl  = new QLabel(QString("%1(%2)").arg(myLan.clean_interval).arg(myLan.times));
    m_WipeLbl->setFixedSize(BTN_WID+100, BTN_HEI-15);
    m_WipeLbl->setAlignment(Qt::AlignLeft);
    m_btnWipeValue = new QPushButton(QString("%1").arg(15));
    m_btnWipeValue->setStyleSheet("background-color:white;");
    m_btnWipeValue->setFixedSize(BTN_WID, BTN_HEI-15);
    upLayout->addWidget(m_WipeLbl);
    upLayout->addWidget(m_btnWipeValue);

    //! 清灰时间
    QHBoxLayout *midLayout = new QHBoxLayout();
    m_WipeDelayLbl  = new QLabel(QString("%1(ms)").arg(myLan.clean_time));
    m_WipeDelayLbl->setFixedSize(BTN_WID+100, BTN_HEI-15);
    m_WipeDelayLbl->setAlignment(Qt::AlignLeft);
    m_btnWipeDelayValue = new QPushButton("30");
    m_btnWipeDelayValue->setStyleSheet("background-color:white;");
    m_btnWipeDelayValue->setFixedSize(BTN_WID, BTN_HEI-15);
    midLayout->addWidget(m_WipeDelayLbl);
    midLayout->addWidget(m_btnWipeDelayValue);

    //! 风刀控制
    QHBoxLayout *hlaykniftime = new QHBoxLayout();
    m_Airknifetimelb  = new QLabel(QString("%1(s)").arg(myLan.airknife_time));
    m_Airknifetimelb->setFixedSize(BTN_WID+100, BTN_HEI-15);
    m_Airknifetimelb->setAlignment(Qt::AlignLeft);
    m_AirknifeTimebtn = new QPushButton("30");
    m_AirknifeTimebtn->setStyleSheet("background-color:white;");
    m_AirknifeTimebtn->setFixedSize(BTN_WID, BTN_HEI-15);
    m_Airknifetimelb->hide();
    m_AirknifeTimebtn->hide();
    hlaykniftime->addWidget(m_Airknifetimelb);
    hlaykniftime->addWidget(m_AirknifeTimebtn);

    QHBoxLayout *hlayknifchanel = new QHBoxLayout();
    m_Airknifechanellb  = new QLabel(QString("%1").arg(myLan.airknife_channel));
    m_Airknifechanellb->setFixedSize(BTN_WID+100, BTN_HEI-15);
    m_Airknifechanellb->setAlignment(Qt::AlignLeft);
    m_AiarknifChanelbtn = new QPushButton("8");
    m_AiarknifChanelbtn->setStyleSheet("background-color:white;");
    m_AiarknifChanelbtn->setFixedSize(BTN_WID, BTN_HEI-15);
    hlayknifchanel->addWidget(m_Airknifechanellb);
    hlayknifchanel->addWidget(m_AiarknifChanelbtn);

    //! 清灰按钮
    QHBoxLayout *downLayout = new QHBoxLayout();
    m_btnWipe = new QPushButton(myLan.clean);
    m_btnWipe->setIcon(QIcon(":/res/png/action_clean.png"));
    m_btnWipe->setIconSize(QSize(BTN_HEI-5, BTN_HEI-10));
    m_btnWipe->setFixedSize(BTN_WID*1.5, BTN_HEI);
    // m_btnWipe->setStyleSheet(g_enableBtnStyle);

    //! 清灰缸报警复位
    m_resetjarBtn = new QPushButton(myLan.clean_tank_reset);
    m_resetjarBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_resetjarBtn->setIcon(QIcon(":/res/png/Alarm_reset.png"));
    m_resetjarBtn->setIconSize(QSize(40,40));
    downLayout->addWidget(m_resetjarBtn,0,Qt::AlignLeft);
    downLayout->addWidget(m_btnWipe,0,Qt::AlignCenter);
    downLayout->addSpacing(230);


    QVBoxLayout *boxLayout = new QVBoxLayout(m_groupBox);
    boxLayout->addStretch(0);
    boxLayout->addLayout(upLayout);
    boxLayout->addStretch(0);
    boxLayout->addLayout(midLayout);
    boxLayout->addStretch(0);
    boxLayout->addLayout(hlayknifchanel);
    boxLayout->addStretch(0);
    boxLayout->addLayout(hlaykniftime);
    boxLayout->addStretch(0);
    boxLayout->addLayout(downLayout);


    m_backBtn = new QPushButton(myLan.back);
    m_backBtn->setFixedSize(BTN_WID, BTN_HEI);
    QHBoxLayout *backLayout = new QHBoxLayout();
    backLayout->addStretch(0);
    backLayout->addWidget(m_backBtn);

    QVBoxLayout *vBox = new QVBoxLayout(m_widget);
    vBox->addStretch(0);
    vBox->addWidget(m_groupBox);
    vBox->addStretch(0);
    vBox->addLayout(backLayout);
}

/**
 * @brief setWipePage::信号槽绑定
 */
void setWipePage::connectSigAndSlot()
{
    connect(m_btnWipeValue,      SIGNAL(pressed()),              this,  SLOT(onWipeValueBtnPressedSlt()));
    connect(m_btnWipeDelayValue, SIGNAL(pressed()),              this,  SLOT(onWipeDelayValuePressedSlt()));
    connect(m_btnWipe,           SIGNAL(pressed()),              this,  SLOT(onManulWipeBtnPreesedSlt()));
    connect(m_backBtn,           SIGNAL(pressed()),              this,  SLOT(onBackBtnPressedSlt()));
    connect(m_AirknifeTimebtn,   SIGNAL(pressed()),              this,   SLOT(onAirknifeValuechgSlt()));
    connect(m_AiarknifChanelbtn, SIGNAL(pressed()),              this,   SLOT(onAirknifeChannelSlt()));
}


/**
 * @brief setWipePage::清灰间隔设置
 */
void setWipePage::onWipeValueBtnPressedSlt()
{
    int nWipeInterval = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeInterval;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 720, nWipeInterval, myLan.clean_interval, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeInterval = inputPanel->getValue();
        m_btnWipeValue->setText(QString("%1").arg(inputPanel->getValue()));
    }
}

/**
 * @brief setWipePage::清灰时间设置
 */
void setWipePage::onWipeDelayValuePressedSlt()
{
    int nWipeInterval = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeDelay;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 6000, nWipeInterval, myLan.clean_time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeDelay = inputPanel->getValue();
        m_btnWipeDelayValue->setText(QString("%1").arg(inputPanel->getValue()));
    }
}

/**
 * @brief setWipePage::手动清灰
 */
void setWipePage::onManulWipeBtnPreesedSlt()
{
    if (struGsh.nIsWiping[struGsh.nAnalyzeIndex] == 1) return;
    struGsh.nIsWiping[struGsh.nAnalyzeIndex] = 1;
    infoWidget->setLabelText(QString("%1...").arg(myLan.clean));
    infoWidget->raise();
    infoWidget->show();
    gFlow.sendWipePara(1,struGsh.nAnalyzeIndex);
    quint8 data =0;
    data = (1<<(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel-1));
    gFlow.snvchannelonoffctrl(data);
    gFlow.msleep(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeDelay*2+50);
    gFlow.snvchannelonoffctrl(0);
    infoWidget->hide();
    struGsh.nIsWiping[struGsh.nAnalyzeIndex] = 0;
}

/*!
 * \brief 风刀时间
 */
void setWipePage::onAirknifeValuechgSlt()
{
    int ntime = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifetime;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 100, ntime, myLan.time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifetime = inputPanel->getValue();
        m_AirknifeTimebtn->setText(QString("%1").arg(inputPanel->getValue()));
    }
}

/*!
 * \brief 风刀通道
 */
void setWipePage::onAirknifeChannelSlt()
{
    int nchanel = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 8, nchanel, myLan.channel, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel = inputPanel->getValue();
        m_AiarknifChanelbtn->setText(QString("%1").arg(inputPanel->getValue()));
    }
}

/**
 * @brief setWipePage::返回按钮
 */
void setWipePage::onBackBtnPressedSlt()
{
    this->hide();
}

void setWipePage::onResetjarSlt()
{
     gFlow.Setorgetcleanjarstatus(2);
}

/**
 * @brief setWipePage::界面刷新
 */
void setWipePage::refreshDisplay()
{
    m_btnWipeValue->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeInterval));
    m_btnWipeDelayValue->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nWipeDelay));
    m_AiarknifChanelbtn->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifechanel));
    m_AirknifeTimebtn->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nknifetime));
}

