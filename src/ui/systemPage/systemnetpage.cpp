#include "systemnetpage.h"
#include "core/io/websocket/mysocketmgr.h"
#include "core/mythread/netcheckthread.h"

systemNetPage::systemNetPage(QWidget *parent) : QDialog(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief systemNetPage::初始化
 */
void systemNetPage::initParams()
{
    wechat        = "小程序";
    local_machine = "本地网络";
    remote_ctrl   = "远程控制";
    cloud         = "云端";
    m_nIndex = NET_LOCAL_PAGE;
}

/**
 * @brief systemNetPage::搭建界面
 */
void systemNetPage::createPage()
{
    m_Tab = new QTabBar;
    m_Tab->setStyleSheet("QTabBar::tab {min-width: 150px; padding: 8px;min-height:45px;margin-right:5px }"
                            "QTabBar::tab:selected { background-color: #007454; color: white; }"
                            "QTabBar::tab:!selected { background-color: #f0f0f0; color: #333333; }"
                            );
    m_Tab->setDrawBase(false);
    m_Tab->addTab(myLan.net);
    m_Tab->addTab(myLan.vpn_connect);
    m_Tab->addTab(myLan.cloud);
    m_Tab->setCurrentIndex(0);

    m_stack = new QStackedWidget();

    pageNetConfig = new netConfigPage();
    m_stack->addWidget(pageNetConfig);

    pageRemote = new remoteCtrlPage();
    m_stack->addWidget(pageRemote);

    pageCloud = new QWidget();
    m_stack->addWidget(pageCloud);
    m_hlaycloud = new QHBoxLayout(pageCloud);

    m_stack->setCurrentIndex(0);
    m_stack->setStyleSheet("QWidget{background-color:white;}");


    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addWidget(m_Tab, 0, Qt::AlignLeft |Qt::AlignTop);
    mainLay->addWidget(m_stack);
}

/**
 * @brief systemNetPage::信号槽绑定
 *       忘了添加物联网显示页面和连接和断开vpn的信号槽
 */
void systemNetPage::connectSigAndSlot()
{
    connect(m_Tab,       SIGNAL(currentChanged(int)), this,    SLOT(onTabIndexChangedSlt(int)));
    connect(pageRemote,  SIGNAL(sigVpnconnectsta(bool)), this, SLOT(onrefreshVPNStatus(bool)));
}

/**
 * @brief systemNetPage::返回按钮
 */
void systemNetPage::onBackBtnPressedSlt()
{
    this->hide();
}

/**
 * @brief systemNetPage::界面切换
 * @param index
 */
void systemNetPage::onTabIndexChangedSlt(int index)
{
    m_nIndex = index;

    m_stack->setCurrentIndex(m_nIndex);

    refreshDisplay();
}

/*!
 * \brief 槽: 刷新物联网窗口
 * \param nIdx
 */
void systemNetPage::onRefreshWebwindow(int nIdx)
{
    if (m_win != nullptr) {
        delete m_win;
        m_win = nullptr;
    }

    if (m_container != nullptr) {
        m_hlaycloud->removeWidget(m_container);
        delete m_container;
        m_container = nullptr;
    }

    m_wid = nIdx;
    m_win = QWindow::fromWinId(m_wid);
    m_container = QWidget::createWindowContainer(m_win);
    // m_container->setMaximumHeight(WIN_HEI-410);
    m_hlaycloud->addWidget(m_container);
}

/*!
 * \brief 槽： 刷新VPN连接状态
 * \param bStatus
 */
void systemNetPage::onrefreshVPNStatus(bool bStatus)
{
    m_VPNStatus = bStatus;
}

/**
 * @brief systemNetPage::刷新界面
 */
void systemNetPage::refreshDisplay()
{
    //! 权限判断
    if(struGsh.nAuthority == AUTHORITY_PRIMARY){
        m_Tab->setTabEnabled(CLOUD_PAGE, false);
    }else{
        m_Tab->setTabEnabled(CLOUD_PAGE, true);
    }

   if(m_nIndex == NET_LOCAL_PAGE){
        pageNetConfig->getHostAddress();
    }else if(m_nIndex == NET_REMOTE_PAGE){
        pageRemote->onRefreshVPNStatus(m_VPNStatus);
    }
}
