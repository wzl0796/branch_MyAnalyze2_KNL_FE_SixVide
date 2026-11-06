#include "remotectrlpage.h"
#include "core/globalflow/globalflow.h"
#include "common/myinputmethod.h"
#include "common/myinputpanel.h"
#include "common/mymessagebox.h"
#include <QCoreApplication>

remoteCtrlPage::remoteCtrlPage(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();

    getConfFromFile();

    getPasswordFromFile();
}

/**
 * @brief remoteCtrlPage::初始化参数
 */
void remoteCtrlPage::initParams()
{
    ipAdress       = QString("IP%1").arg(myLan.address);
    serverAdress   = QString("%1%2").arg(myLan.server).arg(myLan.address);
    serverPort     = QString("%1%2").arg(myLan.server).arg(myLan.port);
    user_name      = myLan.username;
    passwd         = myLan.password;
    connect_server = myLan.connect_server;
    fail           = myLan.failed;
    close_connect  = QString("%1").arg(myLan.disconect);
}

/**
 * @brief remoteCtrlPage::搭建界面
 */
void remoteCtrlPage::createPage()
{
    //! IP地址
    m_localVirtualAddrLbl = new QLabel(ipAdress);
    m_localVirtualAddrLbl->setFixedWidth(160);
    m_localVirtualAddrBtn = new QPushButton("0.0.0.0");
    m_localVirtualAddrBtn->setFixedSize(BTN_WID*4, BTN_HEI);
    QHBoxLayout *hlayip = new QHBoxLayout();
    hlayip->addStretch();
    hlayip->addWidget(m_localVirtualAddrLbl,Qt::AlignLeft);
    hlayip->addSpacing(30);
    hlayip->addWidget(m_localVirtualAddrBtn);
    hlayip->addStretch();

    //! 服务器地址
    m_serverIPLbl = new QLabel(serverAdress);
    m_serverIPLbl->setFixedWidth(160);
    m_serverIPBtn = new QPushButton("");
    m_serverIPBtn->setFixedSize(BTN_WID*4, BTN_HEI);
    QHBoxLayout *hlayserip = new QHBoxLayout();
    hlayserip->addStretch();
    hlayserip->addWidget(m_serverIPLbl,Qt::AlignLeft);
    hlayserip->addSpacing(30);
    hlayserip->addWidget(m_serverIPBtn);
    hlayserip->addStretch();

    //! 服务器端口
    m_portLbl = new QLabel(serverPort);
    m_portLbl->setFixedWidth(160);
    m_portBtn = new QPushButton("");
    m_portBtn->setFixedSize(BTN_WID*4, BTN_HEI);
    QHBoxLayout *hlayport = new QHBoxLayout();
    hlayport->addStretch();
    hlayport->addWidget(m_portLbl,Qt::AlignLeft);
    hlayport->addSpacing(20);
    hlayport->addWidget(m_portBtn);
    hlayport->addStretch();

    //! 用户名
    m_usernameLbl = new QLabel(user_name);
    m_usernameLbl->setFixedWidth(160);
    m_usernameBtn = new QPushButton("");
    m_usernameBtn->setFixedSize(BTN_WID*4, BTN_HEI);
    QHBoxLayout *hlayuser = new QHBoxLayout();
    hlayuser->addStretch();
    hlayuser->addWidget(m_usernameLbl,Qt::AlignLeft);
    hlayuser->addSpacing(30);
    hlayuser->addWidget(m_usernameBtn);
    hlayuser->addStretch();

    //! 密码
    m_passwordLbl = new QLabel(passwd);
    m_passwordLbl->setFixedWidth(160);
    m_passwordBtn = new QPushButton("");
    m_passwordBtn->setFixedSize(BTN_WID*4, BTN_HEI);
    QHBoxLayout *hlaypass = new QHBoxLayout();
    hlaypass->addStretch();
    hlaypass->addWidget(m_passwordLbl,Qt::AlignLeft);
    hlaypass->addSpacing(30);
    hlaypass->addWidget(m_passwordBtn);
    hlaypass->addStretch();

    //! 连接
    m_connectBtn = new QPushButton(connect_server);
    m_connectBtn->setFixedSize(BTN_WID*1.5, BTN_HEI);
    m_connectBtn->setStyleSheet(g_enableBtnStyle);
    m_connectBtn->setIconSize(QSize(50,50));
    m_connectBtn->setIcon(QIcon(":/res/png/Action-connect.png"));

    //! 关闭连接
    m_disconnectBtn = new QPushButton(close_connect);
    m_disconnectBtn->setFixedSize(BTN_WID*1.5, BTN_HEI);
    m_disconnectBtn->setStyleSheet(g_disableBtnStyle);
    m_disconnectBtn->setIconSize(QSize(50,50));
    m_disconnectBtn->setIcon(QIcon(":/res/png/Action-disconnect.png"));
    m_disconnectBtn->setEnabled(false);
    QHBoxLayout *hlaybtn= new QHBoxLayout();
    hlaybtn->addStretch();
    hlaybtn->addWidget(m_connectBtn);
    hlaybtn->addSpacing(120);
    hlaybtn->addWidget(m_disconnectBtn);
    hlaybtn->addStretch();

    m_Gb = new QGroupBox("");
    QVBoxLayout *upLay = new QVBoxLayout(m_Gb);
    upLay->addLayout(hlayip);
    upLay->addLayout(hlayserip);
    upLay->addLayout(hlayport);
    upLay->addLayout(hlayuser);
    upLay->addLayout(hlaypass);
    upLay->addSpacing(80);
    upLay->addLayout(hlaybtn);


    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addWidget(m_Gb);
}

