#ifndef DEFINE_H
#define DEFINE_H

#ifndef MY_ZL
#define MY_ZL
#endif

#include <QWidget>

const char HEAD_AA = 0xAA;
const uchar HEAD_AD = 0xAD;
#define WIN_WID       1920   //! 界面宽
#define WIN_HEI       1080   //! 界面高

#define BTN_WID       130    //! 按钮宽
#define BTN_HEI       70     //! 按钮高
#define SMALL_BTN_WID 50     //! 小按钮高度

#define GROUP_WID     380    //! 组合框宽
#define GROUP_HEI     300    //! 组合框高

#define CMD_VERSION         0x1000    //! 版本查询
#define CMD_CAM_ONOFF       0x0001    //! 运行开始/结束
#define CMD_CAM_SIGNAL      0x0002    //! 信号调试
#define CMD_CTRL_SIGNAL     0x0002    //! 信号调试
#define CMD_CURVE           0x0003    //! 波形获取
#define CMD_CTRL_FEED       0x0003    //! 振动器控制
#define CMD_INT_EJTEST      0x001B  //! 喷阀自检
#define CMD_CAMERA          0x0004    //! 相机校正
#define CMD_CTRL_WIPE       0x0005    //! 清灰电磁阀控制
#define CMD_CAMERA_PIXEL    0x0005    //! 像元划分
#define CMD_ROW_FREQUENCY   0x0006    //! 行频设置
#define CMD_ANAGAIN         0x0008    //! 模拟增益
#define CMD_DGIGAIN         0x0009    //! 数字增益
#define CMD_CLEANJAR_STATUS 0x0007    //! 清灰缸状态
#define CMD_SVCTRL          0x000A    //! 电磁阀控制
#define CMD_IMG_UPLOAD      0x0010    //! 图像上传
#define CMD_CAM_REVISE      0x0011    //! 相机板复位
#define CMD_SOC_VERSION     0x0012    //! soc版本查询
#define CMD_HALOGEN_VERSION 0x0012    //! 卤素灯版本查询
#define CMD_HALOGEN_STATUS  0x0002    //! 卤素灯状态
#define CMD_CTRL_VLAVE_STA  0x0018   //!  电压检测
#define CMD_SOURCE_VERSION  0x0012    //! 恒流源板版本查询
#define CMD_UNIT_IM01_VERSION     0x0012
#define CMD_BKG_VALUE       0x0013    //! 背景阈值下发
#define CMD_CAM_WAVE_BACK   0x001D    //! 校正波形界面返回时发送    0：无效        1:有效
#define CMD_SRC_LIGHT       0x000C    //! 光源板亮度调整
#define CMD_SOC_SAMPLE      0x0023    //! 采样行数
#define CMD_MACHINE_TYPE    0x000A    //!  机型参数
#define CMD_SOC_ONOFF       0x4100    //! 供料开关  深度学习参数设置(Nvidia AI平台专用)
#define CMD_SOC_PARAMS      0x4101   //! SOC算法参数
#define CMD_SOC_PARAMS2      0x4136   //! SOC算法参数
#define CMD_SOC_EXTRAPARAMS   0x415C   //! SOC算法参数
#define CMD_SOC_ADPARAMS    0x4102    //! SOC高级参数
#define CMD_SOC_MODEL_SWITCH 0x4105   //! SOC模型切换
#define CMD_SOC_MODELINFO   0x4106    //! SOC模型信息查询
#define CMD_SOC_IMG_EXPORT  0x410A    //! 导出图片
#define CMD_SOC_LOG_EXPORT  0x410D    //! 导出图片
#define CMD_SOC_IMG_CLEAR   0x410C    //! 清除图片
#define CMD_SOC_MODELLAB_INFO 0x4110  //! SOC标签信息获取
#define CMD_SOC_UP_STATE      0x410F  //! SOC平台操作状态查询
#define CMD_SOC_EXPORT_IMAGES_NET      0x4140
#define CMD_SOC_BACKGROUND_CONDITION   0x4001
#define CMD_SOC_BACKGROUND_INFO 0x401A
#define CMD_SOC_CHANNEL_INFO    0x4103
#define CMD_SOC_SYSTEM_TIME     0x4104
#define CMD_SOC_Model_LINE      0x0042
#define CMD_SOC_SHUTDOWN        0x410B
#define CMD_SOC_LOG_EXPORT      0x410D
#define CMD_SOC_INFER_RESULT    0x6006
#define CMD_SOC_BOTTLE_INFERRESULT    0x6007
#define CMD_SOC_IMG_PARAMS            0x4117
#define CMD_SOC_INFRA_CHANNELINFO     0x415C
#define CMD_SOC_INFRA_MODELLABINFO    0x415D   //! SOC高光谱相机标签
#define CMD_SOC_WORKMODE              0x415D   //! SOC工作模式
#define CMD_SOC_TRADITONSENS          0x415E   //! SOC传统灵敏度
#define CMD_CTRL_REQMETAL             0x0008
#define CMD_EJECTOR_PACKET          0x6001
#define CMD_DELAY_TIME               0x0013
#define CMD_AIR_ALARM               0x0017
#define CMD_BLOW_TIME               0x0019
#define CMD_HIGFRE_PROTECT          0x001A
#define CMD_CAM_TRIGER_TYPE         0x0034
#define CMD_EN_SNV                  0x0015
#define CMD_CAPLINE_TOCTRL          0x0014
#define CMD_SNVCHANNEL_ONOFF        0x000B
#define CMD_VALBOARD_PARAMS         0x1006
#define CMD_VALBOARD_ONOFF          0x0022
#define CMD_SOC_STAINFO             0x4172
#define CMD_SOC_CAPIMG_LAB_ENABLE   0x4121
#define CMD_SOC_CALIBRATE_VALUE     0x4700
// #define CMD_SOC_CALIBRATE_VALUE_2   0x4701
#define CMD_CAM_HIGH_SPECTRAL_TEMPCTRL  0x0043          //! 温控
#define CMD_EJECTOR_ONOFF           0x0015

