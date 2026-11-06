/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycurve.h
 * \brief       波形显示界面头文件
 * \date        2015.01.14
 */
#ifndef MYCURVE_H
#define MYCURVE_H

#include <QWidget>
#include <vector>
#include <QPainter>
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLayout>
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"


#define CURVE_WIDGET_HEIGHT 420
#define CURVE_WIDGET_WIDTH	850

class MyAxis;

/* 绘制R、G、B的标志 */
enum{
    colorR = 0,
    colorG,
    colorB,
    colorN
};

/* 绘制前后视的标志 */
enum {
    FRONT_VIEW = 0,
    REAR_VIEW,
	ASSIST_FRONT_VIEW,
	ASSIST_REAR_VIEW
};

/* 绘制类型 */
enum {
    TYPE_WAVE = 0,           //! 绘制波形
    TYPE_EJECT_TIMES = 1,    //! 绘制吹气次数
    TYPE_CONTROL_DATA        //! 控制数据
};

/* 显示波形的类型索引号 */
enum{
    WAVE_RED = 0,       //! 红
    WAVE_GREEN,         //! 绿
    WAVE_BLUE,          //! 蓝
    WAVE_INFRARED,      //! 红外
    WAVE_INFRARED1,     //! 高光谱
    WAVE_INFRARED2,     //! 高光谱
    WAVE_INFRARED3,     //! 高光谱
    WAVE_INFRARED4,     //! 高光谱
    WAVE_INFRARED5,     //! 高光谱
    WAVE_INFRARED6,     //! 高光谱
    WAVE_INFRARED7,     //! 高光谱
    WAVE_INFRARED8     //! 高光谱
};

/**
 *砻谷机左右辊数据
 */
enum {
    DATA_LEFT,
    DATA_RIGHT
};

enum{
    CURVE_DEFAULT = 0,       //!
    CURVE_INF,         //! 红外相机设置页面波形
    CURVE_COLOR           //! 可见相机设置页面波形
};

class MyCurveThread : public QThread
{
    Q_OBJECT
public:
    MyCurveThread();
    ~MyCurveThread();

    void getLenAndWait(void);
    bool getRunningState(void);

public slots:
    void stopWave(void);
    void startWave(void);
    void waveThreadRun();

signals:
    void sReceiveData();

protected:
    void run();

private:
    bool bIsRunning;
    int m_nWaittime;        //! 获取波形等待时间
};

/*!
 * \brief 坐标轴及波形绘制类
 */
class MyAxis : public QWidget
{
    Q_OBJECT
public:
    explicit MyAxis(int index = 0, QWidget *parent = 0);
    ~MyAxis();

    void setMinValueX(int value);
    void setMaxValueX(int value);
    void setMinValueY(int value);
    void setMaxValueY(int value);
    void setBorder(int left, int right, int up, int down);      // 设置坐标轴的边界
    int getLeftBorder(void);                            // 获取左边界
    int getRightBorder(void);                           // 获取右边界
    int getUpBorder(void);                              // 获取上边界
    int getDownBorder(void);                            // 获取下边界
    void setTickAttr(void);                             // 设置标记属性
    void setTickNum(void);                              // 设置标记代表的值
    void setTickValue(void);                            // 设置标记的数量
    void paintAxis(QPainter &painter);                  // 绘制整体的坐标轴
    void paintAxisX(QPainter &painter);                 // 绘制坐标轴x
    void paintAxisY(QPainter &painter);                 // 绘制坐标轴y
    void setXByData(void);                              // 根据数据设置横轴坐标
    void setYByData(void);                              // 根据数据设置纵轴坐标
    void drawWanted(QPainter &painter);                 // 绘制想要的波形
    void drawColorWave(QPainter &painter);              // 绘制彩色波形
    void drawMachineWave(QPainter &painter);            // 绘制砻谷机数据
    void drawSingleWave(QPainter &painter,int flag);    // 根据标志位显示单一的波形
    void drawMachineData(QPainter &painter,int flag);   // 根据标志位显示单一的波形(砻谷机数据显示)
    void getNewdata();                                  // 获取下一帧数据
    void setDrawType(int type);                         // 设置绘制的类型
    void drawEjectTimes(QPainter &painter);             // 绘制吹气次数
    void drawSingleTimes(QPainter &painter, int flag);  // 绘制单视的吹气次数波形
    void drawIdeResult(QPainter &painter);              // 绘制颜色算法识别结果
    void drawSingleResult(QPainter &painter, int flag); // 绘制单个算法识别结果
    void setBorderShow(bool bFlag, int showBorderIndex); // 设置边界是否显示  showBorderIndex:0-全通道 1-左侧  2-右侧
    bool getBorderShow(void);                           // 获取边界是否显示
    void setMultiWaveFlag(bool bFlag);                  // 设置是否绘制重叠波形标志位
    void setBorderValue(int left, int right);           // 设置边界数
    void setPixmap(QPixmap pixmap);                     // 设置需要绘制的图像
    void setSignalType(int type);                       // 设置当前波形所属的相机类型，0：可见光，1：红外
    void setAnalyzeIndex(int analyze, int machine = 0); // 设置分析仪序号

protected:
    virtual void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    int m_nDrawType;
    bool m_bDrawMultiWave;
    QPixmap m_pixmap;          // 图像用于保存波形页面的显示
    int m_nSignalType;              // 用于标志当前波形所属的相机类型，0：可见光，1：红外

    /* 绘制坐标轴类的参数 */
    QFont font;
    int minValueX;
    int maxValueX;
    int minValueY;
    int maxValueY;
    quint8 leftBorder;
    quint8 rightBorder;
    quint8 upBorder;
    quint8 downBorder;
    int minorTickValueX;
    int minorTickValueY;
    int majorTickValueX;
    int majorTickValueY;
    int majorXNum;
    int majorYNum;
    int minorXNum;
    int minorYNum;
    uchar *redData;
    uchar *greenData;
    uchar *blueData;
    qreal valueXPerPixel;
    qreal valueYPerPixel;

    /* 左边界与右边界位置 */
    int m_nLeft;
    int m_nRight;
    bool m_bDrawBoarder;
	int m_nShowBoarderIndex;

    //! 新增
    int m_Index;
    int m_analyzeIndex;
    int m_machineIndex;
    int sCaliRed[512];               //! 左
    int sCaliBlue[512];              //! 右

};

/*!
 * \brief 波形显示控制类
 */
class MyCurve : public QWidget
{
    Q_OBJECT
public:
    explicit MyCurve(int width = CURVE_WIDGET_WIDTH, int height = CURVE_WIDGET_HEIGHT, int index = 0, QWidget *parent = 0);
    ~MyCurve();

    void setBorderShow(bool bFlag, int showBorderIndex); //! 设置是否显示边界线
    void setBorderValue(int left, int right);            //! 设置边界数
    void clearPixmap(void);                             //! 清空绘制重叠波形时保存的图像数据
    void setDrawType(int type);
    void setAnalyzeIndex(int analyze, int machine = 0);

public slots:
    void onUpdatePixmapSlt(void);                       //! 响应刷新图像的信号

private:
    QFont font;
    bool m_bMultiWave;                                  //! 标志是否显示重叠波形
    QPixmap m_pixmap;                                   //! 图像用于保存波形页面的显示

    MyAxis      *axisWidget;
    QHBoxLayout *axisPageHBLayout;
    QGridLayout *unitGridLayout;
    QSpacerItem *horizontalSpacer;
};


extern MyCurveThread *g_curveDataThread;

#endif // MYCURVE_H
