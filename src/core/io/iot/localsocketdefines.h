#ifndef MYLOCALSOCKETDEFINES_H
#define MYLOCALSOCKETDEFINES_H

#include <QVector>

#define KEY_GENERAL_WINID   "winID"
#define KEY_GENERAL_NETWORK "netState"
#define KEY_GENERAL_CONSOLE "consoleState"
#define KEY_ANALYZE         "analyser"
#define KEY_VPN             "vpn"
#define KEY_VERSION         "version"
#define KEY_ALARM           "alarm"
#define KEY_TOKEN           "mqttToken"
#define KEY_UPLOADFILE      "uploadFile"
#define KEY_QUERYFILE       "queryFile"
#define KEY_DOWNLOADFILE    "downloadFile"
#define KEY_SCANCODE        "scancode"
#define KEY_NOTIFY          "notify"

/** 远程控制协议*/
#define CMD_REMOTECTRL_OBTAIN_IP        600     //! 获取IP
#define CMD_REMOTECTRL_OPEN_CLOSE_VPN   601     //! 打开关闭VPN
#define CMD_REMOTECTRL_OPEN_CLOSE_CTRL  602     //! 打开关闭远程控制

#define AUTO_BACKUP_INTERVAL (3*24*3600)        //! 自动备份时间间隔
#define INTERVAL_SOCKET_CHECK   30000            //! 中间件状态检查间隔
#define INTERVAL_SOCKET_CONNECT 30000            //! 中间件连接等待间隔

/*!
 * \enum 枚举命令类型
 */
enum {
    CMD_TYPE_REQUEST = 1,       //! 请求类命令
    CMD_TYPE_RESPONSE           //! 回复类命令
};

enum {
    UPLOAD_AUTO = 1,
    UPLOAD_MANUAL = 2,
};

#endif // MYLOCALSOCKETDEFINES_H
