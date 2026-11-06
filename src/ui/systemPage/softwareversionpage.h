#ifndef SOFTWAREVERSIONPAGE_H
#define SOFTWAREVERSIONPAGE_H

#include <QWidget>
#include "QMovie"
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include "params/define.h"
#include "core/globalflow/globalflow.h"
#include "core/globalflow/mylanguage.h"

class analyzeVersion : public QWidget
{
    Q_OBJECT
public:
    enum checkitem{
        ITEM_CONTROL =0,
        ITEM_CAM1,
        ITEM_CAM2,
        ITEM_CAM3,
        ITEM_CAM4,
        ITEM_CAM5,
        ITEM_CAM6,
        ITEM_SOC1,
        ITEM_SOC2,
        ITEM_SOC3,
        ITEM_SOC4,
        ITEM_SOC5,
        ITEM_SOC6,
        ITEM_MODEL,
        ITEM_PLUS_LIGHT,  //! KA单独添加一组灯
        ITEM_HALOGEN,
        ITEM_VALVE,       //! 电阀驱动版
        ITEM_EJECTOR,     //! 微阀驱动板
        ITEM_ALL
    };
public:
    explicit analyzeVersion(int analyze, QWidget *parent = 0);

    void initParams(void);
    void createPage(void);
    void refreshPage();
    void startVersionCheck(void);
    void setModelInfoVisable();

signals:

public slots:

private:
    QGroupBox        *m_verInfogb;
    QGroupBox        *itemGroup[ITEM_ALL];
    QLabel           *headIconLbl[ITEM_ALL];         //! 检测项目lable
    QLabel           *headInfoLbl[ITEM_ALL];         //! 检测项目显示lable
    QLabel           *simpleInfoBtn[ITEM_ALL];       //! 检测项目结果显示Btn
    QLabel           *movieLbl[ITEM_ALL];            //! 进度加载显示lable
    QFrame           *headLine[ITEM_ALL];
    QMovie           *movie;

    QGroupBox        *m_modelInfogb;
    QGroupBox        *m_modelitemgp[MAX_CAM_NUM];
    QLabel           *m_modelIconlb[MAX_CAM_NUM];         //! 模型信息
    QLabel           *m_modelInfolb[MAX_CAM_NUM];
    QLabel           *m_modelInfoReslb[MAX_CAM_NUM];
    QLabel           *m_modelmovielb[MAX_CAM_NUM];
    QFrame           *m_modelframe[MAX_CAM_NUM];
private:
    int                m_analyzeIndex;
    QStringList        checkItemStrList;                     //! 检测模块名称
    QHBoxLayout        *m_headHLayout[ITEM_ALL];
    QHBoxLayout        *m_lineHLayout[ITEM_ALL];
    QVBoxLayout        *m_headVLayout[ITEM_ALL];
    QGridLayout        *m_glayPage;

private:
    void clearAndrelayout();
    void refreshItemShowSta();
    void refreshItemContens();
    void refreshModelInfo();
};

class softwareVersionPage : public QWidget
{
    Q_OBJECT
public:
    explicit softwareVersionPage(QWidget *parent = 0);

    void startVersionCheck(void);
    void refreshPage();
    void setScreenVerLabShow();

signals:

public slots:
    void refreshVerison(void);

private:
    void createPage(void);
    void connecctSigAndSlot(void);

private:
    QStringList        checkItemStrList; //! 检测模块名称

    QWidget          *m_versionWid;                  //! 版本页面
    QLabel           *machineLogoLbl;
    QLabel           *softwareDetailLbl;
    analyzeVersion   *analyzePage[MAX_ANALYZE_NUM];

    QStackedWidget   *m_stackWid;
};

#endif // SOFTWAREVERSIONPAGE_H
