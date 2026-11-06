#include "delaycode.h"

DelayCode g_delagCode;

DelayCode::DelayCode(QObject *parent) : QObject(parent)
{
    //! 加密文件共存储3份，每份包含.dc和.time两个文件
    //! .dc文件存储写入频率低信息，包括加密使能、期限码和异常开机次数
    //! .time文件存储最新系统时间，每隔1小时判断时间有效时写入
    m_fileListInfo = QStringList() << QString("%1%2/encry.dc").arg(APP_DIR).arg("cnf")
                                   << "/etc/encry.dc1" << "/opt/encry.dc2";
    m_fileListTime = QStringList() << QString("%1%2/encry.time").arg(APP_DIR).arg("cnf")
                                   << "/etc/.time1" << "/opt/.time2";
}

/*!
 * \brief DelayCode::getText
 * \param nResult
 * \return
 */
QString DelayCode::getText(int nResult)
{
    QString str = myLan.sucess;
    switch(nResult){
    case DC_SUCCESS:
        str = myLan.sucess;
        break;
    case DC_LENGTH_ERROR:
        str = myLan.dc_length_error;
        break;
    case DC_ILLEGAL_CHAR:
        str = myLan.dc_illegal_char;
        break;
    case DC_ILLEGAL:
        str = myLan.dc_illegal;
        break;
    case DC_ID_MISMATCH:
        str = myLan.dc_id_mismatch;
        break;
    case DC_SAVE_ERROR:
        str = QString("%1%2").arg(myLan.save).arg(myLan.failed);
        break;
    case DC_EXPIRE:
        str = myLan.msg_dc_expire;
        break;
    case DC_INVAILD_CALIBRATION:
        str = myLan.invalid_code;
        break;
    default:
        break;
    }
    return str;
}

/*!
 * \brief 检查期限码
 * \param sDelayCode
 * \param sId
 * \param date
 * \param bCheckExpire
 * \return
 */
int DelayCode::checkDelayCode(QString sDelayCode, QString &sId, QDate &date, bool bCheckExpire)
{
    int nRet = analysisDelayCode(sDelayCode, sId, date);
    if (nRet == DC_SUCCESS) {
        //! 1.检查机编是否匹配
#ifdef Q_OS_UNIX
        if (m_bEn) {    //! 加密状态需与存储期限码机编匹配
            QString sMachineIDLocal;
            QDate dateLocal;
            nRet = analysisDelayCode(m_sDelayCode, sMachineIDLocal, dateLocal);

            if (sId != sMachineIDLocal && sMachineIDLocal != DEFAULT_MACHINEID) {
                return DC_ID_MISMATCH;
            }
        } else {        //! 解密状态需与界面设置机编匹配
            if (struCnfg.master_device_code != sId &&
                struCnfg.master_device_code != DEFAULT_MACHINEID) {
                return DC_ID_MISMATCH;
            }
        }
#endif
        //! 2.检查期限码是否到期
        if (bCheckExpire) {
            QDate dateNow = QDate::currentDate();
            if (dateNow < m_dateTime.date()) {
                dateNow = m_dateTime.date();
            }
            if (date <= dateNow) {
                return DC_EXPIRE;
            }
        }
    }

    return nRet;
}

/*!
 * \brief 根据期限码解析出机编和期限时间
 * \param delayCode 输入：期限码
 * \param machineID 输出：机编
 * \param limitTime 输出：期限时间
 * \return 解析结果
 */
int DelayCode::analysisDelayCode(QString sDelayCode, QString &sId, QDate &date)
{
    char limit[DCCRYPT_MAX_STR], sn[DCCRYPT_MAX_STR];
    int nRet = dc_decrypt(sDelayCode.toLatin1().data(), limit, sn);

    if (nRet == DC_SUCCESS) {
        sId = QString(sn);
        int nTime = atoi(limit);
        date.setDate(nTime/10000, (nTime%10000)/100, (nTime%10000)%100);
    }
    return nRet;
}

/*!
 * \brief 加解密
 * \param sDelayCode：期限码
 * \param bCheckExpire：检查期限码到期
 * \return 加解密结果
 */
int DelayCode::encryptAndDecrypt(QString sDelayCode, bool bCheckExpire)
{
    //! 检查期限码
    QString sId;
    QDate date;
    int nRet = checkDelayCode(sDelayCode, sId, date, bCheckExpire);

    if (nRet != DC_SUCCESS) {
        return nRet;
    }

    //! 判断完全解密
    m_bEn = (date >= QDate(2099, 1, 1)) ? false : true;
    if (!m_bEn) {
        return removeDelayCode();
    }

    //! 保存系统时间
    QDateTime timeNow = QDateTime::currentDateTime();
    if (timeNow > m_dateTime) {
        m_dateTime = timeNow;
    }
    nRet = saveDateTime();
    if (nRet != DC_SUCCESS) {
        return nRet;
    }

    //! 保存期限码
    m_sDelayCode = sDelayCode;
    m_sMachineId = sId;
    m_dateExpire = date;
    return saveDelayCode();
}

/*!
 * \brief 保存系统时间
 * \return
 */
int DelayCode::saveDateTime()
{
    int nRet = DC_TIME_SAVE_ERROR;
    foreach (QString fileName, m_fileListTime) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_dateTime.toString("yyyyMMddhhmmss").toLatin1().toBase64());
            file.close();
            nRet = DC_SUCCESS;
        }
    }
    gFlow.SystemCmd("sync");
    return nRet;
}

/*!
 * \brief 保存加解密信息
 */
