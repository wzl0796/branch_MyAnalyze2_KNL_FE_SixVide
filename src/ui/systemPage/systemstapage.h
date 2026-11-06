#ifndef SYSTEMSTAPAGE_H
#define SYSTEMSTAPAGE_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QGroupBox>
#include "params/define.h"
#include "params/globalparams.h"
#include "core/globalflow/mylanguage.h"

/*!
 * \enum 枚举状态栏的所有信息
 */
enum {
    INFO_MACHINE_ID = 0,    //! 机器编号
    INFO_RUN_TIME_TOTAL,    //! 累计运行时间
    INFO_FEED_TIME_TOTAL,   //! 累计加工时间
    INFO_FEED_STATUS,       //! 供料状态
    INFO_NETWORK_STATUS,    //! 网络状态
    INFO_AIRALRM_STATUS,   //! 气压报警状态
    INFO_HALOGEN_STATUS,    //! 卤素灯状态
    INFO_CLEANJAR_STATUS,   //! 清灰缸报警
    INFO_CONTROL_STATUS,    //! 控制板报警
    INFO_AUTOSAMPLE_STATUS, //! 自动取样状态
    INFO_CAMTEMPURE_STATUS, //! 高光谱相机温度
    INFO_MAX
};

class SystemStaPage : public QDialog
{
    Q_OBJECT
public:
    explicit SystemStaPage(QWidget *parent = nullptr);
    ~SystemStaPage();

    void refreshPage();
    void refreshDisplay(int nSta=0);
    void stopTimer(){m_timer->stop();}

private slots:
    void onTimerOutSlt();

private:
    void createPage();
    void connectSigAndSlot();
    void refreshAlarmList(int nsta);
    void addAlarmItem(const QString& alarmText, int alarmType);
    void updateSystemSta();
private:
    QGroupBox  *m_pageGb;
    QLabel     *m_lblIcon[INFO_MAX];
    QLabel     *m_lblInfo[INFO_MAX];

    QVector<QIcon> m_vIcon;
    QTimer *m_timer;

    QMap<int, QString> m_alarmTypeMap; //! 报警类型映射
};

#endif // SYSTEMSTAPAGE_H
