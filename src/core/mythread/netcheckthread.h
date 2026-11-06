#ifndef NETCHECKTHREAD_H
#define NETCHECKTHREAD_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "params/define.h"
#include "core/globalflow/globalflow.h"

class MySocketio;

class netCheckThread : public QThread
{
    Q_OBJECT
public:
    netCheckThread();
    ~netCheckThread();

void stop(){m_isRunning = false;}

protected:
    void run();

signals:
    void sigUpdateSysSta(int);
    
public slots:

private:
    int m_nFailedTimes;          /*! 记录网络通信失败次数 */
    int nLastNetwortStat;        /*! 记录前一次网络连接状态 */
    bool m_isRunning;
    int  m_nAlarm;

    QTimer *m_timer;
    qulonglong nCnt;             /*! 计数器 */

private:
    void checkNetworkStatus(void);           /*! 检查网络状态 */
    void processAlarmStatus();
};


extern netCheckThread *g_netThread;
#endif // NETCHECKTHREAD_H