int DelayCode::saveDelayCode()
{
    QByteArray ba;
    ba.append(QString::number(m_bEn?1:0) + '/');
    ba.append(m_sDelayCode + '/');
    ba.append(QString::number(m_nErrorTimes));

    int nRet = DC_SAVE_ERROR;
    foreach (QString fileName, m_fileListInfo) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(ba.toBase64());
            file.close();
            nRet = DC_SUCCESS;
        }
    }
    gFlow.SystemCmd("sync");
    return nRet;
}

int DelayCode::removeDelayCode()
{
    QStringList strList = m_fileListInfo + m_fileListTime;
    foreach (QString fileName, strList) {
        QFile file(fileName);
        file.remove();
    }
    m_dateTime = DEFAULT_TIME;
    m_nErrorTimes = DEFAULT_ERROR_TIMES;
    return DC_SUCCESS;
}

/*!
 * \brief 开机检测延迟付款
 * \param nRemindDays
 * \param nRemindTimes
 * \return
 */
int DelayCode::detectDelayCode(int &nRemindDays, int &nRemindTimes)
{
    //! 读取延迟付款信息，解密则退出
    getDelayCode();
    getDateTime();
    if (!m_bEn) {
        return DETECT_NORMAL;
    }

    //! 检查机器编号
    analysisDelayCode(m_sDelayCode, m_sMachineId, m_dateExpire);
    if (m_sMachineId != struCnfg.master_device_code) {
        struCnfg.master_device_code = m_sMachineId;
        gFlow.saveGlobalParams(CNF_GLOBAL);
        qInfo().noquote() << "Sync machine ID to" << m_sMachineId;
    }

    //! 检查加密到期时间
    QDateTime dateSystem = QDateTime::currentDateTime();
    QDateTime dateTime = (dateSystem > m_dateTime) ? dateSystem : m_dateTime;
    QDateTime dateLimit = QDateTime(m_dateExpire);
    if (dateLimit <= dateTime) {
        return DETECT_EXPIRE;
    }

    //! 检查系统时间
    if (m_bStartUp && dateSystem < m_dateTime) {
        m_bStartUp = false;
        if (m_nErrorTimes == DEFAULT_ERROR_TIMES) {
            m_nErrorTimes = 1;
        } else {
            m_nErrorTimes++;
        }
        nRemindTimes = MAX_ERROR_TIMES - m_nErrorTimes;
        saveDelayCode();
        return DETECT_TIME_ABNORMAL;
    }

    nRemindDays = dateSystem.daysTo(dateLimit);
    return DETECT_NORMAL;
}

/*!
 * \brief 获取延迟付款信息
 * \return
 */
bool DelayCode::getDelayCode()
{
    foreach (QString fileName, m_fileListInfo) {
        QByteArray ba;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            ba = QByteArray::fromBase64(file.readAll());
            QList<QByteArray> list = ba.split('/');
            if (list.size() == INFO_TYPE) {
                m_bEn = list[INFO_ENABLE].toInt() == 0 ? false: true;
                m_sDelayCode = QString(list[INFO_CODE]);
                m_nErrorTimes = list[INFO_ERROR_TIMES].toInt();
                return true;
            }
        }
    }
    return false;
}

/*!
 * \brief 获取系统时间
 * \return
 */
bool DelayCode::getDateTime()
{
    foreach (QString fileName, m_fileListTime) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = QByteArray::fromBase64(file.readAll());
            m_dateTime = QDateTime::fromString(QString(ba), "yyyyMMddhhmmss");
            qDebug("%s %d",__FUNCTION__,__LINE__);
            qDebug()<<m_dateTime;
            return true;
        }
    }
    return false;
}

/*!
 * \brief DelayCode::calibrateTime
 * \param sDelayCode
 * \return
 */
int DelayCode::calibrateTime(QString sDelayCode)
{
    //! 检查期限码
    QString sId;
    QDate date;
    int nRet = checkDelayCode(sDelayCode, sId, date, false);
    if (nRet != DC_SUCCESS) {
        return nRet;
    }

    //! 判断完全解密
    m_bEn = (date >= QDate(2099, 1, 1)) ? false : true;
    if (!m_bEn) {
        return removeDelayCode();
    }

    //! 判断校准后时间是否到期
    if (date >= m_dateExpire) {
        return DC_EXPIRE;
    }

    //! 判断时间校准码是否有效
    if (date < m_dateTime.date()) {
        return DC_INVAILD_CALIBRATION;
    }

    m_dateTime = QDateTime(date.addDays(1));
    gFlow.setTime(m_dateTime);
    return saveDateTime();
}

void DelayCode::updatetime()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    m_dateTime = dateTime;
    saveDateTime();
}

/*!
 * \brief c
 * \param ev
 */
void DelayCode::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev)
    if (m_bEn) {
        //! 每小时更新1次保存的系统时间
        QDateTime dateTime = QDateTime::currentDateTime();
        if (dateTime > m_dateTime) {
            m_dateTime = dateTime;
            saveDateTime();
        }

        //! 每小时检查1次加密到期
        if (dateTime >= QDateTime(m_dateExpire.addDays(1))) {
            QString str = QString("%1").arg(myLan.msg_dc_expire);

        } else if (dateTime >= QDateTime(m_dateExpire)) {
            QString str = QString("%1").arg(myLan.msg_dc_expire);
        }
    }
}

/*!
 * \brief 槽: 开启定时器
 */
void DelayCode::onStartTimer()
{
    startTimer(HOUR);
}