/** ----------------传统算法 ------------------------------*/
#define CMD_TARDITON_EDGE_PARAMS            0x4003
#define CMD_TARDITON_ROW_PARAMS             0x4006
#define CMD_TARDITON_GRAYA_PARAMS           0x4008
#define CMD_TARDITON_GRAYB_PARAMS           0x4009
#define CMD_TARDITON_DISCOLORA_PARAMS       0x400B
#define CMD_TARDITON_DISCOLORB_PARAMS       0x400C
#define CMD_TARDITON_INTELA_PARAMS          0x4010
#define CMD_TARDITON_INTELB_PARAMS          0x4011
#define CMD_TARDITON_HSV_PARAMS             0x403B
#define CMD_TARDITON_INTEL_COLOR_PARAMS    0x403F
#define CMD_TARDITON_AI_PARAMS             0x406E

#define CMD_LABEL_TRADITONARITH             0x405C

/*-------------------伺服——————————————————————————————————————————*/
#define CMD_MOTOR_ONOFF             0x000D
#define CMD_MOTOR_SPEED_SET         0x000E
#define CMD_MOTOR_INIT              0x000F
#define CMD_MOTOR_STATUS_REQUIRE    0x0010

/*----------------------- 高光谱命令 ---------------------------------*/
#define CMD_UNIT_IM01_WAVE             0x4300           //! 波段
#define CMD_UNIT_IM01_CAMERA           0x4303           //! 校正
#define CMD_UNIT_IM01_ROWFREQ          0x4305           //! 行频
#define CMD_UNIT_IM01_WINDOW           0x4306           //! 开窗
#define CMD_UNIT_IM01_BIAS             0x4307           //! 偏置
#define CMD_UNIT_IM01_GAIN             0x4308           //! 增益
#define CMD_UNIT_IM01_BKVALUE          0x401A           //! 双红外背景条件
#define CMD_UNIT_IM01_TEMPUTERE        0x0016           //! 传感器温度查询

//! FPGA远程升级
#define CMD_CHECK_MODE      0x1001    //! FPGA升级：状态查询
#define CMD_START_BURN      0x1002    //! FPGA升级：通知下位机准备烧写
#define CMD_UPDATE_USER     0x1003    //! FPGA升级：返回用户模式
#define CMD_UPDATE_FACTORY  0x1004    //! FPGA升级：返回工厂模式
#define CMD_FILE_LEN        0x1005    //! FPGA升级：发送文件长度，通知下位机准备接收升级数据

//! SOC升级命令编码
#define CMD_SOC_UPGRADE_TYPE           0x4116           //! SOC 升级类型
#define CMD_SOC_UPGRADE_PACKAGES       0x4118           //! SOC 升级文件包数量
#define CMD_UPGRADE_STATE_QUERY        0x4117           //! SOC 升级结果查询

