/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymessagebox.cpp
 * \brief       消息框控件源文件
 * \date        2015.01.14
 */
#include "mymessagebox.h"

myMessageBox::myMessageBox(int style, QString text, QString titletext, QWidget *parent) :
    QDialog(parent)
{

    isChecked = false;

    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    widget = new QWidget(this);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setGeometry(WIN_WID/3, WIN_HEI/4, 750, 480);
    widget->setStyleSheet(QString("background-color:%1;").arg(g_ColorLightGray3));
    widget->show();

    iconLabel   = new QLabel(titletext, widget);
    iconLabel->setFixedSize(750, 60);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(QString("color:white; background-color:%1;").arg(g_ColorMYGreen));
    iconLabel->setScaledContents(true);
    QHBoxLayout* hlaytilelab = new QHBoxLayout();
    hlaytilelab->addWidget(iconLabel);

    textLabel = new QLabel(text);
    textLabel->setStyleSheet("font:28px;");
    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignHCenter);
    textLabel->setMinimumWidth(550);
    batchBtn = new QPushButton();
    batchBtn->setMinimumWidth(390);
    batchBtn->hide();
    QHBoxLayout *firstRowLayout = new QHBoxLayout();
    firstRowLayout->addWidget(textLabel,1,Qt::AlignCenter);
    firstRowLayout->addWidget(batchBtn,2,Qt::AlignLeft);


    shiftLb = new QLabel(myLan.detector);
    shiftLb->setStyleSheet("font:28px;");
    shiftLb->setWordWrap(true);
    shiftLb->setAlignment(Qt::AlignHCenter);
    shiftLb->hide();
    shiftBtn = new QPushButton(widget);
    shiftBtn->setMinimumWidth(390);
    shiftBtn->hide();
    QHBoxLayout *secondRowLayout = new QHBoxLayout();
    secondRowLayout->addWidget(shiftLb,1,Qt::AlignCenter);
    secondRowLayout->addWidget(shiftBtn,2,Qt::AlignLeft);


    yieldlb = new QLabel(QString("%1(kg)").arg(myLan.yield));
    yieldlb->setStyleSheet("font:28px;");
    yieldlb->hide();
    yieldbtn = new QPushButton("2.0");
    yieldbtn->setMinimumWidth(390);
    yieldbtn->setStyleSheet("text-align:left;");
    yieldbtn->hide();
    QHBoxLayout *thirdRowLayout = new QHBoxLayout();
    thirdRowLayout->addWidget(yieldlb,1,Qt::AlignCenter);
    thirdRowLayout->addWidget(yieldbtn,2,Qt::AlignLeft);
    

    m_adjtileLb = new QLabel(myLan.sorter_joint_config);
    m_adjtileLb->setAlignment(Qt::AlignHCenter);
    m_adjtileLb->setStyleSheet("font:28px;");
    m_adjtileLb->hide();
    m_adjCob = new QComboBox(widget);
    m_adjCob->setFixedSize(BTN_WID+20,BTN_HEI-10);
    QListWidget *lstwid = new QListWidget();
    m_adjCob->setModel(lstwid->model());
    m_adjCob->setView(lstwid);
    m_adjCob->hide();
    m_adjSourceComb = new QComboBox(widget);
    m_adjSourceComb->setFixedSize(BTN_WID+20,BTN_HEI-10);
    QListWidget *sourcelstwid = new QListWidget();
    m_adjSourceComb->setModel(sourcelstwid->model());
    m_adjSourceComb->setView(sourcelstwid);
    m_adjSourceComb->insertItem(0,myLan.finished_product);
    m_adjSourceComb->insertItem(1,myLan.waste_material);
    m_adjSourceComb->insertItem(2,myLan.raw_material);
    m_adjSourceComb->hide();
    QHBoxLayout *fourRowLayout = new QHBoxLayout();
    fourRowLayout->addWidget(m_adjtileLb, 0,Qt::AlignCenter);
    fourRowLayout->addWidget(m_adjCob, 0, Qt::AlignLeft);
    fourRowLayout->addWidget(m_adjSourceComb, 0,Qt::AlignLeft);


    okBtn = new QPushButton(myLan.ok, widget);
    okBtn->setStyleSheet("background-color:white;");
    okBtn->setFixedSize(200,70);
    cancelBtn = new QPushButton(myLan.cancel, widget);
    cancelBtn->setStyleSheet("background-color:white;");
    cancelBtn->setFixedSize(200,70);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);


    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(hlaytilelab);
    mainLayout->addStretch();
    mainLayout->addLayout(firstRowLayout);
    mainLayout->addLayout(secondRowLayout);
    mainLayout->addLayout(thirdRowLayout);
    mainLayout->addLayout(fourRowLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);


    timer =new QTimer(this);

    QPixmap pixmap;
    switch(style) {
    case MSG_QUES:
        pixmap = QPixmap(":/res/png/Message_Help.png");
        break;
    case MSG_TIPS:
        pixmap = QPixmap(":/res/png/Message_Info.png");
        buttonLayout->removeWidget(cancelBtn);
        cancelBtn->setHidden(true);
        break;
    case MSG_INFO:
        pixmap = QPixmap(":/res/png/Message_Info.png");
        okBtn->setHidden(true);
        cancelBtn->setHidden(true);
        break;
    case MSG_WARN:
        pixmap = QPixmap(":/res/png/Message_Warning.png");
        buttonLayout->removeWidget(okBtn);
        okBtn->setHidden(true);
        cancelBtn->setText(myLan.back);
        break;
    case MSG_ABOUT:
        pixmap = QPixmap(":/res/png/Message_Info.png");
        buttonLayout->removeWidget(okBtn);
        okBtn->setHidden(true);
        cancelBtn->setText(myLan.back);
        break;
    }

    connect(okBtn    , SIGNAL(pressed()),     this, SLOT(onOkBtnClicked()));
    connect(cancelBtn, SIGNAL(pressed()),     this, SLOT(onCancelBtnClicked()));
    connect(batchBtn,   SIGNAL(pressed()),   this, SLOT(onBatchBtnpressed()));
    connect(m_adjCob,   SIGNAL(currentIndexChanged(int)), this, SLOT(onrefreshsortersorce(int)));
    connect(shiftBtn,   SIGNAL(pressed()),  this, SLOT(onShiftBtnClicked()));
    connect(yieldbtn, SIGNAL(pressed()), this, SLOT(onYieldBtnClicked()));
    if (style == MSG_INFO) {
        connect(timer,SIGNAL(timeout()),this,SLOT(close()));
        timer->start(3000);
    }
}

