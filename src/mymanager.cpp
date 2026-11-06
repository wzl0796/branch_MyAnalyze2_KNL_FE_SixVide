#include "mymanager.h"


myManager::myManager(QObject *parent) : QObject(parent)
{
    g_curveDataThread = new MyCurveThread();
    g_curveDataThread->start();

    initSerialCom();

    initGlobalObject();

    for (int i = 0; i < struCnfg.analyzeNum; i++) {
        gFlow.addAnalyzeThreadInfo(i);
    }
}

myManager::~myManager()
{

}


/**
 * @brief myManager::初始化全局对象
 */
void myManager::initGlobalObject()
{
    g_netThread = new netCheckThread();

    //! 色线联调
    g_localSocket = new MyLocalSocket();
    connect(g_localSocket, SIGNAL(sigsendadjparams(bool)), &gFlow, SLOT(onInitSorterAdjparams(bool)));

    //! 服务器上传
    g_Lotsocket = new LocalSocketThread();

    //! 扫码枪
    g_barcodeScannerThread = new BarcodeScannerThread();
    g_barcodeScannerThread->start();
}

void myManager::initSerialCom()
{
    g_serial = new MyserialCom();
}