//! 加密命令相关
#define CMD_GET_ENCRYPTINFO    0x2000    //! 获取加密信息命令
#define CMD_SET_ENCRYP_STATUS  0x2001    //! 下发加密状态信息命令
#define CMD_SET_ENCRYP_DATA1   0x2002    //! 下发加密信息命令
#define CMD_SET_ENCRYP_DATA2   0x2003    //! 下发加密信息命令


#define SENSOR_MAX_PIXELS    2048     //! 分辨率上限
#define SENSOR_PIXELS_320    320     //! 640传感器
#define SENSOR_PIXELS_640    640     //! 640传感器
#define SENSOR_PIXELS_1024   1024     //! 640传感器
#define SENSOR_PIXELS_2048   2048     //! 2048传感器

#define EJ_NUM               60         //! 通道对应的阀的个数
#define MAX_CAM_NUM          6        //! 相机数量
#define MAX_WAVE_LINE        12
#define MAX_ANALYZE_NUM      4        //! 最大分析仪数量
#define MAX_TARGET_SCHM      128      //! 最多目标值方案数目
#define MAX_LABELS           30       //! 最大标签数量
#define MAX_LABELS_NAMES     (30+20)       //! 标签名称最大字节数（包含中英文）
#define MAX_SORTERCLASS_NUM  8

#define LEN_PACKET_A         44	      //! 定义A类包长度
#define LEN_PACKET_A_CRC     38	      //! 定义A类包CRC长度
#define LEN_FRAME_FPGA       1024     //! 升级数据包长
#define MAX_LEN_PACKET_I     1024*EJ_NUM+12+4   //! 定义喷阀包最大长度

#define DEST_IP              "192.168.0"   //! 目的端IP
#define DEST_PORT            6000            //! 命令端口号
#define SOC_PORT             5000            //! SOC命令端口号
#define PORT_DATA            6401            //! 下位机通信数据端口号
#define PORT_DATA_1808       6501            //! 下位机通信数据端口号(接收1808数据)

#define IMAGE_HEIGHT         640
#define BIG_IMAGE_HEIGHT     4096
#define SMALL_IMAGE_HEIGHT   150

#define MAX_FILE_SIZE   (1024 * 1024)   //! 定义日志文件的最大容量1MB

#define MEYER_BIN_DIR     "/userdata/meyer/"                 //! meyer.bin所在目录
#define APP_DIR           "/ssd/"                      //! app.bin所在目录
#define CNF_VPNAUTH       "/ssd/cnf/cnf.vpn-auth"      //! VPN认证文件
#define CNF_NETWORK       "/ssd/cnf/cnf.network"       //! 网络配置文件
#define CNF_PROFILE       "/ssd/cnf/cnf.profile"       //! 全局方案参数文件
#define CNF_GLOBAL        "/ssd/cnf/cnf.config"        //! 全局保存参数文件
#define CNF_ANALYZE       "/ssd/cnf/cnf.analyze"       //! 分析仪参数保存文件
#define CNF_NETNAME       "/ssd/cnf/cnf.netname"       //! 网络名称文件

#define LOG_FILE          "/ssd/cnf/log.txt"
#define MOUNT_UDISK_PATH  "/userdata/tmpudisk/"

#define CNF_DATABASE      "/ssd/statisticDataLog.db"   //! 统计数据库
#define FILE_APP_BIN      "app.bin"                    //! bin文件
#define FILE_APP_BIN_BAK  "bak_app.bin"                //! 备份bin文件
#define COM_VIRTUAL       "/dev/ttyUSB"                //! 虚拟串口
#define COM_PRINT         "/dev/ttyS0"                 //! 打印串口

#define LANG_DIR          "./i18n/"      /*! 语种目录 */

#define MAX_SOC_MODEL_NAME_L    (30+20)
#define MAX_SOC_MODEL_COUNT_L   20
#define MAX_SOC_MODEL_COUNT_XL  40
#define MAX_SOC_MODEL_COUNT     10
#define MAX_SOC_MODEL_LABEL_L   30
#define MAX_SOC_MODEL_NAME      40
#define MAX_SOC_MODEL_LABEL     15
#define SOC_IMAGE_PKT_SIZE              61488
#define SOC_IMAGE_MD5_INDEX             18
#define SOC_IMAGE_MD5_SIZE              16

#define SOC_IMAGE_EFFECT_DATA_INDEX2    34
#define MAX_EXTRA_INFO_NUM              6        //! Box支持的最大额外信息数量

#define MAX_SNV_CHANNEL                 8
#define MAX_CPU_CORE                    4
#define MAX_VIBRATOR                    2

