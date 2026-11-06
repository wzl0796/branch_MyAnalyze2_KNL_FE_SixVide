#ifndef CAMERASIGNALPAGE_H
#define CAMERASIGNALPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include <QTabBar>
#include <QStackedWidget>
#include "QListWidget"
#include "params/define.h"
#include "common/mycurve.h"
#include "common/mymessagebox.h"
#include "common/myinputpanel.h"
#include "common/mydevicelist.h"
#include "common/myswitchbutton.h"
#include "common/mylineedit.h"
#include "common/mycheckbox.h"
#include "core/globalflow/mylanguage.h"
#include "ui/cameraSetPage/getimagepage.h"
#include <QStackedLayout>


//! 波段选择
class WaveSelectWgt:public QDialog
{
    Q_OBJECT
public:
    explicit WaveSelectWgt(QWidget *parent = NULL);
    void setSelectedRows(QVector<int> &selects);
    QVector<int> getSelectedRows();
    void setIMSensorType(int nType);

private slots:
    void onToRightBtnClickedSlt();
    void onToLeftBtnClickedSlt();
    void onOkBtnClickedSlt();
    void onCancleBtnClickedSlt();
private:
    void initUI();
    void initLayout();
    void initSigSlots();
private:
    QVBoxLayout     *m_mainLay;
    QLabel          *m_tipLabl;
    QListWidget    *m_leftListWgt;
    QListWidget    *m_rightListWgt;
    QPushButton     *m_toRightBtn;
    QPushButton     *m_toLeftBtn;
    QPushButton     *m_okBtn;
    QPushButton     *m_cancleBtn;

    int             m_waveCnt;
    int             n_nSensorType;      //! 320 还是640高光谱
};

class cameraSignalPage : public QWidget
{
    Q_OBJECT
public:
    enum {
        WAVE_ORI =0,
        WAVE_ADJUST,
        WAVE_SIGNAL,
        WAVE_TYPE
    };

    enum {
        FRONT_VIEW =0,
        FRONT2_VIEW,
        REAR_VIEW,
        REAR2_VIEW
    };
    enum {
        ADJUST_GAIN=0,
        ADJUST_SLOW,
        ADJUST_FAST,
        ADJUST_TYPE
    };

    enum {
        PIXEL_BEGIN=0,
        PIXEL_END,
        PIXEL_TYPE
    };
public:
    explicit cameraSignalPage(QWidget *parent = 0);

    void refreshPageDisplay(void);
signals:

public slots:
    void onNodeBoxChangedSlt(int index);

    void onWaveTypeBtnPressedSlt(int index);

    void onCamViewChanged(int index);

    void onFreValueBtnPressedSlt(int index);

    void onSensorTypeChangedSlt(int index);

    void onSensorWaveChangedSlt(int index);

    void onExposeValueBtnPresseSlt(int index);

    void onNoneExposeValueBtnPresseSlt(int index);

    void onExportWaveBtnPresseSlt(int index);

    void onTemptureReqSlt(int nIdx);

    void onAdjustTargetBtnPressedSlt(int index);

    void onAutoAdjustBtnPressedSlt(void);

    void onDarkAdjustBtnPressedSlt();

    void onTabWidgetChangedSlt(int index);

    void onFastGainBtnPressedSlt(int index);

    void onSlowGainBtnPressedSlt(int index);

    void onLampLblBtnPressedSlt(int index);

    void onPluseLblBtnPressedSlt(int index);

    void onLampValueBtnPressedSlt(int index);

    void onPluseValueBtnPressedSlt(int index);

    void onPluseEnchgSlt(int nIdx);

    void onLampEnchgSlt(int nIdx);

    void onHologenSwitchBtnSlt(bool bCheck);

    void onBGParamsValuePressedSlt();

    void onPixelChannelBtnPressedSlt(int index);

    void onChannelTabChangedSlt(int index);

    void onOddChannelBtnPressedSlt();

    void onWaveBoxChangedSlt(int index);

