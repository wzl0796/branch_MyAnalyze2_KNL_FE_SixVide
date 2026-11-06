#ifndef PAGEBOOT_H
#define PAGEBOOT_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "common/myroundprogressbar.h"
#include "systemPage/softwareversionpage.h"

class pageBoot : public QWidget
{
    Q_OBJECT
public:
    explicit pageBoot(QWidget *parent = nullptr);
    void createpage();
    void layoutpage();
    void connectAndSlt();
    void refreshPage(bool bManual = false);
    void setBootloader();

signals:

public slots:
    void timeOutSlt(void);
//    void handleVersionResultSlt(int boardType, int status);
    void onBtnRefreshClicked(void);
    void onBtnEnterClicked(void);

private:
    QWidget             *m_top;
    MyRoundProgressBar  *m_rpbRound;
    QLabel              *m_lblRound;
    QTimer              *m_timer;
    int                 m_nRoundTimes = 0;

    softwareVersionPage *m_verPage;

    QWidget             *m_down;
    QPushButton        *m_btnRefresh;
    QPushButton        *m_btnEnter;
};

extern pageBoot *g_BootPage;

#endif // PAGEBOOT_H
