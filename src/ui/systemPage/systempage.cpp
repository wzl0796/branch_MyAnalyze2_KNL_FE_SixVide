#include "systempage.h"

SystemPage::SystemPage(QWidget *parent) : QWidget(parent)
{
    cratePage();
    layoutPage();
    connectAndSlt();
}

void SystemPage::cratePage()
{

    QStringList titlelst, iconlst;
    titlelst << myLan.System_Status << myLan.version << myLan.net;

    iconlst << ":/res/png/Message_State.png"
            << ":/res/png/Message_Version.png"
            << ":/res/png/Device_Network.png";


    m_listWid = new QListWidget();
    m_listWid->setMovement(QListView::Static);
    m_listWid->setViewMode(QListView::IconMode);
    m_listWid->setFixedSize(BTN_WID+15, WIN_HEI-180);
    m_listWid->setIconSize(QSize(BTN_HEI, BTN_HEI-15));
    m_listWid->setStyleSheet(QString("QListWidget{border:1px solid lightgray; "
                                     "border-radius:10px;outline:0px;"
                                     "padding:30px,0px,0px,0px;"
                                     "background-color:white;"
                                     "font:25px;}"
                                     "QListWidget::Item:selected{"
                                     "border-right:5px solid #43CD80}"));

    for (int i = 0; i < PAGE_ALL; i++) {
        QListWidgetItem *listItem = new QListWidgetItem(titlelst.at(i));
        listItem->setSizeHint(QSize(BTN_WID+13,90));
        listItem->setTextAlignment(Qt::AlignCenter);
        listItem->setIcon(QIcon(iconlst.at(i)));
        m_listWid->addItem(listItem);
    }
    m_listWid->setCurrentRow(0);

    m_sysStaPage = new SystemStaPage();
    m_verSionPage = new versionPage();
    m_sysNetPage = new systemNetPage();

    m_stackWid = new QStackedWidget();
    m_stackWid->setFixedSize(WIN_WID-315, WIN_HEI-180);
    m_stackWid->setStyleSheet("background-color:white; border-radius:10px;");

    m_stackWid->addWidget(m_sysStaPage);
    m_stackWid->addWidget(m_verSionPage);
    m_stackWid->addWidget(m_sysNetPage);

}

void SystemPage::layoutPage()
{
    QHBoxLayout *hlaymain = new QHBoxLayout();
    hlaymain->addWidget(m_listWid);
    hlaymain->addWidget(m_stackWid);

    setLayout(hlaymain);
}

void SystemPage::connectAndSlt()
{
    connect(m_listWid, &QListWidget::currentRowChanged, this, QOverload<int>::of(&SystemPage::onListWidgetChgSlt));
    connect(this, &SystemPage::sigUpdateIotWid,m_sysNetPage, &systemNetPage::onRefreshWebwindow);
    connect(this, &SystemPage::sigUpdateVpnSta,m_sysNetPage, &systemNetPage::onrefreshVPNStatus);
}

void SystemPage::refreshPage()
{
    int nIdx = m_listWid->currentRow();

    onListWidgetChgSlt(nIdx);
}

void SystemPage::onListWidgetChgSlt(int nIdx)
{
    if(nIdx < 0){
        return;
    }

    if(nIdx !=PAGE_SYSTEM){
        m_sysStaPage->stopTimer();
    }


    switch (nIdx) {
    case PAGE_SYSTEM:
        m_sysStaPage->refreshPage();
        break;
    case PAGE_VERSION:
        m_verSionPage->refreshDisplay();
        break;
    case PAGE_Net:
        m_sysNetPage->refreshDisplay();
        break;
    default:
        break;
    }

    m_stackWid->setCurrentIndex(nIdx);

}

void SystemPage::onPermissionChg()
{
    int nIdx = m_listWid->currentRow();

    if(nIdx == PAGE_VERSION){
        m_verSionPage->refreshDisplay();
    }else if(nIdx == PAGE_Net){
        m_sysNetPage->refreshDisplay();
    }
}