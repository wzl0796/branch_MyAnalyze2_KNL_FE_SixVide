#include "versionpage.h"
#include "core/io/modbus/modbusctlclass.h"
#include "core/globalflow/globalflow.h"
#include "core/io/iot/localsocketthread.h"

versionPage::versionPage(QWidget *parent) : QWidget(parent)
{
    createPage();

    connectSigAndSlot();
}



/**
 * @brief versionPage::界面搭建
 */
void versionPage::createPage()
{
//    setWindowFlags(Qt::FramelessWindowHint);
//    setStyleSheet("background-color: rgb(0,0,0,100);");

//    m_widget = new QWidget(this);
//    m_widget->setWindowFlags(Qt::FramelessWindowHint);
//    m_widget->setGeometry((WIN_WID - 1300)/2, (WIN_HEI - 850)/2, 1300, 850);
//    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");
//    m_widget->show();

    m_stack = new QStackedWidget();
    m_stack->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    pageSoftWare = new softwareVersionPage();
    pageSoftWare->setScreenVerLabShow();
    m_stack->addWidget(pageSoftWare);

    pageFpgaUpgrade = new upgradeFPGAwidget();
    m_stack->addWidget(pageFpgaUpgrade);

    refreshBtn = new QPushButton();
    refreshBtn->setText(myLan.refresh);
    refreshBtn->setFixedSize(BTN_WID+40, BTN_HEI);
    refreshBtn->setIconSize(QSize(50,50));
    refreshBtn->setIcon(QIcon(":/res/png/action_refresh.png"));
    refreshBtn->setStyleSheet(g_PushButtonStyle);

    backBtn = new QPushButton();
    backBtn->setFixedSize(BTN_WID+40, BTN_HEI);
    backBtn->setText(myLan.back);
    backBtn->setStyleSheet(g_PushButtonStyle);
    backBtn->setIconSize(QSize(50,50));
    backBtn->setIcon(QIcon(":/res/png/Action_Back.png"));
    backBtn->hide();

    updateBtn = new QPushButton();
    updateBtn->setFixedSize(BTN_WID+40, BTN_HEI);
    updateBtn->setText(myLan.upgrade);
    updateBtn->setStyleSheet(g_PushButtonStyle);
    updateBtn->setIconSize(QSize(50,50));
    updateBtn->setIcon(QIcon(":/res/png/Update_Chip.png"));
    updateBtn->hide();

    m_modeWgt = new QWidget;
    m_modeWgt->hide();

    //! 切换工厂模式按钮
    m_factoryBtn = new QPushButton(myLan.factory_mode);
    m_factoryBtn->setFixedSize(BTN_WID*1.2, BTN_HEI);
    m_factoryBtn->setIconSize(QSize(50,50));
    m_factoryBtn->setIcon(QIcon(":/res/png/User_Engineer.png"));
    m_factoryBtn->hide();

    //! 切换用户模式按钮
    m_userBtn = new QPushButton(myLan.user_mode);
    m_userBtn->setFixedSize(BTN_WID*1.2+30, BTN_HEI);
    m_userBtn->setIconSize(QSize(50,50));
    m_userBtn->setIcon(QIcon(":/res/png/User_Operator.png"));

    QHBoxLayout *hLay = new QHBoxLayout(m_modeWgt);
    hLay->addWidget(m_factoryBtn);
    hLay->addWidget(m_userBtn);

    QHBoxLayout *downLayout = new QHBoxLayout();
    downLayout->addWidget(m_modeWgt);
    downLayout->addStretch();
    downLayout->addWidget(refreshBtn);
    downLayout->addSpacing(50);
    downLayout->addWidget(updateBtn);
    downLayout->addStretch();
    downLayout->addWidget(backBtn);

    QVBoxLayout *Lay = new QVBoxLayout(this);
    Lay->addWidget(m_stack);
    Lay->addLayout(downLayout);
    Lay->addSpacing(10);
}

/**
 * @brief versionPage::界面刷新
 */