/**
 * @brief remoteCtrlPage::connectSigAndSlot
 */
void remoteCtrlPage::connectSigAndSlot()
{
    connect(m_serverIPBtn,   SIGNAL(pressed()), this, SLOT(onServerIPBtnPressed()));
    connect(m_portBtn,       SIGNAL(pressed()), this, SLOT(onPortBtnPressed()));
    connect(m_usernameBtn,   SIGNAL(pressed()), this, SLOT(onUsernameBtnPressed()));
    connect(m_passwordBtn,   SIGNAL(pressed()), this, SLOT(onPasswordBtnPressed()));
    connect(m_connectBtn,    SIGNAL(pressed()), this, SLOT(onConnectBtnPressed()));
    connect(m_disconnectBtn, SIGNAL(pressed()), this, SLOT(onDisconnectBtnPressed()));
}

/**
 * @brief remoteCtrlPage::响应服务器地址输入框被按下事件
 */
void remoteCtrlPage::onServerIPBtnPressed()
{
    QString server;
    myInputMethod inputDlg(m_serverIPLbl->text(),m_serverIPBtn->text(),g_mainWidget);
    if(inputDlg.exec()==QDialog::Accepted){
        server = inputDlg.getText();
        m_serverIPBtn->setText(server);
        setConfFile();
    }
}

/**
 * @brief remoteCtrlPage::响应服务器端口号被按下事件
 */
void remoteCtrlPage::onPortBtnPressed()
{
    myInputPanel *inputDlg = new myInputPanel(intType, 0, 65534, m_portBtn->text().toInt(), m_portLbl->text(), g_mainWidget);
    if(inputDlg->exec() == QDialog::Accepted)
    {
        m_portBtn->setText(inputDlg->getText());
        setConfFile();
    }
}

/**
 * @brief remoteCtrlPage::响应用户名输入框被按下事件
 */
void remoteCtrlPage::onUsernameBtnPressed()
{
    myInputMethod *inputDlg = new myInputMethod(user_name, m_usernameBtn->text(), g_mainWidget);
    if(inputDlg->exec() == QDialog::Accepted)
    {
        m_usernameBtn->setText(inputDlg->getText());
        setPasswordFile();
    }
}

/**
 * @brief remoteCtrlPage::响应密码输入框被按下事件
 */
void remoteCtrlPage::onPasswordBtnPressed()
{
    myInputMethod inputDlg(passwd, m_passwordBtn->text(), g_mainWidget);
    inputDlg.setPasswordMode(true);
    if (inputDlg.exec() == QDialog::Accepted)
    {
        sPassword = inputDlg.getText();
        QString str;
        for(int i = 0 ; i < sPassword.size() ; ++i) {
            str += "●";
        }
        m_passwordBtn->setText(str);
        setPasswordFile();
    }
}

/**
 * @brief remoteCtrlPage::响应连接按钮按下事件
 */
