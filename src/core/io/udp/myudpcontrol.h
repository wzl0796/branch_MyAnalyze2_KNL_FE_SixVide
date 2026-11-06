#ifndef MYUDPCONTROL_H
#define MYUDPCONTROL_H

#include <QObject>
#include <QUdpSocket>
#include <QMutex>
#include "params/globalparams.h"
#include "params/define.h"

#define DEBUG_PRINT_WRITE 0
#define DEBUG_PRINT_READ 0
//! 枚举FPGA升级状态
enum {
    UPGRADE_UNSTARTED = 0,  /*!< 状态: 未开始 */
    UPGRADE_DOING,          /*!< 状态: 升级中 */
    UPGRADE_FACTORY,        /*!< 状态: 切换至工厂模式 */
    UPGRADE_ERR_FACTORY,    /*!< 状态: 切换工厂模式失败 */
    UPGRADE_SEND,           /*!< 状态: 发送升级数据包 */
    UPGRADE_ERR_SEND,       /*!< 状态: 发送数据包失败 */
    UPGRADE_BURN,           /*!< 状态: 烧写程序 */
    UPGRADE_ERR_BURN,       /*!< 状态: 烧写程序失败 */
    UPGRADE_USER,           /*!< 状态: 切换至用户模式 */
    UPGRADE_ERR_USER,       /*!< 状态: 切换用户模式失败 */
    UPGRADE_ERASE,          /*!< 状态: 控制板擦除 */
    UPGRADE_ERR_ERASE,      /*!< 状态: 控制板擦除失败 */
    UPGRADE_OK              /*!< 状态: 成功 */
};

//! FPGA反馈状态掩码
enum {
    MASK_CTRL_BURN_FLASH_FILE	    = 0x01 << 0,    /*!< 控制板烧写用户配置文件状态 */
    MASK_CTRL_LOAD_CUSTOM_MODE	    = 0x01 << 1,    /*!< 控制板载入用户模式成功状态 */
    MASK_CTRL_LOAD_FACTORY_MODE	    = 0x01 << 2,    /*!< 控制板载入工厂模式成功状态 */
    MASK_CRC_ERROR                  = 0x01 << 3,    /*!< CRC校验错误状态 */
    MASK_CTRL_BURN_SDRAM_PAGE       = 0x01 << 6,    /*!< 控制板页文件写入SDRAM成功状态 */
    MASK_BOARD_ADDRESS_ERROR        = 0x01 << 5,    /*!< 数据包板地址错误状态 */
    MASK_CTRL_ERASE                 = 0x01 << 4,    /*!< 控制板擦除成功状态 */
    MASK_CAM_BURN_FLASH_FILE	    = 0x01 << 0,    /*!< 相机版烧写用户配置文件状态 */
    MASK_CAM_LOAD_CUSTOM_MODE	    = 0x01 << 1,    /*!< 相机版载入用户模式成功状态 */
    MASK_CAM_LOAD_FACTORY_MODE	    = 0x01 << 2,    /*!< 相机版载入工厂模式成功状态 */
    MASK_CAM_SEND_MODE	            = 0x01 << 4     /*!< 相机版文件写入SDRAM成功状态 */
};
extern QMutex g_udpMutex;

class homeWidget;
class myUdpControl : public QObject
{
    Q_OBJECT

public:
    explicit myUdpControl(int n_port, QObject *parent = 0);

    ~myUdpControl();

    void setAnalyzeIndex(int analyzeIndex);

    void rebindSocket(void);

    void writeDatagram(QByteArray data, int nCount = 3, quint16 port = DEST_PORT);   //! UDP发送数据

    int readDatagram(char* data, int ndataLen);   //! UDP数据接收

    void clearUdpSocketPortData();

    bool write(QByteArray ba, bool bResponse=false, bool bBoradCast=false); //! 写操作
    QByteArray read(int nWaitTime=5);   //! 读操作
    QByteArray writeRead(QByteArray ba, int nWaitTime=5);   //! 写读操作

signals:
    void sigImageDataReady(QByteArray,int,int); /*! 信号: 波形刷新 */
    void sigSocInferResultReady(std::vector<BBox> vbox, int nCamId, uint nRow);
    void sigIM01SensorTempture(int);
    void sigSocInferBottleRest(QByteArray,int,uint);

public slots:
    void sltReadData();

private:
    QUdpSocket    *m_udpSocket;
    analyzeShare  *m_pAnalyzeShare;
    analyzeGlobal *m_pAnalyzeGlobal;
    int            m_analyzeIndex;
    int            m_port;
    QMutex         m_mutex;
private:
    void processDatagram(QByteArray datagram);

    QByteArray getContent(QByteArray array);

    int getContentMarkSize(QByteArray array);

    void _analysisVersion(QByteArray array);          /*! 解析版本 */

    void _analysisSocVersion(QByteArray array);       /*! 解析SOC版本 */

    void _analysisCurve(QByteArray array);            /*! 解析波形 */

    void _analysisFpgaStatus(QByteArray array);       /*! 解析升级状态 */

    void _analysisAIUpdateStatus(QByteArray array);   /*! 解析AI板升级状态 */

    void _analysisSocmodelinfo(QByteArray array);    /*! 解析SOC模型信息 */

    void _analysisSocmodellabinfo(QByteArray array);    /*! 解析SOC模型标签信息 */

    void _analysisSocInferRes(QByteArray array);    /*! 解析SOC推理结果 */

    void _analysisIM01SensorTempure(QByteArray array);

    void _analysisSocImgParams(QByteArray array);

    void _analysisMetalNum(QByteArray array);

    void _analysisHalogenStatus(QByteArray array);  //! 检测卤素灯状态

    void _analysiscleanjarStatus(QByteArray array);  //! 检测清灰缸状态

    void _analysisAirAlarm(QByteArray array);  //! 气压报警

    void _analysisStainfo(QByteArray array);  //! 解析统计类别信息

    void _analysisCtrlValveSta(QByteArray array); //! 控制板电压检测

    void _analysisInfraTempure(QByteArray array); //! 红外相机温度
};

#endif // MYUDPCONTROL_H
