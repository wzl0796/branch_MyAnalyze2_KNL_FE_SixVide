#ifndef MYSOCKETMGR_H
#define MYSOCKETMGR_H

#include <QObject>
#include "3rdparty/socketio/mysocketio.h"
#include "3rdparty/qjson/qjson.h"
#include "params/globalparams.h"

/*!
 * \brief WebSocket通信协议
 */
#define CMD_HEART_APP           508     //! 发送给小程序心跳信息
#define CMD_REQ_MAIN            510     //! 小程序请求首页信息
#define CMD_REQ_IP              600     //! APP获取远程连接IP
#define CMD_CTRL_VPN            601     //! APP打开/关闭VPN

using namespace QJson;

/*!
 * \brief websocket管理类
 */
class MySocketMgr : public QObject
{
    Q_OBJECT
public:
    explicit MySocketMgr(QObject *parent = 0);
    ~MySocketMgr();

    void connectServer();                      /*! 连接服务器 */
    void disConnectServer();                   /*! 断开服务器 */
    bool getConnectState(void);                /*! 获取连接状态 */
    void sendHeart();                          /*! websocket心跳检测 */
    void sendMainUI();                         /*! 处理发送首页信息 */
    void closeVPN(JsonObject json);            /*! 关闭VPN */
    void requestSorterSchm(int, QString);      /*! 获取色选机方案信息 */
    void requestSorterOperatorInfo(int analyzeIndex, QString sorterCode,int machineIndex = 0);  /*! 发送请求色选机极简界面信息 */
    void requestSorterOutputInfo(int, QString);
    void requesrSorterAlarmStatusInfo(int, QString);
    void sendFeederMsgToSorter(int, QString, bool); /*! 发送震动器开关消息给整机 */
    void sendChgSchmInfo(int, QString, int);        /*! 发送切换目标方案信息 */
    void sendFeederValueInfo2Sorter(int, QString,int,int);/*! 发送调节震动量信息 */
    void sendOperSensInfo2Sorter(int, QString,int,int);  /*! 发送调节灵敏度信息 */

#ifdef Q_OS_UNIX
    MySocketio* getSocketio();                 /*! 获取socket句柄 */
#endif

signals:
    void sigGetVPNIP();
    void sigCloseVpn();
    void sigRefreshSorterOperPage();           /*! 信号：刷新快检仪映射色选机极简界面 */
    void sigRefreshDisplayPage(int, int);      /*! 信号: 刷新看板界面 */
    void sigControlData(int, int);

public slots:
    void onReceiveSlt(string data);
    void onReqVPNIPSlt();                      /*! 回复VPN虚拟IP */

    //! 获取色选机信息
    void getSorterInfo(void);

private:
#ifdef Q_OS_UNIX
    MySocketio *m_socketio;                    /*! websocket对象 */
#endif

    enum {
        CMD_TYPE_REQUEST = 1,   /*! 请求类命令 */
        CMD_TYPE_RESPONSE       /*! 回复类命令 */
    };

    bool    m_isRequest;
    QString sAppID;                            /*! APP登录ID */
    QString m_curSorterCode;

    int m_AnalyzeIndex;
    int m_MachineIndex;
    QStringList sorterList;
};

extern MySocketMgr *g_socketMgr;

#endif // MYSOCKETMGR_H
