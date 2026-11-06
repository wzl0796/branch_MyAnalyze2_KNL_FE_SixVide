#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDateTime>
#include <QStackedWidget>
#include <QToolButton>
#include "homePage/homewidget.h"
#include "systemPage/versionpage.h"
#include "statusBarPage/setfeederpage.h"
#include "statusBarPage/calibratewidget.h"
#include "statusBarPage/setwipepage.h"
#include "systemPage/systemnetpage.h"
#include "systemPage/systempage.h"
#include "factorySetPage/factorypage.h"
#include "ui/IdentifityPage/identifity.h"
#include "statisticswidget.h"
#include "common/myswitchbutton.h"
#include "common/labelratiodialog.h"
#include "mymanager.h"
#include "core/globalflow/sqliteinter.h"
#include "statusBarPage/setblowtimepage.h"
#include "systemPage/systemstapage.h"
#include "common/passworddialog.h"
#include "sortercompage.h"
#include "pageboot.h"

enum {
    PAGE_HOME =0,       //! 主界面
    PAGE_HISTORY,       //! 化验历史数据
    PAGE_SORTERCONTROL, //! 色选联调页面
    PAGE_IDENTYFITY,    //! 识别
    PAGE_SET,           //! 设置界面
    PAGE_SYSTEM,       //! 系统
    PAGE_ALL
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sigIniMqttFlow();

    void initModbus();

    void sigChangeFeeder();

    void sigSendDataToPlcTcp();                          //! 砻谷段 目前由分析仪发送实时数据到PLC控制中心

    void sigGetPolishDataFromPlcTcp(int, int, int, int);

    void sigGetTargetDataFromPlcTcp(int);
    
    void sigChangeScheme();

    void sigUploadRunningState();

    void sigUploadPXInfo();

    void sigClosedMqtt();

    void sigIM01Tempture(int);

    void sigInitWebinfo();

    void sigInitRemoteSorter();

    void sigInitEncryTimer();

    void sigPermissionChg();

public slots:
    void timeOutSlt(void);

    void onFeederBtnClickedSlt(void);

    void onMenuBtnPressedSlt(int index);

    //! 下侧菜单按钮槽函数
    void onUserBtnPressedSlt(void);

    void onSaveBtnPressedSlt(void);

    void onHaltBtnPressedSlt(void);

    void onFeederSetBtnSlt(void);

    void onCaliParaBtnClicked(void);

    void onblowTimeSetBtnSlt(void);

    void onWipeSetBtnSlt(void);

    void onBeltBtnSlt(void);

    void onCalWaveBtnPressedSlt();

    void onFactoryNameClicked();

    void onSchmBoxChangedSlt(int schemeIndex);       //! 方案切换

    void onSchemEditBtnClicked(void);

    void onrefreshBeltSta();

    void onInitWebSlt(bool bsta);

    void onrefreshautosamplesta(int nIdx, bool breset);

    void onRefreshSystemSta(int Sta);

    void onHandleScanCodeRes(bool bSta);

    void onRefreshSorterpage();

    void onRefreshAdjTips(QStringList lablst);

    void onLabelRatioBtnPressedSlt();

private:
    void initParams(void);

    void createMainPage(void);

    void createHeaderGBox(void);

    void createStatusGBox(void);

    void createListWidget(void);

    void createGlobalWidget(void);

    void connectSigAndSlot(void);

    void initCommunication(void);

    void checkEncryptOK();

    void refreshSchemBox(void);

    void calIMserialsensors(void);

    int m_current;

private:
    //! 顶部菜单栏控件
    QGroupBox    *headerGb;
    QToolButton  *m_UserBtn, *m_sysHaltBtn, *m_saveBtn, *m_feederBtn, *m_dataBtn;
    QLabel       *m_autosamplelb;
    PasswordDialog *m_passwordDlg;

    //! 化验方案
    QLabel         *m_schemLbl;
    QComboBox      *m_schemBox;
    QPushButton    *m_editBtn;

    //! 底部菜单栏
    QGroupBox    *statusGb;
    QToolButton  *m_wipeBtn, *m_setFeederBtn, *m_calibrateBtn, *m_beltBtn;
    QToolButton  *m_blowtimeBtn;
    QPushButton  *m_factoryName;
    QToolButton  *m_calWaveBtn;
    QToolButton  *m_labelRatioBtn;

    //! 左侧菜单栏
    QGroupBox    *mainListGb, *typeGb;
    QLabel       *m_timeLbl, *m_dateLbl;
    QToolButton  *menuBtn[PAGE_ALL];

    //! 主界面stackwidget
    QStackedWidget   *m_stack;
    homeWidget       *mainPage;
    StatisticsWidget *statisticsPage;
    Identifity       *identifityPage;
    factoryPage      *facSetPage;
    SystemPage       *systemPage;
    sortercompage    *sorterPage;

    //! 信号接收
    QSignalMapper   *menulMap;

    //! 定时器
    QTimer          *timer;

    bool           m_bFeeder;         //! 当前开关下料状态
    bool           m_bProcessStopping; //! 标志是否正在异步停止线程进程

    bool           m_backgdrodSta = false;
    int            m_nDetectTimes = 0;
    int            m_nSystemAlarmSta = 0;  //! 系统报警状态 0-正常 1-报警
};

#endif // MAINWINDOW_H
