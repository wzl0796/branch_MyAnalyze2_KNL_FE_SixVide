#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include "define.h"
#include <QMap>
#include <vector>
#include <chrono>
/*!
 * \brief 深度学习模型信息结构体
 */
#pragma pack(push)
#pragma pack(1)
typedef struct SocModelInfo{
    char path[MAX_SOC_MODEL_NAME];
    char cfg[MAX_SOC_MODEL_NAME];
    char cnf[MAX_SOC_MODEL_NAME];
} SocModelInfo;

typedef struct StruSocModelInfo{
    int currentId;
    int state;   //0:正常   -1:失败
    SocModelInfo model[MAX_SOC_MODEL_COUNT];
} StruSocModelInfo;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct SocModelInfo_L{
    char cnf[MAX_SOC_MODEL_NAME_L];
} SocModelInfo_L;

typedef struct StruSocModelInfo_L{
    short currentId;
    short state;   //0:正常   -1:失败
    SocModelInfo_L model[MAX_SOC_MODEL_COUNT_L];
} StruSocModelInfo_L;

typedef struct StruSocModelInfo_XL{
    short currentId;
    short state;   //0:正常   -1:失败
    SocModelInfo_L model[MAX_SOC_MODEL_COUNT_L];
    SocModelInfo_L modelAdd[MAX_SOC_MODEL_COUNT_L];
} StruSocModelInfo_XL;
#pragma pack(pop)

/*!
 * \brief 深度学习识别框
 */
struct BBox
{
    int cx; // cenetr point
    int cy; //
    int bw; // box width
    int bh; // box height
    float prob; // ori_prob * conf，对应上位机灵敏度
    int label; // 标签
    float ex_info[MAX_EXTRA_INFO_NUM]; // 额外信息，用于存储 旋转角度、量化长宽、面积等自行指定的值
};
Q_DECLARE_METATYPE(BBox)


struct myfvKA1Box
{
    int cx; // cenetr point x
    int cy; // cenetr point y
    int bw; // box width
    int bh; // box height
    int topx; // box top left x
    int topy; // box top left y
    std::vector<float> prob; // -1 时，表示无标签
    std::vector<int> oriLabel; // -1 时，表示无标签，保存各相机过置信度结果，无优先级处理，与prob、camID、ori_ex_info一一对应
    std::vector<int> camID; // 0,1,2,3
    std::vector<std::vector<float>> ori_ex_info; // 额外信息，用于存储长宽、面积等自行指定的值
    std::vector<int> label; //待统计标签，多相机合并预测结果
    std::vector<float> ex_info; //待统计信息，多相机合并预测结果
    int count_flag;
    int n; //被不同相机统计次数
};
Q_DECLARE_METATYPE(myfvKA1Box)

/*!
 * \brief 标签映射记录结构体，记录每个框的原始标签及映射后标签信息
 */
struct BBoxLabelMapping {
    float prob;        // 置信度
    int oriLabel;      // 原始标签索引
    int mappedLabel;   // 映射后标签索引 (-1表示标签被丢弃, 与oriLabel相同表示未映射保留原标签)
};

/*!
 * \brief AI导图操作反馈
 */
struct struAIOperate {
    int nStatus;                //! 操作状态反馈
    int nImgExportStatus;       //! 图片导出状态
    int nImgExportPercent;      //! 图片导出比例
    bool bStorageValid;         //! 存储空间有效
    int nStorageFree;           //! 剩余存储空间(MB)
    int nImgNum;                //! 有效图像数
};


/*!
 * \brief 输出化验结果数据
 */
struct structPrint {
    QVector<double>   nRate;       /*! 含杂 */
    double         nOutput;      /*! 产量 */
    double         nImpurity;    /*! 含杂率 */
    int            nPrecision;   /*! 加工精度 */
    int            nRiceGrade;   /*! 大米等级 */
    QString        dateTime;     /*! 时间 */
    QString        sOriginal;    /*! 样品来源信息 */
    QString        sPeople;      /*! 检验员信息 */
    QString        sRecipe;      /*! 配方信息 */
    schemeInfo     scheme;       /*! 方案信息 */
};

/*!
 * \brief 枚举标签的统计类型
 */