#define INTERVAL_CHECK_NETWORK  10      //! 网络连接状态检查间隔
#define NETWORK_FAILED_TIMES    3       //! 网络通信失败次数限制

#define SORTERADJ_BY_VIRTUALIP 0        //! 是否通过虚拟Ip联调色选机

#define DEFAULT_MACHINEID       "6000000000000"     //! 默认机器编号

#define MAX_CLASSIFY_NUM  4         //! 物料统计最大分类类别

const QString g_sCikuDir = ":/res/ciku/pinyin";

/*!
 * \brief g_sCikuDir
 */
enum {
    nMachine_Bottle = 0,
    nMachine_Crop
};

/*!
 * \brief 升级文件目录
 */
const QString g_sUpgradeDir = "/ssd/upgrade/";

enum {
    AUTHORITY_PRIMARY,    //! 初级
    AUTHORITY_SENIOR,     //! 中级
    AUTHORITY_ENGINEER    //! 工程师
};

enum {
    MQ_STATE_INIT            = 0,   //! 初始化状态
    MQ_STATE_GET_TOKEN       = 1,   //! 获取Token成功
    MQ_STATE_GET_TOKEN_ERR   = 2,   //! 获取Token失败
    MQ_STATE_GET_KEY         = 3,   //! 获取密钥成功
    MQ_STATE_GET_KEY_ERR     = 4,   //! 获取密钥失败
    MQ_STATE_CONNECT         = 5,   //! 连接Mq成功
    MQ_STATE_CONNECT_ERR     = 6,   //! 连接Mq失败
    MQ_STATE_ACTIVATE        = 7,   //! 设备激活成功
    MQ_STATE_ACTIVATE_ERR    = 8,   //! 设备激活失败
};

enum sysAlarm{
    SYS_ALARM_AIR = 0,       //! 气压报警
    SYS_ALARM_HALOGEN,       //! 卤素灯报警
    SYS_ALARM_CLEANTANK,     //! 清灰缸报警
    SYS_ALARM_CTRLVLAVE,     //! 控制板报警
    SYS_ALARM_AutoSample,     //! 自动取样堵料报警
    SYS_ALARM_INFRACAMTEMPURE, //! 红外温度
    SYS_ALARM_ALL            //! 所有报警
};

/*!
 * \enum 枚举网络连接状态
*/
enum{
    BOARD_CONTROL, //! 控制板
    BOARD_CAMERA,  //! 相机板
    BOARD_AI,      //! SOC
    BOARD_PLUS_LIGHT, //! 恒流源
    BOARD_HALOGEN, //! 卤素灯
    BOARD_VALVE,  //! 电磁阀
    BOARD_ALL
};

/*!
 * \enum 枚举网络连接状态
 */
enum {
    NETWORK_DISCONNECT = 0, /*! 网络断开 */
    NETWORK_NO_INTERNET,    /*! 无Internet访问 */
    NETWORK_NORMAL          /*! 网络正常 */
};

enum{
    COLOR_TYPE_R = 0,
    COLOR_TYPE_G,
    COLOR_TYPE_B,
    COLOR_TYPE_N,
    COLOR_TYPE_NUM
};

enum {
    LAMP_BACK_UP,
    LAMP_MAT_UP,
    LAMP_BACK_DOWN,
    LAMP_MAT_DOWN,
    LAMP_TYPE
};

enum {
    PLUSE_1,
    PLUSE_2,
    PLUSE_3,
    PLUSE_4,
    PLUSE_5,
    PLUSE_6,
    PLUSE_TYPE
};

typedef enum {
    PIXEL_640 = 1,
    PIXEL_2048 = 2
} PIXEL_SIZE;

enum {
    SENSOR_T03,
    SENSOR_D04
};

/*!
 * \enum 枚举模块类型
 */
enum {
    MODULE_CTRL = 0x01,   /*! 控制板 */
    MODULE_CAM = 0x02,    /*! 相机板 */
    MODULE_PULSE = 0x04,  /*! 脉冲板 */
    MODULE_VAL = 0x05,    /*! 低压阀驱动板 */
    MODULE_SOC = 0x07,     /*! SOC板*/
    MODULE_HALOGEN = 0x09,
    MODULE_SVBOARD = 0X0C /*! 电磁阀驱动板 */
};

typedef enum{
    MODE_FACTORY = 0,     /*! 工厂模式 */
    MODE_USER             /*! 用户模式 */
}BOARD_MODE;

/**
 *  设备基本属性
 */
