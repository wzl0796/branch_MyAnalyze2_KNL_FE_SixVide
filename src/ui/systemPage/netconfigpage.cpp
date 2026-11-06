#include "netconfigpage.h"
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"
#include <QCoreApplication>

netConfigPage::netConfigPage(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();

    checkIsDHCP();
}

/**
 * @brief netConfigPage::初始化参数
 */
void netConfigPage::initParams()
{
    auto_set = myLan.dhclient; //! "自动获取"
    manual_set = myLan.Manual_set; //! "手动获取";
    address = myLan.address; //! "地址"
    netmask = myLan.subnet_mask;   //! "子网掩码"
    gateway = myLan.gateway; //! "网关"
    para_type_error = QString("%1!").arg(myLan.input_error);
    fail = myLan.failed;
    plc_center_ip = "(分布式控制中心)地址";
    plc_center_port = "(分布式控制中心)端口";
    plc_center = "远程控制中心";
}

/**
 * @brief netConfigPage::搭建界面
 */
void netConfigPage::createPage()
{
    /** 自动获取 & 手动设置 */
    m_autoSetBtn = new QPushButton(auto_set);
    m_autoSetBtn->setFixedSize(BTN_WID*1.5, BTN_HEI);
    m_autoSetBtn->setIcon(QIcon(":/res/png/router-line.png"));
    m_autoSetBtn->setIconSize(QSize(50,50));


    m_manualSetBtn = new QPushButton(manual_set);
    m_manualSetBtn->setFixedSize(BTN_WID*1.5, BTN_HEI);
    m_manualSetBtn->setIcon(QIcon(":/res/png/Action_OneKey.png"));
    m_manualSetBtn->setIconSize(QSize(50,50));

    QHBoxLayout *dhcpLay = new QHBoxLayout();
    dhcpLay->addStretch();
    dhcpLay->addWidget(m_autoSetBtn);
    dhcpLay->addSpacing(160);
    dhcpLay->addWidget(m_manualSetBtn);
    dhcpLay->addStretch();

    m_sigMapper = new QSignalMapper;
    QGridLayout *upLay = new QGridLayout();
    for (int i = 0; i < NET_TYPE; i++) {

        m_hostLbl[i] = new QLabel(getNetText(i));
        m_hostLbl[i]->setFixedWidth(160);
        m_hostBtn[i] = new QPushButton();
        m_hostBtn[i]->setFixedSize(BTN_WID*4,BTN_HEI);
        QFont font = m_hostBtn[i]->font();
        font.setLetterSpacing(font.AbsoluteSpacing,4);
        m_hostBtn[i]->setFont(font);


        m_sigMapper->setMapping(m_hostBtn[i], i);
        connect(m_hostBtn[i], &QPushButton::clicked, m_sigMapper, QOverload<>::of(&QSignalMapper::map));

        QHBoxLayout *hlayrow = new QHBoxLayout();
        hlayrow->addStretch();
        hlayrow->addWidget(m_hostLbl[i],0,Qt::AlignLeft);
        hlayrow->addSpacing(30);
        hlayrow->addWidget(m_hostBtn[i]);
        hlayrow->addStretch();
        upLay->addLayout(hlayrow, i, 0);
    }
    upLay->setVerticalSpacing(60);

    m_Gb = new QGroupBox("");
    QVBoxLayout *gbLay = new QVBoxLayout(m_Gb);
    gbLay->addStretch(1);
    gbLay->addLayout(upLay);
    gbLay->addStretch(3);
    gbLay->addLayout(dhcpLay);
    gbLay->addStretch(1);

    QHBoxLayout *hlayMainpage = new QHBoxLayout();
    hlayMainpage->addWidget(m_Gb);
    setLayout(hlayMainpage);
}

/**
 * @brief netConfigPage::信号槽绑定
 */
void netConfigPage::connectSigAndSlot()
{
    connect(m_autoSetBtn,    SIGNAL(clicked(bool)), this,  SLOT(onAutoSetBtnClicked()));
    connect(m_manualSetBtn,  SIGNAL(clicked(bool)), this,  SLOT(onManualSetBtnClicked()));
    connect(m_sigMapper,     SIGNAL(mapped(int)),   this,  SLOT(setHostAddress(int)));
}

/**
 * @brief netConfigPage::checkIsDHCP
 */
void netConfigPage::checkIsDHCP()
{
    if (struCnfg.bEnableDHCP) {
        for (int i = 0; i < NET_TYPE; i++) {
            m_hostBtn[i]->setEnabled(false);
        }
    } else {
        for (int i = 0; i < NET_TYPE; i++) {
            m_hostBtn[i]->setEnabled(true);
        }
    }
}

