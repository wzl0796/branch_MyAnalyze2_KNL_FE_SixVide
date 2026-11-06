#include "systemstapage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

SystemStaPage::SystemStaPage(QWidget *parent) : 
    QDialog(parent)
{
    QStringList strlst;
    strlst<<myLan.serial_num <<myLan.acc_runtime <<myLan.acc_detecttime
           <<myLan.detect << myLan.net <<myLan.airalarm <<myLan.halogenlamp
           <<myLan.cleanjar <<myLan.ctrlboardalarm  << myLan.sample_alarm
           <<QString("%1%2").arg(myLan.camer).arg(myLan.temperature);


    m_vIcon <<QIcon(":/res/png/Message_Dialog.png")
            <<QIcon(":/res/png/Message_Period.png")
            <<QIcon(":/res/png/Message_Period.png")
            <<QIcon(":/res/png/Media_Start.png")
            <<QIcon(":/res/png/Device_Network.png")
            <<QIcon(":/res/png/Sorter_Pressure.png")
            <<QIcon(":/res/png/Message_Warning.png")
            <<QIcon(":/res/png/Message_Warning.png")
            <<QIcon(":/res/png/Message_Warning.png")
            <<QIcon(":/res/png/Message_Warning.png")
            <<QIcon(":/res/png/Message_Warning.png");

    for(int n=0; n<SYS_ALARM_ALL && n<strlst.size(); n++){
        m_alarmTypeMap[n] = strlst.at(n);
    }

    createPage();
    connectSigAndSlot();
}

SystemStaPage::~SystemStaPage()
{
}

void SystemStaPage::refreshPage()
{
    m_timer->start();
    updateSystemSta();
}

void SystemStaPage::createPage()
{
    m_pageGb = new QGroupBox();
    m_pageGb->setMinimumHeight(860);

    QVBoxLayout *vlayitem = new QVBoxLayout(m_pageGb);
    vlayitem->setSpacing(15);
    for(int i=0; i<INFO_MAX; i++){
        m_lblIcon[i] = new QLabel();
        m_lblIcon[i]->setScaledContents(true);
        m_lblIcon[i]->setFixedSize(40,40);
        m_lblIcon[i]->setPixmap(m_vIcon[i].pixmap(m_lblIcon[i]->size()));
        m_lblInfo[i] = new QLabel();

        QHBoxLayout *hlay = new QHBoxLayout();
        hlay->addWidget(m_lblIcon[i]);
        hlay->addSpacing(10);
        hlay->addWidget(m_lblInfo[i]);
        vlayitem->addLayout(hlay);
    }
    m_lblIcon[INFO_FEED_STATUS]->hide();
    m_lblInfo[INFO_FEED_STATUS]->hide();
    vlayitem->addStretch();

    //! 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pageGb);
    mainLayout->addStretch();


    m_timer = new QTimer();
    m_timer->setInterval(1000);
}



void SystemStaPage::connectSigAndSlot()
{
    connect(m_timer, &QTimer::timeout, this, &SystemStaPage::onTimerOutSlt);
}


void SystemStaPage::refreshDisplay(int nSta)
{

//    m_alarmList->clear();
//    refreshAlarmList(nSta);
}

void SystemStaPage::refreshAlarmList(int nsta)
{
//    int nitem=0;
//    for(int n=0; n<SYS_ALARM_ALL && n<m_alarmTypeMap.size(); n++){
//        if(nsta & (1 << n)) {
//            addAlarmItem(m_alarmTypeMap[n], nitem++);
//        }
//    }
}

void SystemStaPage::addAlarmItem(const QString& alarmText, int alarmType)
{
//    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
//    QString itemText = QString("[%1] %2").arg(timeStr).arg(alarmText);
    
//    QListWidgetItem* item = new QListWidgetItem(itemText);
//    item->setTextColor(Qt::red);
//    m_alarmList->insertItem(alarmType, item);
}