enum {
    MACHINE_TITLE  = 0,           //! 设备标题
    MACHINE_ROLLER = 1,           //! 设备辊方向
    MACHINE_ICON   = 2,           //! 设备图片
    MACHINE_SPEED  = 3,           //! 设备化验速度
    MACHINE_CONTROL= 4,           //! 设备远程控制使能
    MACHINE_STATE  = 5,           //! 设备状态
    MACHINE_BASED_MODEL = 6
};

/**
 * 设备物理属性
 */
enum {
    MACHINE_MODEL_ONE   = 0,        //! 设备属性1
    MACHINE_MODEL_TWO   = 1,        //! 设备属性2
    MACHINE_MODEL_THREE = 2,        //! 设备属性3
    MACHINE_MODEL_FOUR  = 3,        //! 设备属性4
    MACHINE_MODEL_FIVE  = 4,        //! 设备属性5
    MACHINE_MODEL_SIX   = 5,        //! 设备属性6
    MACHINE_MODEL_SEVEN = 6,        //! 设备属性7
    MACHINE_MODEL_EIGHT = 7,        //! 设备属性8
    MACHINE_MODEL_TOTAL = 8
};

enum {
    MACHINE_ANALYZE_TARGET_ONE   = 0,    //! 化验指标1
    MACHINE_ANALYZE_TARGET_TWO   = 1,    //! 化验指标2
    MACHINE_ANALYZE_TARGET_THREE = 2,    //! 化验指标3
    MACHINE_ANALYZE_TARGET_FOUR  = 3,    //! 化验指标4
    MACHINE_ANALYZE_TARGET_FIVE  = 4,    //! 化验指标5
    MACHINE_ANALYZE_TARGET_SIX   = 5,    //! 化验指标6
    MACHINE_ANALYZE_TARGET_TOTAL = 6     //! 化验指标7
};

/*!
 * \enum 枚举物料类别
 */
enum MATERIAL_TYPE{
    MATERIAL_RICE = 0,    /*! 大米 */
    MATERIAL_WHEAT,       /*! 小麦 */
    MATERIAL_STICKY,      /*! 糯米 */
    MATERIAL_PADDY,       /*! 稻谷 */
    MATERIAL_BROWN,       /*! 糙米 */
    MATERIAL_RED,         /*! 红米 */
    MATERIAL_PURPLE,      /*! 紫米 */
    MATERIAL_NUM
};

/*!
 * \enum 枚举取样类型
 */
enum {
    SAMPLE_RAW = 0,         /*! 原粮 */
    SAMPLE_GOOD,            /*! 成品 */
    SAMPLE_WASTE1,          /*! 废料1 */
    SAMPLE_WASTE2           /*! 废料2 */
};

/*!
 * \enum 砻谷机-化验室指标类型
 */
enum ANALYSIS_INDEX{
    A_TARGET_ONE = 0,    /*! 糙碎率 */
    A_TARGET_TWO,        /*! 脱壳率 */
    A_TARGET_THREE,      /*! 爆腰率 */
    A_TARGET_TOTAL       /*! 指标总数*/
};

/*!
 * \enum 抛光机-化验室指标类型
 */
enum POLISH_INDEX{
    P_TARGET_ONE = 0,    /*! 白度*/
    P_TARGET_TWO,        /*! 碎米率*/
    P_TARGET_THREE,      /*! 爆腰率*/
    P_TARGET_TOTAL       /*! 指标总数*/
};

/*!
 * \enum 碎米分级筛-化验室指标类型
 */
enum RESCREEN_INDEX{
    R_TARGET_ONE = 0,    /*! 成品含碎*/
    R_TARGET_TWO,        /*! 大碎*/
    R_TARGET_THREE,      /*! 中碎*/
    R_TARGET_FOUR,       /*! 小碎*/
//    R_TARGET_FIVE,       /*! 碎米含整*/
    R_TARGET_TOTAL       /*! 指标总数*/
};

/*!
 * \enum 色选机-化验室指标类型
 */
enum SORT_INDEX{
    S_TARGET_ONE = 0,    /*! 黄米率*/
    S_TARGET_TWO,        /*! 垩白率*/
    S_TARGET_THREE,      /*! 碎米率*/
    S_TARGET_TOTAL       /*! 指标总数*/
};

