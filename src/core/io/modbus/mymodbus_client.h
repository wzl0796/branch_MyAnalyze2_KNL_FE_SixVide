/*! \copyright Copyright (C) 2021 Hefei Meyer Optoelectronic Technology Inc.
 *             All rights reserved.
 * \date    2021.07.22
 * \author  王斌
 * \brief   Modbus通信
 */
#ifndef MYMODBUSC_H
#define MYMODBUSC_H

#include "params/define.h"
#include <QObject>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusDataUnit>
#include <QTime>
#include <QApplication>
#include <QtDebug>

//! PC->PLC
#define IPC_PLC_SPEED               1000      //! 速度
#define IPC_PLC_RESET               1002      //! 置零
#define IPC_PLC_POSITION            1004      //! 目标位置
#define IPC_PLC_START               1006      //! 开始
#define IPC_PLC_POINT               1008      //! 点动

//! PLC->PC
#define PLC_IPC_SPEED               2000      //! 速度
#define PLC_IPC_POSITION            2002      //! 目标位置
#define PLC_IPC_STATUS              2004      //! 状态

//! PC->PLC
#define PLC_ANAPOWER_ADDR           3000        //! 节点电源控制地址
#define PLC_ANAPOWER_OFFSET         10          //! 节点电源控制偏置
#define PLC_MACHINE_OFFSET          2           //! 联调设备电源控制偏置

//! PLC->PC
#define PLC_VER                     3200        //! plc版本查询
#define PLC_ANA_POWER               (PLC_ANAPOWER_ADDR+1)  //! 分析仪1开关状态
#define PLC_SERVER_SOFT_SHUTDOWN    3100        //! 软关机
#define PLC_RED_LIGHT_ALARM         3210        //! 报警红灯开关
#define PLC_YELLOW_LIGHT_ALARM      3212        //! 报警黄灯开关
#define PLC_GREEN_LIGHT_ALARM       3214        //! 报警绿灯开关

//! PLC->砻谷机(天鑫) 数据读
#define PLC_MILL_DATA               4000        //! 读plc砻谷机数据

//! PLC->砻谷机(天鑫) 数据写
#if 1
#define PLC_SET_TIME_FIRST_LEFT     4100        //! 1号砻谷机 位一运行时间设置 左辊
#define PLC_SET_TIME_SENCOND_LEFT   4102        //! 1号砻谷机 位二运行时间设置 左辊
#define PLC_SET_SPEED_LEFT          4104        //! 1号砻谷机 线速差设置      左辊    2号砻谷机 左辊 4124
#define PLC_SET_CURRENT_LEFT        4106        //! 1号砻谷机 电流设置        左辊    2号砻谷机 左辊 4126
#define PLC_SET_START_LEFT          4108        //! 1号砻谷机 电机启停        左辊    2号砻谷机 左辊 4128



#define PLC_SET_SPEED_RIGHT         4114        //! 1号砻谷机 线速差设置      右辊    2号砻谷机 右辊 4134
#define PLC_SET_CURRENT_RIGHT       4116        //! 1号砻谷机 电流设置        右辊    2号砻谷机 右辊 4136

#define PLC_SET_START_RIGHT         4118        //! 1号砻谷机 电机启停        右辊    2号砻谷机 右辊 4138
#endif


#define PLC_RESCREEN_DATA           5000
#define PLC_SET_RESCREEN_DATA       5100

#define PLC_SET_POLISH_DATA         6100

//! 小麦通信协议
//! 发送
#define MD_WHEAT_GOOD              00      //! 好料
#define MD_WHEAT_INCOMPLETE        01      //! 破碎
#define MD_WHEAT_SICKNESS          02      //! 病斑
#define MD_WHEAT_MILDEW            03      //! 生霉
#define MD_WHEAT_WORMHOLE          04      //! 虫洞
#define MD_WHEAT_SPROUT            05      //! 生芽
#define MD_WHEAT_IMPURITY          06      //! 杂质

