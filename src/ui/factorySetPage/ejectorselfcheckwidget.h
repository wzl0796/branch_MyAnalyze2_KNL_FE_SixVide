#ifndef EJECTORSELFCHECKWIDGET_H
#define EJECTORSELFCHECKWIDGET_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "common/mycheckbox.h"
#include "common/mylineedit.h"
#include "common/myinputpanel.h"
#include "core/globalflow/globalflow.h"
#include "params/define.h"

class ejectorSelfCheckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ejectorSelfCheckWidget(QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void onStartClicked();
    void onStopClicked();
    void onBackClicked();
    void onEjectorEditChanged(void);
    void onEjectorSliderChanged(int value);
    void onTimerTimeout();

private:
    void sendSelfCheck(int onOff);
    void setEjectorNumber(int value);
    int interval() const;

    QLabel    *m_curEjcLb;
    MyLineEdit *m_ejectorEdit;
    QSlider     *m_ejectorSlider;
    QRadioButton *m_highSpeed;
    QRadioButton *m_lowSpeed;
    MyCheckBox  *m_testEnabled;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_backButton;
    QTimer *m_timer;
    int m_currentEjector;
    bool m_outputOn;
};

#endif // EJECTORSELFCHECKWIDGET_H
