#ifndef GLOBALFLOW_H
#define GLOBALFLOW_H

#include <QObject>
#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QThread>
#include <QTextStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QtConcurrent/QtConcurrent>
#include <QSettings>
#include <QPainter>
#include <QMutex>
#include <QTimer>
#include <QUdpSocket>
#include <QtNetwork>
#include <QImage>
#include <QImageWriter>
#include <QtGui/private/qzipreader_p.h>
#include <QtGui/private/qzipwriter_p.h>
#include "params/globalparams.h"
#include "3rdparty/printer/include/PrinterLibs.h"
#include "core/io/localsocket/mylocalsocket.h"
#include "common/delaycode.h"

class GlobalFlow : public QObject
{
    Q_OBJECT
public:
    explicit GlobalFlow(QObject *parent = 0);
    ~GlobalFlow();

    void initAll(void);                                   //! 初始化所有参数

    void sendAll(bool bStart = false);                    //! 开机发送参数

    void sendAnalyzeParams(int index);                    //! 单个分析仪发送参数

    void sleep(int secs);                                 //! 设置秒级的延迟

    void msleep(int mSecs);                               //! 设置毫秒级的延迟

    bool isUdiskExist(void);

    bool mountUDisk(void);                                //! U盘挂载

    bool umountUDisk(QString dir="/udisk");              //!卸载U盘

    int compressDir(QString src, QString des);   //! 压缩目录

    void uncompressDir(QString srcFile, QString des);

    bool zipReader(QString sFile, QString sDir);
    bool zipWiter(QString sZipPath, QString sDir);

    int SystemCmd(QString sCmd);

    qint64 getDirSize(const QString& path);               //! 获取磁盘大小

    uint fpgaCrc(const char *sData, int nLen);            //! CRC校验

    void initCommunication(bool bManual = false);

    bool checkCommunicationRet(void);                     //! 版本查询结果

    int checkAnalyzeVersion(int analyIndex);

    void initVersion(void);                               //! 初始化分析仪版本

    int  getMachineCamNum(void);                           //!  获取相机总个数

    bool inputStrIsValid(QString str);          /*! 校验字符正则（中文，英文，数字，下划线） */

    bool inputCodeIsValid(QString str);          /*! 校验字符正则（中文，英文，数字，下划线） */

    QByteArray packet_TypeA_NetData(char sCameratype, char sModuleType, char sModuleAddr, int nCmd, const char* pData, int nDataLength);

    QByteArray packet_TypeI_NetData(char sModuleType, char sModuleAddr, int nCmd, const char* pData, int nDataLength);

    analyzeShare* getSpecifyAnalyzeShare(int anaIndex);

    analyzeGlobal* getSpecifyAnalyzeGlobal(int anaIndex);

    QString getSchemeName(int index);                            //! 获取方案名称

    QString getSchmCodeFromIndex(int index);                     //! 获取目标方案编码

    QString getSchmNameFromCode(QString code);                   //! 获取目标方案名称

    int     getSchmIndexFromCode(QString);                       //! 获取目标方案索引

    QString getInitSchemeName(int index);                        //! 获取默认方案名称

    void onPrint(QList<QStringList> conttitle, QList<QStringList> contlst,
                 QStringList titlelst, QStringList taillst);       /*! 打印结果数据 */

    void PrintByrow(int nCol, QVector<QStringList>vRow, QStringList titlelst, QStringList taillst);

    bool renderReport2Pixmap(int cols,const QVector<QStringList> &data,
                                QStringList titlelist, QStringList tailedlist);

    QString putPrintInfoInOrder(bool bPrint,QList<QStringList> labtileLst,
                                QList<QStringList>  contlst,
                                QList<QStringList> titlelst ); /*! 打印数据规整 */

    void qStringTochar(QString src, char *dst, int ndstlen);

    QString getMegerlabKeyByValue(const QString& str,const int &ncamId);

    bool bMegerlabshow(const QString& str, const int &ncamid);

    void ResetGlobalStaParams(void);

    void ResetMegreLabLst(int nCamID);

    void ResetLabIdx(int nCamId);

    void setTime(QDateTime dateTime);

    //! 全局共享参数操作
    void initParamsShare(void);

    void getciku();                             /*! 获取词库信息 */

    void initDB(void);                          /*! 初始化数据库*/

    void initAnalyzeShareInfo(analyzeShare* anaInfo, analyzeGlobal* anaGlobal);

    void recordStatusToDB(void);                          //! 统计数据写入数据库
    void recordStatusToDBMerger(void);                    //! 颜色材质融合统计数据写入数据库

    QString getLabStaResult(int nGbid, QString lab, QString staCat, bool& bSta,float nDiff =0.0, QString strchild=QString());
    QString getLabStaResultMerger(int nGbid, QString lab, QString staCat, float rRatio, bool& bSta, float nDiff = 0.0, QString strchild = QString());