void versionPage::refreshDisplay()
{
    pageSoftWare->refreshPage();
    if(struGsh.nAuthority == AUTHORITY_PRIMARY){
        updateBtn->hide();
    }else{
        updateBtn->show();
    }

    if(struGsh.nFeederState){
        updateBtn->setDisabled(true);
        refreshBtn->setDisabled(true);
    }else{
        updateBtn->setDisabled(false);
        refreshBtn->setDisabled(false);
    }
}

/**
 * @brief versionPage::信号槽绑定
 */
void versionPage::connectSigAndSlot()
{
    connect(refreshBtn,       SIGNAL(pressed()),            this, SLOT(onRefreshBtnPressedSlt()));
    connect(updateBtn,        SIGNAL(pressed()),            this, SLOT(onUpgradeBtnPressedSlt()));
    connect(backBtn,          SIGNAL(pressed()),            this, SLOT(onBackBtnPressedSlt()));
    connect(m_factoryBtn,     SIGNAL(clicked(bool)),        this, SLOT(onFactoryBtnClicked())); //工厂模式
    connect(m_userBtn,        SIGNAL(clicked(bool)),        this, SLOT(onUserBtnClicked()));    //用户模式
//    connect(pageFpgaUpgrade,  SIGNAL(enableBtnSig(bool)),   this, SLOT(setBtnState(bool)));
//    connect(pageSoftWare,     SIGNAL(versionReady(bool)),   this, SLOT(setBtnState(bool)));
}

/**
 * @brief versionPage::刷新按钮
 */
void versionPage::onRefreshBtnPressedSlt()
{
    setBtnState(false);

    pageSoftWare->startVersionCheck();

    gFlow.initCommunication(true);

    pageSoftWare->refreshVerison();

    setBtnState(true);

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
        QJsonObject staicsjson;
        staicsjson["Version"] = true;
        g_Lotsocket->uploadInfo(staicsjson);
    }

    for(int n=0; n<BOARD_ALL; n++){
        if(struGsh.bSysBoardSta[n]){
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
                QJsonObject staicsjson;
                staicsjson["telemetry"] = true;
                g_Lotsocket->uploadInfo(staicsjson);
            }
            break;
        }
    }

}

/**
 * @brief versionPage::进入升级界面
 */
void versionPage::onUpgradeBtnPressedSlt()
{
    int pageIndex = m_stack->currentIndex();

    if (pageIndex == PAGE_UPGRADE) {
        struGsh.bUpgrading = true;
        gFlow.msleep(50);

        updateBtn->setEnabled(false);
        backBtn->setEnabled(false);
        m_factoryBtn->setEnabled(false);
        m_userBtn->setEnabled(false);

        pageFpgaUpgrade->startUpgradeProcess();
        gFlow.initCommunication(true);

        updateBtn->setEnabled(true);
        backBtn->setEnabled(true);
        m_factoryBtn->setEnabled(true);
        m_userBtn->setEnabled(true);
        struGsh.bUpgrading = false;
    }else{
        m_stack->setCurrentIndex(PAGE_UPGRADE);
        pageFpgaUpgrade->refreshPage();
        refreshBtn->hide();
        backBtn->show();
        m_modeWgt->show();
    }
}

/**
 * @brief versionPage::返回按钮
 */
void versionPage::onBackBtnPressedSlt()
{
    int pageIndex = m_stack->currentIndex();

    if (pageIndex != PGAE_SOFT_WARE) {
        m_stack->setCurrentIndex(PGAE_SOFT_WARE);
        refreshBtn->show();
        refreshBtn->setText(myLan.refresh);
        m_modeWgt->hide();
        backBtn->hide();
    }
}

/**
 * @brief versionPage::更新按钮状态
 * @param state
 */
void versionPage::setBtnState(bool state)
{
    refreshBtn->setEnabled(state);
    updateBtn->setEnabled(state);
    backBtn->setEnabled(state);
}

/*!
 * \brief 槽函数: 切换工厂模式
 */
void versionPage::onFactoryBtnClicked()
{
    pageFpgaUpgrade->switchMode(MODE_FACTORY);
}

/*!
 * \brief 槽函数: 切换用户模式
 */
void versionPage::onUserBtnClicked()
{
    pageFpgaUpgrade->switchMode(MODE_USER);
}