enum {
    LONGGU_SPEED_LEFT = 0,     /*! 线速差左*/
    LONGGU_SET_CURRENT_LEFT,   /*! 设置电流左*/
    LONGGU_CURRENT_LEFT,       /*! 实时电流左*/
    LONGGU_PRESSURE_LEFT,      /*! 紧辊压力左*/
    LONGGU_FREQUENCY_LEFT,     /*! 实时频率左*/
    LONGGU_SPEED_RIGHT,        /*! 线速差右*/
    LONGGU_SET_CURRENT_RIGHT,  /*! 设置电流右*/
    LONGGU_CURRENT_RIGHT,      /*! 实时电流右*/
    LONGGU_PRESSURE_RIGHT,     /*! 紧辊压力右*/
    LONGGU_FREQUENCY_RIGHT,    /*! 实时频率右*/
    LONGGU_TOTAL
};

/**
 * 1808化验返回结果-糙米
 */
enum {
    BROWN_DG = 0,          /*! 稻谷*/
    BROWN_ZT = 1,          /*! 整糙透明*/
    BROWN_ZH = 2,          /*! 整糙红*/
    BROWN_ZL = 3,          /*! 整糙绿*/
    BROWN_ST = 4,          /*! 碎糙透*/
    BROWN_SH = 5,          /*! 碎糙红*/
    BROWN_SL = 6,          /*! 碎糙绿*/
    BROWN_ZZ = 7,          /*! 杂质*/
    BROWN_BY = 8,          /*! 爆腰*/
    BROWN_TOTAL
};

/**
 * 1808化验返回结果-大米
 */
enum {
    DISP_RICE_WHOLE_TRANS = 0,  /*! 整透粒数*/
    DISP_RICE_WHOLE_WHITE = 1,  /*! 整白*/
    DISP_RICE_BRO_TRANS = 2,    /*! 碎透*/
    DISP_RICE_BRO_WHITE = 3,    /*! 碎白*/
    DISP_RICE_BROKEN_AREA = 4,  /*! 碎米面积*/
    DISP_RICE_YELLOW_AREA = 5,  /*! 黄米面积*/
    DISP_RICE_WHITE_AREA = 6,   /*! 白米面积*/
    DISP_RICE_AREA = 7,         /*! 米面积和*/
    DISP_RICE_BY = 8,           /*! 爆腰粒数*/
    RICE_TOTAL
};

/*!
 * \enum MODBUS类型
 */
enum {
    TYPE_TCP = 0,         /*! TCP */
    TYPE_SERIAL,          /*! Serial */
    MODBUS_TYPE           /*! 类型 */
};

/**
 *调节指标
 */
enum {
    TARGET_MIN = 0,
    TARGET_MAX = 1,
    TARGET_TYPE = 2
};

/**
 *设备辊方向
 */
enum {
    ROLLER_LEFT  = 0,
    ROLLER_RIGHT = 1,
    ROLLER_TYPE  = 2
};

enum STATUS_TYPE{
    NODE_TYPE,
    ADJUST_TYPE,
    STATUS_NUM
};

enum MACHINE_STATUS{
    MNORMAL,
    UNSET_MCODE,
    SCHEM_UNBIND,
    OFFLINE
};

enum NODE_STATUS{
    NNORMAL,
    UNSET_NCODE,
    DISCONNECT,
    SIG_EXCEPT,
    WIPE_FAILED,
    FEEDER_EXCEPT
};

/*!
 * \enum 产线细分类
 */
enum {
    SCENE_SORT_HULL     = 0,   /*! 砻谷段 */
    SCENE_SORT_MILL     = 1,   /*! 碾米段 */
    SCENE_SORT_PRESCREE = 2,   /*! 初筛段*/
    SCENE_SORT_POLISH   = 3,   /*! 抛光段 */
    SCENE_SORT_SORT     = 4,   /*! 色选段 */
    SCENE_SORT_RESCREEN = 5,   /*! 二筛段 */
    SCENE_SORT_PACKET   = 6    /*! 打包段 */
};

#define SCENE_MAX 7

/**
 * @brief 智联平台设备类型
 */
enum MACHINE_TYPE{
    MACHINE_SAMPLER = 10,         //! 扦样机
    MACHINE_ANALYZER = 20,        //! 分析仪
    MACHINE_MOISTURER = 30,       //! 水份仪
    MACHINE_METAL_DETECTER = 40,  //! 重金属检测仪
    MACHINE_VIBRATER = 50,        //! 振动清理筛
    MACHINE_STONE_CLEARER = 60,   //! 去石机
    MACHINE_RIDGE_VALLEY = 70,    //! 砻谷机
    MACHINE_GRAIN_COARSE = 80,    //! 谷糙分离筛
    MACHINE_SORTER = 90,          //! 色选机
    MACHINE_TRAFFIC = 100,        //! 流量秤
    MACHINE_RICE_MILL = 110,      //! 碾米机
    MACHINE_RICE_GRAD = 120,      //! 碎米分级筛
    MACHINE_POLISH = 130,         //! 抛光机
    MACHINE_PACKAGE = 140,        //! 打包秤
    MACHINE_SERVER = 160          //! 边缘服务器
};