//! 大米通信协议
#define MD_RICE_GOOD               0      //! 样品纯度
#define MD_RICE_SICKNESS           1      //! 不完善率
#define MD_RICE_YELLOW             2      //! 黄米率
#define MD_RICE_DYELLOW            3      //! 深黄率
#define MD_RICE_LYELLOW            4      //! 浅黄率
#define MD_RICE_EWHITE             5      //! 垩白率
#define MD_RICE_BELLYWHITE         6      //! 整白率
#define MD_RICE_COREWHITE          7      //! 芯白率
#define MD_RICE_WHITEDEGREE        8      //! 垩白度
#define MD_RICE_WHITERATE          9      //! 垩白粒率
#define MD_RICE_BROKEN            10      //! 碎米率
#define MD_RICE_SBROKEN           11      //! 小碎米率
#define MD_RICE_LENGTH            12      //! 长
#define MD_RICE_WIDTH             13      //! 宽
#define MD_RICE_WHRATE            14      //! 长宽比
#define MD_RICE_MIX               15      //! 互混率
#define MD_RICE_EMBROY            16      //! 留胚率
#define MD_RICE_BACKLINE          17      //! 背线率
#define MD_RICE_BRANDEGREE        18      //! 留皮度
#define MD_RICE_PRECISION         19      //! 加工精度
#define MD_RICE_GRADE             20      //! 大米等级
#define MD_RICE_SCHM              21      //! 化验方案
#define MD_RICE_OUTPUT            22      //! 检测粒数
#define MD_RICE_PEOPLE            23      //! 检验员
#define MD_RICE_ORIGINAL          24      //! 样品来源
#define MD_END_DETECT             25      //! 结束检测标志位 1：正在检测 0：结束检测
#define MD_MATTYPE                26      //! 检测物料 0：大米    5：糙米
#define MD_STARTTYPE              27      //! 启动类别 1：设备启动 2：modbus启动

//! 角山通信协议
#define MD_LK2360_SAMPLE          28      //! 取样索引

//! 接收
#define MD_FEED_STATUS            31      //! 下料状态
#define MD_FEED_AMOUNT            33      //! 振动量
#define MD_CLEAR                  34      //! 清零
#define MD_DETECTNO_ENA           36      //! 检测编号使能
#define MD_DETECTNO1              37      //! 检测编号

/*!
 * \brief Modbus通信类
 */
class MyModbusC : public QObject
{
    Q_OBJECT
public:
    explicit MyModbusC(int nType, QObject *parent = nullptr);

    void setId(int nId) {m_nId = nId;}      //! 设置ID
    bool connectToModbus(void);     //! 连接MODBUS服务器
    bool readCoils(int addr, quint16 num, int serverId);    //! 读线圈数据
    bool readHoldingRegisters(int addr, quint16 num = 1, int serverId = 1); //! 读保持寄存器
    bool writeCoils(QString str, int addr, int num);//! 写线圈数据
    bool writeHoldingRegisters(QVector<int> vData, int addr, int serverId = 1); //! 写保持寄存器
    bool isActive(void);    //! 判断Modbus连接状态
    void setServer(QString ip, int nPort);  //! 设置服务器IP及端口
    void setServer(QString portName);
    QVector<quint16> values() const { return m_values;}

protected:
    void timerEvent(QTimerEvent *event);    //! 定时器事件

signals:

public slots:
    void onStateChanged(QModbusDevice::State state);//! 槽：Modbus状态变化
    void readReady();                //! 读数据槽函数

private:
    void msleep(int msecs);          //! 休眠函数
    bool _confirmConnected(void);    //! 确认连接状态

private:
    QModbusClient *m_client;
    QString m_sIP, m_portName;
    int m_nPort;
    int m_nId = 0;
    int m_nType;      /*! 0 TCP / 1 Serial */
    QVector<quint16> m_values;
};

#endif // MYMODBUSC_H