typedef enum{
     STATISTIC_A1 = 1,//! A1:直接用
     STATISTIC_A2,   //!  A2:数值*系数
     STATISTIC_A3,  //! A3:数值*系数然后算占比
     STATISTIC_A4,  //! A4:直接算占比
     STATISTIC_A5, //! A5:算平均
     STATISTIC_A6, //! A6:数值*系数 算平均
     STATISTIC_A7, //! A7：直接分类
     STATISTIC_A8, //! A8：直接分类 算占比
     STATISTIC_A9, //! A9：数值*系数 分类
     STATISTIC_A10 //! A10：数值*系数 分类 占比
}labstaType;

/*!
 * \brief 标签统计结果结构体
 */
struct slabSta
{
    QString name;                      //! 标签名称
    QMap<QString,QString>   staInfo;   //! 统计类别及方式 key：统计类别 Value：统计方式
    QMap<QString,QList<float>> staRes; //! 统计类别结果 key:统计方式 value:统计结果
};

/*!
 * \brief 物料统计结果结构体
 */
struct smatSta
{
    QMap<QString,QString>   staInfo;      //! 统计类别及方式 Key: 统计类别
    QMap<QString,QList<float>> staRes;   //! 统计类别结果   Key: 统计类别
};

/*!
 * \brief 深度学习参数
 */
struct AIparams
{
    int sens[MAX_CAM_NUM][MAX_SORTERCLASS_NUM*MAX_LABELS];   //! 标签灵敏度
    int nCapLine[MAX_CAM_NUM];           //! 采集行
    int nCashLine[MAX_CAM_NUM];          //! 保留行
    int nModelIdx[MAX_CAM_NUM];          //! 模型序号
    bool bEnlabs[MAX_CAM_NUM][MAX_LABELS]; //! 标签使能
    int nModelLine;                   //! 模型支持的数据类型  NRG NRB NGB RGB NRGB
    int nRes;                        //! 模型分辨率 低 分辨率 高分辨率 超高分辨率 0-512 1-1024 2-2048
    QVector<QMap<QString,int>> vlabelidxmap;       //! 标签优先级
    bool bShapelabEn[MAX_CAM_NUM][MAX_LABELS];    //! 形选标签使能
    bool bshapeEn[MAX_CAM_NUM];                   //! 长短
    bool bshapewidEn[MAX_CAM_NUM];                //! 宽窄
    bool bshapeAreaEn[MAX_CAM_NUM];               //! 面积
    int  nshapelegthlow[MAX_CAM_NUM];
    int  nshapelegthhig[MAX_CAM_NUM];
    int  nshapewidthlow[MAX_CAM_NUM];
    int  nshapewidthhig[MAX_CAM_NUM];
    int  nshapearealow[MAX_CAM_NUM];
    int  nshapeareahig[MAX_CAM_NUM];
    QMap<QString,float> vlabthdmap;           //! 标签阈值
    QVector<QMap<QString,int>> vlabMutexmap;  //! 互斥标签
};

/*!
 * \brief 色选联调的标签参数
 */
struct sorterAdjlabParm
{
    float  nlowthd;         //! 标签下阈值
    float  nhigthd;         //! 标签上阈值
    bool   bEn;             //! 标签使能
    float  nshapeLowthd;    //! 形选标签下阈值
    float  nshapeHigthd;   //! 形选标签上阈值
    bool   bshapeEn;       //! 形选标签使能
};

/*!
 * \brief 分选联调的参数
 */
struct SorterParams
{
    int    nmaxNum;                                       //! 最大分次调节个数
    char   sortclassname[MAX_SORTERCLASS_NUM][30];
    int    nsourceIdx[MAX_SORTERCLASS_NUM];               //! 化验源序号： 0： 成品 1：废料
    bool   bEnClass[MAX_SORTERCLASS_NUM];               //! 次是否使能
    bool   bEnACC[MAX_SORTERCLASS_NUM];                 //! 次是否使能
    int    nMinNumthd[MAX_SORTERCLASS_NUM];             //! 最少调节个数下限制
    int    nMaxSpantime;                                //! 最大调节时间
    int    nAdjtimes[MAX_SORTERCLASS_NUM];
    int    nAdjwaittime[MAX_SORTERCLASS_NUM];          //! 调节等待时间
    int    nsamplespan[MAX_SORTERCLASS_NUM];          //! 调节等待时间
    int    nChannel;                                  //! 长吸口
    int    nLooptimes;                               //! 轮间隔时间
    bool   bScheduled;                              //! 定时取样
    int    nScheduletimes;                          //! 定时取样时间
    QVector<QHash<QString,sorterAdjlabParm>> vlabParm;
};

