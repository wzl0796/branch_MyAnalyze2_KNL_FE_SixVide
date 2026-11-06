#ifndef NETCONFIGPAGE_H
#define NETCONFIGPAGE_H

#include <QWidget>
#include <QtCore>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QtNetwork>
#include "common/myinputpanel.h"
#include "common/mymessagebox.h"
#include "params/define.h"
#include "core/globalflow/mylanguage.h"

#define DHCP_TIMEOUT    10*1000

/*!
 * \enum 枚举网络配置类型
 */
enum {
    NET_IP = 0,     //! IP地址
    NET_MASK,       //! 子网掩码
    NET_GATEWAY,    //! 网关
    NET_TYPE        //! 3
};

class netConfigPage : public QWidget
{
    Q_OBJECT
public:
    explicit netConfigPage(QWidget *parent = 0);

    void checkIsDHCP();

    QString getNetText(int index);       //! 获取网络配置选项文本

    void getHostAddress();

    QString getIP();

    QString getGateway();

    bool runDHCP();

    bool isIPAddr(QString ip);

signals:

public slots:
    void setHostAddress(int index);

    void onAutoSetBtnClicked();

    void onManualSetBtnClicked();

private:
    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    QString  auto_set;
    QString  manual_set;
    QString  address;
    QString  netmask;
    QString  gateway;
    QString  fail;
    QString  para_type_error;

    QString  plc_center_ip;
    QString  plc_center_port;
    QString  plc_center;

private:
    QLabel              *m_hostLbl[NET_TYPE];
    QPushButton         *m_hostBtn[NET_TYPE];
    QPushButton         *m_autoSetBtn, *m_manualSetBtn;
    QSignalMapper       *m_sigMapper;
    QGroupBox           *m_Gb;
};

#endif // NETCONFIGPAGE_H
