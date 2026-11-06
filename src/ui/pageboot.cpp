#include "pageboot.h"

pageBoot *g_BootPage;

pageBoot::pageBoot(QWidget *parent) : QWidget(parent)
{
    createpage();
    layoutpage();
    connectAndSlt();
}

void pageBoot::createpage()
{
    resize(WIN_WID,WIN_HEI);
    setStyleSheet("background-color:white");

    //! 圆形process bar 和"通信自检中，请稍候..."label
    QPalette palette;
    palette.setBrush(QPalette::Base,Qt::white);
    QGradientStops gradientPoints;
    gradientPoints << QGradientStop(0, QColor(0, 116, 87))
                   << QGradientStop(0.5, Qt::white)
                   << QGradientStop(1, QColor(0, 116, 87));

    m_rpbRound = new MyRoundProgressBar();
    m_rpbRound->setDrawImgOrText(true);
    m_rpbRound->setPalette(palette);
    m_rpbRound->setRange(0, 100);
    m_rpbRound->setValue(1);
    m_rpbRound->setFixedSize(180, 180);
    m_rpbRound->setFormat("%v");
    m_rpbRound->setDataColors(gradientPoints);
    m_rpbRound->setPoint(":/res/png/Vendor_Meyer.png");


    m_timer = new QTimer(this);
    m_timer->setInterval(20);

    m_lblRound = new QLabel(QString("%1%2").arg(myLan.system_com_selfcheck).arg("........."));
    m_lblRound->setFixedHeight(180);
    QFont lbfont = m_lblRound->font();
    lbfont.setPixelSize(42);
    m_lblRound->setFont(lbfont);

    m_top = new QWidget;
    m_top->setFixedHeight(190);
    m_top->setStyleSheet("background-color:white");

    m_verPage = new softwareVersionPage();
    m_verPage->refreshPage();

    //! 刷新&进入系统按钮
    m_down = new QWidget();
    m_btnRefresh = new QPushButton(myLan.refresh);
    m_btnRefresh->setFixedSize(BTN_WID+60, BTN_HEI);
    m_btnRefresh->setIconSize(QSize(50,50));
    m_btnRefresh->setIcon(QIcon(":/res/png/action_refresh.png"));
    m_btnRefresh->setStyleSheet(g_PushButtonStyle);

    m_btnEnter = new QPushButton(myLan.skip);
    m_btnEnter->setFixedSize(BTN_WID+60, BTN_HEI);
    m_btnEnter->setStyleSheet(g_PushButtonStyle);
    m_btnEnter->setIconSize(QSize(50,50));
    m_btnEnter->setIcon(QIcon(":/res/png/action_enter.png"));
}

void pageBoot::layoutpage()
{
    QHBoxLayout *topLay = new QHBoxLayout(m_top);
    topLay->addStretch(0);
    topLay->addWidget(m_rpbRound);
    topLay->addSpacing(40);
    topLay->addWidget(m_lblRound);
    topLay->addStretch(0);

    QHBoxLayout *downLay = new QHBoxLayout(m_down);
    downLay->addSpacing(25);
    downLay->addWidget(m_btnRefresh);
    downLay->addStretch();
    downLay->addWidget(m_btnEnter);
    downLay->addSpacing(25);

    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addWidget(m_top);
    mainLay->addWidget(m_verPage);
    mainLay->addWidget(m_down);
}

void pageBoot::connectAndSlt()
{
    connect(m_timer,  &QTimer::timeout,  this,  &pageBoot::timeOutSlt);
    connect(m_btnRefresh, &QPushButton::pressed, this, &pageBoot::onBtnRefreshClicked);
    connect(m_btnEnter, &QPushButton::pressed, this, &pageBoot::onBtnEnterClicked);
}


void pageBoot::setBootloader()
{
    m_down->hide();
    m_timer->start();
    m_verPage->startVersionCheck();

}

void pageBoot::refreshPage(bool bManual)
{
    m_timer->start();
    m_down->hide();
    m_verPage->startVersionCheck();
    gFlow.initCommunication(false);
    m_verPage->refreshPage();
    m_timer->stop();
    
    if(!bManual){
        bool ret = gFlow.checkCommunicationRet();
        if(!ret){
            QTimer::singleShot(2000, this, [=]{
                onBtnEnterClicked();
            });
        }else{
            m_down->show();
        }
    }else{
        m_down->show();
    }
}

void pageBoot::timeOutSlt()
{
    m_nRoundTimes++;
    m_nRoundTimes = m_nRoundTimes > 100 ? 1 : m_nRoundTimes;
    m_rpbRound->setValue(m_nRoundTimes);
}

void pageBoot::onBtnRefreshClicked()
{
    refreshPage(true);
}

void pageBoot::onBtnEnterClicked()
{
    this->close();
    this->deleteLater();
}
