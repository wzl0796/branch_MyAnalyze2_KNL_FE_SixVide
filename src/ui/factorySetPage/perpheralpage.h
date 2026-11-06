#ifndef PERPHERALPAGE_H
#define PERPHERALPAGE_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "src/common/mylineedit.h"
#include "src/common/myinputpanel.h"
#include "params/globalparams.h"
#include "src/common/myswitchbutton.h"
#include "core/io/myserial/myserialcom.h"
#include "common/mymutex.h"
#include "core/globalflow/mylanguage.h"

class perpheralPage : public QWidget
{
    Q_OBJECT
public:
    explicit perpheralPage(QWidget *parent = nullptr);

    void createpage(void);
    void laypage(void);
    void connectAndSlt(void);
    void refreshPage(void);

signals:
    void sigrefreshBeltStatus();
    void sigEjectorEnChg(bool bsta);

public slots:
    void onHalogenthdChg();
    void onInitMotoSlt();
    void onFixMotoreSpeedSlt();
    void onSwitchMotorOnoroffSlt(bool status);
    void onSwSnvEnchgSlt(bool sta);
    void onSNVChannelOnOff(int nIdx);
    void onInfratempCtrlChgSlt(bool bSta);
    void onInfratempthdChgSlt();

private:
    QWidget *m_wid;
    QGroupBox *m_HalogenGb;

    QLabel    *m_Halogenthdlb;
    MyLineEdit *m_Halogenle;
    QLabel     *m_Enhalogenlb;
    mySwitchButton *m_EnHalogenSw;

    QLabel     *m_autoCalhalogenlb;
    mySwitchButton *m_autoCalHalogenSw;
    QLabel    *m_InfraTempthdlb;
    MyLineEdit *m_InfraTemple;
    QLabel     *m_InfraTempCtrlb;
    mySwitchButton *m_InfraTempCtrSw;
    QLabel    *m_infraTempureEnlb;     //! 红外相机温度报警
    mySwitchButton *m_infraTempureEnsw;


    //! 伺服
    QGroupBox *m_motorGb;
    QLabel    *m_motorOnofflb;
    mySwitchButton *m_motorSw;
    QLabel    *m_speedlb;
    MyLineEdit *m_motorSpeedle;
    QPushButton *m_motorInitBtn;

    //! other
    QGroupBox *m_otherGb;
    QLabel    *m_pluslightEnlb;
    mySwitchButton *m_pluslightEnsw;
    QLabel    *m_backgdrodEnlb;
    mySwitchButton *m_backgdrodEnsw;
    QLabel    *m_clenjarEnlb;
    mySwitchButton *m_cleanjarEnsw;
    QLabel    *m_AiralarmEnlb; //! 气压报警
    mySwitchButton *m_AiralarmEnsw;
    QLabel    *m_SNVEnlb; //! 电磁阀
    mySwitchButton *m_SNVsw;
    QLabel         *m_ValEnlb; //! 低压阀
    mySwitchButton *m_Valsw;
    QLabel         *m_MetalEnlb; //! 金探
    mySwitchButton *m_Metalsw;
    QLabel         *m_multiVibratorlb;
    mySwitchButton *m_multiVibratorsw;

    //! 电磁阀控制
    QGroupBox *m_SNVGb;
    QLabel    *m_SNVChannellb[MAX_SNV_CHANNEL];
    mySwitchButton *m_SVNEnsw[MAX_SNV_CHANNEL];

};

#endif // PERPHERALPAGE_H