/**
 * @brief netConfigPage::获取对应的网络选项文本
 * @param index
 * @return
 */
QString netConfigPage::getNetText(int index)
{
    QString text;
    switch (index) {
    case NET_IP:
        text = QString("IP")+address;
        break;
    case NET_MASK:
        text = netmask;
        break;
    case NET_GATEWAY:
        text = gateway;
        break;
    }

    return text;
}

/**
 * @brief netConfigPage::网关获取
 * @return
 */
QString netConfigPage::getGateway()
{
    QProcess process;
    process.setReadChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_UNIX
    process.start("route", QStringList() << "-n");
#else
    process.start("route", QStringList() << "PRINT");
#endif

    if (process.waitForFinished()) {
        QString input = process.readAll();
#ifdef Q_OS_UNIX
        QStringList allLine = input.split("\n");
#else
        QStringList allLine = input.split("\r\n");
#endif
        foreach(QString line, allLine) {
            QStringList field = line.split(" ", QString::SkipEmptyParts);
            if (field.size() > 0) {
                QString value = field.first();
                if(value.compare("0.0.0.0") == 0) {
#ifdef Q_OS_UNIX
                    return field.at(field.indexOf(value)+1);
#else
                    return field.at(field.indexOf(value)+2);
#endif
                }
            }
        }
    }
    return NULL;
}

/**
 * @brief netConfigPage::runDHCP
 * @return
 */
bool netConfigPage::runDHCP()
{
    /** 获取当前网络连接状态 */
    QString filePath = QString("/sys/class/net/%1/carrier").arg(QString(struCnfg.sWifiName));
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    int stat = file.readAll().left(1).toInt();
    if (stat == 1) {
        QProcess process;
        QString appDir = QCoreApplication::applicationDirPath();
        QString dhclientPath = appDir + "/dhcp/dhclient";
        QString dhclientConfPath = appDir + "/dhcp/dhclient.conf";
        QString cmd = QString("%1 %2 -cf %3").arg(dhclientPath).arg(QString(struCnfg.sWifiName)).arg(dhclientConfPath);
        process.start(cmd);
        process.waitForFinished(DHCP_TIMEOUT);
        process.start("killall dhclient");
        process.waitForFinished(1000);

        /** 判断DHCP是否成功 */
        if (getIP().compare("") != 0) {
            printf("getDHCP ip ok\n");
            fflush(0);
            return true;
        }
        printf("getDHCP ip fail\n");
        fflush(0);
    }

    /** 若DHCP失败则还原网络配置 */
#ifdef Q_OS_UNIX
    QString cmd;
    cmd = QString("ifconfig %1 %2 netmask %3 up").arg(QString(struCnfg.sWifiName)).arg(m_hostBtn[NET_IP]->text()).arg(m_hostBtn[NET_MASK]->text());
    system(qPrintable(cmd));
    cmd = QString("route add -net 0.0.0.0 gw %1").arg(m_hostBtn[NET_GATEWAY]->text());
    system(qPrintable(cmd));
#endif

    return false;
}

/**
 * @brief netConfigPage::获取IP信息
 * @return
 */
QString netConfigPage::getIP()
{
    QString ip = "";

    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface inter, interfaceList) {
        if(inter.name() == QString(struCnfg.sWifiName)) {
            QList<QNetworkAddressEntry> hostAddressList = inter.addressEntries();
            foreach (QNetworkAddressEntry address, hostAddressList) {
                if (address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ip = address.ip().toString();
                    break;
                }
            }
        }
    }

    return ip;
}

/**
 * @brief netConfigPage::获取网络配置信息
 */
