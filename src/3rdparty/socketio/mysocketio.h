#ifndef MYSOCKETIO_H
#define MYSOCKETIO_H

#include <QObject>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include "3rdparty/socketio/sio_client.h"

using namespace sio;
using namespace std;

extern socket::ptr g_socket;
extern bool g_bConnected;
extern bool g_bIsLogin;
extern bool g_bIsOnline;

class ConnectionListener
{
    sio::client &handler;

public:
    ConnectionListener(sio::client &h):handler(h) {
    }

    void onConnected() {
        cout << "websocket connected" << endl;
        g_bConnected = true;
        g_bIsLogin = true;
    }
    
    void onClosed(client::close_reason const& reason) {
        cout << "websocket closed" << endl;
        g_bConnected = false;
    }

    void onFailed() {
        cout << "websocket failed" << endl;
        g_bConnected = false;
    }
};

class MySocketio : public QObject
{
    Q_OBJECT

public:
    MySocketio();

    void connect(string url);               /*!< 连接服务器 */
    void disconnect(void);                  /*!< 断开连接 */

    void bindEvents(socket::ptr &socket);   /*!< 绑定事件 */
    void sendMsg(string data);              /*!< 发送数据 */
    void sendMsgToSorter(string data);      /*!< 发送数据给整机 */
    void sendLogin(string data);            /*!< 发送登陆信息 */
    void sendLineLogin(string data);        /*!< 发送产线登陆信息 */
    void sendRoom(string data);             /*!< 按组发送数据 */
    void sendHeart(string data);            /*!< 发送心跳 */

    bool getConnectState(void);             /*!< 获取连接状态 */
    bool getIsLogin(void);                  /*!< 获取登陆状态 */
    void setIsLogin(bool flag);             /*!< 设置登陆状态 */
    bool getIsOnline(void);                  /*!< 获取在线状态 */
    void setIsOnline(bool flag);            /*!< 设置在线状态 */

signals:
    void receive(string data);              /*!< 接收数据 */

private:
    char m_server[64];
    client m_client;
};

#endif // MYSOCKETIO_H