    int  getLabstacatNum(int nCamid);                    //! 获取标签统计类别数

    int  getMatstacatNum(int nCamid);                             //! 获取物料统计类别数

    bool copyDir(const QString &srcPath, const QString &dstPath);

    //! 全局方案参数
    bool findSchmFromList(QString str);                   //! 判断方案名称存在

    void delSchmFromList(int index);                      /*! 删除方案 */

    void delSchmFromList(QString name);                   /*! 删除方案 */

    void renameSchmName(int index, QString name);         //! 重命名方案

    bool getAnalyzeProfileParams(struProfile *profile, const QString fileName);  //! 从配置文件中读取分析仪方案参数

    bool saveAnalyzeProfileParams(struProfile *profile, const QString fileName); //! 保存分析仪方案参数至配置文件中

    void sendProfileParams(void);                         //! 发送方案参数

    //! 全局保存参数
    void getGlobal(void);                                 //! 获取全局保存参数

    void getNetSetting(void);                             //! 获取网络配置参数

    void initGlobalParams(void);                          //! 初始化全局保存参数

    bool getGlobalParams(const QString fileName);         //! 从配置文件中读取全局参数

    bool getGlobalAnalyzeParams(analyzeGlobal *globalParams, const QString fileName);    //! 从配置文件中读取各分析仪参数

    bool saveGlobalParams(const QString fileName);        //! 保存全局参数至方案文件中

    void saveNetSetting();                                //! 保存网络配置文件参数

    bool saveGlobalAnalyzeParams(analyzeGlobal *globalParams, const QString fileName);    //! 从配置文件中读取各分析仪参数

    bool saveSpecifyAnalyze(int analyzeIndex);            //! 保存分析仪数据至方案文件中

    void addAnalyzeThreadInfo(int analyzeIndex);          //! 增加每个分析仪下面的线程信息

    void deleteAnalyzeInfo(int analyeIndex);              //! 删除分析仪信息

    void ResetPxOfEj(int nCamid);

    bool bAiSetCpu(int nCpuId); /*! 绑核设置 */

    void resetStats(void);

    bool calibrate(const QVector<QMap<QString, double>> &ratios,
                   const QVector<std::vector<std::vector<BBox>>> &samples);

    //!  openvpn操作
    bool getVPNAddr(void);        //! 获取本地虚拟网络地址
    bool connetToVPN(void);      //! 判断屏连接服务器是否成功
    void closeVPN();             //! 关闭vpn地址

    //! IO发送数据
    void sendMachineType(void);                         //! 下发机型参数

    void sendImgWaveReq(char nAnalyzeIndex, char nUnit, int nTime, char mode, int nRow, bool isSimulate = false, bool isGetBak = false ,bool isEnable = false);        //! 获取波形

    void sendCtrlSignalMode(char nAnalyzeIndex, char mode);                                                    //! 切控制板工作模式

    void sendCamSignalMode(char nAnalyzeIndex, int nUnitAddr, char mode);                                      //! 切换校正波形和原始波形

    void switchWaveToCalibrateID(int nAnalyzeIndex, int nUnitAddr);

    void sendCalibratePara(char nAnalyzeIndex, char nCalibrationID, char mode, int nUnitAddr);                 //! 发送校正目标

    void sendRowFreqPara(char nAnalyzeIndex, int nUnitAddr, int mode);                                                        //! 发送行频

    void sendGainPara(char nAnalyzeIndex, int nUnitAddr);                                                      //! 发送增益

    void sendcamtrigerType(int nAnalyzeIndex);                                                               //! 相机触发方式

    void sendLampPara(char nAnalyzeIndex);                                                                     //! 发送灯光值

    void sendPlusePara(char nAnalyzeIndex);                                                                     //! 发送脉冲灯光值

    void sendBackGroundValue(char nAnalyzeIndex,int nUnitAddr);                                               //! 下发背景波动范围

    void sendCameraPixelChannel(char nAnalyzeIndex, int nUnitAddr);                                            //! 下发像元划分

    void sendFeederPara(char nAnalyzeIndex,bool bMansw = false,
                        bool onoff = false, bool onoff2 = false);                                            //! 振动器控制

    void setEjectorSelfCheck(int nEjNum, int nOnOff, int nId= 0);  //! 喷阀自检

    void resetFeederPara(char nAnalyzeIndex);                                                                  //! 重置振动器震动量

    void sendWipePara(char turn, char nAnalyzeIndex);                                                          //! 清灰控制

    void sendAbsorbPara(char turn, char nAnalyzeIndex, int timeout);                                            //! 吸料控制

    void sendEjtpacket(QByteArray array,int nDataLength);                                                   //! 发送喷阀数据