struct TraditionRetainParams
{
    int mode = 0;
    bool bright = true;
    int sensitivity = 0;
    bool bEn = false;
};

struct TraditionAlgorithmParams
{
    bool bEnable = false;
    int  mode = 0;
    int  nSens;
    bool bright = true;
    int rowCount = 32;
    int badPointCount = 1024;
    int pass = 0;
    bool cutEdge = false;
    bool independentDelay = false;
    int lowerLimit = 0;
    TraditionRetainParams retain[2];   //! 0: 保留1（色差） 1：保留2 (灰度)
};

/*!
 * \brief 方案参数
 */
struct struProfile {
    int nLampValue[LAMP_TYPE];                       //! 灯光值
    int nPluseValue[PLUSE_TYPE];                     //! 灯光值

    bool bEnlamp[LAMP_TYPE];                        //! 灯光使能
    bool bEnPluse[PLUSE_TYPE];

    QString nLampName[LAMP_TYPE];                    //! 灯光名称
    QString nPluseName[PLUSE_TYPE];                  //! 灯光名称
    bool    bHologenLampOn;                          //! 卤素灯开关

    int nGainFast[MAX_CAM_NUM][COLOR_TYPE_NUM];     //! 粗调
    int nGainSlow[MAX_CAM_NUM][COLOR_TYPE_NUM];     //! 细调
    int n12WavesVal[MAX_CAM_NUM][MAX_WAVE_LINE];     //! 波段值

    int    nStaisticType;                           //! 统计类型 0:ppm 1:百分比

    float  ncoffvalue;                               //! 合框函数：重叠系数(交并比)
    int    nareathd;
    int    noffsetvalue;                            //! 合框函数：中心矩形框大小
    bool   bEnMultilab;

    QString slabSecondary[MAX_CAM_NUM];             //! 次一级标签的名称

    int nType[MAX_CAM_NUM];                         //! 识别结果是独立还是与其他相机合并   0:相同  1:独立
    int nCoidx[MAX_CAM_NUM];                        //!  与那个相机结果进行合并
    int nStaticGroup;                               //!  统计组个数
    QVector<QStringList> vUnitaddrlist;             //!  统计组中相机的地址

    QMap<QString,float> Area2weightmap;             //! 转换系数
    QMap<QString,bool>  labprotymap;                //! 标签属性
    QMap<QString,bool>  blabshowmap;                //! 标签使能
    QVector<QMultiMap<QString, QString>> vMegerlabmap;  //!vec:相机ID  map:标签合并 key是父标签 val： 子标签
    QVector<QMap<QString,int>> vlabIdxmap;          //! 标签的序号
    QVector<QMap<QString, QString>> vlabMixmap;     //! 标签互混映射 key: 原始标签 val: 映射标签

    int   nTotalThd[MAX_CAM_NUM];                   //! 材质 总阈值 0:颜色   1：材质  2:老化 3:透明
    int   nMeatal;                                  //! 金属
    AIparams strctAI;                               //! Soc算法参数

    SorterParams strcSortclass;                     //! 分级色选联调参数

    TraditionAlgorithmParams traditionArith[TRADITION_ARITH_COUNT]; //! 传统算法参数，按算法索引
    bool bTraditionLabelEn[MAX_CAM_NUM][MAX_LABELS] = {}; //! 标签传统算法使能
    int nTraditionLabelAlgorithm[MAX_CAM_NUM][MAX_LABELS] = {}; //! 标签关联的传统算法
    QStringList traditionLabelNames[MAX_CAM_NUM]; //! 保存传统算法设置对应的标签名称
    bool bTraditionLabelNamesInitialized[MAX_CAM_NUM] = {}; //! 标签名称快照是否有效

    QMap<QString,bool> staclassEnmap;               //! 统计类别是否显示

    bool    bEnautoSample;                          //! 是否自动取样
    bool    bEnSorterClass;                         //! 是否需要与色选机联调

    int     nMatclassifythd[MAX_CLASSIFY_NUM];      //! 物料统计分类阈值

