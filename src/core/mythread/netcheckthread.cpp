#include "netcheckthread.h"
#include "params/globalparams.h"
#include "core/io/websocket/mysocketmgr.h"


netCheckThread *g_netThread;

netCheckThread::netCheckThread()
{
    m_nFailedTimes = 0;
    nCnt = 1;
    nLastNetwortStat = NETWORK_DISCONNECT;
    m_nAlarm = 0;
}

netCheckThread::~netCheckThread()
{
    if(m_timer){
        qDebug()<<"destry timer";
        delete m_timer;
    }
}

/**
 * @brief netCheckThread::线程运行
 */
void netCheckThread::run()
{
    m_isRunning = true;
    while (m_isRunning) {

        if(struGsh.nFeederState && !struGsh.bUpgrading && !struGsh.bCapimg
            && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect
            && struCnfg.nMachineType == nMachine_Bottle
            && nCnt %2 == 0) {//! 检查金属探测个数
            gFlow.reqMetalNum();
        }

        //! 检查气压报警
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAirAlarm
            && !struGsh.bUpgrading && !struGsh.bCapimg
            && nCnt % 120 ==0) {
            gFlow.reqAiralarm();
        }

        //! 检查卤素灯状态和红外相机
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist
            && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[gFlow.getMachineCamNum()-1] == SENSOR_IM04
            && !struGsh.bUpgrading && !struGsh.bCapimg
            && nCnt % 60 == 0) {

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnHalogen){
                gFlow.reqHalogenStatus(struGsh.nAnalyzeIndex);
                
            }


            //! 红外相机温度(高光谱)
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnCamTempureAlarm){
                gFlow.reqIM01SensorTempure(struGsh.nAnalyzeIndex,gFlow.getMachineCamNum()-1);
            }
        }

        //! 检查清灰缸状态
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEncleanjar
            && !struGsh.bUpgrading && !struGsh.bCapimg
            && nCnt % 180 == 0) {
            gFlow.Setorgetcleanjarstatus(1);
        }

        //! 控制板报警
       if(!struGsh.bUpgrading && !struGsh.bCapimg && nCnt % 180 ==0){
          gFlow.reqCtrlBoardalarm();
       }

        processAlarmStatus();

        /** 检查网络连接状态 */
        if(nCnt % 60 ==0){
            checkNetworkStatus();
        }

        sleep(1);
        nCnt ++;
    }
}

/*!
* @brief 综合处理报警信息
*/
void netCheckThread::processAlarmStatus() 
{
    int nLastSta = m_nAlarm;
    m_nAlarm = 0;

    bool halogenAlarm = false, infraAlarm = false;
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnHalogen) {
        if(struGsh.nHalogenValue[0] < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd ||
           struGsh.nHalogenValue[1] < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nHalogenThd) {
            halogenAlarm = true;

        }
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnCamTempureAlarm){
        if((struGsh.nInfraTempure > struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd+10 || 
            struGsh.nInfraTempure < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nInfratempthd-10) &&
            struGsh.nInfraTempure !=-1 ){
            infraAlarm = true;
        }
    }

    m_nAlarm |= ((struGsh.nAirStatus && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnAirAlarm) ? (1 << SYS_ALARM_AIR) : (0 << SYS_ALARM_AIR));
    m_nAlarm |= ((halogenAlarm && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnHalogen) ? (1 << SYS_ALARM_HALOGEN) : (0 << SYS_ALARM_HALOGEN));
    m_nAlarm |= ((struGsh.ncleanjarstatus && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEncleanjar) ? (1 << SYS_ALARM_CLEANTANK) : (0 << SYS_ALARM_CLEANTANK));
    m_nAlarm |= (struGsh.bCtrlVlaveAlarm ? (1 << SYS_ALARM_CTRLVLAVE) : (0 << SYS_ALARM_CTRLVLAVE));
    m_nAlarm |= ((struGsh.bAutosampleAlarm && struGsh.nFeederState && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample)? (1 << SYS_ALARM_AutoSample) : (0 << SYS_ALARM_CTRLVLAVE));
    m_nAlarm |= ((infraAlarm &&  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnCamTempureAlarm)? (1 << SYS_ALARM_INFRACAMTEMPURE) : (0 << SYS_ALARM_INFRACAMTEMPURE));

    if(nLastSta != m_nAlarm) {
        emit sigUpdateSysSta(m_nAlarm);
    }

}
 
/**
 * @brief netCheckThread::检查网络状态
 */
void netCheckThread::checkNetworkStatus()
{
    if (nCnt%INTERVAL_CHECK_NETWORK == 0) {
        /** 获取当前网络连接状态 */
        QString filePath = QString("/sys/class/net/%1/carrier").arg(QString(struCnfg.sWifiName));
        QFile file(filePath);
        QTextStream out(&file);
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
        int stat = 0;
        out >> stat;
        file.close();

        if (stat == 0) {
            struGsh.nNetwortStat = NETWORK_DISCONNECT;
            nLastNetwortStat = struGsh.nNetwortStat;
            m_nFailedTimes = 0;
        } else {
            /** 与服务器通信以判断是否可连接互联网 */
            QProcess process;
            QString cmd = QString("ping smartapi.chinameyer.com -c 15");
            process.start(cmd);
            process.waitForFinished(6*1000);

            QString strres;
            strres = QString(process.readAll());
            if (strres.contains("ttl",Qt::CaseInsensitive)) {
                struGsh.nNetwortStat = NETWORK_NORMAL;
                m_nFailedTimes = 0;
            } else {
                m_nFailedTimes++;
            }
            process.close();

            if (m_nFailedTimes >= NETWORK_FAILED_TIMES) {
                m_nFailedTimes = 0;
                struGsh.nNetwortStat = NETWORK_NO_INTERNET;
                nLastNetwortStat = struGsh.nNetwortStat;
            }
        }
    }
}
