#ifndef DELAYCODE_H
#define DELAYCODE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QCryptographicHash>
#include "3rdparty/libdccrypt/include/dccrypt.h"
#include "mysettime.h"
#include "myinputmethod.h"
#include "core/globalflow/mylanguage.h"
#include "core/globalflow/globalflow.h"


class DelayCode : public QObject
{
    Q_OBJECT
public:
    explicit DelayCode(QObject *parent = nullptr);

public:
    /*! \enum 枚举加解密结果 */
    enum {
        DC_SUCCESS = 0,             //! 成功
        DC_LENGTH_ERROR = -1,       //! 期限码长度不正确
        DC_ILLEGAL_CHAR = -2,       //! 期限码含有非法字符
        DC_ILLEGAL = -9,            //! 非法的期限码
        DC_ID_MISMATCH = -10,       //! 机器编号不匹配
        DC_SAVE_ERROR = -11,        //! 期限码保存失败
        DC_TIME_SAVE_ERROR = -12,   //! 系统时间保存失败
        DC_EXPIRE = -13,            //! 期限码已到期
        DC_INVAILD_CALIBRATION = -14   //! 无效的时间校准码
    };

    /** \enum 枚举存储文件信息 */
    enum {
        INFO_ENABLE = 0,        //! 使能
        INFO_CODE,              //! 期限码
        INFO_ERROR_TIMES,       //! 异常开机次数
        INFO_TYPE               //! 信息类型
    };

    /** \enum 枚举检测结果 */
    enum {
        DETECT_NORMAL = 0,      //! 加密未到期或完全解密
        DETECT_TIME_ABNORMAL,   //! 时间异常
        DETECT_EXPIRE           //! 加密到期
    };


    static const int DC_LENGTH = 24;
    static const int MAX_ERROR_TIMES = 5;
    static const int DEFAULT_ERROR_TIMES = 0xFF;
    static const int HOUR = (3600*1000);
    const QDateTime DEFAULT_TIME = QDateTime(QDate(2000,1,1));

public:
    QString getText(int nResult);   //! 根据加解密结果获取文本

    int checkDelayCode(QString sDelayCode, QString &sId, QDate &date, bool bCheckExpire=true);  //! 检查期限码
    int analysisDelayCode(QString sDelayCode, QString &sId, QDate &date);   //! 解析期限码
    int encryptAndDecrypt(QString sDelayCode, bool bCheckExpire=true);      //! 加解密
    int saveDateTime(void);         //! 保存系统时间
    int saveDelayCode(void);        //! 保存延迟付款信息
    int removeDelayCode(void);      //! 删除期限码

    int detectDelayCode(int &nRemindDays, int &nRemindTimes);   //! 开机检测延迟付款
    bool getDelayCode(void);        //! 获取延迟付款信息
    bool getDateTime(void);         //! 获取系统时间
    QString getMachineId(void) {return m_sMachineId;}   //! 获取期限码对应机编
    QDate   getLocalTime(void) {return m_dateTime.date();}
    QDate   getExpireDate(void) {return m_dateExpire;}    //! 获取加密到期日期

    int calibrateTime(QString sDelayCode);      //! 校准时间

    bool getEnable(void) {return m_bEn;}        //! 获取加密使能
    void updatetime();
protected:
    void timerEvent(QTimerEvent *ev);   //! 定时器事件

private:
    QStringList m_fileListInfo;     //! 存储加密信息的文件列表
    QStringList m_fileListTime;     //! 存储实时时间的文件列表

    bool m_bEn = false;             //! 加密使能
    QString m_sDelayCode;           //! 期限码
    QString m_sMachineId;           //! 机器编号
    QDate m_dateExpire;             //! 加密到期时间
    QDateTime m_dateTime = DEFAULT_TIME;        //! 保存的系统时间
    int m_nErrorTimes = DEFAULT_ERROR_TIMES;    //! 异常开机次数
    bool m_bStartUp = true;         //! 开机标志

signals:

public slots:
    void onStartTimer();

};

extern DelayCode g_delagCode;
#endif // DELAYCODE_H
