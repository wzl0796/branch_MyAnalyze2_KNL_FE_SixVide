#ifndef REMOTECTRLPAGE_H
#define REMOTECTRLPAGE_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>
#include <QtNetwork>
#include <QRegExp>
#include "params/define.h"
#include "core/globalflow/mylanguage.h"

class remoteCtrlPage : public QWidget
{
    Q_OBJECT
public:
    explicit remoteCtrlPage(QWidget *parent = 0);

    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

signals:
    void sigResponseVPNIP();                        //! 回复VPN虚拟IP
    void sigVpnconnectsta(bool bsta);

public slots:
    void onServerIPBtnPressed(void);                //! 响应服务器地址输入框被按下事件
    void onPortBtnPressed(void);                    //! 响应服务器端口号被按下事件
    void onUsernameBtnPressed(void);                //! 响应用户名输入框被按下事件
    void onPasswordBtnPressed(void);                //! 响应密码输入框被按下事件
    void onConnectBtnPressed(void);                 //! 响应连接按钮按下事件
    void onDisconnectBtnPressed(void);              //! 响应断开连接按钮按下事件
    void onResponseVPNIPSlt();                      //! 相应APP获取IP槽函数
    void onAppCloseVpn();
    void onRefreshVPNStatus(bool bstatus);          //! 根据web服务器连接vpn请求刷新VPN状态

private:
    /* 远程控制页面相关控件 */
    QLabel      *m_serverIPLbl;
    QPushButton *m_serverIPBtn;
    QLabel      *m_portLbl;
    QPushButton *m_portBtn;
    QLabel      *m_usernameLbl;
    QPushButton *m_usernameBtn;
    QLabel      *m_passwordLbl;
    QPushButton *m_passwordBtn;
    QLabel      *m_localVirtualAddrLbl;
    QPushButton *m_localVirtualAddrBtn;
    QPushButton *m_connectBtn;
    QPushButton *m_disconnectBtn;
    QGroupBox   *m_Gb;
    QString     sPassword;

    QString ipAdress;
    QString serverAdress;
    QString serverPort;
    QString user_name;
    QString passwd;
    QString fail;
    QString connect_server;
    QString close_connect;

private:
    void getConfFromFile(void);                     //! 进入远程升级界面时从本地文件中读取配置信息
    void getPasswordFromFile(void);                 //! 进入远程升级界面时从本地文件中读取用户名密码
    void setConfFile(void);                         //! 根据用户设置修改配置文件
    void setPasswordFile(void);                     //! 根据用户设置修改用户名密码文件
    bool getLocalVirtualAddr(void);                 //! 获取本地虚拟网络地址
    bool connetToServer(void);                      //! 判断屏连接服务器是否成功

    /*! meyerCare连接VPN流程槽函数 */
    void onServerUnaliveSlt(void);                  //! 响应服务器失效的槽函数
    void onConnectServerSlt();                      //! 连接服务器槽函数
};

#endif // REMOTECTRLPAGE_H
