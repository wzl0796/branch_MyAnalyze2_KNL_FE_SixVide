#include "myudp.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"

//myUdp *g_myUdp;
QList<myUdp*>       g_udpSocket;
QList<QThread*>     g_udpthread;

myUdp::myUdp(int port_num, QObject *parent) : QObject(parent)
{
    n_Port         = PORT_DATA + port_num;
    n_Port_1808    = PORT_DATA_1808 + port_num;
    m_analyzeIndex = port_num;
}

myUdp::~myUdp()
{
//    my_udp_ai->~myUdpAI();
    my_udp_control->~myUdpControl();
}

/**
 * @brief myUdp::sltInitSocket
 */
void myUdp::sltInitSocket()
{
    my_udp_control = new myUdpControl(n_Port);
//    gFlow.bAiSetCpu(3);

    my_udp_control->setAnalyzeIndex(m_analyzeIndex);


    connect(my_udp_control, SIGNAL(sigImageDataReady(QByteArray,int,int)), this, SIGNAL(sigImageDataReady(QByteArray,int,int)));
    connect(my_udp_control,SIGNAL(sigSocInferResultReady(std::vector<BBox>,int,uint)),this,
            SIGNAL(sigSocinferFinished(std::vector<BBox>,int,uint))); //! SOC推理结果上传
    connect(my_udp_control, SIGNAL(sigIM01SensorTempture(int)), this, SIGNAL(sigIM01Tempure(int)));
    connect(my_udp_control,SIGNAL(sigSocInferBottleRest(QByteArray,int,uint)),this,
            SIGNAL(sigSocinferBottleFinished(QByteArray,int,uint))); //! SOC推理结果上传
}

/**
 * @brief myUdo::重新绑定socket
 */
void myUdp::rebindSocket()
{
    my_udp_control->setAnalyzeIndex(m_analyzeIndex);

    my_udp_control->rebindSocket();

//    my_udp_ai->setAnalyzeIndex(m_analyzeIndex);

    //    my_udp_ai->rebindSocket();
}

/*!
 * \brief 槽: 发送数据包
 * \param data
 * \param nCount
 * \param port
 */
void myUdp::onSenddataSlt(QByteArray data, int nCount, quint16 port)
{
    my_udp_control->writeDatagram(data,nCount,port);
}

/**
 * @brief myUdp::发送数据
 * @param data
 * @param nCount
 * @param port
 */
void myUdp::writeDatagram(QByteArray data, int nCount, quint16 port)
{
    emit sigemitdata(data,nCount,port);
}

/*!
 * \brief 读取回包
 * \param buff
 * \param rcvLength
 * \param timeout
 * \return
 */
int myUdp::readDatagram(char *buff, int rcvLength, int timeout)
{
    int length = 0;
    QTime time;
    time.start();

    do{
        QByteArray tmp;
        int singleLength = my_udp_control->readDatagram(tmp.data(),rcvLength);
        if(singleLength > 0){
            if(checkHeadAndTail(tmp.constData(), singleLength)){     //每一帧头尾校验正确
                memcpy(buff+length, tmp.data(), tmp.size());
                length += singleLength;
                printf("length: %d   \t", length); fflush(0);
            }
        }
    } while(time.elapsed() < timeout
             && length < rcvLength);

    return length;
}


/**
 * @brief myUdp::设置局部变量
 * @param analyzeIndex
 */
void myUdp::setAnalyzeIndex(int analyzeIndex)
{
    m_analyzeIndex = analyzeIndex;
}

/**
 * @brief myUdp::resetSimBuffer
 */
void myUdp::resetSimBuffer()
{
//    my_udp_ai->resetSimBuffer();
}

void myUdp::clearudpbuff()
{
    my_udp_control->clearUdpSocketPortData();
}

bool myUdp::checkHeadAndTail(const char *recvbuf, int dataLen)
{
    bool result = false;
    if((recvbuf[0] == 0xAA
         && recvbuf[1] == 0xAA
         && recvbuf[2] == 0xAA
         && recvbuf[3] == 0xAA
        )
        ||(recvbuf[0] == 0xAD
            && recvbuf[1] == 0xAD
            && recvbuf[2] == 0xAD
            && recvbuf[3] == 0xAD
            )){
        result = true;
    }else{
        printf("checkHead error\n");
        fflush(0);
    }
    return result;
}