    struProfile& operator = (const struProfile& other){
        if(this != &other){
//            memcpy(nThreadValue, other.nThreadValue, sizeof(nThreadValue));
            memcpy(nLampValue, other.nLampValue, sizeof(nLampValue));
            memcpy(nPluseValue, other.nPluseValue, sizeof(nPluseValue));

            for(int i = 0; i < LAMP_TYPE; i++)
                nLampName[i] = other.nLampName[i];

            for(int i = 0; i < PLUSE_TYPE; i++)
                nPluseName[i] = other.nPluseName[i];

            bHologenLampOn = other.bHologenLampOn;

            for(int i=0; i<MAX_CAM_NUM; i++)
                slabSecondary[i] = other.slabSecondary[i];

            memcpy(nGainFast, other.nGainFast, sizeof(nGainFast));
            memcpy(nGainSlow, other.nGainSlow, sizeof(nGainSlow));
            memcpy(n12WavesVal, other.n12WavesVal, sizeof(n12WavesVal));
            memcpy(nMatclassifythd, other.nMatclassifythd, sizeof(nMatclassifythd));


            bEnMultilab = other.bEnMultilab;
            vUnitaddrlist = other.vUnitaddrlist;
            Area2weightmap = other.Area2weightmap;
            labprotymap = other.labprotymap;
            blabshowmap = other.blabshowmap;
            vMegerlabmap = other.vMegerlabmap;
            vlabIdxmap = other.vlabIdxmap;
            vlabMixmap = other.vlabMixmap;

            memcpy(strctAI.sens,other.strctAI.sens,sizeof(strctAI.sens));
            memcpy(strctAI.nCapLine,other.strctAI.nCapLine,sizeof(strctAI.nCapLine));
            memcpy(strctAI.nCashLine,other.strctAI.nCashLine,sizeof(strctAI.nCashLine));
            strctAI.nModelLine = other.strctAI.nModelLine;
            strctAI.nRes= other.strctAI.nRes;
            strctAI.vlabelidxmap = other.strctAI.vlabelidxmap;
            strctAI.vlabMutexmap = other.strctAI.vlabMutexmap;
            strctAI.vlabthdmap = other.strctAI.vlabthdmap;
            memcpy(strctAI.bEnlabs,other.strctAI.bEnlabs,sizeof(strctAI.bEnlabs));
            memcpy(strctAI.nModelIdx, other.strctAI.nModelIdx, sizeof(strctAI.nModelIdx));
            memcpy(strctAI.nshapelegthlow, other.strctAI.nshapelegthlow, sizeof(strctAI.nshapelegthlow));
            memcpy(strctAI.nshapelegthhig, other.strctAI.nshapelegthhig, sizeof(strctAI.nshapelegthhig));
            memcpy(strctAI.nshapewidthlow, other.strctAI.nshapewidthlow, sizeof(strctAI.nshapewidthlow));
            memcpy(strctAI.nshapewidthhig, other.strctAI.nshapewidthhig, sizeof(strctAI.nshapewidthhig));
            memcpy(strctAI.bshapeEn, other.strctAI.bshapeEn, sizeof(strctAI.bshapeEn));
            memcpy(strctAI.bshapewidEn, other.strctAI.bshapewidEn, sizeof(strctAI.bshapewidEn));
            memcpy(strctAI.bshapeAreaEn, other.strctAI.bshapeAreaEn, sizeof(strctAI.bshapeAreaEn));
            memcpy(strctAI.nshapearealow, other.strctAI.nshapearealow, sizeof(strctAI.nshapearealow));
            memcpy(strctAI.nshapeareahig, other.strctAI.nshapeareahig, sizeof(strctAI.nshapeareahig));
            memcpy(strctAI.bShapelabEn, other.strctAI.bShapelabEn, sizeof(strctAI.bShapelabEn));
            memcpy(traditionArith, other.traditionArith, sizeof(traditionArith));
            memcpy(bTraditionLabelEn, other.bTraditionLabelEn, sizeof(bTraditionLabelEn));
            memcpy(nTraditionLabelAlgorithm, other.nTraditionLabelAlgorithm,
                   sizeof(nTraditionLabelAlgorithm));
            for (int i = 0; i < MAX_CAM_NUM; ++i) {
                traditionLabelNames[i] = other.traditionLabelNames[i];
            }
            memcpy(bTraditionLabelNamesInitialized, other.bTraditionLabelNamesInitialized,
                   sizeof(bTraditionLabelNamesInitialized));
            bEnautoSample = other.bEnautoSample;
            bEnSorterClass = other.bEnSorterClass;
            strcSortclass.nmaxNum = other.strcSortclass.nmaxNum;
            strcSortclass.nMaxSpantime = other.strcSortclass.nMaxSpantime;
            strcSortclass.nChannel = other.strcSortclass.nChannel;
            memcpy(strcSortclass.nMinNumthd, other.strcSortclass.nMinNumthd, sizeof(strcSortclass.nMinNumthd));
            memcpy(strcSortclass.sortclassname, other.strcSortclass.sortclassname, sizeof(strcSortclass.sortclassname));
            memcpy(strcSortclass.nsourceIdx, other.strcSortclass.nsourceIdx, sizeof(strcSortclass.nsourceIdx));
            memcpy(strcSortclass.bEnClass,other.strcSortclass.bEnClass,sizeof(strcSortclass.bEnClass));
            memcpy(strcSortclass.bEnACC,other.strcSortclass.bEnACC,sizeof(strcSortclass.bEnACC));
            memcpy(strcSortclass.nAdjtimes, other.strcSortclass.nAdjtimes, sizeof(strcSortclass.nAdjtimes));
            memcpy(strcSortclass.nAdjwaittime, other.strcSortclass.nAdjwaittime, sizeof(strcSortclass.nAdjwaittime));
            memcpy(strcSortclass.nsamplespan, other.strcSortclass.nsamplespan, sizeof(strcSortclass.nsamplespan));

            strcSortclass.vlabParm.clear();
            strcSortclass.vlabParm.resize(MAX_SORTERCLASS_NUM);
            for(int i=0; i<other.strcSortclass.vlabParm.size(); i++){
                strcSortclass.vlabParm[i] = other.strcSortclass.vlabParm[i];
            }
        }

        return *this;
    }
};