void SystemStaPage::updateSystemSta()
{
    auto *pAnalyzegb = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex];

    m_lblInfo[INFO_MACHINE_ID]->setText(QString("%1:   %2")
                                            .arg(myLan.machine)
                                            .arg(struCnfg.master_device_code)
                                        );

    m_lblInfo[INFO_RUN_TIME_TOTAL]->setText(QString("%1: %2")
                                            .arg(myLan.acc_runtime)
                                                .arg(QString("%1:%2:%3")
                                                         .arg(pAnalyzegb->nRuntime/3600)
                                                         .arg((pAnalyzegb->nRuntime%3600)/60)
                                                         .arg((pAnalyzegb->nRuntime%3600)%60))
                                        );

    m_lblInfo[INFO_FEED_TIME_TOTAL]->setText(QString("%1: %2")
                                                .arg(myLan.acc_detecttime)
                                                 .arg(QString("%1:%2:%3")
                                                          .arg(pAnalyzegb->nFeedertime/3600)
                                                          .arg((pAnalyzegb->nFeedertime%3600)/60)
                                                          .arg((pAnalyzegb->nFeedertime%3600)%60))
                                            );

    // m_lblInfo[INFO_FEED_STATUS]->setText(QString("%1: %2")
    //                                              .arg(myLan.detect)
    //                                              .arg(struGsh.nFeederState ? myLan.open : myLan.close)
    //                                     );

    bool bNetnormal = struGsh.nNetwortStat == NETWORK_NORMAL?true:false;
    if(bNetnormal){
        m_lblInfo[INFO_NETWORK_STATUS]->setText(QString("%1: %2")
                                                    .arg(myLan.net)
                                                    .arg(myLan.normal)
                                                );
        m_lblInfo[INFO_NETWORK_STATUS]->setStyleSheet("color:black;");

    }else{
        m_lblInfo[INFO_NETWORK_STATUS]->setText(QString("%1: %2")
                                                    .arg(myLan.net)
                                                    .arg(bNetnormal ? myLan.normal : myLan.abnormal)
                                                );
        m_lblInfo[INFO_NETWORK_STATUS]->setStyleSheet("color:red;");
    }



    if(pAnalyzegb->bEnSNV){
        m_lblIcon[INFO_AIRALRM_STATUS]->show();
        m_lblInfo[INFO_AIRALRM_STATUS]->show();
        if(struGsh.nAirStatus){
            m_lblInfo[INFO_AIRALRM_STATUS]->setText(QString("%1: %2")
                                                        .arg(myLan.airalarm)
                                                        .arg(myLan.abnormal)
                                                    );
            m_lblInfo[INFO_AIRALRM_STATUS]->setStyleSheet("color:red;");

        }else{
            m_lblInfo[INFO_AIRALRM_STATUS]->setText(QString("%1: %2")
                                                        .arg(myLan.airalarm)
                                                        .arg(myLan.normal)
                                                    );
            m_lblInfo[INFO_AIRALRM_STATUS]->setStyleSheet("color:black;");
        }
    }else{
        m_lblIcon[INFO_AIRALRM_STATUS]->hide();
        m_lblInfo[INFO_AIRALRM_STATUS]->hide();
    }


    if(pAnalyzegb->bEnHalogen){
        bool halogenAlarm = false;
        if(struGsh.nHalogenValue[0] < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd ||
            struGsh.nHalogenValue[1] < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd) {
            halogenAlarm = true;
        }

        if(m_lblInfo[INFO_HALOGEN_STATUS]->isHidden()){
            m_lblInfo[INFO_HALOGEN_STATUS]->show();
            m_lblIcon[INFO_HALOGEN_STATUS]->show();
        }

        if(halogenAlarm){
            m_lblInfo[INFO_HALOGEN_STATUS]->setText(QString("%1: %2")
                                                        .arg(myLan.halogenlamp)
                                                        .arg(myLan.abnormal)
                                                    );
            m_lblInfo[INFO_HALOGEN_STATUS]->setStyleSheet("color:red;");

        }else{
            m_lblInfo[INFO_HALOGEN_STATUS]->setText(QString("%1: %2")
                                                        .arg(myLan.halogenlamp)
                                                        .arg(myLan.normal)
                                                    );
            m_lblInfo[INFO_HALOGEN_STATUS]->setStyleSheet("color:black;");
        }
    }else{
        if(!m_lblInfo[INFO_HALOGEN_STATUS]->isHidden()){
            m_lblInfo[INFO_HALOGEN_STATUS]->hide();
            m_lblIcon[INFO_HALOGEN_STATUS]->hide();
        }
    }

    if(struGsh.ncleanjarstatus){
        m_lblInfo[INFO_CLEANJAR_STATUS]->setText(QString("%1: %2")
                                                     .arg(myLan.cleanjar)
                                                     .arg(myLan.abnormal)
                                                 );
        m_lblInfo[INFO_CLEANJAR_STATUS]->setStyleSheet("color:red;");
    }else{
        m_lblInfo[INFO_CLEANJAR_STATUS]->setText(QString("%1: %2")
                                                     .arg(myLan.cleanjar)
                                                     .arg(myLan.normal)
                                                 );
        m_lblInfo[INFO_CLEANJAR_STATUS]->setStyleSheet("color:black;");
    }



    if(struGsh.bCtrlVlaveAlarm){
        m_lblInfo[INFO_CONTROL_STATUS]->setText(QString("%1: %2")
                                                    .arg(myLan.ctrlboardalarm)
                                                    .arg(myLan.abnormal)
                                                );
        m_lblInfo[INFO_CONTROL_STATUS]->setStyleSheet("color:red;");


    }else{
        m_lblInfo[INFO_CONTROL_STATUS]->setText(QString("%1: %2")
                                                    .arg(myLan.ctrlboardalarm)
                                                    .arg(myLan.normal)
                                                );
        m_lblInfo[INFO_CONTROL_STATUS]->setStyleSheet("color:black;");

    }


    if(pAnalyzegb->analyzeProfile.bEnautoSample){
        if(m_lblInfo[INFO_AUTOSAMPLE_STATUS]->isHidden()){
            m_lblInfo[INFO_AUTOSAMPLE_STATUS]->show();
            m_lblIcon[INFO_AUTOSAMPLE_STATUS]->show();
        }
        if(struGsh.bAutosampleAlarm){
            m_lblInfo[INFO_AUTOSAMPLE_STATUS]->setText(QString("%1: %2")
                                                           .arg(myLan.auto_sample)
                                                           .arg(struGsh.bAutosampleAlarm?myLan.abnormal:myLan.normal)
                                                       );
            m_lblInfo[INFO_AUTOSAMPLE_STATUS]->setStyleSheet("color:red;");

        }else{
            m_lblInfo[INFO_AUTOSAMPLE_STATUS]->setText(QString("%1: %2")
                                                           .arg(myLan.auto_sample)
                                                           .arg(struGsh.bAutosampleAlarm?myLan.abnormal:myLan.normal)
                                                       );
            m_lblInfo[INFO_AUTOSAMPLE_STATUS]->setStyleSheet("color:black;");

        }

    }else{
        if(!m_lblInfo[INFO_AUTOSAMPLE_STATUS]->isHidden()){
            m_lblInfo[INFO_AUTOSAMPLE_STATUS]->hide();
            m_lblIcon[INFO_AUTOSAMPLE_STATUS]->hide();
        }
    }

    if(pAnalyzegb->bEnCamTempureAlarm){
        bool bAlarm = false;

        if((struGsh.nInfraTempure > struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd+10 ||
                struGsh.nInfraTempure < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd-10) &&
                struGsh.nInfraTempure !=-1 ){
            bAlarm = true;
        }

        if(bAlarm){
            m_lblInfo[INFO_CAMTEMPURE_STATUS]->setStyleSheet("color:red;");

        }else{
            m_lblInfo[INFO_CAMTEMPURE_STATUS]->setStyleSheet("color:black;");
        }

        m_lblInfo[INFO_CAMTEMPURE_STATUS]->setText(QString("%1: %2(%3)")
                                                       .arg(QString("%1%2").arg(myLan.camer).arg(myLan.temperature))
                                                       .arg(bAlarm?myLan.abnormal:myLan.normal)
                                                       .arg(struGsh.nInfraTempure)
                                                   );
        m_lblInfo[INFO_CAMTEMPURE_STATUS]->show();
        m_lblIcon[INFO_CAMTEMPURE_STATUS]->show();
    }else{
        m_lblInfo[INFO_CAMTEMPURE_STATUS]->hide();
        m_lblIcon[INFO_CAMTEMPURE_STATUS]->hide();
    }
}



/*!
 * \brief SystemStaPage::onTimerOutSlt
 */
void SystemStaPage::onTimerOutSlt()
{
    updateSystemSta();
}