    void onIM01ThreadPressedSlt();

    void onIM01LimitPressedSlt();

    void onChangeScheme();

    void onbackGroundIdxSlt(int nIdx);  //! 背景条件

    void onbackGroundValueSlt();    //! 背景阈值

    void onbackGroundRatioSlt();    //! 背景比例

    void onIM01SensorTempure(int tmp);    //!  传感器温度

    void onApplyMachineSlt();

    void onCaptureBtnPressedSlt();

    void onEjectorDiveBtnPressedSlt(int nIdx);
private:
    void initParams(void);

    void initCameraParmas(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void refreshWaveBtnState(void);

    void refreshViewBtnState(void);

    void refreshFreValue(void);

    void refreshAdjustValue(void);

    void refreshThreadValue(void);

    void refreshPixelValue(void);

    void refreshEjectorValue(void);

    void refreshInfraBiasValue();

    void refreshBackGroundParams();

    void getAdjustSignal(void);

    void refreshCamParam();

    void refreshLightParams(void);

    void createCurvePage();    //! 波形
    void createParamSetPage(); //! 参数设置

    void createCamSubPage();   //! 相机参数
    void createLightSubPage(); //! 灯光控制
    void createSensorSubPage();//! 传感器

    int nCalibrateID;                   //! 校正ID 1-1024 2-2048
    int n_wave_type;                    //! 波形类型
    int n_view;                         //! 当前相机序号
    int n_analyzeNum;                   //! 分析仪数量

    bool m_bManualChange;                 //! 手动改变分辨率

    QSignalMapper  *waveTypeMap;
    QSignalMapper  *adjustTargetMap;
    QSignalMapper  *fastAdjustMap;
    QSignalMapper  *slowAdjustMap;
    QSignalMapper  *frequencyMap;
    QSignalMapper  *sensorMap;
    QSignalMapper  *exposeMap;
    QSignalMapper  *NoneexposeMap;
    QSignalMapper  *exportWavesMapper;
    QSignalMapper  *tempreqMapper;
    QSignalMapper  *sensorWavemap;

    QSignalMapper  *lampLblAdjustMap;
    QSignalMapper  *pluseLblAdjustMap;

    QSignalMapper  *lampValueAdjustMap;
    QSignalMapper  *lampEnmap;
    QSignalMapper  *pluseValueAdjustMap;
    QSignalMapper  *pluseEnmap;
    QSignalMapper  *pixelValueAdjustMap;
    QSignalMapper  *ejectorDiveMap;
private:
    QWidget         *m_pagecamSet;
    getImagePage    *m_pageGetImage;
    QStackedLayout  *m_stacklaytopPage;

    //! 左侧--->波形
    QGroupBox    *curveGBox;
    QSignalMapper *m_camBtnMap;
    QPushButton  *m_camBtn[MAX_CAM_NUM];
    MyAxis       *myAxis;
    QPushButton  *waveBtn[WAVE_TYPE];


    //! 右侧--->参数
    QGroupBox    *paramsGBox;
    QTabWidget     *m_tabWidget;
    QWidget       *m_camSubPage;     //! 相机参数
    QWidget       *m_lightSubPage;   //! 灯光控制
    QWidget       *m_sensorSubPage;  //! 传感器

    //! 相机
    QLabel       *adjustTargetLbl;
    QLabel       *adjustTargetTypeLbl[COLOR_TYPE_NUM];
    QPushButton  *adjustTargetTypetBtn[COLOR_TYPE_NUM];
    QPushButton  *adjustAutoBtn;
    QPushButton  *darkAdjustBtn;

    QLabel   *adjustTypeLbl[ADJUST_TYPE];

    QLabel   *colorTypeLbl[COLOR_TYPE_NUM];
    QLabel   *colorTypeBtn[COLOR_TYPE_NUM];

    QPushButton *fastAdjustNum[COLOR_TYPE_NUM];
    QPushButton *slowAdjustNum[COLOR_TYPE_NUM];


    QLabel        *pixelTitleLbl;
    QLabel        *beginChannelLbl[PIXEL_TYPE];
    QPushButton   *beginChanelBtn[PIXEL_TYPE];
    QPushButton   *autoDivBtn;

    QLabel        *ejectorSetTitleLbl[3];
    QLabel        *diveEjectorLb[2];
    QPushButton   *diveEjectorBtn[2];
    QPushButton   *diveEjectorPiexlBtn[2];


    QWidget      *infraBiasWidget;
    QLabel        *infraBiasLbl;
    QTabBar       *channelTabBar;
    QLabel        *oddChannelLbl;
    QPushButton   *oddChannelBtn;

    QLabel        *waveLbl;  //! 红外偏置选择什么波段
    QComboBox     *waveBox;  //! 红外偏置选择什么波段


    QPushButton  *m_captureBtn;

    //! 灯光
    QLabel      *lampTitleLbl;
    QPushButton *lampLbl[LAMP_TYPE];
    QPushButton *lampValuetBtn[LAMP_TYPE];
    MyCheckBox  *m_lampEn[LAMP_TYPE];


    QPushButton *pluseLbl[PLUSE_TYPE];
    QPushButton *pluseValuetBtn[PLUSE_TYPE];
    MyCheckBox    *m_pluseEn[PLUSE_TYPE];

    QLabel      *halogenLampLbl;  //卤素灯
    mySwitchButton *hologenSwitchBtn;



    //! 传感器
    QLabel        *BkGroundParamsLbl;
    QLabel        *rangeLbl;
    QPushButton   *rangeValueBtn;

    QGroupBox    *sensorGBox[MAX_CAM_NUM];
    QLabel       *freValueLbl[MAX_CAM_NUM];
    QPushButton  *freValueBtn[MAX_CAM_NUM];

    QLabel       *sensorTypeLbl[MAX_CAM_NUM];
    QComboBox    *sensorTypeCombox[MAX_CAM_NUM];
    QComboBox    *sensorWaveComb[MAX_CAM_NUM];

    QLabel       *exposeValueLbl[MAX_CAM_NUM];
    QPushButton  *exposeValueBtn[MAX_CAM_NUM];
    QLabel       *NoneexposeValueLbl[MAX_CAM_NUM];
    QPushButton  *NoneexposeValueBtn[MAX_CAM_NUM];
    QLabel       *tempurelb[MAX_CAM_NUM];
    QPushButton  *tempurebtn[MAX_CAM_NUM];

    QLabel       *wavesLabels[MAX_CAM_NUM]; //! 波段
    QPushButton  *exportWaveBtn[MAX_CAM_NUM];

    QGroupBox    *backGBox;

    QLabel       *threadLbl;  //阈值
    QPushButton  *threadBtn;

    QLabel       *upLimitLbl;  //上限
    QPushButton  *upLimitBtn;

    //! 背景条件(SOC）
    QGroupBox *m_backgb;
    QLabel *m_backlb;
    QComboBox *m_bckcomb;
    QLabel *m_bckthdlb;
    MyLineEdit *m_bckle;
    QLabel *m_bckratiolb;
    MyLineEdit *m_bckratiole;


    QString ori_wave;
    QString adjust_wave;
    QString adjust_signal;
    QString adjust_target;

    QString auto_adjust;
    QString dark_adjust;

    QString fre_adjust;
    QString fre_value;
    QString gain_adjust;
    QString fast_adjust;
    QString slow_adjust;
    QString lamp_adjust;
    QString mat_lamp;
    QString back_lamp;
    QString inf_lamp;
    QString back_params;
    QString range_parmas;
    QString thread_params;
    QString pixel_channel;
    QString begin_pixel;
    QString end_pixel;
    QString auto_pixel;

    QString adjust_warn;
    QString resolution;
    QString sensor_type;
    QString expose_value;
    QString Noneexpose_value;
    QString m_strExportWaves;
    QString infra_bias;

};

#endif // CAMERASIGNALPAGE_H