typedef enum{
    SENSOR_VL05 = 0,
    SENSOR_IM01,
    SENSOR_VL08,
    SENSOR_IM04
}SENSOR_TYPE;

/**
 * @brief 相机参数结构体
 */
struct struCamera {
    int nFreQuence[MAX_CAM_NUM];                         //! 帧频
    int sensorType[MAX_CAM_NUM];                         //! 传感器类型 0: VL05  1: IM01 2: VL08 3: IM03
    int exposeValue[MAX_CAM_NUM];                        //! 曝光值
    int nWavesSix[MAX_CAM_NUM][MAX_WAVE_LINE];           //! 波段值
    int NoneexposeValue[MAX_CAM_NUM];                    //! 非曝光值
    int nPixelBegin[MAX_CAM_NUM];                        //! 起始像元
    int nPixelEnd[MAX_CAM_NUM];                          //! 终止像元
    int npEjBegin[MAX_CAM_NUM][EJ_NUM];                 //! 阀的起始像元
    int npEjEnd[MAX_CAM_NUM][EJ_NUM];                   //! 阀的终止像元
    int nEjectorBegin[MAX_CAM_NUM];                     //! 相机的起始阀序号
    int nEjectorEnd[MAX_CAM_NUM];                       //! 相机的终止阀序号
    int nEjectorBeginPiexl[MAX_CAM_NUM];                //! 相机的起始阀的像元
    int nEjectorEndPiexl[MAX_CAM_NUM];                   //! 相机的终止阀的像元
    int nAdjustTarget[MAX_CAM_NUM][COLOR_TYPE_NUM];      //! 校正目标
    int nWaveSeg[MAX_CAM_NUM];                           //! 波段 6 or 12 波段
//    int nGainFast[MAX_CAM_NUM][COLOR_TYPE_NUM];        //! 粗调
//    int nGainSlow[MAX_CAM_NUM][COLOR_TYPE_NUM];          //! 细调

    int infraBias[MAX_CAM_NUM][4];

    int nThreadValue[MAX_CAM_NUM];                       //! 波动阈值

    int IM01_back_thread[MAX_CAM_NUM];                   //! IM01_背景阈值
    int IM01_back_limit[MAX_CAM_NUM];                    //! IM01_背景上限

    int nBackgdIdx[MAX_CAM_NUM];                         //! 背景上限   黑 红 绿 黄 蓝 紫 青 白
    int nBackgdValue[MAX_CAM_NUM];                       //! 背景阈值
    int nBackgdRatio[MAX_CAM_NUM];                       //! 背景比例
    int nWorkMode[MAX_CAM_NUM];                          //! 相机的工作模式，针对瓶片机型 0:常规（深度学习）  1:传统  2：常规+高光谱 3：紫外
};

