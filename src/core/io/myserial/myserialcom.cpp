#include "myserialcom.h"

MyserialCom *g_serial;

MyserialCom::MyserialCom(QObject *parent) : QObject(parent)
{
    openserial();

    QByteArray array;
    array.resize(2); 
    array[0] = 0;
    array[1] = 7;
    m_motorinitHash[CMD_MOTOR_SPEED_DISPLAY] =array;
    m_motorinitHash[CMD_MOTOR_STATUS] =array;

    array[1] = 18;
    m_motorinitHash[CMD_MOTOR_STATUS2] =array;

    array[1] = 2;
    m_motorinitHash[CMD_MOTOR_DIRECTION] =array;

    array[0] = (65536>>8)&0xFF;
    array[1] = 65536&0x00FF;
    m_motorinitHash[CMD_MOTOR_ACCTIME] =array;

    array[0] = 0;
    array[1] = 20;
    m_motorinitHash[CMD_MOTOR_FUNCITON] =array;

    array[1] = 21;
    m_motorinitHash[CMD_MOTOR_FUNCITON2] =array;

    array[1] = 0;
    m_motorinitHash[CMD_MOTOR_FUNCITON3] =array;
    m_motorinitHash[CMD_MOTOR_FUNCITON4] =array;
    m_motorinitHash[CMD_MOTOR_FUNCITON5] =array;

}

MyserialCom::~MyserialCom()
{
    if (m_serialport->isOpen()) {
        m_serialport->close();
    }
}

/*!
 * \brief MyserialCom::openserial
 * \return
 */
bool MyserialCom::openserial()
{
#ifdef Q_OS_UNIX
    if (QFile::exists("/dev/femisc")) {
        int fd = open("/dev/femisc", O_RDWR);
        if (fd == -1) {
            qWarning("Failed to open file /dev/femisc");
            return false;
        }
        qDebug()<<"init 485 sucess";
        ioctl(fd, IOCTL_FE_MISC_Set_Com0_485, 0);
        close(fd);
    }
#endif

    bool bStat = true;

    struct PortSettings comSetting = {BAUD9600, DATA_8, PAR_NONE, STOP_2, FLOW_OFF, 100};
    QString strName = QString("/dev/ttyS1");
    m_serialport = new QextSerialPort(strName, comSetting, QextSerialPort::Polling);
    bool bRet = m_serialport->open(QIODevice::ReadWrite);
    if (!bRet) {
        bStat = false;
        qDebug()<<"open serial failed!";
    }

    return bStat;
}

/*!
 * \brief 初始化电机
 */
void MyserialCom::InitMotor()
{
    QByteArray array;
    array.resize(2);
    array[0] = 0;
    array[1] = 1;

    write(array, CMD_INIT_MOTOR, false, HEAD_WRITE);
}

/*!
 * \brief 开关伺服
 */
void MyserialCom::SwMotorOnOff(bool bStatus)
{
    QByteArray array;
    array.resize(2);
    array[0] = 0;
    array[1] = bStatus?1:0;
    write(array, CMD_MOTOR_ONOFF, false, HEAD_WRITE);

}

/*!
 * \brief 设置伺服速度
 */
void MyserialCom::SendMotorSpeed(int nSpeed)
{
    QByteArray array;
    array.resize(7);
    array.fill(0);

    array[0] = 0;
    array[1] = 0x02;
    array[2] = 0x04;
    array[3] = nSpeed/256;
    array[4] = nSpeed%256;

    write(array, CMD_MOTOR_SPEED, false, HEAD_OTHER);

}

/*!
 * \brief 重置点击初始化参数
 */
void MyserialCom::ResetMotorParams()
{
    QHash<int,QByteArray>::const_iterator it = m_motorinitHash.constBegin();
    while(it !=m_motorinitHash.constEnd()){
        int cmd = it.key();
        QByteArray array = it.value();
        write(array, cmd, false, HEAD_WRITE);
        ++it;
    }
}

/*!
 * \brief 串口读函数
 */
int MyserialCom::read(char *buf, int len, int portNum, int timeout)
{
    return -1;
}

/*!
 * \brief Modbus校验计算
 */
unsigned int MyserialCom::ModbusCrc16(char *ptr, int len)
{
    int i;
    unsigned char j;
    unsigned int crc = 0xFFFF;
    for(i = 0; i < len; i++){
        crc ^=((*ptr++) & 0x00FF);
        for(j = 0; j < 8; j++){
            if(crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else {
                crc >>= 1;
            }
        }
    }
    return(crc);
}

/*!
 * \brief 串口写函数
 */
int MyserialCom::write(QByteArray array, int cmd, bool bRead, int headtye)
{
    int nDatalen = array.size();
    int ret = 0;
    char *Packet = new char[nDatalen+6];

    switch (headtye) {
    case HEAD_READ:
        Packet[0] = 0x01;
        Packet[1] = 0x03;
        break;
    case HEAD_WRITE:
        Packet[0] = 0x01;
        Packet[1] = 0x06;
        break;
    case HEAD_OTHER:
        Packet[0] = 0x01;
        Packet[1] = 0x10;
        break;
    default:
        break;
    }

    Packet[2] = cmd/256;
    Packet[3] = cmd%256;

    memcpy(Packet + 4, array.data(), nDatalen);

    unsigned int crc = ModbusCrc16(Packet, nDatalen+4);

    Packet[nDatalen+4] = crc%256;
    Packet[nDatalen+5] = crc/256;

#if PRINT_SERIAL_WRITE
    printf("[serial W:]  ");

    for(int i=0 ;i< nDatalen + 6; i++){
        printf("%02x  ",(uchar)Packet[i]);
    }
    printf("\n");fflush(0);
#endif

    if(m_serialport != NULL){
        if(bRead){
            m_serialport->readAll();
        }
        ret = m_serialport->write(Packet, nDatalen+6);
        gFlow.msleep(50);
    }

    delete []Packet;
    return ret;
}