void netConfigPage::getHostAddress()
{
    gFlow.getNetSetting();

    /* get ip, netmask, mac */
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface inter, interfaceList) {
        if(0 == strcmp(inter.name().toLatin1().data(),struCnfg.sWifiName)){
            QList<QNetworkAddressEntry> hostAddressList = inter.addressEntries();
            foreach (QNetworkAddressEntry address, hostAddressList) {
                if (address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    if(struCnfg.sLocalIP.compare(address.ip().toString()) != 0) {
                        if (!struCnfg.bEnableDHCP) {
#ifdef Q_OS_UNIX
                            QString cmd;
                            cmd = QString("ifconfig %1 %2 netmask %3 up").arg(QString(struCnfg.sWifiName)).arg(struCnfg.sLocalIP).arg(struCnfg.sNetMask);
                            system(qPrintable(cmd));
                            cmd = QString("route add -net 0.0.0.0 gw %1").arg(struCnfg.sGateWay);
                            system(qPrintable(cmd));
#endif
                        } else
                            struCnfg.sLocalIP = address.ip().toString();
                    }

                    if(struCnfg.bEnableDHCP){
                        struCnfg.sNetMask = address.netmask().toString();
                    } else if(struCnfg.sNetMask.compare(address.netmask().toString()) != 0){
                        struCnfg.sNetMask = address.netmask().toString();
                    }
                    break;
                }
            }
        }
    }

    /* get gateway */
    struCnfg.sGateWay = getGateway();

    gFlow.saveNetSetting();

    m_hostBtn[NET_IP]->setText(struCnfg.sLocalIP);
    m_hostBtn[NET_MASK]->setText(struCnfg.sNetMask);
    m_hostBtn[NET_GATEWAY]->setText(struCnfg.sGateWay);
}

/**
 * @brief netConfigPage::设置主机网络配置信息
 * @param index
 */
void netConfigPage::setHostAddress(int index)
{
    QString hostAddress;
    bool isAddrOk = false;
    myInputPanel inputDlg(textType, 0, 0, 0, getNetText(index),g_mainWidget);
    inputDlg.setText(m_hostBtn[index]->text());
    if (inputDlg.exec() == QDialog::Accepted) {
        /* get input value */
        hostAddress = inputDlg.getText();
        isAddrOk = isIPAddr(hostAddress);
        if(!isAddrOk) {
            infoWidget->raise();
            infoWidget->setLabelText(para_type_error);
            infoWidget->showSecs();
            return;
        }

        /* config network interface */
        QString cmd;
        switch (index) {
        case NET_IP:
            struCnfg.sLocalIP = hostAddress;
#ifdef Q_OS_UNIX
            cmd = QString("ifconfig %1 %2 netmask %3 up").arg(QString(struCnfg.sWifiName)).arg(hostAddress).arg(m_hostBtn[NET_MASK]->text());
            system(qPrintable(cmd));
#endif
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
                gFlow.sendSorterIPparams();
            }
            break;
        case NET_MASK:
            struCnfg.sNetMask= hostAddress;
#ifdef Q_OS_UNIX
            cmd = QString("ifconfig %1 %2 netmask %3 up").arg(QString(struCnfg.sWifiName)).arg(m_hostBtn[NET_IP]->text()).arg(hostAddress);
            system(qPrintable(cmd));
#endif
            break;
        case NET_GATEWAY:
            struCnfg.sGateWay = hostAddress;
#ifdef Q_OS_UNIX
            cmd = QString("route add -net 0.0.0.0 gw %1").arg(hostAddress);
            system(qPrintable(cmd));
#endif
            break;
        }

        /* save to text control */
        m_hostBtn[index]->setText(hostAddress);

        /* save setting file */
        gFlow.saveNetSetting();
    }
}

/**
 * @brief netConfigPage::自动获取IP地址
 */
void netConfigPage::onAutoSetBtnClicked()
{
    infoWidget->raise();
    infoWidget->setLabelText(QString("%1....").arg(myLan.dhclient));
    infoWidget->delayShow();

    struCnfg.bEnableDHCP = true;
    for (int i = 0; i < NET_TYPE; i++) {
        m_hostBtn[i]->setEnabled(false);
    }

    /** 运行DHCP线程 */
    if (!runDHCP()) {
        infoWidget->setLabelText(QString("%1!").arg(myLan.dhclient_failed));
        infoWidget->showSecs();
        return;
    }

    /** 获取网络信息并更新控件显示 */
    getHostAddress();

    infoWidget->hide();
}

/**
 * @brief netConfigPage::手动获取IP地址
 */
void netConfigPage::onManualSetBtnClicked()
{
    struCnfg.bEnableDHCP = false;
    for (int i = 0; i < NET_TYPE; i++) {
        m_hostBtn[i]->setEnabled(true);
    }
}



/**
 * @brief netConfigPage::ip校验
 * @param ip
 * @return
 */
bool netConfigPage::isIPAddr(QString ip)
{
    bool ok;
    QRegExp rx2("(\\d+)(\\.)(\\d+)(\\.)(\\d+)(\\.)(\\d+)");
    int pos  = rx2.indexIn(ip);

    if (pos>-1) {
        for(int i=0; i<4;i++){
            if(rx2.cap(i*2+1).toInt(&ok) > 255){
                return false;
            }
        }
        if(rx2.cap(1).toInt(&ok) == 0) {
            return false;
        }
    } else {
        return false;
    }
    return true;
}
