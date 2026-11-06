#include "setfeederpage.h"

setFeederPage::setFeederPage(QWidget *parent) : QDialog(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief setFeederPage::初始化参数
 */
void setFeederPage::initParams()
{
    feeder_set = QString("%1").arg(myLan.feeder_set);
    feeder_value = myLan.vibrate_magnitude;
    getFeederParams();

    sliderMap = new QSignalMapper();
    feederValueMap = new QSignalMapper();
    feederswonOffMap = new QSignalMapper();
}

/**
 * @brief setFeederPage::搭建界面
 */
void setFeederPage::createPage()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    m_widget = new QWidget(this);
    m_widget->setWindowFlags(Qt::FramelessWindowHint);
    m_widget->setGeometry((WIN_WID - 800)/2, (WIN_HEI - 650)/2, 800, 650);
    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");
    m_widget->show();

    m_titleLbl = new QLabel(feeder_set);
    m_titleLbl->setAlignment(Qt::AlignCenter);
    m_titleLbl->setFixedSize(BTN_WID,BTN_HEI-20);
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addSpacing(40);
    topLayout->addWidget(m_titleLbl);
    topLayout->addStretch(0);

    QStringList titlelst;
    titlelst << myLan.vibrator << myLan.external_vibrator;

    groupBox = new QGroupBox();
    groupBox->setFixedSize(700, 500);

    for (int i = 0; i <MAX_VIBRATOR; i++) {
        feederLbl[i] = new QLabel(titlelst.at(i));
        feederLbl[i]->setFixedHeight(BTN_HEI);
        feederLbl[i]->setMinimumWidth(BTN_WID);
        feederLbl[i]->setFixedSize(BTN_WID, BTN_HEI-40);
        feederLbl[i]->setAlignment(Qt::AlignCenter);

        m_slider[i] = new QSlider(Qt::Vertical);
        m_slider[i]->setFocusPolicy(Qt::StrongFocus);
        m_slider[i]->setTickPosition(QSlider::TicksRight);
        m_slider[i]->setFixedSize(70, 260);
        m_slider[i]->setRange(0, 100);
        m_slider[i]->setPageStep(1);
        sliderMap->setMapping(m_slider[i], i);
        connect(m_slider[i], SIGNAL(valueChanged(int)), sliderMap, SLOT(map()));

        feederValueBtn[i] = new QPushButton();
        feederValueBtn[i]->setFixedSize(BTN_WID/2+10, BTN_HEI-10);
        feederValueMap->setMapping(feederValueBtn[i], i);
        connect(feederValueBtn[i], SIGNAL(pressed()), feederValueMap, SLOT(map()));

        m_feedonoffsw[i] = new mySwitchButton();
        m_feedonoffsw[i]->setFixedSize(BTN_WID, BTN_HEI-10);
        feederswonOffMap->setMapping(m_feedonoffsw[i], i);
        connect(m_feedonoffsw[i], SIGNAL(checkedChanged(bool)), feederswonOffMap, SLOT(map()));
    }


    QHBoxLayout *midLayout = new QHBoxLayout(groupBox);
    for(int j=0; j<MAX_VIBRATOR; j++){
        QVBoxLayout* vlayitem = new QVBoxLayout();
        vlayitem->addWidget(feederLbl[j], 0, Qt::AlignCenter);
        vlayitem->addWidget(m_slider[j], 0, Qt::AlignCenter);
        vlayitem->addWidget(feederValueBtn[j], 0, Qt::AlignCenter);
        vlayitem->addWidget(m_feedonoffsw[j], 0, Qt::AlignCenter);
        midLayout->addLayout(vlayitem);
    }

    m_backBtn = new QPushButton();
    m_backBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_backBtn->setText(myLan.back);
    QHBoxLayout *downLayout = new QHBoxLayout();
    downLayout->addWidget(m_backBtn, 0, Qt::AlignRight);

    QVBoxLayout *vLay = new QVBoxLayout(m_widget);
    vLay->addLayout(topLayout);
    vLay->addWidget(groupBox,0,Qt::AlignCenter);
    vLay->addLayout(downLayout);
}

/**
 * @brief setFeederPage::信号槽绑定
 */
void setFeederPage::connectSigAndSlot()
{
    connect(feederValueMap,   SIGNAL(mapped(int)),  this,   SLOT(onFeederValueBtnSlt(int)));
    connect(sliderMap,        SIGNAL(mapped(int)),  this,   SLOT(onSliderValueChangedSlt(int)));
    connect(m_backBtn,        SIGNAL(pressed()),    this,   SLOT(onBackBtnPressedSlt()));
    connect(feederswonOffMap, SIGNAL(mapped(int)), this,    SLOT(onfeederonoffchg(int)));
}

/**
 * @brief setFeederPage::供料量设置
 */
void setFeederPage::onFeederValueBtnSlt(int index)
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 100, nFeeder[index], feeder_value, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        nFeeder[index] = inputPanel->getValue();
        feederValueBtn[index]->setText(QString("%1").arg(nFeeder[index]));
        m_slider[index]->setValue(nFeeder[index]);
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nFeeder[index] = nFeeder[index];
        if(struGsh.nFeederState){
            gFlow.sendFeederPara(struGsh.nAnalyzeIndex);
        }else{
            onfeederonoffchg(index);
        }
    }
}

/**
 * @brief setFeederPage::onSliderValueChangedSlt
 * @param value
 */
void setFeederPage::onSliderValueChangedSlt(int index)
{
    nFeeder[index] = m_slider[index]->value();
    feederValueBtn[index]->setText(QString("%1").arg(nFeeder[index]));
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nFeeder[index] = nFeeder[index];

    if(struGsh.nFeederState){
        gFlow.sendFeederPara(struGsh.nAnalyzeIndex);
    }else{
        onfeederonoffchg(index);
    }
}

/**
 * @brief setFeederPage::返回按钮
 */
void setFeederPage::onBackBtnPressedSlt()
{
    this->close();
}

/*!
 * \brief 振动器开关
 * \param index
 */
void setFeederPage::onfeederonoffchg(int index)
{
    Q_UNUSED(index);
    gFlow.sendFeederPara(struGsh.nAnalyzeIndex, true,
                         m_feedonoffsw[0]->getChecked(),
                         m_feedonoffsw[1]->getChecked());
}

/**
 * @brief setFeederPage::获取振动量
 */
void setFeederPage::getFeederParams()
{
    for(int j=0; j<MAX_VIBRATOR; j++){
        nFeeder[j] = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nFeeder[j];
    }
}

/**
 * @brief setFeederPage::刷新界面
 */
void setFeederPage::refreshDisplay()
{
    getFeederParams();

    bool bEnmuti = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnMultivibrator;
    for(int i=1;i<MAX_VIBRATOR; i++){
        feederLbl[i]->setHidden(!bEnmuti);
        m_slider[i]->setHidden(!bEnmuti);
        feederValueBtn[i]->setHidden(!bEnmuti);
        m_feedonoffsw[i]->setHidden(!bEnmuti);
    }

    for(int j=0; j<MAX_VIBRATOR; j++){
        feederValueBtn[j]->setText(QString("%1").arg(nFeeder[j]));
        m_slider[j]->setValue(nFeeder[j]);
        m_feedonoffsw[j]->setDisabled(struGsh.nFeederState);
    }
}
