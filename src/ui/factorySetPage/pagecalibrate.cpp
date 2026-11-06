#include "pagecalibrate.h"

pagecalibrate::pagecalibrate(QWidget *parent) : QWidget(parent)
{
    createwid();
    laypage();
    connectAndslt();
}

/*!
 * \brief 初始化页面
 */
void pagecalibrate::createwid()
{
    m_mainwid = new QWidget();

    m_txtedit = new QTextEdit();
    m_txtedit->setWordWrapMode(QTextOption::WordWrap);

    m_piexlsetGb = new QGroupBox(myLan.cal_coord);
    m_piexlsetGb->setMaximumHeight(400);
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_camNamelb[i] = new QLabel();
        m_camNamelb[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        m_noffpiexle[i] = new MyLineEdit();
        m_noffpiexle[i]->setFixedWidth(120);
        m_noffpiexle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_noffpiexle[i]->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamoffPiexl[i]));
    }
    m_noffpiexle[0]->setDisabled(true);

    m_calibratebtn = new QPushButton(myLan.calibrate);
    m_calibratebtn->setFixedSize(BTN_WID+40,BTN_HEI);
    m_calibratebtn->setStyleSheet(g_enableBtnStyle);


    //! 含水量标定
    m_waterCalGb = new QGroupBox(myLan.water_cal);
    m_calIndexlb = new QLabel(myLan.select_cal_group);
    m_calIndexcomb = new QComboBox();
    m_calIndexcomb->setFixedSize(BTN_WID,BTN_HEI);
    QListWidget *calIndexlistwid = new QListWidget();
    m_calIndexcomb->setModel(calIndexlistwid->model());
    m_calIndexcomb->setView(calIndexlistwid);
    m_calIndexcomb->setFixedSize(BTN_WID+40,BTN_HEI);


    for(int i=0; i<MAX_CALIBRATE_NUM; i++){
        m_titlelb[i] = new QLabel(QString("%1").arg(i+1));
        m_oriValuelb[i] = new QLabel();
        m_waterValuele[i] = new MyLineEdit();
        m_calIndexcomb->insertItem(i,QString("%1").arg(i+1));
    }

    m_calIndexcomb->setCurrentIndex(-1);
    m_calWaterBtn = new QPushButton(myLan.calibrate);
    m_calWaterBtn->setFixedSize(BTN_WID+40,BTN_HEI);
    m_calWaterBtn->setStyleSheet(g_enableBtnStyle);
    m_calWaterApplyBtn = new QPushButton(myLan.apply);
    m_calWaterApplyBtn->setFixedSize(BTN_WID+40,BTN_HEI);
    m_calWaterApplyBtn->setStyleSheet(g_enableBtnStyle);
    m_clearCalBtn = new QPushButton(myLan.clear);
    m_clearCalBtn->setFixedSize(BTN_WID+40,BTN_HEI);
    m_clearCalBtn->setStyleSheet(g_enableBtnStyle);

}

/*!
 * \brief 页面总布局
 */
void pagecalibrate::laypage()
{
    QGridLayout *glayleft = new QGridLayout();
    glayleft->setContentsMargins(10,50,10,10);
    for(int i=0; i<MAX_CAM_NUM; i++){
        glayleft->addWidget(m_camNamelb[i],i,0);
        glayleft->addWidget(m_noffpiexle[i],i,1);
    }

    QVBoxLayout *vlaymid = new  QVBoxLayout();
    vlaymid->setContentsMargins(10,50,10,10);
    vlaymid->addWidget(m_txtedit);

    QVBoxLayout *vlayrigt = new QVBoxLayout();
    vlayrigt->addWidget(m_calibratebtn);
    vlayrigt->addStretch();

    QHBoxLayout *hlaycoord = new QHBoxLayout(m_piexlsetGb);
    hlaycoord->addLayout(glayleft);
    hlaycoord->addSpacing(30);
    hlaycoord->addLayout(vlaymid);
    hlaycoord->addLayout(vlayrigt);


    //! 水分标定
    QGridLayout *glaywater = new QGridLayout();
    for(int i=0; i<MAX_CALIBRATE_NUM; i++){
        if(i%8 == 0){
            glaywater->addWidget(new QLabel(QString("%1:  ").arg(myLan.cal_group)),(i/8)*3,0);
            glaywater->addWidget(new QLabel(QString("%1:  ").arg(myLan.cal_value)),(i/8)*3+1,0);
            glaywater->addWidget(new QLabel(QString("%1:  ").arg(myLan.actual_value)),(i/8)*3+2,0);
        }
        glaywater->addWidget(m_titlelb[i],(i/8)*3,i%8+1);
        glaywater->addWidget(m_oriValuelb[i],(i/8)*3+1,i%8+1);
        glaywater->addWidget(m_waterValuele[i],(i/8)*3+2,i%8+1);
    }


    QVBoxLayout *vlaywater = new QVBoxLayout();
    vlaywater->addWidget(m_calIndexlb);
    vlaywater->addWidget(m_calIndexcomb);
    vlaywater->addSpacing(20);
    vlaywater->addWidget(m_calWaterBtn);
    vlaywater->addSpacing(20);
    vlaywater->addWidget(m_clearCalBtn);
    vlaywater->addSpacing(20);
    vlaywater->addWidget(m_calWaterApplyBtn);

    QHBoxLayout *hlaywater = new QHBoxLayout(m_waterCalGb);
    hlaywater->setContentsMargins(10,50,10,10);
    hlaywater->addLayout(glaywater);
    hlaywater->addStretch();
    hlaywater->addLayout(vlaywater);


    QVBoxLayout *laywid = new QVBoxLayout(m_mainwid);
    laywid->addWidget(m_piexlsetGb);
    laywid->addWidget(m_waterCalGb);

    this->setLayout(laywid);
}