    void setHSTempCtrl(int threshold, bool enable);                                          //! 高光谱温控

    void sendBlowTime(void);

    void sendDelayTime(void);

    void sendOnOff(int nAnalyzeIndex);                                                      //! 开关下料

    void sendSocOnOff(int nAnalyzeIndex);                                                   //! SOC行数恢复

    void sendbackgdrodOnoffcmd(bool bsta);                                                 //!  背景棍开关

    void ResetCameraData(int nAnalyzeIndex);                                                //! 重置相机板、高算力，使得图片行数从1开始

    void sendCameraParams(bool bStart = false);                                             //! 发送相机参数

    void sendUpgradeInfo(int nAnalyzeIndex, int cmd, char type, int nUnit, char *data, int nlength);     //! 发送升级参数

    void sendFpgaUpgradeData(int nAnalyzeIndex, char module_type, char module_addr, int arg1, int arg2, QByteArray data);

    void sendFpgaFacType(int nAnalyzeIndex,char type, int nUnit);

    void writeUpdateDataForSoc(int nAnalyzeIndex, char sModuleType, char moduleAddr, int nCmd, int nPackageId, const QByteArray baFile, int validDataLen, bool bIsMD5);

    void sendImgTo1808(int nAnalyzeIndex, char module_type, char module_addr, int arg1, int arg2, QByteArray data);   //! 发送图片至1808

    void sendSampleLinePara(int nAnalyzeIndex, int nSampleLine);                           //! 1808采集行数

    void reqCameraVersion(int nAnalyzeIndex, int nUnit);                                   //! 查询相机版本

    void reqCtrlVersion(int nAnalyzeIndex);                                                 //! 查询控制板版本

    void reqSourceVersion(int nAnalyzeIndex);                                                 //! 查询脉冲板版本

    void reqHalogenVersion(int nAnalyzeIndex);                                              //! 卤素灯检测板

    void  reqSVBoardVersion(int nAnalyzeIndex);                                            //! 查询电磁阀版本

    void  reqValBoardVersion(int nAnalyzeIndex);                                            //! 查询低压阀版本

    void reqHalogenStatus(int nAnalyzeIndex);                                               //! 卤素灯的状态

    void reqCtrlBoardalarm(void);                                                                //! 气压报警查询
    
    void reqSocVersion(int nAnalyzeIndex, int camIndex);                                    //! 查询SOC版本查询

    void reqSocModelinfo(int nAnalyzeIndex, int camIndex);                                 //! 查询SOC模型信息

    void SocParamssend(int nAnalyzeIndex, int camIndex);                                    //! SOC算法参数

    void SocCaplabEnParamsSend(void);                                                       //! 拍标记图标签使能

    void SocParamssendOne(int nAnalyzeIndex, int camIndex, int camAddr);

    void SocAdParamssend(int nAnalyzeIndex, int camIndex);                                 //! SOC算法高级参数

    void SocModelSWitch(int nAnalyzeIndex, int camIndex);                                 //! SOC模型切换

    void SocImgClear(int nAnalyzeIndex, int camIndex);                                   //! soc图片清除

    void reqSocModelLabelinfo(int nAnalyzeIndex, int camIndex);                         //! soc模型标签信息获取

    void SoclabelPropity(int nAnalyzeIndex, int camIndex);                              //! soc标签优先级

    void SocChannelinfo();                                                              //! soc像元信息

    void SocShutdown();                                                                 //! soc关机

    void reqStainfo(int nCamId);                                                                 //! 查询统计信息

    void SendSocParams(int nCmd, char* pData, int nLen, int nAnalyzeIndex, int nCamId, bool bAllCam);  //! soc参数下发                                                             //! soc参数下发

    void resetSoc(bool bReset = true);

    void sendSOCDateTime();                                                             //! soc系统时间

    void sendSocBackConditon();                                                        //! soc背景条件

    void SocModelLine();                                                               //! soc的模型支持线数

    void sendSocWorkMode(int nCamId);                                                  //! soc的工作模式

    void sendSocTraditionSens(int nCamId);                                             //! soc传统算法灵敏度

    QString getCamName(int nCamid);                                                    //! 获取相机名称

    void ReqSocInferParams(int nCamId);                                                       //! 获取SOC的推理关键参数

    void CurBoxMerge(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<BBox> &vecBoxes,
                     int camID, float IOUThr, std::vector<float> conf);

    void CatchedBoxMerge(std::vector<myfvKA1Box> &vecFvKA1Boxes,
                         std::vector<myfvKA1Box> &vecCatchedFvKA1Boxes,
                         float IOUThr, int curRows);

