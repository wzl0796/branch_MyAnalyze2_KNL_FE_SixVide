#ifndef LOCALSOCKETTHREAD_H
#define LOCALSOCKETTHREAD_H

#include <QObject>
#include <QThread>
#include <functional>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "localsocket.h"
#include "localsocketdefines.h"
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"

using namespace std;

enum{
    CTRL_SUCCESS = 200,
    CTRL_FAIL    = 105,
    CTRL_BUSY    = 104
};

class LocalSocketThread : public QObject
{
    Q_OBJECT
public:
    LocalSocketThread(QObject *parent = nullptr);

    QJsonObject packetProperty();
    QJsonObject packetversion();
    QJsonObject packetScheme();
    QJsonObject packetKeyComponents();
    QJsonObject packetStatistics();


    void uploadInfo(QJsonObject json);

    void getBackupList(QString categoryId, int page, int pageSize);
    void downloadFile(QString categoryId, QString url, QString fileName);

signals:
    void sigHandleMsg(QJsonObject);
    void sigSyncTime(void);
    void sigsetWindow(int nWinID);
    void sigrefreshVPNStatus(bool bConnect);
    void sigScanCodeRet(bool bRes);

protected:

public slots:
    void onInitLocalSocket();

    void onRecieveDatas(QByteArray ba);
    void onSendData();

    void onSendMachineInfo();
    void onSendLanguage(int isEnglish);

    void sendJson(QJsonObject js);

    void uploadLogs(int mode);
    void uploadParams(QString dir, int mode);
    void uploadSorterAdjParams(QString dir, int mode);


    void onHandleMsg(QJsonObject obj);

    void onuploadstaticRes(void);

private:
    void initMapPacketFunctions();
    void initMapControlFunctions();

    QDate      getBuildQdate();

    //! 版本
    QJsonObject packProVersionFPGA();
    QJsonArray  packVersionInterface();
    QJsonArray  packVersionCamera();
    QJsonArray  packVersionAssistCamera();
    QJsonArray  packVersionAI();
    QJsonArray  packVersionVibrator();
    QJsonObject packProVersionArm();
    QJsonArray  packVersionLamp();
    QJsonArray  packVersionHalogen();
    QJsonArray  packVersionEjector();

    //! 关键部件
    QJsonObject packVibrator();
    QJsonObject packElectric();
    QJsonArray  packLampstatus();
    QJsonObject packHalogen();
    QJsonObject packCameraTemperature();
    QJsonArray  packChannelMajor();
    QJsonArray  packChannelAssist();
    QJsonObject packStatisticEjectorcount();

    //! 遥测数据
    QJsonObject packetTelemetry();

    bool isInterComError();             //! 接口板通信异常
    bool isCameraComError(int role);    //! 相机板通信异常
    bool isLampComError();              //! 恒流源板通信异常
    bool isHalogenComError();           //! 卤素灯检测板板通信异常
    bool isEjectorComError();           //! 喷阀驱动板通信异常
    bool isAIComError();                //! AI板通信异常
    bool isVibratorComError();          //! 振动驱动板通信异常

    void handleMessage(QJsonObject json);
    void handleGeneral(QJsonObject json);
    void handlePressEvent();

    void handleOpenVPN(QJsonObject json);     //! 打开VPN
    void handleCloseVPN(QJsonObject json);    //! 关闭VPN
    void handleAlarmEvent(QJsonObject json);
    void sendVirtualIP(QString virtualIP, QString responseID);

    void requestCtrlPackage(int res, QString str);

    void handleUploadFile(QJsonObject json);
    void handleQueryFile(QJsonObject json);

    void handleScanCodeEvent(QJsonObject jsonObj);
    void handleNotifyEvent(QJsonObject jsonObj);
//    QString getCheckCode(int moduleType);


    QJsonObject packReqBackup(QString categoryId, int page, int pageSize);
    QString     getAppCode();
    QString     getProductId();

private:
    LocalSocket *m_localSocket;
    QThread     *m_localSocketthread;

    QMap<int, QString> m_mapFpga;
    QMap<QString, int> m_mapWarningFpga;

    QMap<QString, function<QJsonObject(LocalSocketThread *)>> m_mapPacketFunction;
    QMap<QString, function<int(LocalSocketThread *, QJsonValue, QString &)>> m_mapCtrlHandleFunction;

    float m_vYield;
    QString m_src, m_from;
    bool m_bSendAIStats;
    bool m_bNtp = true;
};

extern LocalSocketThread* g_Lotsocket;
#endif // LOCALSOCKETTHREAD_H
