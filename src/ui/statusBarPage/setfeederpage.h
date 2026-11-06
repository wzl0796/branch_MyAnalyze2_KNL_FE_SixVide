#ifndef SETFEEDERPAGE_H
#define SETFEEDERPAGE_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QGroupBox>
#include <QMouseEvent>
#include "common/myinputpanel.h"
#include "params/define.h"
#include "core/globalflow/globalflow.h"
#include "params/globalparams.h"
#include "common/myswitchbutton.h"
#include "core/globalflow/mylanguage.h"

class setFeederPage : public QDialog
{
    Q_OBJECT
public:
    explicit setFeederPage(QWidget *parent = 0);

    void refreshDisplay(void);

signals:

public slots:
    void onFeederValueBtnSlt(int index);

    void onSliderValueChangedSlt(int index);

    void onBackBtnPressedSlt(void);

     void onfeederonoffchg(int index);

private:
    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void getFeederParams(void);


    QString  feeder_set;

    QString  feeder_value;

    int nFeeder[MAX_VIBRATOR];

private:
    QWidget       *m_widget;
    QLabel        *m_titleLbl;
    QGroupBox     *groupBox;
    QLabel        *feederLbl[MAX_VIBRATOR];
    QSlider       *m_slider[MAX_VIBRATOR];
    QPushButton   *feederValueBtn[MAX_VIBRATOR];
    mySwitchButton *m_feedonoffsw[MAX_VIBRATOR];
    QPushButton   *m_backBtn;

    QSignalMapper *sliderMap;
    QSignalMapper *feederValueMap;
    QSignalMapper *feederswonOffMap;
};

#endif // SETFEEDERPAGE_H