    void NormalMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes,
                                std::vector<int> vlabidx, int CamNum);

    void KHZMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx,
                             std::vector<int> vmutuallylabidx, int CamNum);

    void KxgLHMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx,
                               int CamNum, std::vector<float> conf);

     void LvDouLHMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum,
                             std::vector<float> conf);

    void ZiRanMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum);

    void YuMiLHMergedBoxProcess(std::vector<myfvKA1Box> &vecFvKA1Boxes,
                           std::vector<int> vlabidx, int CamNum, std::vector<float> conf);

    void ShanZhaPianLHMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum,
                                       std::vector<float> conf);

    void STMergedBoxProcess(std::vector <myfvKA1Box> &vecFvKA1Boxes, std::vector<int> vlabidx, int CamNum,
                            std::vector<float> conf);

    void sendInfoToSoc(int nAnalyzeIndex, char module_type, char module_addr, int nCmd, const char *data, int nDataLength);

    bool bHighResolution(int nAnalyzeIndex, int nUnit);

    void sendCalibrateVal2Soc(QVector<float> vValue,bool bClear);

    void sendBoardRevise(int nAnalyzeIndex, int nUnit);

    void saveInfo(QString str);

    void sendMotorCtrl(int nCmd, QByteArray array, int nDatalen);

    bool ResetStatuslabNamelist(void);

    QString getAdminPwd();         /*! 获取高级权限密码 */

    QString getCheckCode(int moduleType);

    void setIM01Waves(char nAnalyzeIndex, int nUnitAddr);
    void setIM01Window(char nAnalyzeIndex, int nUnitAddr);
    void setIM01RowFreq(char nAnalyzeIndex, int nUnitAddr);
    void setInfBiasingParams(char nAnalyzeIndex, int nUnitAddr);
    void setBackgroundSensorIM01(char nAnalyzeIndex, int nUnitAddr);
    void setsetAnaGainParams(char nAnalyzeIndex, int nUnitAddr);
    void reqIM01SensorTempure(char nAnalyzeIndex, int nUnitAddr);

    bool bIsUltraCam(int nCamId);                           /*! 是否是紫外相机*/
    bool bIsIMSerialCam(int nCamId);                         /*! 是否为高光谱系列相机*/
    bool bIsSupportAI(int nCamId);                           /*! 相机是否使用深度学习推理 */
    QList<QString> getAIlabfromstagp(int nStaId);
    void ResetLabthdhod();
    void ResetLabArea2Wcof();
    void ResetLabproperty();
    void ResetAIlabPri();
    void ResetAIlabMutex();
    void validateLabMixMap(int nCamId);

    QJsonObject qstr2Json(const QString& str);

    void reqMetalNum(void);
    void reqAiralarm(void);

    void sendSorterlabparams(bool bclear);
    void sendSorterIPparams(bool bshow = false);
    void sendStaticsrestoSorter(void);
    void ReqSorterStatus(void);
    void sendFeederstatoSorter(void);

    //! 传统算法相关命令
    void sendEdgetraditoncmd();
    void sendRowtraditoncmd();
    void sendgrayAtraditoncmd(int nArithId);
    void sendgrayBtraditoncmd();
    void senddiscolorAtraditoncmd(int nArithId);
    void senddiscolorBtraditoncmd();
    void packetTrationcmd();

    void sendlaboftraditon2Soc();

    /*! 加密相关 */
    void getEncryptInfo();
    void WriteEncryptedStatus();
    void WriteEncryptedDateLimit();
    void WriteEncryptedCurDate();

    void char2Hex(char cChar, uchar& rHex);
    void hex2Char(uchar rHex, char& rChar);

    void printLog(QString sLog);

    int getStaticIdfromCamid(int nCamid);

    void Setorgetcleanjarstatus(int ntype);  //! 1： 获取状态  2：复位

    void sendcapLine2cotrlBoard(int nAnalyzeIndex);

    void sendcapLine2camBoard(int nAnalyzeIndex);

    void sendHigfreProtect(int nAnalyzeIndex);

    //! 电磁阀操作
    void sendSNVEnstatus(int  nAnalyzeIndex);

    void snvchannelonoffctrl(quint8 data);

    //! 低压阀
    void sendvalBoardparams(void);
    void sendvalBoardOnff(void);
    void sendEjectorOnOff(int nAnalyzeIndex,  bool bOnOff);

private:
    int getHanNumFromString(QString str);
    QString joinStr(QString leftStr, QString rightStr, int maxLen);
    QString preProcessStr(QString str, int nMaxlen);

signals:
//    void versionCheckOK(void);

    void initSocketSig(void);

    void rebindSocketSig(void);

    void sigGetImageData(QByteArray, int,int);

    void sigRefreshStatisHeader(int);      /*! 刷新统计标题 */

    void sigNotifyAdjTips(QStringList lablst);
public slots:
    void onInitSorterAdjparams(bool binit);

private:
    int nNetNum;
};

extern GlobalFlow gFlow;

#endif // GLOBALFLOW_H