/*!
 * \brief 低压阀驱动板参数
 */
struct svalveParams
{
    bool  bEn;
    int   nacctime;
    float nacctime2;
    int   D2type;
    int   voltype;
    int   nfre;
    int   nspan;
};

/*!
 * \brief 分析仪的全局参数
 */
struct analyzeGlobal{
    analyzeGlobal(){
        struCaParams = new struCamera();
        bMergerColorMaterial = false;
    }
    ~analyzeGlobal(){
        if(struCaParams){
            delete struCaParams;
        }
    }

    char        sAnalyzeName[50];                    /*! 分析仪名称 */
    char        sIp[25];                             /*! 分析Ip地址*/
    int         nFeeder[MAX_VIBRATOR];               /*! 分析仪供料量*/
    float       nMotorSpeed;                         /*! 伺服电机速度 */
    int         nWipeInterval;                       /*! 清灰时间间隔 */
    int         nWipeDelay;                          /*! 清灰延迟停止 */
    int         nknifetime;                          /*! 风刀时间 */
    int         nknifechanel;                        /*! 风刀通道*/
    int         nBlowDelay;
    int         nBlowTime;
    int         nTruncTime;                          /*! 截断时间*/
    int         nDegTime;                            /*! 消磁时间*/
    int         nCacheLine;                          /*! 剔除缓存行*/
    int         nhfpTime;
    int         nCacheType;                          /*! 剔除缓存行类型*/
    int         nRectResize;

    PIXEL_SIZE  nPixel[MAX_CAM_NUM];                 //! 像素大小1024、2048
    struCamera *struCaParams;                        //! 相机参数

    int         nHalogenThd;                             //!卤素灯阈值

    schemeInfo  anlyze_schemeList[MAX_TARGET_SCHM]; /*! 分析仪方案保存名称 */
    struProfile analyzeProfile;                     /*! 分析仪方案参数 */

    bool    bHasAssist;                              //! 是否有辅配
    int     nMajorCamNum;                            //! 主配相机个数
    QString CamName[MAX_CAM_NUM];                    //! 相机名称
    bool    bEnHalogen;                              //! 使能卤素灯
    bool    bEnPulseLight;                           //! 额外恒流源
    bool    bEnBackdgrod;                            //! 背景棍使能
    bool    bEnshape;                                //! 使能形选
    bool    bEncleanjar;
    bool    bEnPurityStats;                         //! 使能杂质统计计数
    bool    bEnAirAlarm;                            //! 气压报警使能
    int     nCamTrigerType;                         //! 相机触发方式
    bool    bEnSNV;                                 //! 电磁阀使能
    bool    bEnVal;                                 //! 低压阀使能
    bool    bEjectorOnoff = true;                   //! 喷阀开关
    bool    bEnmetalDetect;                         //! 金探是否使能
    bool    bEnweb;
    int     nCamoffPiexl[MAX_CAM_NUM];              //! 合框相机的偏置像素
    bool    bEnlightSync;                           //! 灯光跟随使能(瓶片)
    bool    bEnAutocalIMSensor;                     //! 自动校正高光谱相机
    bool    bEnMultivibrator;
    bool    bEnAdjSorterTips;
    bool    bEnCamTempureAlarm;                     //! 红外温度报警
    int     nInfratempthd;                          //! 红外温度阈值
    bool    bEninfratempCtrl;                       //! 红外温度控制使能
    bool    bMergerColorMaterial;                   //! 颜色材质合并使能
    svalveParams svalParms;                          //! 低压阀参数
    qulonglong nRuntime;                              //! 累积运行时间
    qulonglong nFeedertime;                           //! 累计供料时间
};

struct analyzeShare{
    ~analyzeShare(){}
    bool    bFeeder;                            //! 供料开始/停止
    bool    bWipe;                              //! 清灰标志
    bool    bCheckVerStartUp;                   //! 开机第一次查询版本
    char    sCamVersion[MAX_CAM_NUM][20];       //! 相机板版本
    char    sCtrlVersion[20];                   //! 控制板版本
    char    sPulseVersion[20];                  //! 恒流源控制板版本
    char    sHalogenVersion[20];                //! 卤素灯检测板版本
    char    sSVBoardVersion[20];                //! 电磁阀驱动板
    char    sValveBoardVersion[20];             //! 低压阀驱动板
    char    sSocVersion[MAX_CAM_NUM][20];       //! SOC版本
    bool    sSocModelSta[MAX_CAM_NUM];          //! SOCModel加载状态

