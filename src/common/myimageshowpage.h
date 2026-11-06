#ifndef MYIMAGESHOWPAGE_H
#define MYIMAGESHOWPAGE_H

#include <QDialog>
#include <QMouseEvent>
#include <QtCore>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QSlider>
#include "params/define.h"

class myViewLable : public QLabel
{
    Q_OBJECT

public:
    explicit myViewLable();

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void showPos(int x, int y);
};

class myImageShowPage : public QDialog
{
    Q_OBJECT
public:
    explicit myImageShowPage(QWidget *parent = 0);

    void initParams(void);

    void createPage(void);

    void connectSigAndSlt(void);

    void refreshPage(int index);

    void refreshImgDisplay(void);

    QPoint getTransferScrollValue(int x, int y, bool isEnlarge = true);

signals:

public slots:
    void onBackBtnPressedSlt(void);

    void showMousePos(int x, int y);

    void horizonSlideMoved(int nDisX);

    void verticalSlideModed(int nDisY);

    void onEnlargeBtnClicked(void);

    void onReduceBtnClicked(void);

    void horizonSlideRangeChanged(int min, int max);

    void verticalSlideRangeChanged(int min, int max);
private:
    QWidget             *m_widget;
    QScrollArea         *m_simImgScrollArea;
    myViewLable         *imgLbl;
    QLabel              *RGBNLbl;
    QPushButton         *backBtn;
    QPushButton         *enLargeBtn;
    QPushButton         *reduceBtn;

private:
    int    nScale;             // 当前图片的显示尺寸
    int    nHSliderDis;        // 水平滚动条移动距离
    int    nVSliderDis;        // 垂直滚动条移动距离
    int    m_nImgHeight;
    int    m_nImgWidth;

    QPoint lastScrollBias;
};

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsScene *scene = 0, QWidget *parent = 0, qreal factor = 1);

    bool viewportEvent(QEvent *event) Q_DECL_OVERRIDE;

signals:
    void sigShowRGB(int, int);

private:
    qreal totalScaleFactor;
};


class myView : public QDialog
{
    Q_OBJECT

public:
    myView(QWidget *parent = 0, bool bSimulate = true);

    ~myView();

    void setViewData(int index);

    void setViewData(QString picName);

    void setTabIndex(int);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:

public slots:
    void sltShowRGB(int x, int y);
    void onSliderValueChangedSlt(int value);

    void onReduceBtnSlt();
    void onLargeBtnSlt();

private:
    QWidget             *m_widget;
    QWidget             *upwidget, *downwidget;
    QGraphicsScene      *scene;
    GraphicsView        *view;
    QGraphicsPixmapItem *pixmapItem;
    QLabel              *m_RGBLbl, *m_angleLbl[3];
    QLabel              *m_lenAreaLbl;

    QPushButton    *m_reduceBtn;    //缩小
    QPushButton    *m_largeBtn;     //放大

    QSlider        *m_slider;

    uchar    *pDataDisplay;
    uchar    *pBigDataDisplay;
//    uchar    *pInfraDisplay;
//    uchar    *pSimDisplay;
//    uchar    *pHSVDisplay;
    int      nCurRiceArea;
    double   fLength;
    double   fWHRate;

    int  m_width;
    int  m_height;

    bool m_bSimulate;

    float m_totalScaleFactor;
};

#endif // MYIMAGESHOWPAGE_H