void remoteCtrlPage::onConnectBtnPressed()
{
    infoWidget->raise();
    infoWidget->setLabelText(connect_server+QString("VPN"));
    infoWidget->delayShow();

    bool bCon = connetToServer();
    infoWidget->hide();

    if(bCon)    //! 连接服务器成功
    {
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
        m_connectBtn->setStyleSheet(g_disableBtnStyle);
        m_disconnectBtn->setStyleSheet(g_enableBtnStyle);
#if SORTERADJ_BY_VIRTUALIP
        gFlow.sendSorterIPparams(false);
#endif
        emit sigVpnconnectsta(true);
    }
    else        //! 连接服务器失败
    {
#ifdef Q_OS_UNIX
        system("killall openvpn");
#endif
        emit sigVpnconnectsta(false);
        infoWidget->raise();
        infoWidget->setLabelText(connect_server+fail);
        infoWidget->showSecs();
    }
}

/**
 * @brief remoteCtrlPage::响应断开连接按钮按下事件
 */
void remoteCtrlPage::onDisconnectBtnPressed()
{
#ifdef Q_OS_UNIX
    char cmd[64];
    sprintf(cmd, "killall openvpn");
    system(cmd);
#endif

    struGsh.sIp = "";

    emit sigVpnconnectsta(false);
    m_disconnectBtn->setStyleSheet(g_disableBtnStyle);
    m_connectBtn->setStyleSheet(g_enableBtnStyle);

    m_connectBtn->setEnabled(true);
    m_disconnectBtn->setEnabled(false);
    
    m_localVirtualAddrBtn->setText(struGsh.sIp);

}

/**
 * @brief remoteCtrlPage::进入远程升级界面时从本地文件中读取配置信息
 */
void remoteCtrlPage::getConfFromFile()
{
    QFile file;
    file.setFileName("./vpn/CHS/client.conf");
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream out(&file);
    QString str, subStr;
    QString ipStr, portStr;
    int ipLen = 0;
    while (!out.atEnd()) {
        str = out.readLine();
        subStr = str.mid(0, 6);
        if (subStr == QString("remote")) {
            for (int i = 7; i < str.length(); i++) {
                if (str.at(i) == ' ') {
                    ipLen = i-7;
                    ipStr = str.mid(7, ipLen);
                    m_serverIPBtn->setText(ipStr);
                    portStr = str.mid(8+ipLen, str.length());
                    m_portBtn->setText(portStr);
                    break;
                }
            }
        }
    }

    file.close();
}

/**
 * @brief remoteCtrlPage::进入远程升级界面时从本地文件中读取用户名密码
 */
void remoteCtrlPage::getPasswordFromFile()
{
    QFile file;
    file.setFileName(CNF_VPNAUTH);

    /* 若认证文件不存在,则新建文件并写入默认的用户名与密码 */
    if(!file.exists()){
        if(!file.open(QIODevice::WriteOnly))
        {
           qDebug("%s : File create error!", CNF_VPNAUTH);
           return;
        }
        QTextStream in(&file);
        in<<"PCUSER1"<<"\n"<<"140219";
        m_usernameBtn->setText("PCUSER1");
        sPassword = "140219";
        m_passwordBtn->setText("●●●●●●");

        file.close();

        return;
    }

    /* 若认证文件存在,则从文件中读取用户名与密码 */
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug("%s : File open error!", CNF_VPNAUTH);
        return;
    }

    QTextStream out(&file);
    QString usernameStr, passwordStr;

    out>>usernameStr;
    out>>passwordStr;
    m_usernameBtn->setText(usernameStr);
    sPassword = passwordStr;
    QString str;
    for(int i = 0 ; i < passwordStr.size() ; ++i) {
        str += "●";
    }
    m_passwordBtn->setText(str);

    file.close();
}

/**
 * @brief remoteCtrlPage::根据用户设置修改配置文件
 */
void remoteCtrlPage::setConfFile()
{
    QFile file;
    file.setFileName("./vpn/CHS/client.conf");
    if( !file.open(QIODevice::ReadWrite))
    {
        qDebug("client.conf open error!");
        return;
    }

    QTextStream io(&file);
    QString str;
    int lineNum = 0;
    while( !io.atEnd())
    {
        str = io.readLine();
        lineNum++;
        QString tmpStr = str.mid(0, 6);
        if(tmpStr == QString("remote"))
        {
            str = QString("remote %1 %2").arg(m_serverIPBtn->text()).arg(m_portBtn->text());

#ifdef Q_OS_UNIX
            char cmd[128];
            sprintf(cmd, "sed -i '%dc\%s' ./vpn/CHS/client.conf", lineNum, str.toLatin1().data());
            system(cmd);
#endif
        }
    }
}