/*!
 * \brief 信号和槽
 */
void pagecalibrate::connectAndslt()
{
    QSignalMapper* sigcalibratelemap = new QSignalMapper();
    for(int i=0; i<MAX_CAM_NUM; i++){
        sigcalibratelemap->setMapping(m_noffpiexle[i], i);
        connect(m_noffpiexle[i], SIGNAL(pressed()), sigcalibratelemap, SLOT(map()));
    }
    connect(sigcalibratelemap,SIGNAL(mapped(int)),this,SLOT(onCalibrateValChg(int)));

    connect(m_calibratebtn,SIGNAL(pressed()),this,SLOT(onAutoCalibrateSlt()));


    QSignalMapper* sigwatercallemap = new QSignalMapper();
    for(int i=0; i<MAX_CALIBRATE_NUM; i++){
        sigwatercallemap->setMapping(m_waterValuele[i], i);
        connect(m_waterValuele[i], SIGNAL(pressed()), sigwatercallemap, SLOT(map()));
    }
    connect(sigwatercallemap,SIGNAL(mapped(int)),this,SLOT(onWatercalAcutalChg(int)));
    connect(m_calWaterBtn,SIGNAL(pressed()), this, SLOT(onCalWaterBtnSlt()));
    connect(m_calWaterApplyBtn,SIGNAL(pressed()), this, SLOT(onApplyCalWaterParamsBtnSlt()));
    connect(m_clearCalBtn, SIGNAL(pressed()), this, SLOT(onClearBtnPressed()));
    connect(m_clearCalBtn, SIGNAL(released()), this, SLOT(onClearBtnReleased()));
}

/*!
 * \brief 刷新页面
 */
void pagecalibrate::refreshpageCalibrate()
{
    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum){
            m_camNamelb[i]->show();
            m_noffpiexle[i]->show();
            m_camNamelb[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
            m_noffpiexle[i]->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamoffPiexl[i]));
        }else{
            m_camNamelb[i]->hide();
            m_noffpiexle[i]->hide();
        }
    }
}

/*!
 * \brief 槽：标定阈值
 * \param nIdx
 */
void pagecalibrate::onCalibrateValChg(int nIdx)
{

}

/*!
 * \brief 自动标定
 */
void pagecalibrate::onAutoCalibrateSlt()
{
    m_calibratebtn->setDisabled(true);
    struGsh.bCalibrate = true;
    for(int i=0; i<MAX_CAM_NUM; i++){
        struGsh.calipoint[i].setX(0);
        struGsh.calipoint[i].setY(0);
    }
    m_txtedit->clear();

    infoWidget->raise();
    infoWidget->delayShow();
    infoWidget->setLabelText(myLan.apply_and_wait);

    //! 开关供料
    gFlow.getSpecifyAnalyzeShare(0)->bFeeder = true;
    gFlow.sendSocOnOff(0);
    gFlow.msleep(3000);
    gFlow.sendOnOff(0);
    g_serial->SwMotorOnOff(true);


    //! 同步等待
    QEventLoop loop;
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    timer.start(4500);
    loop.exec();


    //!  关供料
    gFlow.getSpecifyAnalyzeShare(0)->bFeeder = false;
    gFlow.sendOnOff(0);
    gFlow.sendSocOnOff(0);
    g_serial->SwMotorOnOff(false);


    //! 校验结果
    bool bsucess = true;
    QString caliinfostr;
    for(int i=0; i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i++){
        if((struGsh.calipoint[i].x()==0) && (struGsh.calipoint[i].y()==0)){
            bsucess = false;
            continue;
        }else{
            caliinfostr += struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i];
            caliinfostr += QString(":  x=%1  y=%2").arg(struGsh.calipoint[i].x()).arg(struGsh.calipoint[i].y());
            caliinfostr += "\n";
        }
    }

    m_txtedit->setText(caliinfostr);
    struGsh.bCalibrate = false;
    m_calibratebtn->setDisabled(false);

    if(bsucess){
        infoWidget->setLabelText(QString("%1%2").arg(myLan.calibrate).arg(myLan.sucess));
        for(int i=1; i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i++){
           if(i==1){
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamoffPiexl[i] = struGsh.calipoint[i].x() - struGsh.calipoint[0].x();
           }else{
               struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamoffPiexl[i] = (1024-struGsh.calipoint[i].x()) - struGsh.calipoint[0].x();
           }
       }

        for(int i=0; i<struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMajorCamNum; i++){
           m_noffpiexle[i]->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nCamoffPiexl[i]));
       }
    }else{
        infoWidget->setLabelText(QString("%1%2").arg(myLan.calibrate).arg(myLan.failed));
    }

    infoWidget->showSecs();
}

