#ifndef MYPROCESS_H
#define MYPROCESS_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"
#include "core/io/myserial/myserialcom.h"


class MyProcess : public QThread
{
    Q_OBJECT
public:
    explicit MyProcess();

    virtual ~MyProcess();

    void setAnalyzeIndex(int analyze);

signals:
    void sigUploadWeb();

    void sigautosamplesta(int,bool);

public slots:
    void startTimerSlt(void);

    void stopTimerSlt(void);

protected:
    void run();

private:
    int    m_accLastCount = 0;
    bool   bFinishLoop = false;
    bool   bRunning;
    bool   isSendSVCmd;
    int    m_analyzeIndex;                     /*! 分析仪序号 */
    int    nWipeCnt;                           /*! 清灰次数标志位 */
    long   nLastRecycleNum;                    /*! 定时回料数据*/
    long   nLastOutPut;                        /*! 记录前一次产量，用于计算每秒物料个数 */
    long   nCounter;                           /*! 统计定时器 */
    int    nAdjtimes = 0;                      /*! 调节次数 */
    bool   bclean;                             /*! 清灰使能 */
    analyzeShare  *m_pAnalyzeShare;
    analyzeGlobal *m_pAnalyzeGlobal;
    QTime          m_Maxthdtime;
    bool           m_bFirst[6];
    bool           m_bTimeout = false;

    int            m_nNormalChanel;
};

extern QList<MyProcess*>  g_Process;

#endif // MYPROCESS_H
