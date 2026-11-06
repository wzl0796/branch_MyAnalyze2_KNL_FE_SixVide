#ifndef PAGEMACHINE_H
#define PAGEMACHINE_H

#include <QWidget>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include "common/myswitchbutton.h"
#include "common/mylineedit.h"
#include "core/globalflow/globalflow.h"
#include "common/myinputpanel.h"
#include "params/globalparams.h"
#include "common/mymessagebox.h"
#include "common/mycheckbox.h"
#include "common/myinputmethod.h"
#include "common/setexporttypewidget.h"
#include "common/selectfiledialog.h"
#include "core/globalflow/mylanguage.h"

class pageMachine:public QWidget
{
    Q_OBJECT
public:
    explicit pageMachine(QWidget* parent = nullptr);
    ~pageMachine();

    void createpage(void);
    void laypage(void);
    void connectAndSlt(void);
    void refreshPagemachine(void);
    bool backupParams(int tooltype, int paramsType);

signals:
    void sigChgMotorspeed(int nspeed);
    void sigSwMotorOnOFF(bool status);
    void sigInitMotor();
    void sigChgStatusType();
    void sigInitWeb(bool);
    void sigInitRemoteSorter();
    void sigautosampleEnsta(int,bool);
    void sigMachineTypechg(bool bBottle);
    void siguploadparmas(QString dir, int mode);
    void sigApplyMachine();
    void sigSimulateEn(bool bEn);
public slots:
    void onOnekeyparmBtnSlt();
    void onApplyBtnPressSlt();
    void onMachineCombIdxSlt(int nIdx);
    void onModetypeChgSlt(int nIdx);

    void onEnAssistCamChgSlt(bool bEn);
    void onCanNameBtnChgSlt(int nIdx);
    void onCamnumlepressedSlt(void);
    void onrationchkpressedSlt(bool bsta);
    void onEncosorteradjustChg(bool bEn);
    void onEnPurityStatics(bool bSta);
    void onTrigerTypeChgSlt(int nIdx);
    void onautoSampleChgSlt(bool bsta);
    void onEnwebSlt(bool bSta);
    void onParamsbckBtnPressedSlt(void);
    void onParamsRestoreBtnPressedSlt(void);
    void onLightSyncChgSlt(bool bSta);
    void onStaicTypeChgSlt(int bSta);
    void onAdjSorterTipsChgSlt(bool bSta);
    void onSimulateSwChanged(bool bEn);
private:
    void refreshStatisGb(bool bReset = false);
    void refreshcamNameGb();
    int _backParamstolocal();
    int _backParamstoUdisk();
    int _restoreParamsBylocal(int nType);
    int _restoreParamsByUdisk(int nType);
    void refreshlayout();
    void setMachinedefaultParm();

private:
    QWidget *m_wid;
    QStackedWidget *m_stackwid;


    //! 机型参数设置
    QGroupBox *m_machineGb;
    QGridLayout *m_glayMachine;
    QLabel    *m_machinelb;
    QComboBox *m_machicomb;
    QLabel     *m_bEnMutilablb;
    mySwitchButton *m_bEnMultilabSw;
    QLabel    *m_bEnAssistlb;
    mySwitchButton *m_bEnAssistSw;
    QLabel    *m_camNumlb;
    MyLineEdit *m_camNumle;
    QLabel      *m_bEnadjbysorterlb;
    mySwitchButton *m_bEnadjbysortSw;
    QLabel         *m_bEnPuritylb;
    mySwitchButton *m_bEnPuritySw;
    QLabel         *m_trigertypelb;
    QComboBox      *m_trigertypecomb;
    QLabel         *m_autosamplelb;
    mySwitchButton *m_autosampleSw;
    QLabel         *m_weblb;
    mySwitchButton *m_webEnSw;
    QLabel         *m_lightsynclb;
    mySwitchButton *m_lightsyncsw;
    QLabel         *m_staticTypelb;
    QComboBox      *m_staticTypecob;
    QLabel         *m_bEnAdjTipslb;
    mySwitchButton *m_bEnAdjTipsSw;
    QLabel         *m_bSimulatelb;
    mySwitchButton *m_simlateSw;

    //! 统计组划分
    QGroupBox *m_camsetGb;
    QLabel    *m_camIdxlb[MAX_CAM_NUM];
    MyCheckBox *m_alonechk[MAX_CAM_NUM];

    //! 相机名称
    QGroupBox   *m_camNamesetGb;
    QPushButton *m_camNamebtn[MAX_CAM_NUM];


    QPushButton *m_oneKeyparmsBtn;
    QPushButton *m_applyBtn;
    QPushButton *m_paramsBackBtn;
    QPushButton *m_paramsRestoreBtn;



    bool m_bSetcamNum[2];

};

#endif // PAGEMACHINE_H
