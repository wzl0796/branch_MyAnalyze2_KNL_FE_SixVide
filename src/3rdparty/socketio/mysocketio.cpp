#include "mysocketio.h"

socket::ptr g_socket;
bool g_bConnected = false;
bool g_bIsLogin = false;
bool g_bIsOnline = false;

/*!
 * \brief MySocketio::MySocketio
 */
MySocketio::MySocketio()
{
}

/*!
 * \brief MySocketio::connect
 */
void MySocketio::connect(string url)
{
    //! 设置监听器
    ConnectionListener l(m_client);
    
    g_socket = m_client.socket();
    bindEvents(g_socket);
    
    m_client.set_open_listener(bind(&ConnectionListener::onConnected, &l));
    m_client.set_close_listener(bind(&ConnectionListener::onClosed, &l, placeholders::_1));
    m_client.set_fail_listener(bind(&ConnectionListener::onFailed, &l));

    //! 连接服务器
    map<string, string> query;
    query.insert(pair<string, string>("token", "meyer-sorter-2018"));
    m_client.connect(url, query);
}

/*!
 * \brief MySocketio::disconnect
 */
void MySocketio::disconnect()
{
    m_client.sync_close();
    m_client.clear_con_listeners();
}

/*!
 * \brief MySocketio::bindEvents
 * \param socket
 */
void MySocketio::bindEvents(socket::ptr &socket)
{
    socket->on("message",
                sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
                   string message = data->get_string();
                   emit receive(message);
                }));

    socket->on("heart",
                sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list &ack_resp) {
                   string message = data->get_string();
                   g_bIsOnline = true;
                }));
}

/*!
 * \brief MySocketio::send
 * \param data
 */
void MySocketio::sendMsg(string data)
{
    g_socket->sio_emit("message", data);
}

/*!
 * \brief MySocketio::sendMsgToSorter
 * \param data
 */
void MySocketio::sendMsgToSorter(string data)
{
    g_socket->sio_emit("direct", data);
}

/*!
 * \brief MySocketio::sendHeart
 * \param data
 */
void MySocketio::sendHeart(string data)
{
    g_socket->sio_emit("heart", data);
}

/*!
 * \brief MySocketio::sendLogin
 * \param data
 */
void MySocketio::sendLogin(string data)
{
    g_socket->sio_emit("login", data);
}

/*!
 * \brief MySocketio::sendLineLogin
 */
void MySocketio::sendLineLogin(string data)
{
    g_socket->sio_emit("joinRoom", data);
}

/*!
 * \brief MySocketio::sendRoom
 * \param data
 */
void MySocketio::sendRoom(string data)
{
    g_socket->sio_emit("room", data);
}

/*!
 * \brief MySocketio::getConnectState
 * \return
 */
bool MySocketio::getConnectState()
{
    return g_bConnected;
}

/*!
 * \brief MySocketio::getIsLogin
 * \return
 */
bool MySocketio::getIsLogin()
{
    return g_bIsLogin;
}

/*!
 * \brief MySocketio::setIsLogin
 * \param flag
 */
void MySocketio::setIsLogin(bool flag)
{
    g_bIsLogin = flag;
}

/*!
 * \brief MySocketio::getIsOnline
 * \return
 */
bool MySocketio::getIsOnline()
{
    return g_bIsOnline;
}

/*!
 * \brief MySocketio::setIsOnline
 * \param flag
 */
void MySocketio::setIsOnline(bool flag)
{
    g_bIsOnline = flag;
}

