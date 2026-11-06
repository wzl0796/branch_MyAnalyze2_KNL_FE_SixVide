#ifndef SORTERCOMPAGE_H
#define SORTERCOMPAGE_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QSignalMapper>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QComboBox>
#include <QStackedLayout>
#include "common/mylineedit.h"
#include "common/myinputmethod.h"
#include "common/myinputpanel.h"
#include "params/globalparams.h"
#include "common/mycheckbox.h"
#include "common/mymessagebox.h"
#include "common/myswitchbutton.h"
#include "core/io/localsocket/mylocalsocket.h"
#include "core/globalflow/mylanguage.h"

#define max_titleNum 4

class sortercompage : public QWidget
{
    Q_OBJECT
public:
    explicit sortercompage(QWidget *parent = nullptr);

    void createpage(void);
    void createparamspage(void);
    void laypage(void);
    void layparamspage(void);
    void connectAndSlt(void);
    void refreshpage(void);

signals:

public slots:
    void onsorterclassBtnPressed(int nidx);
    void onmaxsortNumchg(void);
    void onEnnamestatuschg(void);
    void onlabchkstatuschg(int nIdx);
    void onlabLowthdChg(int nIdx);
    void onlabHigthdChg(int nIdx);
    void onlabTarthdChg(int nIdx);
    void onresetPIDparams(void);
    void onshowadjPage(void);
    void onupdateLocalsocketstatus(int ncode);
    void onsourceindexchg(int nidx);
    void onEnSorterclssChg(bool bsta);
    void onEnAccChkChg(bool bsta);
    void onMinNumChg();
    void onMaxAdjspantimeChg();
    void onMaxAdjtimesChg();
    void onMaxAutosamplewait();
    void onsamplespanChg();
    void onsvnopencombChg(int nIdx);
    void onLoopSpantimeChg(void);
    void onStaicGpChg(void);
    void onSwitchPageBtn(void);  //! 页面切换槽函数
    void onSwitchParamsBtn(void); //! 参数设置页面切换槽函数
    void onshapechkstatuschg(int nIdx);    //! 形选参数使能状态改变
    void onshapelowthdChg(int nIdx);       //! 形选参数下阈值改变
    void onshapehigthdChg(int nIdx);       //! 形选参数上阈值改变
    void onAutosampleChg(bool bsta);
    void onScheduleEnChg(bool bSta);
    void onScheduleTimeChg(void);
private:
    void refreshtitleBtn(int nCurIdx);
    void refreshlabparams(int nCurIdx);
    void refreshshapelabparams(int nCurIdx);  //! 刷新形选参数
    void refreshSampleChannelinfo();
    void refresnvopencomb();
private:
    QWidget     *m_wid;

    QPushButton *m_maxSortNumtileBtn[MAX_SORTERCLASS_NUM];      //!分次设置
    QPushButton *m_StaicGpBtn; //! 切换相机视角

    QGridLayout *m_glaylab;
    QGroupBox   *m_gb;
    QScrollArea *m_labscroarea;
    QLabel      *m_titlelb[max_titleNum];
    MyCheckBox  *m_labchk[MAX_LABELS];
    MyLineEdit  *m_lowthdle[MAX_LABELS];          //! 下阈值参数
    MyLineEdit  *m_higthdle[MAX_LABELS];          //! 上阈值参数
    MyLineEdit  *m_tarthdle[MAX_LABELS];          //! 上阈值参数
    
    QPushButton *m_renameBtn;
    QPushButton *m_showadjpageBtn;
    QPushButton *m_switchParamsBtn;     //! 参数设置
    QPushButton *m_switchPageBtn;       //! 形选标签

    QWidget     *m_paramsWid;
    QGroupBox   *m_params_gb;           //! 取样口参数
    QLabel      *m_sourcelb;
    QComboBox   *m_sourcecomb;  
    QLabel      *m_EnSortlb;
    MyCheckBox  *m_EnSortclass;
    QLabel      *m_EnAccModelb;
    MyCheckBox  *m_EnAccChk;

    QLabel      *m_maxSortNumlb;
    MyLineEdit  *m_maxSortNumle;

    QLabel      *m_minNumlb;   //! 调节下限
    MyLineEdit  *m_minNumle;

    QLabel      *m_maxTimespanlb;   //! 时间上限
    MyLineEdit  *m_maxTimespanle;

    QLabel      *m_maxAdjtimelb;   //! 单口调节次数
    MyLineEdit  *m_maxAdjtimele;

    QLabel      *m_maxAdjwaitlb;   //! 等待间隔
    MyLineEdit  *m_maxAdjwaitle;

    QLabel      *m_samplespanlb;   //! 取样间隔
    MyLineEdit  *m_samplespanle;


    QGroupBox  *m_otherparamsgb;  //! 其他参数

    QLabel      *m_Loopspanlb;   //! 轮取样间隔时间
    MyLineEdit  *m_Loopspanle;

    QLabel      *m_snvopenlb;   //! 长吸口
    QComboBox   *m_snvopencomb;

    QLabel         *m_autoSampleEnlb;
    mySwitchButton *m_autoSampleEnSw;

    QLabel         *m_scheduleEnlb;  //! 定时吸料
    mySwitchButton *m_scheduleEnSw;

    QLabel         *m_scheTimelb;    //! 定时吸料时间
    MyLineEdit     *m_scheTimele;

    QPushButton     *m_reSetPIDBtn;

    QLabel      *m_localsocketstaLb;

    //!  形选参数控件
    QGroupBox   *m_shape_gb;   
    QStackedLayout *m_stackedLayout;
    QLabel      *m_shapetitlelb[3];
    MyCheckBox  *m_shapechk[MAX_LABELS];
    MyLineEdit  *m_shapeLowthdle[MAX_LABELS];
    MyLineEdit  *m_shapeHigthdle[MAX_LABELS];


    int         m_nSortidx = 0;  //! 取样口ID
    int         m_nCamID = 0;  //! 相机序号ID
};

#endif // SORTERCOMPAGE_H
