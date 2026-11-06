#ifndef PAGECALIBRATE_H
#define PAGECALIBRATE_H

#include <QWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSignalMapper>
#include <QPushButton>
#include <QTimer>
#include <QElapsedTimer>
#include "common/mylineedit.h"
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"
#include "core/io/myserial/myserialcom.h"
#include "core/globalflow/mylanguage.h"
#define MAX_CALIBRATE_NUM 7
class pagecalibrate : public QWidget
{
    Q_OBJECT
public:
    explicit pagecalibrate(QWidget *parent = nullptr);

    void createwid(void);
    void laypage();
    void connectAndslt();
    void refreshpageCalibrate();

signals:

public slots:
    void onCalibrateValChg(int nIdx);
    void onAutoCalibrateSlt(void);
    void onWatercalAcutalChg(int nIdx);
    void onCalWaterBtnSlt();
    void onApplyCalWaterParamsBtnSlt();
    void onClearwatercalSlt();
    void onClearBtnPressed();
    void onClearBtnReleased();
private:
    QWidget   *m_mainwid;

    QGroupBox  *m_piexlsetGb;
    QTextEdit *m_txtedit;
    QLabel     *m_camNamelb[MAX_CAM_NUM];
    MyLineEdit *m_noffpiexle[MAX_CAM_NUM];
    QPushButton *m_calibratebtn;

    QGroupBox  *m_waterCalGb;
    QLabel     *m_titlelb[MAX_CALIBRATE_NUM];
    QLabel     *m_oriValuelb[MAX_CALIBRATE_NUM];
    MyLineEdit *m_waterValuele[MAX_CALIBRATE_NUM];
    QLabel     *m_calIndexlb;
    QComboBox  *m_calIndexcomb;
    QPushButton *m_calWaterBtn;
    QPushButton *m_calWaterApplyBtn;
    QPushButton *m_clearCalBtn;
    QElapsedTimer m_clearBtnTimer;
};

#endif // PAGECALIBRATE_H
