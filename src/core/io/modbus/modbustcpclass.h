#ifndef MODBUSTCPCLASS_H
#define MODBUSTCPCLASS_H

#include <QObject>
#include "mymodbus_client.h"

#define TCP_BROKEN_LEFT_DATA  1500
#define TCP_TUOKE_LEFT_DATA   1502
#define TCP_BAOYAO_LEFT_DATA  1504
#define TCP_BROKEN_RIGHT_DATA 1506
#define TCP_TUOKE_RIGHT_DATA  1508
#define TCP_BAOYAO_RIGHT_DATA 1510
#define TCP_SAMPLE_MILL_INFO  1900
#define TCP_SAMPLE_POLISH_INFO 3902

#define TCP_SPEED_LEFT        1002
#define TCP_MILL_TARGET       1600
#define TCP_POLISH_TARGET     3500
#define TCP_POLISH_CURRENT    3090
#define TCP_POLISH_PRESSURE   3084

#define TCP_POLISH_SUIMI      3620
#define TCP_POLISH_BAIDU      3622
//#define TCP_SET_CURRENT_LEFT  1004
//#define TCP_SPEED_RIGHT       1006
//#define TCP_SET_CURRENT_RIGHT 1008

class modbusTcpClass : public QObject
{
    Q_OBJECT
public:
    explicit modbusTcpClass(QString ip, int nPort, QObject *parent = 0);

    bool connectToModbus(void);
signals:
    void sigRefreshTargetData(void);

    void updatePLCData(void);

public slots:
    void initTcpSerial(void);

    void getPlcData();

    void sendMillInfoToPlc();                                                 /*! 发送线速差信息到中控 */

    void readMillInfoFromPlc(QVector<int> &vData, int nCmd, int index = 1);

    void readTargetData(int type);                                                /*! 获取目标值信息 */

    void readPolishDataFromPlc(int i, int j, int index, int type);

    void sendSampleInfo(int analyzeIndex,int nValue);                             /*! 发送取样点信息 */

    void sendControlData(int nCmd, int value);                                    /*! 发送数据到组态PLC */
private:
    enum{
        TYPE_BROKEN = 0,
        TYPE_TUOKE = 1,
        TYPE_BAOYAO = 2
    };
    MyModbusC *m_myModbus;

    QString server_ip;
    int server_num;

    bool get_state = false;
};

extern modbusTcpClass* g_modbusTcpClass;

#endif // MODBUSTCPCLASS_H