/**
 * 砻谷机属性标签
 */
enum {
    LONGGU_LABLE_NONE = 0,                  //! 无属性
    LONGGU_LABLE_CURRENT_FAST,              //! 快辊电流
    LONGGU_LABLE_CURRENT_SLOW,              //! 慢辊电流
    LONGGU_LABLE_SPEED_FAST,                //! 快辊线速度
    LONGGU_LABLE_SPEED_SLOW,                //! 慢辊线速度
    LONGGU_LABLE_SPEED_DIFFER,              //! 线速差
    LONGGU_LABLE_SPEED_SUM,                 //! 线速和
    LONGGU_LABLE_TOTAL
};

/**
 * 抛光机界面数据索引
 */
enum {
    POLISH_NONE = 0,                        //! 无属性
    POLISH_FLOW = 1,                        //! 流量
    POLISH_CURRENT = 2,                     //! 电流
    POLISH_PRESSURE = 3,                    //! 压力
    POLISH_TYPE
};

/**
 *PLC类型
 */
enum {
    PLC_INIT_TYPE = 0,
    PLC_LONGGU = 1,
    PLC_SUIMISHAI = 2,
    PLC_PAOGUANG = 3,
    PLC_TOTAL
};

/*!
* \enum 色选机整机类型
*/
enum {
    SORTER_SERIAL,       /*! 串联 */
    SORTER_PARALLEL      /*! 并联 */
};

#define CMD_REQ_SCHMBIND        519     //! 获取绑定方案

#define SDK_SORTER_OBTAIN_INFO  200     //! 获取整机信息
#define SDK_SORTER_FEEDER       201     //! 控制色选机供料开关设置
#define SDK_SORTER_SENS         202     //! 调节色选机灵敏度
#define SDK_SORTER_OUTPUT       203     //! 调节色选机供料量
#define SDK_SORTER_SCHM         204     //! 设置色选机方案
#define SDK_SORTER_OBTAIN_OUTPUT 205    //! 获取色选机产量信息
#define SDK_SORTER_OBTAIN_ALARM  206    //! 获取色选机报警信息

#define OPER_SET_SENS           207     //! 极简界面设置色选机灵敏度
#define MAX_GROUP_TICK          6       //! 最大剔除组数量

/*!
 * \brief 物料算法列表
 */
enum {
    ARITH_GREY_A = 0,                 /*! 0 灰度杂质 A */
    ARITH_GREY_B = 1,                 /*! 1 灰度杂质 B */
    ARITH_DISCOLOR_A = 2,             /*! 2 色差杂质 A */
    ARITH_DISCOLOR_B = 3,             /*! 3 色差杂质 B */
    ARITH_INTEL_A = 4,                /*! 4 智能模式 A */
    ARITH_INTEL_B = 5,                /*! 5 智能模式 B */
    ARITH_INTEL_C = 6,                /*! 6 智能模式 C */
    ARITH_INTEL_D = 7                /*! 7 智能模式 D */
};

enum {
    TRADITION_ARITH_COUNT = ARITH_DISCOLOR_B + 1
};

/*!
 * \brief 大米仿真列表
 */
enum {
    RICE_ALL = 0,               //全部
    RICE_WHOLE = 1,             //整米
    RICE_WHOLE_WHITE = 2,       //整白
    RICE_WHOLE_TRANS = 3,       //整透
    RICE_BROKEN = 4,            //碎米
    RICE_BROKEN_WHITE = 5,      //碎白
    RICE_BROKEN_TRANS = 6,      //碎透
    RICE_WHITE = 7,             //白米
    RICE_TRANS = 8,             //透明米
    RICE_YELLOW = 9,            //黄
    RICE_DUST = 10,             //杂质
    RICE_BY = 11,               //爆腰
};

/*!
 * \brief 糙米仿真列表
 */