void pagecalibrate::onWatercalAcutalChg(int nIdx)
{
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 100.0, m_waterValuele[nIdx]->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        m_waterValuele[nIdx]->setText(QString::number(n_sample,'f',1));
    }
}

void pagecalibrate::onCalWaterBtnSlt()
{
    struGsh.nCalValue = 0;

    int nIdex = m_calIndexcomb->currentIndex();
    if(nIdex ==-1){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.select_cal_group_first));
        infoWidget->showSecs();
        return;
    }

    struGsh.bCalWaterContent = true;

    m_calWaterBtn->setDisabled(true);
    m_oriValuelb[nIdex]->clear();

    infoWidget->raise();
    infoWidget->delayShow();
    infoWidget->setLabelText(myLan.apply_and_wait);

    //! 开关供料
    gFlow.getSpecifyAnalyzeShare(0)->bFeeder = true;
    gFlow.sendSocOnOff(0);
    gFlow.msleep(3000);
    gFlow.sendOnOff(0);
    g_serial->SwMotorOnOff(true);


    //! 同步等待
    QEventLoop loop;
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    timer.start(4500);
    loop.exec();


    //!  关供料
    gFlow.getSpecifyAnalyzeShare(0)->bFeeder = false;
    gFlow.sendOnOff(0);
    gFlow.sendSocOnOff(0);
    g_serial->SwMotorOnOff(false);


    //! 校验结果
    bool bsucess = struGsh.nCalValue ==0?false:true;

    if(bsucess){
        infoWidget->setLabelText(QString("%1%2 \n%3").arg(myLan.calibrate).arg(myLan.sucess)
                                     .arg(myLan.please_input_calvalue));
        m_oriValuelb[nIdex]->setText(QString::number(struGsh.nCalValue,'f',1));
    }else{
        infoWidget->setLabelText(QString("%1%2").arg(myLan.calibrate).arg(myLan.failed));
    }

    struGsh.bCalWaterContent = false;
    m_calWaterBtn->setDisabled(false);
    m_calIndexcomb->setCurrentIndex(-1);
    infoWidget->showSecs();

}

void pagecalibrate::onApplyCalWaterParamsBtnSlt()
{
    QVector<float> vValue;
    int nval =0;
    for(int i=0; i<MAX_CALIBRATE_NUM; i++){
        if(!m_waterValuele[i]->text().isEmpty()
            && !m_oriValuelb[i]->text().isEmpty()
            && m_oriValuelb[i]->text().toFloat()!=0){
            nval++;
            float nval = QString::number(m_waterValuele[i]->text().toFloat(),'f',1).toFloat();
            float nval2 = QString::number(m_oriValuelb[i]->text().toFloat(),'f',1).toFloat();

            vValue.push_back(QString::number(m_waterValuele[i]->text().toFloat(),'f',1).toFloat());
            vValue.push_back(QString::number(m_oriValuelb[i]->text().toFloat(),'f',1).toFloat());
        }
    }

    if(nval>1){
        gFlow.sendCalibrateVal2Soc(vValue,false);
    }else{
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.please_calibrate_first));
        infoWidget->showSecs();
    }
}

/*!
 * \brief 清除水分标定
 */
void pagecalibrate::onClearwatercalSlt()
{
    QVector<float> vValue;
    for(int i=0; i<MAX_CALIBRATE_NUM; i++){
        m_oriValuelb[i]->clear();
        m_waterValuele[i]->clear();
    }
    gFlow.sendCalibrateVal2Soc(vValue,true);
}

void pagecalibrate::onClearBtnPressed()
{
    m_clearBtnTimer.start();
}

void pagecalibrate::onClearBtnReleased()
{
    const int LONG_PRESS_MS = 2000;
    qint64 elapsed = m_clearBtnTimer.elapsed();

    if (elapsed < LONG_PRESS_MS) {
        for (int i = 0; i < MAX_CALIBRATE_NUM; ++i) {
            m_oriValuelb[i]->clear();
            m_waterValuele[i]->clear();
        }
    } else {
        myMessageBox msg(MSG_QUES,QString("%1?").arg(myLan.confirm_del), myLan.confirm_del, g_mainWidget);
        int ret = msg.exec();
        if (ret == QDialog::Accepted) {
            // reuse existing function which clears and sends
            onClearwatercalSlt();
        }
    }
}
