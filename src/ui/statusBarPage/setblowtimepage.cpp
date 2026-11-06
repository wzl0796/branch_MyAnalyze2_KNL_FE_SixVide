#include "setblowtimepage.h"
#include <QDebug>

setblowtimepage::setblowtimepage(QWidget *parent): QDialog(parent)
{
    createPage();
}

setblowtimepage::~setblowtimepage()
{

}

void setblowtimepage::createPage()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    m_wid = new QWidget();
    m_wid->setWindowFlags(Qt::FramelessWindowHint);
    m_wid->setGeometry((WIN_WID+30 - 1050)/2, (WIN_HEI - 600)/2, 1050, 600);
    m_wid->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    m_gb = new QGroupBox();
    m_gb->setFixedSize(850, 500);

    m_delaylb  = new QLabel(QString("%1(ms)").arg(myLan.delay_time));
    m_delaylb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_delaylb->setAlignment(Qt::AlignCenter);
    m_delaybtn = new QPushButton("30");
    m_delaybtn->setStyleSheet("background-color:white;");
    m_delaybtn->setFixedSize(BTN_WID, BTN_HEI-15);


    m_blowlb  = new QLabel(QString("%1(ms)").arg(myLan.blow_time));
    m_blowlb->setFixedSize(BTN_WID+70, BTN_HEI-15);
    m_blowlb->setAlignment(Qt::AlignCenter);
    m_blowlb->hide();
    m_blowbtn = new QPushButton("30");
    m_blowbtn->setStyleSheet("background-color:white;");
    m_blowbtn->setFixedSize(BTN_WID, BTN_HEI-15);
    m_blowbtn->hide();


    m_resizeRectLb  = new QLabel(QString("%1(y)").arg(myLan.Rect_ratio));
    m_resizeRectLb->setFixedSize(BTN_WID+50, BTN_HEI-15);
    m_resizeRectLb->setAlignment(Qt::AlignCenter);
    m_resizeRectLe = new MyLineEdit();
    m_resizeRectLe->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_resizeRectLe->setFixedSize(BTN_WID, BTN_HEI-15);


    m_bckbtn = new QPushButton(myLan.back);
    m_bckbtn->setFixedSize(BTN_WID, BTN_HEI);

    QHBoxLayout *hlaydelay = new QHBoxLayout();
    hlaydelay->addWidget(m_delaylb);
    hlaydelay->addWidget(m_delaybtn);

    QHBoxLayout *hlayblow = new QHBoxLayout();
    hlayblow->addWidget(m_blowlb);
    hlayblow->addWidget(m_blowbtn);

    QHBoxLayout *hlayRect = new QHBoxLayout();
    hlayRect->addWidget(m_resizeRectLb);
    hlayRect->addWidget(m_resizeRectLe);

    QHBoxLayout *hlaybckbtn = new QHBoxLayout();
    hlaybckbtn->addStretch();
    hlaybckbtn->addWidget(m_bckbtn);

    QVBoxLayout *vlaygb = new QVBoxLayout(m_gb);
    vlaygb->addLayout(hlaydelay);
    vlaygb->addLayout(hlayblow);
    vlaygb->addLayout(hlayRect);

    QVBoxLayout *vlaywid = new QVBoxLayout(m_wid);
    vlaywid->addWidget(m_gb);
    vlaywid->addLayout(hlaybckbtn);

    QHBoxLayout *hlaymain = new QHBoxLayout();
    hlaymain->addWidget(m_wid,0,Qt::AlignCenter);
    setLayout(hlaymain);


    connect(m_bckbtn, &QPushButton::pressed, this, &setblowtimepage::onBackBtnpressedSlt);
    connect(m_delaybtn, &QPushButton::pressed, this, &setblowtimepage::onDelayBtnpressedSlt);
    connect(m_blowbtn, &QPushButton::pressed, this, &setblowtimepage::onBlowBtnpressedSlt);
    connect(m_resizeRectLe, SIGNAL(pressed()), this ,SLOT(onRectResizeChgSlt()));

}

/*!
 * \brief 刷新page
 */
void setblowtimepage::refreshpage()
{
    m_delaybtn->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay));
    m_blowbtn->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowTime));
    m_resizeRectLe->setText(QString("%1%").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nRectResize));
}

void setblowtimepage::onBackBtnpressedSlt()
{
    this->close();
}

void setblowtimepage::onDelayBtnpressedSlt()
{
    int nWipeInterval = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 6000, nWipeInterval, myLan.delay_time, g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowDelay = inputPanel->getValue();
        m_delaybtn->setText(QString("%1").arg(inputPanel->getValue()));
        gFlow.sendBlowTime();
    }
}

void setblowtimepage::onBlowBtnpressedSlt()
{
    int nWipeInterval = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowTime;
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 6000, nWipeInterval, "吹气时间", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nBlowTime = inputPanel->getValue();
        m_blowbtn->setText(QString("%1").arg(inputPanel->getValue()));
        gFlow.sendBlowTime();
    }
}

/*!
 * \brief 缩框占比
 */
void setblowtimepage::onRectResizeChgSlt()
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