enum {
    BROWN_ALL           = 0,       //全部
    BROWN_PADDY         = 1,       //稻谷
    BROWN_WHOLE_TRANS   = 2,       //整糙透
    BROWN_WHOLE_RED     = 3,       //整糙红
    BROWN_WHOLE_GREEN   = 4,       //整糙绿
    BROWN_BROKEN_TRANS  = 5,       //碎糙透
    BROWN_BROKEN_RED    = 6,       //碎糙红
    BROWN_BROKEN_GREEN  = 7,       //碎糙绿
    BROWN_IMPURITY      = 8,       //杂质
    BROWN_HUSK          = 9,       //稻皮
    BROWN_BAOYAO        = 10,      //爆腰
};

/*!
 * \brief 报警列表
 */
enum SORTER_STATUS{
    ALARM_ID_PRESSURE,      /*! 气压报警 */
    ALARM_ID_BLOWER,        /*! 风机报警 */
    ALARM_ID_TEMPERATURE,   /*! 温度报警 */
    ALARM_ID_COMM,          /*! 设备通信报警 */
    ALARM_NUM
};

/*!
 * \brief 枚举模型类型
 */
enum ModelType{
    MODEL_DETECT,
    MODEL_SEG,
    MODEL_TYPE
};
/*!
 * \brief 方案编码/名称
 */
struct schemeInfo {
    QString code;        /*! 方案编码 保存名称*/
    QString name;        /*! 方案名称 显示名称*/
};

enum {
    CAMERA_CK = 0x00,
    CAMERA_QD = 0x01
};

enum {
    OPERATE_UPGRADE_SOFT = 0x00,      /*! 升级软件 */
    OPERATE_UPGRADE_MODEL = 0x01,       /*! 升级模型 */
    OPERATE_EXPORT_IMG = 0x02,      /*! 导出图片 */
    OPERATE_EXPORT_LOG = 0x03      /*! 导出日志 */
};

/*!
 * \brief 加密方案数据
 */
struct structEncryptInfo {
    int codeSize[4];      /*! 加密码各个位长度 */
    uchar getStat;        /*! 信息获取状态 */
    uchar nEna;           /*! 加密使能 */
    uchar nLenth;         /*! 加密长度 */
    uchar nLimitCnt;      /*! 累计保存使用限制 */
    char codeTmp[25];     /*! 加密码 */
    QString savedTime;    /*! 保存的时间 */
    QString sCode;        /*! 加密码 */
};

const QString g_ColorLightGreen  = "rgb(234,242,255)";
const QString g_ColorMYGreen     = "rgb(0,141,75)";
const QString g_ColorGray    =     "rgb(147,152,172)";
const QString g_ColorLightGray   = "rgb(224,226,231)";
const QString g_ColorLightGray2  = "rgb(216,216,216)";
const QString g_ColorLightGray3  = "rgb(241,240,245)";
const QString g_ColorGreen       = "rgb(0,255,0)";
const QString g_ColorRed         = "rgb(255,0,0)";
const QString g_ColorPinch       = "rgb(255,0,255)";
const QString g_ColorDarkYellow  = "rgb(255,97,3)";
const QString g_ColorLightYellow = "rgb(255,215,0)";
const QString g_ColorBrown       = "rgb(94,38,18)";
const QString g_ColorCoreRed     = "rgb(255,192,203)";
const QString g_ColorBlue    =     "rgb(0,0,255)";
const QString g_ColorStone   =     "rgb(118,128,105)";

const QString g_enableBtnStyle = QString("background-color:%1;color:%2;border-width:0px;").arg(g_ColorLightGray3).arg(g_ColorMYGreen);
const QString g_disableBtnStyle = QString("background-color:%1;color:%2;border-width:0px;").arg(g_ColorLightGray3).arg(g_ColorLightGray2);

const QString g_PushButtonStyle = "QPushButton {"
        "text-align: center;"
        "padding:10px;"
        "outline: none;"
        "background-color: white;"
        "border-color:#dfdfdf;"
        "border-width:2px;"
        "border-style:solid;"
        "border-radius: 10px;"
        "font: 26px \"Microsoft YaHei\";}"

        "QPushButton:pressed {"
        "color:white;"
        "padding-left:6px;"
        "padding-top:6px;"
        "background-color: #007454;}";

/*!
 * \brief 饼状图颜色
 */
const QColor g_GraphGreen = QColor(100,188,130);
const QColor g_GraphYellow = QColor(246,171,0);

/*!
 * \brief 校正直方图颜色
 */
const QColor g_cali_color_1 = QColor(223,166,0);
const QColor g_cali_color_2 = QColor(0,100,55);
const QColor g_cali_color_3 = QColor(140,1,145);
const QColor g_cali_color_4 = QColor(137,137,137);

extern QWidget     *g_mainWidget;          /*! 主页面 */
#endif // DEFINE_H
