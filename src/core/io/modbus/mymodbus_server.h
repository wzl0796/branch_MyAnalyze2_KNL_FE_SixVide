/*! \copyright Copyright (C) 2021 Hefei Meyer Optoelectronic Technology Inc.
 *             All rights reserved.
 * \date    2021.07.22
 * \author  王斌
 * \brief   Modbus通信
 */
#ifndef MYMODBUS_H
#define MYMODBUS_H

#include <QObject>
#include <QModbusTcpServer>
#include <QModbusRtuSerialSlave>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusDataUnit>
#include <QTime>
#include <QApplication>
#include <QtDebug>
#include "params/define.h"
#include "core/globalflow/globalflow.h"
#include "mymodbus_client.h"

enum CommunType{
    InputReg,
    HoldReg,
    TypeNum
};

/*!
 * \brief Modbus通信类
 */
class MyModbus : public QObject
{
    Q_OBJECT
public:
    explicit MyModbus(int nType, QObject *parent = nullptr);

    void setId(int nId) {m_nId = nId;}      //! 设置ID
    bool connectToModbus(void);             //! 连接
    void disConnect();                      //! 断开连接
    bool isActive(void);                    //! 判断Modbus连接状态
    void setRegister(QList<int>, QList<int>); //! 发送数据
    void dataWritten(QModbusDataUnit::RegisterType table, int address, int size);
    QVector<quint16> values() const { return m_values;}
    void setServer(QString ip, int nPort);  //! 设置服务器IP及端口
    void setServer(QString portName);       //! 设置端口

protected:
    void timerEvent(QTimerEvent *event);    //! 定时器事件

signals:
    void sigFeederCtrl();                   //! 供料开关控制
    void sigChgSchm();                      //! 切换方案
    void sigChgPeople();                    //! 切换检验员
    void sigChgOriginal();                  //! 切换来源
    void sigClear();                        //! 清零
    void sigRefreshDetectSN();              //! 刷新检测编号
    void sigRefreshStatisHeader(int,QString);   //! 刷新统计标题

public slots:
    void onStateChanged(QModbusDevice::State state);//! 槽：Modbus状态变化
    void handleDeviceError(QModbusDevice::Error newError);

private:
    void msleep(int msecs);               //! 休眠函数
    bool _confirmConnected(void);         //! 确认连接状态
    void _analyzeRiceData(int, quint16); //! 解析数据

private:
    QModbusServer *m_server;
    QString m_sIP, m_portName;
    int m_nPort;
    int m_nId = 0;
    int m_nType;      /*! 0 TCP / 1 Serial */
    QVector<quint16> m_values;
    QByteArray sDetectdata;
    analyzeShare  *m_pAnalyzeShare;
    analyzeGlobal *m_analyze;
};

extern MyModbus *m_modbusS;

#endif // MYMODBUS_H
