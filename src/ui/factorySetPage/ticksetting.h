#ifndef TICKSETTING_H
#define TICKSETTING_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QButtonGroup>
#include "params/globalparams.h"
#include <core/globalflow/globalflow.h>
#include "common/mymutex.h"
#include "src/common/myswitchbutton.h"
#include "src/common/mylineedit.h"
#include "common/mypushbutton.h"
#include "src/common/myinputpanel.h"
#include "src/ui/factorySetPage/ejectorselfcheckwidget.h"


class ticksetting : public QWidget
{
    Q_OBJECT
public:
    explicit ticksetting(QWidget *parent = nullptr);
    void createpage(void);
    void laypage(void);
    void connectAndSlt(void);
    void refreshPage(void);

signals:

public slots:
    void onValEnchgSlt(bool bsta);
    void onAcctimelechgslt(void);
    void onAcctime2lechgslt(void);
    void onVoltypechgslt(int nIdx);
    void onD2typechgslt(int nIdx);
    void onValfretypechgslt(int nIdx);
    void onValspantimechgslt(void);

    void onDelayBtnpressedSlt();
    void onBlowBtnpressedSlt();
    void onTruncBtnpressedSlt();
    void onDegBtnpressedSlt();
    void onCacheBtnpressedSlt();
    void onmutexBtnchagedSlt(int nIdx);
    void onHfpBtnpressedSlt();
    void onRectResizeChgSlt();

private:
    QStackedWidget *m_stackWidget;

    QWidget   *m_wid;

    QGroupBox *m_valveGb;
    QLabel    *m_valOnffLb;
    mySwitchButton *m_valOnffSw;
    QLabel    *m_valSpeedLb;
    MyLineEdit *m_valSpeedLe;
    QLabel    *m_valSpeed2Lb;
    MyLineEdit *m_valSpeed2Le;
    QLabel    *m_valSpantimeLb;
    MyLineEdit *m_valSpantimeLe;
    QLabel    *m_valFreLb;
    myMutex   *m_valFreMut;
    QLabel    *m_valD2Lb;
    myMutex   *m_valD2Mut;
    QLabel    *m_valModeLb;
    myMutex   *m_valModeMut;

    QGroupBox *m_ticktimeGb;
    QLabel    *m_delaylb;
    MyLineEdit *m_delayLe;
    QLabel    *m_blowlb;
    MyLineEdit *m_blowLe;
    QLabel    *m_Trunclb;
    MyLineEdit *m_TruncLe;
    QLabel    *m_Deglb;
    MyLineEdit *m_DegLe;
    QLabel     *m_Hfplb;
    MyLineEdit *m_HfpLe;
    QLabel     *m_ejectorOnoffLb;
    mySwitchButton *m_ejectorOnoffSw;
    
    QLabel     *m_Cachelb;
    MyLineEdit *m_CacheLe;
    QRadioButton *m_PluscacheBtn;
    QRadioButton *m_NegacacheBtn;
    QButtonGroup *m_mutexBtnGp;


    QGroupBox *m_resizeRectGb;
    QLabel *m_resizeRectLb;
    MyLineEdit *m_resizeRectLe;


    //! 喷阀自检
    myPushButton *m_ejcCheckBtn;
    ejectorSelfCheckWidget *m_selfCheckWidget;

};

#endif // TICKSETTING_H