    //! 升级相关参数
    int   nUpdateType;                       //! 升级板子类型
    bool  nUpgradeStatusInfo;                //!
    int   bAIUpdStatus;                      /*! AI处理板升级标志 */
};


/*!
 * \brief 服务器的一些全局参数
 */
struct struShare {
    bool  nFeederState;                      //! 开关下料
    bool  nGetBak;                           //! 去背景
    bool  nIsCapaturing;                     //! 是否正在拍照
    int   nIsWiping[MAX_ANALYZE_NUM];        //! 是否正在清灰

    uchar nUnit;                             //! 当前通道
    uchar sRgbRow[SENSOR_MAX_PIXELS*6];      //! 一帧波形数据信息
    uchar sRowRed[SENSOR_MAX_PIXELS];        //! 红值信号
    uchar sRowGreen[SENSOR_MAX_PIXELS];      //! 绿值信号
    uchar sRowBlue[SENSOR_MAX_PIXELS];       //! 蓝值信号
    uchar sRowNInf[SENSOR_MAX_PIXELS];       //! 红外信号
    uchar sRowNInf1[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf2[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf3[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf4[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf5[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf6[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf7[SENSOR_MAX_PIXELS];      //! 高光谱信号
    uchar sRowNInf8[SENSOR_MAX_PIXELS];      //! 高光谱信号

    int nRunCnt;                             //! 本次运行时间
    int nThreadValue;                        //! 触发阈值
    int nAuthority;                          //! 权限
    int nAnalyzeIndex;                       //! 当前操作分析仪序号

    int nCheckTimes;                         //! 版本查询次数

    int nNetwortStat;                        //! 网络状态

    QString sIp;                             //! VPN虚拟IP地址
    QMultiMap<QString,QString> pinyin_map;   //! 词典
    QList<analyzeShare*>  analyzeInfo;       //! 分析仪详细信息 不需要保存

    int     nWaveIndex;

    int     nMetaNum = 0;

    bool bSocModelUnitResult[MAX_CAM_NUM];              //! 标志位：模型信息是否收到
    StruSocModelInfo_L  sModelUnitInfo[MAX_CAM_NUM];   //! 存储的所有模型信息

    QVector<QMap<int, QString>> vlabNamemap;      //! Soc模型的标签名及对应下标（原始标签集合） key: 下标  value:标签
    QVector<QMap<QString, QString>> vlabEngNamemap; //! Soc模型的标签名及对应下标（原始标签集合）
    QVector<QStringList>    vMegreLablst;         //! 合并标签后的总标签集合(以相机为单位)  一级标签(不含子标签)

    int nSocImgHei;                               //! 模型输入的高
    int nSocImgWid;                               //! 模型输入的宽
    int nSoccaptype;                             //! SOC图片保存类型
    int nSoccapthd;                             //! Soc采样阈值
    QVector<int> nCamidvc;                       //! 标记有几个相机进行拍照


    QMap<int,uint> socLineinfomap;               //! Soc推理行号统计缓存 （需要合并结果）
    std::vector<std::vector<BBox>> vSocInfer;    //! 深度学习推理结果缓存 （需要合并结果）
    QVector<QList<BBoxLabelMapping>> vCamLabelMapping; //!仿真 每个相机的标签映射记录(原始label及映射后label)
    std::vector<int> PriorityForKFD;            //! 深度学习界面设置标签优先级
    std::vector<int> vlabMutex;                 //! 标签互斥


    std::vector<std::vector<myfvKA1Box>> vCrossBoxesOld; //! 单个相机的缓存集合
    std::vector<std::vector<myfvKA1Box>> vCrossBoxesOldSingleNum; //! 单个相机的缓存集合

    //! 新方案统计
    QVector<QMap<QString, slabSta>> vlabStamap;   //! key:标签名 val：标签的统计信息 （按统计组进行划分）
    QVector<QMap<QString,float>>    vlabTotalmap; //! 统计方式需要算占比的总数 Key:统计类别 value:分母
    QVector<smatSta>  vsMatinfo;                  //! 物料的统计信息
    QVector<QMap<QString,float>> vmatTotalmap;    //! 统计方式需要算占比的总数 key:计算方式

    QMap<uint, QMap<int, QList<BBox>>> lineBoxMap;  //! 以行号为索引的矩形框记录，格式：QMap<行号, QMap<相机ID, QList<BBox>>>

    int nModeltype;
    int nMergertype;                               //! 合框函数的类型

    //! 定标
    QMap<QString, double> aiLabelRatio;           //! AI标签定标占比，key:标签名 value:百分比
    std::vector<std::vector<BBox>> vSocInferReady; //! 满足全相机条件后的推理结果快照
    bool   bCalibrateSoc;                              //! 标志位：是否需要标定


    int  nAIModel[MAX_CAM_NUM];                  //! SOC模型的个数
    QStringList totallabNamelst;                //! 所有相机标签名集合

    float nlegthRatio;                          //! 坚果 形选时将像素点转成mm的系数
    float nwidRatio;                            //! 坚果 形选时将像素点转成mm的系数

    int   nAssisttomainCamId;                  //! 辅配相机关联到的主配相机(相机升级时使用)

    int   nHalogenValue[2];                    //! 卤素灯AD值
    bool  bUpgrading = false;

    int   nInfraTempure;

    QString batchname;                        //! 化验批次名称
    QString duty_shift;                       //! 化验班次
    float   detect_yield;                     //! 化验产量

    int     nSorterAdjIdx = 0;              //!  自动取样口序号

    bool    bCapimg= false;                //!  标志：是否在拍照
    bool    bEnCaplabs[MAX_CAM_NUM][MAX_LABELS];   //! 拍标记图标签使能


    bool    bBeltOnff = false;
    bool    bCalibrate = false;               //! 是否标定
    QPoint  calipoint[MAX_CAM_NUM];           //! 是否标定
    float   nCalValue;
    bool    bCalWaterContent;

    int     ncleanjarstatus;
    bool    bCtrlVlaveAlarm = false;
    int     nAirStatus = 0;
    bool    bStatics;                        //! 自动取样-是否进行统计
    bool    bAutosampleAlarm ;               //! 标志 自动取样报警

    bool    bEnSimulate;                    //! 是否仿真使能

    //! 物联网上传状态
    qulonglong nfeederTime = 0;             //! 上传物料网供料时间
    qulonglong nRuningTime = 0;             //! 本次开机运行时长
    QString    strNetabtime;               //! 系统网络异常时间
    bool       bSysBoardSta[BOARD_ALL];    //! 系统电路板通讯异常

    QList<bool>    bSorterstalst;
    QVector<QStringList> vShapeLab;         //! 形选标签  Vec: 取样口的最大大小

    bool bAISimulate;                       //! AI板仿真
    bool bAISaveMat;
    
    std::chrono::high_resolution_clock::time_point starttime;
    std::chrono::high_resolution_clock::time_point endtime;
    std::chrono::high_resolution_clock::time_point spantime;
    std::chrono::high_resolution_clock::time_point Ejectime;
};

/*!
 * \brief 服务器的全局参数
 */
struct struGlobal {
    bool     bEnableDHCP;                      /*! DHCP使能 */

    char     sCamName[50];                     /*! 相机网络地址 */
    char     sWifiName[50];                    /*! 无线名称 */
    QString  sLocalIP;                         /*! 本机IP地址 */
    QString  sNetMask;                         /*! 本机子网掩码 */
    QString  sGateWay;                         /*! 默认网关 */
    QString  sServerAddr;                      /*! 服务器地址 */
    QString  sPort;                            /*! 端口号 */

    QString  gCompanyName;                     /*! 公司名称 */
    QString  gDeviceNmae;                       /*! 设备名称 */

    int      analyzeNum;                       /*! 分析仪数量 */

    int      nSchemTotal;                      /*! 方案总数*/
    int      nCntScheme;                       /*! 当前使用方案*/
    QList<schemeInfo>      schemeList;        /*! 方案信息*/

    QList<analyzeGlobal*>  analyzeInfo;       /*! 分析仪详细信息 */

    int     nEnableDelayCodeInfo;             /*! 加密提醒使能标志 */

    QString master_device_code;                 /*! 设备编号 */
    int     nMachineType;                     //! 0:瓶片  1：坚果
};

extern struct struShare   struGsh;           //! 全局共享参数
extern struct struGlobal  struCnfg;          //! 全局保存参数
extern struct structEncryptInfo g_encryInfo;

#endif // GLOBALPARAMS_H