myMessageBox::~myMessageBox()
{
    widget->hide();
}

void myMessageBox::setOKBtnText(QString text)
{
    okBtn->setText(text);
}

void myMessageBox::setCancelBtnText(QString text)
{
    cancelBtn->setText(text);
}

void myMessageBox::setautoClose(bool bclose)
{
    if(!bclose){
        timer->stop();
    }
}

/*!
 * \brief myMessageBox::getBathBtntext
 * \return
 */
QString myMessageBox::getBathBtntext()
{
    return batchBtn->text();
}

/*!
 * \brief myMessageBox::getshiftBtntext
 * \return
 */
QString myMessageBox::getshiftBtntext()
{
    return shiftBtn->text();
}

void myMessageBox::settextNotCenter()
{
//    textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void myMessageBox::setsortersourceidx(int nIdx)
{
    if(nIdx < 3){
        m_adjSourceComb->setCurrentIndex(nIdx);
    }
}

void myMessageBox::setBatchBtntext(QString text)
{
    batchBtn->setText(text);
    batchBtn->setStyleSheet("QPushButton{text-align:left}");
}

/*!
 * \brief myMessageBox::setBatchBtnshow
 * \param bShow
 */
void myMessageBox::setBatchBtnshow(bool bShow)
{
    batchBtn->setHidden(bShow);
    if(!bShow){
        textLabel->setMinimumWidth(120);
    }
}

/*!
 * \brief myMessageBox::setShiftBtntext
 * \param text
 */
void myMessageBox::setShiftBtntext(QString text)
{
    shiftBtn->setText(text);
    shiftBtn->setStyleSheet("QPushButton{text-align:left}");

}
/*!
 * \brief myMessageBox::setShiftBtnshow
 * \param bShow
 */
void myMessageBox::setShiftBtnshow(bool bShow)
{
    shiftLb->setHidden(bShow);
    shiftBtn->setHidden(bShow);
}

void myMessageBox::setadjcontetshow(bool bShow, QStringList itemlst)
{
    m_adjtileLb->setHidden(bShow);
    m_adjCob->setHidden(bShow);
    m_adjSourceComb->setHidden(bShow);
    m_adjSourceComb->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nsourceIdx[0]);

    if(!bShow){
        for(int i=0; i<itemlst.size(); i++){
            m_adjCob->insertItem(i,itemlst.at(i));
        }
    }
}

int myMessageBox::getsorterschmeidx()
{
    return m_adjCob->currentIndex();
}

/*!
 * \brief 返回化验源序号
 * \return
 */
int myMessageBox::getsortersourceidx()
{
    return m_adjSourceComb->currentIndex();
}

//  确定按钮
void myMessageBox::onOkBtnClicked()
{
    widget->hide();
    accept();
}

//  取消按钮
void myMessageBox::onCancelBtnClicked()
{
    widget->hide();
    reject();
}

void myMessageBox::onBatchBtnpressed()
{
INPUT_NAME1:
    QString str;
    myInputMethod input(myLan.input_batchname, batchBtn->text(), g_mainWidget);
    int ret = input.exec();
    if (ret == QDialog::Accepted) {
        str = input.getText().trimmed();
        if(str.compare("") == 0) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.input_forbid_empyty);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        if(!gFlow.inputStrIsValid(str)) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.contains_specialCharters);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        batchBtn->setText(str);
    }
}

