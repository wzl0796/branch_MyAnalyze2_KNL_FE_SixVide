#ifndef MYSERIALCOM_H
#define MYSERIALCOM_H

#include <QObject>
#include <QFile>
#include "3rdparty/qextserialport/qextserialport.h"
#include "core/globalflow/globalflow.h"
#ifdef Q_OS_UNIX
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifndef CTL_CODE
#define CTL_CODE(function,method) ((4<<16)|((function)<<2) |(method))
#endif
#define  IOCTL_FE_MISC_Set_Com0_485  CTL_CODE(12,0)


#define CMD_MOTOR_ONOFF             0x040E
#define CMD_MOTOR_SPEED             0x0116
#define CMD_INIT_MOTOR              0x030C

#define CMD_MOTOR_SPEED_DISPLAY     0x0004
#define CMD_MOTOR_STATUS            0x0022
#define CMD_MOTOR_STATUS2           0x0024
#define CMD_MOTOR_DIRECTION         0x0102
#define CMD_MOTOR_ACCTIME           0x0144
#define CMD_MOTOR_FUNCITON          0x0218
#define CMD_MOTOR_FUNCITON2         0x021A
#define CMD_MOTOR_FUNCITON3         0x021E
#define CMD_MOTOR_FUNCITON4         0x0220
#define CMD_MOTOR_FUNCITON5         0x0222



#define PRINT_SERIAL_WRITE 0
#define PRINT_SERIAL_READ 0

enum HeadType{
  HEAD_READ,
  HEAD_WRITE,
  HEAD_OTHER,
  HEAD_ALL
};

class MyserialCom : public QObject
{
    Q_OBJECT
public:
    explicit MyserialCom(QObject *parent = nullptr);
    ~MyserialCom();

    bool openserial(void);
    void InitMotor(void);
    void SwMotorOnOff(bool bStatus);
    void SendMotorSpeed(int nSpeed);
    void ResetMotorParams(void);

    int read(char *buf, int len, int portNum=1, int timeout = 0);

private:
    unsigned int ModbusCrc16(char *ptr, int len);
    int write(QByteArray array , int cmd, bool bRead, int headtye);

signals:

public slots:

private:
    QextSerialPort  *m_serialport;
    QHash<int,QByteArray> m_motorinitHash;  //! 初始化命令
};

extern MyserialCom *g_serial;

#endif // MYSERIALCOM_H
