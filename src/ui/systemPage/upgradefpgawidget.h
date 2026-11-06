#ifndef UPGRADEFPGAWIDGET_H
#define UPGRADEFPGAWIDGET_H

#include <QWidget>
#include <QTabBar>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTreeView>
#include <QHeaderView>
#include <QListWidget>
#include <QSignalMapper>
#include <QStandardItemModel>
#include <QCryptographicHash>
#include <qmath.h>
#include "QLayout"
#include "core/mythread/filecopythread.h"
#include "params/define.h"
#include "core/globalflow/globalflow.h"
#include "core/globalflow/mylanguage.h"
#include "common/mycheckbox.h"

/* 通用机型下远程升级部分宏定义 */
#define PACKET_COUNT_BIG    16384       // 发送4M rpd文件需要读取包的次数
#define PACKET_COUNT        8192        // 发送2M rpd文件需要读取包的次数
#define PACKET_COUNT_1M     4096        // 发送1M rpd文件需要读取包的次数
#define PACKET_COUNT_SMALL  2048        // 发送512K rpd文件需要读取包的次数
#define PACKET_COUNT_TINY   512         // 发送128K rpd文件需要读取包的次数
#define FILE_PACKET_SIZE    256         // 文件包的大小
#define PACKET_SIZE         24          // 命令包的大小

#define FILE_SIZE_8M    8*1024*1024
#define FILE_SIZE_4M    4*1024*1024
#define FILE_SIZE_2M    2*1024*1024
#define FILE_SIZE_1M    1024*1024
#define FILE_SIZE_512K  512*1024
#define FILE_SIZE_128K  128*1024

#define LEN_GENERAL_FILE_PACKET_DATA  (60*1024)     //! 定义SOC升级大文件包有效参数长度
#define LEN_FILE_PACKET_DATA          (60*1024+14)  //! 定义SOC升级大文件包长度

//! 升级信息结构体
struct structUpgradeInfo {
    bool bEn;                         /*!< 板使能 */
    int nBoard;                       /*!< 需升级板号: 0-广播地址(所有电路板)  需要升级的分析仪地址*/
    int nBoardTotal;                  //! 分析仪的个数
    int nMode;                        /*!< 升级模式*/
    int nstep[MAX_ANALYZE_NUM];
    QString sUpgradeInfo[MAX_ANALYZE_NUM];    /*!< 当前升级内容*/
    QString sFileName;                /*!< 升级文件名称 */
    QByteArray fileBuf;
    int nWriteLen;
    int nStatus[MAX_ANALYZE_NUM];      /*!< 升级状态 */
    QVector<int> qvControlUnit;       /*!< 升级板控制板地址 */
    int nBoardUnit;                   /*!< 当前板地址*/
};

/*!
 * \enum SOC升级反馈状态，枚举升级结果
 */
enum {
    UPGRADE_NULL = 0,           /*! 初始状态 */
    UPGRADE_SOC_OK,             /*! 成功 */
    UPGRADE_FILE_REPLACE_FAILED,/*! 文件替换失败 */
    UPGRADE_FILE_LACK,          /*! 文件数据包缺失 */
    UPGRADE_FILE_VERTIFY_FAILED /*! 文件验证失败 */
};

class upgradeFPGAwidget : public QWidget
{
    Q_OBJECT
public:
    explicit upgradeFPGAwidget(QWidget *parent = 0);

    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void refreshPage(void);

    void startUpgradeProcess(void);                    /*!< 槽函数: 开始升级流程 */

    void switchMode(BOARD_MODE mode);

    void enableBtn(bool bEnable);

signals:
//    void enableBtnSig(bool);                              /*!< 信号: 使能按钮*/

public slots:
    void upgradeTimeOutSlt();                             /*!< 槽函数: 升级查询定时器*/
    void burnTimeOutSlt();                                /*!< 槽函数: 烧写查询定时器*/
    void onTabIndexChangedSlt(int nIndex);                /*!< 槽函数: 升级方式切换 */
    void onSelBtnsPressed(int nIndex);                    /*!< 槽函数: 使能按钮 */
    void onBoardComboBoxesPressed(int nIndex);            /*!< 槽函数: 组合框 */
    void onFileBtnsPressed(int nIndex);                   /*!< 槽函数: 文件选择 */
    void onModelIdxChgedSlt(int nIdx);                    /*!< 槽函数: 升级模型序号选择 */
    void onClearParamsChgSlt(bool bCheck);
private:
    enum {
        BOARD_LINUX = 0,
        BOARD_FRONT,
        BOARD_FRONT2,
        BOARD_REAR,
        BOARD_REAR2,
        BOARD_CAM_EXTRA1,
        BOARD_CAM_EXTRA2,
        BOARD_CONTROL,
        BOARD_VALVE,
        BOARD_SOC,
        BOARD_SOC_MODEL,
        BOARD_TOTAL
    };

    enum {
        UPGRADE_LOCAL = 0,       /*!< 本地升级 */
        UPGRADE_UDISK,           /*!< U盘升级 */
        UPGRADE_SSD              /*!< 硬盘升级 */
    };

