#ifndef MODBUSCTLCLASS_H
#define MODBUSCTLCLASS_H

#include "mymodbus_client.h"
#include "mymodbus_server.h"

class modbusCtlClass : public QObject
{
    Q_OBJECT
public:
    explicit modbusCtlClass(QObject *parent = 0);

    void powerAnaCmd(int, bool);
    void powerMachineCmd(int, int, bool);
    void getPlcVersion();
    bool getAnaPower(int index);
    void softShutDown();
    void alarmLightSet(bool bRed = false, bool bYellow = false, bool bGreen = false);


signals:
    void sigGetPlcVer();

public slots:
    void getMillData();

    int setMillData(QVector<int> vData, int nCmd, int nTimeOut = 50);

    void onCtrMillByPlcSlt(int data, int nCmd);
private:
    MyModbusC* m_myModbus;

    bool set_state;
    bool get_state;
};

extern modbusCtlClass* g_modbusCtlClass;

#endif // MODBUSCTLCLASS_H