/*!
 * \brief myMessageBox::onrefreshsortersorce
 */
void myMessageBox::onrefreshsortersorce(int nIdx)
{
    m_adjSourceComb->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nsourceIdx[nIdx]);
}

/*!
 * \brief myMessageBox::onShiftBtnClicked
 */
void myMessageBox::onShiftBtnClicked()
{
INPUT_NAME1:
    QString str;
    myInputMethod input(myLan.inputy_shiftname, shiftBtn->text(), g_mainWidget);
    int ret = input.exec();
    if (ret == QDialog::Accepted) {
        str = input.getText().trimmed();
        if(str.compare("") == 0) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.input_forbid_empyty);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        if(!gFlow.inputStrIsValid(str)) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.contains_specialCharters);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        shiftBtn->setText(str);
    }
}

void myMessageBox::onYieldBtnClicked()
{
    float nval = yieldbtn->text().toFloat();
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 10, nval, "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        yieldbtn->setText(QString("%1").arg(n_sample));
    }
}


bool myMessageBox::getSel()
{
    return isChecked;
}

void myMessageBox::setText(QString text)
{
    textLabel->setText(text);
}

void myMessageBox::setTextColor(int color)
{
    QPalette pe;
    pe.setColor(QPalette::WindowText,color);
    textLabel->setPalette(pe);
}

myInfoWidget::myInfoWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0,0,WIN_WID, WIN_HEI);
    
    int margin = 10;
    int nLabelHei = 120;

    widget = new QWidget(/*this*/);
//    widget->setGeometry(width()/3, height()/4, 750, 480);
    widget->setFixedSize(750, 480);
    widget->setStyleSheet(QString("background-color: #F1F0F5;"));

    QVBoxLayout *vLay = new QVBoxLayout(this);
    vLay->addWidget(widget, 0, Qt::AlignCenter);

    warnningLabel = new QLabel(widget);
    warnningLabel->setGeometry(margin+10, widget->height()/3+30, widget->width()-2*margin, nLabelHei);
    warnningLabel->setStyleSheet("font:35px;");
    warnningLabel->setAlignment(Qt::AlignHCenter);
    warnningLabel->setWordWrap(true);
    warnningLabel->setMinimumHeight(420);
    iconLabel = new QLabel(widget);
    iconLabel->setGeometry(margin,margin,50,50);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(":/res/png/Message_Info.png"));
    cancelBtn  =  new QPushButton(myLan.cancel, this);
    cancelBtn->setIcon(QIcon(":/res/png/Action_Cancel.png"));
    cancelBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    cancelBtn->setGeometry(QRect(width() / 2 + 5, height() / 2 + 70, BTN_WID, BTN_HEI));
    cancelBtn->hide();
    connect(cancelBtn,SIGNAL(pressed()),this,SLOT(onCancelBtnClicked()));
}

/* 析构函数*/
myInfoWidget::~myInfoWidget()
{
}

void myInfoWidget::setVisableCancelBtn(bool isShow)
{
    if (isShow) {
        cancelBtn->show();
    } else {
        cancelBtn->hide();
    }
}

void myInfoWidget::onCancelBtnClicked(void)
{
    cancelBtn->hide();
    emit sCancelBtnClicked();
    this->hide();
}

/* 设置标签上的提醒文本 */
void myInfoWidget::setLabelText(QString text)
{
    widget->show();
    warnningLabel->setText(text);
}

/* 延迟一定时间后再显示 */
void myInfoWidget::delayShow()
{
    this->show();    
    QDateTime dieTime = QDateTime::currentDateTime().addMSecs(10);
    while (QDateTime::currentDateTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

/* 隐藏提示框，仅覆盖当前页面 */
void myInfoWidget::hideCentralWidget()
{
    widget->hide();
}

/* 显示secs秒后隐藏 */
void myInfoWidget::showSecs(int secs)
{
    this->show();
    QDateTime dieTime = QDateTime::currentDateTime().addSecs(secs);
    while (QDateTime::currentDateTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    this->hide();
}

/* 延迟一定时间后隐藏 */
void myInfoWidget::delayHide()
{
    QDateTime dieTime = QDateTime::currentDateTime().addSecs(1);
    while (QDateTime::currentDateTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    this->hide();
}

/**
 * @brief myInfoWidget::获取文本
 */
QString myInfoWidget::getLabelText()
{
    return warnningLabel->text();
}


myBackWidget::myBackWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0, 0, WIN_WID, WIN_HEI);
    label = new QLabel(this);
    label->setGeometry(0, 0, WIN_WID, WIN_HEI);
    setStyleSheet("background-color: rgb(0,0,0,80);");
}

/* 析构函数*/
myBackWidget::~myBackWidget()
{
}

myBackWidget *backWidget = NULL;
myInfoWidget *infoWidget;