    QTabBar     *m_updateTab;
    QPushButton *m_selBtns[BOARD_TOTAL];
    QLabel      *m_boardType[BOARD_TOTAL];
    QComboBox   *m_boardNum[BOARD_TOTAL];
    QPushButton *m_boardFile[BOARD_TOTAL];
    QTreeView   *m_sourceView;                     /*!< 结果显示控件 */
    QComboBox   *m_socmodelIdxcmb;
    QLabel      *m_labelIdxlb;
    QListWidget *modelidxlstwid;
    MyCheckBox  *m_clearParamschk;

    QSignalMapper *m_selBtnSigMap;              /*!< 选择按钮信号集 */
    QSignalMapper *m_boardComboBoxSigMap;       /*!< 升级板选择控件信号集 */
    QSignalMapper *m_fileSigMap;                /*!< 文件信号集 */
    bool bSocAllUpd[2];                            /*!< Soc是否全部升级 */
    QMap<int, QString>SocUpgdStamap;
    QMap<int, QString>SocUpgdInfomap;
    QMap<int, QString>SocModelUpgdStamap;
    QMap<int, QString>SocModelUpgdInfomap;
    int m_socmodelIdx = 0;

private:
    void initLanParams(void);
    void initUpgradeInfo(void);
    void refreshSelsBtnDisplay(void);            /*!< 刷新板使能的显示 */
    void refreshFileBtnDisplay(void);            /*!< 刷新选择文件的显示 */
    void refreshBoardComboBoxDisplay(void);      /*!< 刷新组合框的显示 */
    void clearUpgradeStatus(int nType);          /*!< 清空对应下位机的升级状态 */
    bool checkUpgradeModelIdx(int nIdx);         /*!< 校验升级模型的ID序号 */
    void refreshModelindexComb();     /*!< 刷新模型升级下标 */

    /*!< 功能函数 */
    QString getTextByStatus(int nType, int nStat, int nBoard);  /*!< 根据状态获取文本 >*/
    QString getTextUpgradeInfo(int nType,int step);     /*!< 根据状态获取文本 >*/
    void refreshInitDisplay(void);                      /*!< 刷新升级状态 >*/
    void refreshCurrentDisplay(int nType);              /*!< 刷新当前板升级结果显示 >*/
    bool checkStatus(int nBoardType,int nStatType);     /*!< 检查板级升级状态 >*/
    bool checkBoardStatus(int nType, int nUnit);        /*!< 检查对应下位机的升级状态 >*/
    bool checkUpdateStatus(int nIndex, int nBoardType, int nUnit, int nStatType);    /*!< 检查下位机状态 >*/

    /*! 升级流程*/
    bool checkUpgradeBoardEn(void);                     /*!< 检查要升级板级使能 >*/
    bool checkFileIsInValid(int index);                 /*!< 检查升级文件是否合理 >*/
    void upgradeLinux(QString dir, QString file_name);  /*!< 升级边缘服务器软件 >*/
    bool upgradeCtrlBoard();                            /*!< 控制板升级 >*/
    bool upgradeValveBoard();                           /*!< 微阀板升级 >*/
    bool upgradeCamBoard(int nType);                    /*!< 相机板升级 >*/
    bool upgradeAIBoard(int nType, int BoardUnit);      /*!< AI板升级 >*/

    bool stepSwitchToFactory(int nType);                /*!< 升级步骤1: 切换至工厂模式 >*/
    bool ctrlEraseFlash();                              /*!< 升级步骤2: 擦除控制板的用户空间 >*/
    bool ctrlSendFileByNet();                           /*!< 升级步骤3: 向控制板发送文件 >*/
    bool stepSwitchToUser(int nType);                   /*!< 升级步骤4: 切换至用户模式 */
    bool setfacType(int nType, int nUint);

    bool getModuleType(int nBoardType, char &module_type, int &nUnit);
    bool getResults();
    void refreshResultDisplay();

    int  upgrade_type;                                  /*!< 升级类型*/
    int  curBoardMode;                                  /*!< 当前板类型 */
    int  step;                                          /*!< 当前步骤 */
    int  nBurnPages;

    structUpgradeInfo     m_struInfo[BOARD_TOTAL];      /*!< 升级信息 */

    FileCopyThread        *m_copyFilethread;            /*! 拷贝子线程 */
    QTimer                *upgradeTimer;                /*! 升级计时器 */
    QTimer                *burnTimer;                   /*! 升级计时器 */

    /*! 升级词根*/
    QStringList m_strList;
    QList<QString> CtrlStepInfoList;
    QList<QString> CamStepInfoList;
    QList<QString> SocStepInfoList;
    QList<QString> EjectorStepInfoList;

    QString curCamId;


    bool m_bchgstep = false; //! 辅配相机需要特殊处理，借用标志位改变step
    bool m_bClear;

};

#endif // UPGRADEFPGAWIDGET_H