/**
 * @brief remoteCtrlPage::根据用户设置修改用户名密码文件
 */
void remoteCtrlPage::setPasswordFile()
{
    QFile file;
    file.setFileName(CNF_VPNAUTH);
    QTextStream in(&file);

    if(!file.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        qDebug("%s : File open error!", CNF_VPNAUTH);
        return;
    }

    in<<m_usernameBtn->text()<<"\n"<<sPassword;

    file.close();
}

/**
 * @brief remoteCtrlPage::获取本地虚拟网络地址
 * @return
 */
bool remoteCtrlPage::getLocalVirtualAddr()
{
    struGsh.sIp.clear();
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface inter, interfaceList)
    {
        if(inter.name() == QString("tun0"))
        {
            QList<QNetworkAddressEntry> hostAddressList = inter.addressEntries();
            foreach (QNetworkAddressEntry address, hostAddressList)
            {
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    struGsh.sIp = address.ip().toString();
                    m_localVirtualAddrBtn->setText(struGsh.sIp);
                }
            }

            return true;
        }
    }

    return false;
}

/**
 * @brief remoteCtrlPage::判断屏连接服务器是否成功
 * @return
 */
bool remoteCtrlPage::connetToServer()
{
#ifdef Q_OS_UNIX
    char cmd[256];
    int ret;
    // 获取应用程序目录
    QString appDir = QCoreApplication::applicationDirPath();
    QString confPath = appDir + "/vpn/CHS/client.conf";
    QString openvpnCmd = QString("cd /usr/sbin; ./openvpn --config \"%1\" &").arg(confPath);
    QByteArray ba = openvpnCmd.toLocal8Bit();
    sprintf(cmd, "%s", ba.data());
    ret = system(cmd);

    if (ret != 0) {
        // 可以在这里加备用路径逻辑
    }
#endif
    int maxTimes = 30;
    for(int i = 0; i < maxTimes; i++)
    {
        if(getLocalVirtualAddr())
            return true;
        gFlow.msleep(500);
    }
    return false;
}

/*!
 * \brief 槽函数: 响应服务器失效的槽函数
 */
void remoteCtrlPage::onServerUnaliveSlt()
{
    onDisconnectBtnPressed();
}

/*!
 * \brief 槽函数: 连接服务器槽函数
 */
void remoteCtrlPage::onConnectServerSlt()
{
    m_connectBtn->setEnabled(false);
    m_connectBtn->setStyleSheet(g_disableBtnStyle);

    if(getLocalVirtualAddr()) {
        m_disconnectBtn->setEnabled(true);
        m_disconnectBtn->setStyleSheet(g_enableBtnStyle);
        return;
    }

    struGsh.sIp = "";
    if(connetToServer()) {
        m_disconnectBtn->setEnabled(true);
        m_disconnectBtn->setStyleSheet(g_enableBtnStyle);
         emit sigVpnconnectsta(true);
    }else{
        m_connectBtn->setEnabled(true);
        m_connectBtn->setStyleSheet(g_enableBtnStyle);
#ifdef Q_OS_UNIX
        system("killall openvpn");
#endif
        emit sigVpnconnectsta(false);
    }
}

/*!
 * \brief 槽函数: 相应APP获取IP
 */
void remoteCtrlPage::onResponseVPNIPSlt()
{
    onConnectServerSlt();
    emit sigResponseVPNIP();
}

void remoteCtrlPage::onAppCloseVpn()
{
    onDisconnectBtnPressed();
}

/*!
 * \brief remoteCtrlPage::onRefreshVPNStatus
 * \param bstatus
 */
void remoteCtrlPage::onRefreshVPNStatus(bool bstatus)
{
    if(bstatus){
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
        m_connectBtn->setStyleSheet(g_disableBtnStyle);
        m_disconnectBtn->setStyleSheet(g_enableBtnStyle);
    }else{
        m_disconnectBtn->setStyleSheet(g_disableBtnStyle);
        m_connectBtn->setStyleSheet(g_enableBtnStyle);

        m_connectBtn->setEnabled(true);
        m_disconnectBtn->setEnabled(false);
    }

    m_localVirtualAddrBtn->setText(struGsh.sIp);

}
