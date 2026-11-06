#ifndef MODBUSASCIICLASS_H
#define MODBUSASCIICLASS_H

#include <QObject>
#include "params/define.h"
#include <QSerialPort>
#include <QSerialPortInfo>

#define PLC_SPEED_LEFT       4596
//#define PLC_SET_CURRENT_LEFT 4616
#define PLC_FREQUENCY_LEFT   5576
#define PLC_CURRENT_LEFT     5577
#define PLC_PRESSURE_LEFT    42672

#define PLC_SPEED_RIGHT       4646
//#define PLC_SET_CURRENT_RIGHT 4666
#define PLC_FREQUENCY_RIGHT   5608
#define PLC_CURRENT_RIGHT     5609
#define PLC_PRESSURE_RIGHT    42673

class modbusASCIIclass : public QObject
{
    Q_OBJECT
public:
    explicit modbusASCIIclass(int index, QObject *parent = 0);

    void getData(int nCmd);

    int readSerialData(bool print = false);

signals:

public slots:
    void openDevice(void);

    void writeData(int nCmd, int value);

private:
    QSerialPort *mymodbus_ascii;
    int port_num;
    QString portName;
    uchar asciiToHex(uchar ascii);
    unsigned char hexToAscii(unsigned char hex);
    unsigned char cal_lrc2(unsigned char *data, int len);
};

extern QList<modbusASCIIclass*>       g_modbusAscii;
extern QList<QThread*>                g_mod_ascii_thread;


#endif // MODBUSASCIICLASS_H
