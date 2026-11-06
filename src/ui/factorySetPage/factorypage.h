#ifndef FACTORYPAGE_H
#define FACTORYPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include "params/globalparams.h"
#include "ui/cameraSetPage/camerasignalpage.h"
//#include "ui/cameraSetPage/getimagepage.h"
#include "engineersetpage.h"
#include "setlangwidget.h"
#include "ui/factorySetPage/pagemachine.h"
#include "ui/factorySetPage/perpheralpage.h"
#include "ui/factorySetPage/pagecalibrate.h"
#include "ui/factorySetPage/ticksetting.h"

class factoryPage : public QWidget
{
    Q_OBJECT
public:
    explicit factoryPage(QWidget *parent = 0);

    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void refreshDisplay(void);

    int getpageIdx(void);

signals:
    void sigEnfeeder(bool);
    void sigChangeScheme();
    void sigIM01SenTempture(int);
    void sigrefreshBeltSta();
    void sigInitwebSocket(bool);
    void sigInitRemoteSorter();
    void sigautosampleEnstatus(int,bool);
    void sigUploadParamsbyCloud(QString,int);
    void sigMachineTypeChg(bool bBootle);
    void sigSimulateEnChg(bool bEn);
public slots:
    void onListSetWidRowChangedSlt();
    void onProcessMachineChgSlt(bool bBottle);

private:
    enum {
        PAGE_SIGNAL_SET,
        PAGE_MACHINE_SET,
        PAGE_PERPHER,
        PAGE_LANG_SET,
        PAGE_ENGINEER_SET,
        PAGE_CALIBRATE,
        PAGE_TICK_SET,
        PAGE_ALL
    };

    QListWidget       *listSetWig;
    QStackedWidget    *m_stack;
    cameraSignalPage  *pageCameraSignal;
//    getImagePage      *pageGetImage;
    engineerSetPage   *pageEngineer;
    setLangWidget     *pageSetLang;
    pageMachine       *pageMachinewid;
    perpheralPage     *perpherwid;
    pagecalibrate     *pageCalib;
    ticksetting       *pagetickSet;

private:
    QString lan_signal_set;
    QString lan_image_get;
    QString lan_simulate;
    QString lan_author_card;
    QString lan_lang_set;
    QString lan_machine_set;
    QString lan_pepher;
    QString lan_calibrate;
    QString lab_sortercom;

    void lisetWigIconChanged(int index);
};

#endif // FACTORYPAGE_H
