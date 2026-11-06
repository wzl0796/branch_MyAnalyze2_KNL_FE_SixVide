/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mycurve.cpp
 * \brief       波形显示界面源文件
 * \date        2015.01.14
 */
#include "mycurve.h"

MyCurveThread *g_curveDataThread;

MyCurveThread::MyCurveThread()
{
    m_nWaittime = 100;
}

MyCurveThread::~MyCurveThread()
{

}

/**
 * @brief MyCurveThread::根据包长确定等待时间
 */
void MyCurveThread::getLenAndWait()
{
    m_nWaittime = 100;
}

bool MyCurveThread::getRunningState()
{
    return bIsRunning;
}

void MyCurveThread::run()
{
    bIsRunning = false;

    QTimer *timer = new QTimer();

    connect(timer, SIGNAL(timeout()), this, SLOT(waveThreadRun()), Qt::DirectConnection);
    timer->start(300);

    exec();
}

void MyCurveThread::waveThreadRun()
{
    if (bIsRunning) {
        memset(struGsh.sRgbRow, 0, sizeof(struGsh.sRgbRow));

        gFlow.sendImgWaveReq(struGsh.nAnalyzeIndex, struGsh.nUnit, 0, 0, 0);

        msleep(100);

        emit sReceiveData();
    }
}

void MyCurveThread::startWave()
{
    bIsRunning = true;
}

void MyCurveThread::stopWave()
{
    bIsRunning = false;
}

/***************************************************
 *
 * 绘制坐标轴类
 *
 ****************************************************/
MyAxis::MyAxis(int index, QWidget *parent) :
        QWidget(parent)
{
    m_nDrawType = TYPE_WAVE;
    m_bDrawBoarder = false;
    m_bDrawMultiWave = false;
    m_nSignalType = 0;
	m_nShowBoarderIndex = 0;
    m_Index = index;
    setMinValueX(0);
    setMinValueY(0);
    setMaxValueY(255);
    setMaxValueX(2048);

    setTickAttr();
    font.setPixelSize(16);
    setBorder(50,10,10,50); // 设置坐标轴的左右上下边界
}

MyAxis::~MyAxis()
{
}

/* 根据分辨率来设置X坐标 */
void MyAxis::setXByData()
{
    int maxValue = 512;
    if (m_nDrawType == TYPE_WAVE) {
        if(!gFlow.bHighResolution(struGsh.nAnalyzeIndex, struGsh.nUnit)){
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[struGsh.nUnit] == SENSOR_IM01){
                maxValue =  SENSOR_PIXELS_320;
            }else{
                maxValue =  SENSOR_PIXELS_640;
            }
        }else{
            maxValue =  SENSOR_PIXELS_2048;
        }
    } else if (m_nDrawType == TYPE_CONTROL_DATA) {
        maxValue = 512;
    }

    setMaxValueX(maxValue);     // 设置x轴上的最大值
}

/* 设置x轴上的最小值 */
void MyAxis::setMinValueX(int value)
{
    minValueX = value;
}

/* 设置x轴上的最大值 */
void MyAxis::setMaxValueX(int value)
{
    maxValueX = value;
}

/* 设置y轴上的最小值 */
void MyAxis::setMinValueY(int value)
{
    minValueY = value;
}

/* 设置y轴上的最大值 */
void MyAxis::setMaxValueY(int value)
{
    maxValueY = value;
}

/* 设置坐标轴的边界 */
void MyAxis::setBorder(int left, int right, int up, int down)
{
    leftBorder = left;
    rightBorder = right;
    upBorder = up;
    downBorder = down;
}

/* 获取左边界 */
int MyAxis::getLeftBorder()
{
    return leftBorder;
}

/* 获取右边界 */
int MyAxis::getRightBorder()
{
    return rightBorder;
}

/* 获取上边界 */
int MyAxis::getUpBorder()
{
    return upBorder;
}

/* 获取下边界 */
int MyAxis::getDownBorder()
{
    return downBorder;
}

/* 设置标记属性 */
void MyAxis::setTickAttr()
{
    setTickValue();
    setTickNum();
}

/* 设置标记的数目 */
void MyAxis::setTickNum()
{
    majorXNum = (maxValueX-minValueX)/majorTickValueX;  // 长刻度数量
    minorXNum = (maxValueX-minValueX)/minorTickValueX;  // 短刻度数量
    majorYNum = (maxValueY-minValueY)/majorTickValueY;
    minorYNum = (maxValueY-minValueY)/minorTickValueY;
}

/* 计算记号上的值 */
void MyAxis::setTickValue()
{
    switch(maxValueX-minValueX) {   
    case 60:
        majorTickValueX = 5;    // 长刻度单位
        minorTickValueX = 1;    // 短刻度单位
        break;
    case 320:
    case 512:
    case 640:
        majorTickValueX = 50;   // 长刻度单位
        minorTickValueX = 10;   // 短刻度单位
        break;
    case 1024:
        majorTickValueX = 100;  // 长刻度单位
        minorTickValueX = 20;   // 短刻度单位
        break;
    case 2048:
        majorTickValueX = 200;  // 长刻度单位
        minorTickValueX = 40;   // 短刻度单位
        break;
    case 2592:
        majorTickValueX = 200;  // 长刻度单位
        minorTickValueX = 40;   // 短刻度单位
        break;
    default:
        majorTickValueX = 5;   // 长刻度单位
        minorTickValueX = 1;   // 短刻度单位
        break;
    }

    switch(maxValueY) {
    case 50:
        majorTickValueY = 10;
        minorTickValueY = 2;
        break;
    case 100:
        majorTickValueY = 20;
        minorTickValueY = 4;
        break;
    case 200:
        majorTickValueY = 40;
        minorTickValueY = 8;
        break;
    case 255:
        majorTickValueY = 50;
        minorTickValueY = 10;
        break;
    case 400:
        majorTickValueY = 80;
        minorTickValueY = 16;
        break;
    case 600:
        majorTickValueY = 120;
        minorTickValueY = 24;
        break;
    case 1000:
        majorTickValueY = 200;
        minorTickValueY = 40;
        break;
    default:
        majorTickValueY = 50;
        minorTickValueY = 10;
        break;
    }
}

/* 更新波形信号数据 */
void MyAxis::getNewdata()
{   
    if(!gFlow.bHighResolution(struGsh.nAnalyzeIndex,struGsh.nUnit)){ // N R G B
        int npiex = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[struGsh.nUnit] == SENSOR_IM01?
            SENSOR_PIXELS_320 : SENSOR_PIXELS_640;

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[struGsh.nUnit] == 0){//! 6波段
            for(int i = 0; i < npiex; i++){
                struGsh.sRowRed[i] = struGsh.sRgbRow[i*8+2];
                struGsh.sRowGreen[i] = struGsh.sRgbRow[i*8+3];
                struGsh.sRowBlue[i] = struGsh.sRgbRow[i*8+4];
                struGsh.sRowNInf[i] = struGsh.sRgbRow[i*8+5];
                struGsh.sRowNInf1[i] = struGsh.sRgbRow[i*8+6];
                struGsh.sRowNInf2[i] = struGsh.sRgbRow[i*8+7];
            }
        }else{//! 12波段
            for(int i = 0; i < npiex; i++){
                struGsh.sRowRed[i] = struGsh.sRgbRow[i*14+2];
                struGsh.sRowGreen[i] = struGsh.sRgbRow[i*14+3];
                struGsh.sRowBlue[i] = struGsh.sRgbRow[i*14+4];
                struGsh.sRowNInf[i] = struGsh.sRgbRow[i*14+5];
                struGsh.sRowNInf1[i] = struGsh.sRgbRow[i*14+6];
                struGsh.sRowNInf2[i] = struGsh.sRgbRow[i*14+7];
                struGsh.sRowNInf3[i] = struGsh.sRgbRow[i*14+8];
                struGsh.sRowNInf4[i] = struGsh.sRgbRow[i*14+9];
                struGsh.sRowNInf5[i] = struGsh.sRgbRow[i*14+10];
                struGsh.sRowNInf6[i] = struGsh.sRgbRow[i*14+11];
                struGsh.sRowNInf7[i] = struGsh.sRgbRow[i*14+12];
                struGsh.sRowNInf8[i] = struGsh.sRgbRow[i*14+13];
            }
        }
    } else {// 0 物料点 N R G B
        for(int i = 0; i < SENSOR_PIXELS_2048; i++){
            struGsh.sRowNInf[i] = struGsh.sRgbRow[i*6+2];
            struGsh.sRowRed[i] = struGsh.sRgbRow[i*6+3];
            struGsh.sRowGreen[i] = struGsh.sRgbRow[i*6+4];
            struGsh.sRowBlue[i] = struGsh.sRgbRow[i*6+5];
        }
    }
}


/* 设置绘制的类型 */
void MyAxis::setDrawType(int type)
{
    m_nDrawType = type;
}

/* 绘制吹气次数 */
void MyAxis::drawEjectTimes(QPainter &painter)
{
//	if (struGsh.bIsAssist) {
//    	drawSingleTimes(painter, ASSIST_FRONT_VIEW);
//    	drawSingleTimes(painter, ASSIST_REAR_VIEW);
//	} else {
//    	drawSingleTimes(painter, FRONT_VIEW);
//    	drawSingleTimes(painter, REAR_VIEW);
//	}
//    memset(struGsh.pEjectTimes, 0, sizeof(struGsh.pEjectTimes));
}

/* 绘制单视的吹气次数波形 */
void MyAxis::drawSingleTimes(QPainter &painter, int flag)
{
    int data[2048] = {0};
    int *pData = data;
    QPen pen;
    pen.setWidthF(2);
    switch(flag){
    case FRONT_VIEW:
//        pData = struGsh.pEjectTimes;
        pen.setColor(Qt::red);
        break;
    case REAR_VIEW:
//        pData = struGsh.pEjectTimes+MAX_EJECTOR_SIXTY;
        pen.setColor(Qt::blue);
        break;
	case ASSIST_FRONT_VIEW:
//        pData = struGsh.pEjectTimes+MAX_EJECTOR_SIXTY*2;
        pen.setColor(Qt::red);
        break;
    case ASSIST_REAR_VIEW:
//        pData = struGsh.pEjectTimes+MAX_EJECTOR_SIXTY*3;
        pen.setColor(Qt::blue);
        break;
    default:
        break;
    }

    bool isFirstpoint = true;
    QRect rect = this->rect();
    rect.adjust(leftBorder,upBorder,-rightBorder,-downBorder);
    painter.setClipRect(rect);
    painter.setPen(pen);

    QPointF currentPoint, lastPoint;
    int tmp = maxValueX-minValueX+1;
    for (int i = 1; i < tmp; i++) {
        if (isFirstpoint) {
            lastPoint.setX(leftBorder+i/valueXPerPixel);
            lastPoint.setY(rect.height()+upBorder-pData[i-1]/valueYPerPixel);
            isFirstpoint = false;
        } else {
            currentPoint.setX(leftBorder+i/valueXPerPixel);
            currentPoint.setY(rect.height()+upBorder-pData[i-1]/valueYPerPixel);
            painter.drawLine(lastPoint,currentPoint);
            lastPoint = currentPoint;
        }
    }
}

/* 设置边界是否显示 */
void MyAxis::setBorderShow(bool bFlag, int showBoarderIndex)
{
    m_bDrawBoarder = bFlag;
	m_nShowBoarderIndex = showBoarderIndex;
}

/* 获取边界是否显示 */
bool MyAxis::getBorderShow()
{
    return m_bDrawBoarder;
}

/* 设置是否绘制重叠波形标志位 */
void MyAxis::setMultiWaveFlag(bool bFlag)
{
    m_bDrawMultiWave = bFlag;
    m_pixmap = QPixmap();
}

/**
 * @brief MyAxis::setBorderValue
 * @param left
 * @param right
 */
void MyAxis::setBorderValue(int left, int right)
{
    m_nLeft = left;
    m_nRight = right;
}

/* 设置需要绘制的图像 */
void MyAxis::setPixmap(QPixmap pixmap)
{
    m_pixmap = pixmap;
}

void MyAxis::setSignalType(int type)
{
    m_nSignalType = type;
}

/**
 * @brief MyAxis::setAnalyzeIndex
 * @param analyze
 * @param machine
 */
void MyAxis::setAnalyzeIndex(int analyze, int machine)
{
    m_analyzeIndex = analyze;
    m_machineIndex = machine;
}

/* 覆写绘制事件　*/
void MyAxis::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(font);

    /* 若绘制重叠波形则无需重绘坐标轴 */
    paintAxis(painter);
    if (m_bDrawMultiWave) {
        painter.drawPixmap(leftBorder,
                           upBorder,
                           width()-leftBorder-rightBorder,
                           height()-upBorder-downBorder,
                           m_pixmap);
    }

    drawWanted(painter);
}

/* 绘制整体的坐标轴 */
void MyAxis::paintAxis(QPainter &painter)
{
    if(width()<leftBorder+rightBorder || height()<upBorder+downBorder)
        return;

    /* 根据绘制类型刷新坐标轴参数 */
    if(m_nDrawType == TYPE_WAVE) {
        setXByData();
    } else if (m_nDrawType == TYPE_EJECT_TIMES) {
        setYByData();
    } else if (m_nDrawType == TYPE_CONTROL_DATA){
        setXByData();
        setYByData();
    }
    //设置x和y轴上大刻度和小刻度表示的值
    setTickAttr();

    paintAxisX(painter);
    paintAxisY(painter);
}

/* 绘制坐标轴x */
void MyAxis::paintAxisX(QPainter &painter)
{
    int dispVal= 0;
    qreal majorTickX, minorTickX;
    QPen pen_axis(Qt::black, 2);
    QPen pen_gridline(Qt::black, 1, Qt::DotLine);
    QPen pen_majortick(Qt::black, 2);
    QPen pen_minortick(Qt::black, 1);
    QPen pen_border(QColor(180, 0, 180));
    pen_border.setWidth(1);

    valueXPerPixel = ((qreal)(maxValueX-minValueX)/(qreal)(width()-leftBorder-rightBorder));
    QPointF leftDownPoint(leftBorder, height()-downBorder);
    QPointF rightDownPoint(width()-rightBorder, height()-downBorder);
    /* 记录下tick点的坐标 */
    QPointF tmp(leftDownPoint);
//    int pixelSize = painter.fontInfo().pixelSize();
    QFontMetrics fm = painter.fontMetrics();
    int pixelSize = fm.height();
    qreal halfTextLength = ((((qreal)width()-leftBorder-rightBorder))/(majorXNum-1))/3.0;
    QRect textRect;

    painter.setPen(pen_axis);
    painter.drawLine(leftDownPoint,rightDownPoint);

    /* 1.绘制主标记及纵轴虚线 */
    for (int i = 1; i <= majorXNum; i++) {
        painter.setPen(pen_majortick);
        majorTickX = (qreal)(i*majorTickValueX/valueXPerPixel);
        tmp.setX(leftBorder+majorTickX);
        /* 绘制tick */
        painter.drawLine(tmp,tmp+QPointF(0,8));
        textRect.setTopLeft(QPoint(tmp.x()-halfTextLength,tmp.y()+10));
        textRect.setSize(QSize(2*halfTextLength,pixelSize));
        /* 绘制X轴tick上的值 */
        dispVal = i*majorTickValueX;
//        if(m_nDrawType == TYPE_EJECT_TIMES){
//            dispVal = i*majorTickValueX;
//        }else{
//            dispVal = myFlow.getChannelScaleValue(i*majorTickValueX, struGsh.nUnit%2, struGsh.bIsAssist);
//        }
        painter.drawText(textRect, QString("%1")
                         .arg(minValueX+dispVal),QTextOption(Qt::AlignHCenter));
        painter.setPen(pen_gridline);
        /* 绘制坐标轴中的网格 */
        painter.drawLine(tmp,QPointF(tmp.x(),upBorder));
    }

    /* 2.绘制辅标记 */
    painter.setPen(pen_minortick);
    for (int i = 1; i <= minorXNum; i++) {
        minorTickX = (qreal)(i*minorTickValueX/valueXPerPixel);
        tmp.setX(leftBorder+minorTickX);
        painter.drawLine(tmp,tmp+QPointF(0,5));
    }

    //1024分辨率时，在512位置画线；
    //同理2048分辨率时，在1024位置画线，方便校正条调试相机
    if(TYPE_WAVE == m_nDrawType){
        QPen pen_midLine(Qt::magenta, 1.5);
        painter.setPen(pen_midLine);
        /* 绘制坐标轴中的网格 */

        tmp.setX(leftBorder+(maxValueX-minValueX)/2/valueXPerPixel);
        painter.drawLine(tmp,QPointF(tmp.x(),upBorder));

        //2048分辨率在424、1624位置画线
        if(SENSOR_PIXELS_2048 == maxValueX || SENSOR_PIXELS_320 == maxValueX || SENSOR_PIXELS_640 == maxValueX){
//            int mid = (maxValueX-minValueX)/2;
//            int left = mid - 600;
//            int right = mid + 600;

            struCamera *curCaParams = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;
            int left = curCaParams->nPixelBegin[struGsh.nUnit];
            int right = curCaParams->nPixelEnd[struGsh.nUnit];

            QPen pen_leftLine(/*Qt::darkCyan*/QColor(74, 191, 195), 2);
            painter.setPen(pen_leftLine);
            tmp.setX(leftBorder+left/valueXPerPixel);
            painter.drawLine(tmp,QPointF(tmp.x(),upBorder));

            tmp.setX(leftBorder+right/valueXPerPixel);
            painter.drawLine(tmp,QPointF(tmp.x(),upBorder));
        }
    }

    /* 3.根据需要绘制左右边界线 */
    if(m_bDrawBoarder) {
//        painter.setPen(pen_border);
//        /* 左边界线 */
//        qreal fLeft;
//        if (struGsh.bIsAssist) {
//            fLeft = (qreal)(struCnfc.struLevelCameraAssist[struGsh.nLevel].nChannelBegin[struGsh.nUnit]);
//        } else {
//            if(m_nShowBoarderIndex ==1 || m_nShowBoarderIndex ==3) {
//                fLeft = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nMultiHalfChannelBegin[struGsh.nUnit][0]);
//            }else if(m_nShowBoarderIndex == 2){
//                fLeft = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nMultiHalfChannelBegin[struGsh.nUnit][1]);
//            }else {
//                fLeft = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nChannelBegin[struGsh.nUnit]);
//            }
//        }

//        fLeft = myFlow.getChannelScaleValue(fLeft, struGsh.nUnit%2, struGsh.bIsAssist, true);
//        fLeft = (qreal)fLeft/valueXPerPixel;

//        tmp.setX(leftBorder+fLeft);
//        painter.drawLine(tmp, QPointF(tmp.x(), upBorder));

//        /* 右边界线 */
//        qreal fRight;
//        if (struGsh.bIsAssist) {
//            fRight = (qreal)(struCnfc.struLevelCameraAssist[struGsh.nLevel].nChannelEnd[struGsh.nUnit]);
//        } else {
//            if(m_nShowBoarderIndex ==1) {
//                fRight = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nMultiHalfChannelEnd[struGsh.nUnit][0]);
//            }else if(m_nShowBoarderIndex ==2 || m_nShowBoarderIndex ==3) {
//                fRight = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nMultiHalfChannelEnd[struGsh.nUnit][1]);
//            }else{
//                fRight = (qreal)(struCnfc.struLevelCamera[struGsh.nLevel].nChannelEnd[struGsh.nUnit]);
//            }
//        }

//        fRight = myFlow.getChannelScaleValue(fRight, struGsh.nUnit%2, struGsh.bIsAssist, true);
//        fRight = (qreal)fRight/valueXPerPixel;

//        tmp.setX(leftBorder+fRight);
//        painter.drawLine(tmp, QPointF(tmp.x(), upBorder));
    }
}

/* 绘制坐标轴y */
void MyAxis::paintAxisY(QPainter &painter)
{
    qreal majorTickY, minorTickY;
    QPen pen_axis(Qt::black, 2);
    QPen pen_gridline(Qt::black, 1, Qt::DotLine);
    QPen pen_majortick(Qt::black, 2);
    QPen pen_minortick(Qt::black, 1);
    QPen pen_gridMainLine(Qt::black, 1, Qt::DashDotLine);

    valueYPerPixel = ((qreal)(maxValueY-minValueY)/(qreal)(height()-upBorder-downBorder));
    QPointF leftDownPoint(leftBorder,height()-downBorder);
    QPointF leftUpPoint(leftBorder,upBorder);
    QPointF tmp(leftDownPoint);
    QRect textRect;
    qreal halfTextHeight = ((((qreal)height()-upBorder-downBorder))/(majorYNum-1))/3.0;

    painter.setPen(pen_axis);
    painter.drawLine(leftDownPoint,leftUpPoint);

    /* 绘制主标记及横轴虚线 */
    for (int i = 1; i <= majorYNum; i++) {
        painter.setPen(pen_majortick);
        majorTickY = (qreal)(i*majorTickValueY/valueYPerPixel);
        tmp.setY(height()-downBorder-majorTickY);
        painter.drawLine(tmp, tmp+QPointF(-8,0));

        textRect.setTopLeft(QPoint(tmp.x()-40,tmp.y()-halfTextHeight));
        textRect.setSize(QSize(35,2*halfTextHeight));

        painter.drawText(textRect,QString("%1")
                         .arg(minValueY+i*majorTickValueY),QTextOption(Qt::AlignCenter));
//        painter.setPen(pen_gridline);
        painter.setPen(/*pen_gridline*/pen_gridMainLine);
        painter.drawLine(tmp, QPointF(width()-rightBorder,tmp.y()));
    }

    /* 绘制辅标记 */
    painter.setPen(pen_minortick);
    for (int i = 1; i <= minorYNum; i++) {
        minorTickY = (qreal)(i*minorTickValueY/valueYPerPixel);
        tmp.setY(height()-downBorder-minorTickY);
        painter.drawLine(tmp,tmp+QPointF(-5,0));
        painter.setPen(pen_gridline);
        painter.drawLine(tmp, QPointF(width()-rightBorder, tmp.y()));
    }
}

/* 根据数据设置纵轴坐标 */
void MyAxis::setYByData()
{
    int maxValue = 255;
//    for (int i = 0; i < 500; i++) {
//        if(struGsh.pEjectTimes[i] > maxValue) {
//            maxValue = struGsh.pEjectTimes[i];
//        }
//    }
    if (m_Index == 1 || m_Index == 4) {
        maxValue = 190;
    } else if (m_Index == 2) {
        maxValue = 20;
    } else if (m_Index == 3) {
        maxValue = 50;
    }
    if(maxValue <= 10) {
        setMaxValueY(10);
    } else if (maxValue > 10 && maxValue <= 50) {
        setMaxValueY(50);
    } else if (maxValue > 50 && maxValue < 100) {
        setMaxValueY(100);
    } else if (maxValue >= 100 && maxValue < 200) {
        setMaxValueY(200);
    } else if (maxValue >= 200 && maxValue < 400) {
        setMaxValueY(400);
    } else if (maxValue >= 400 && maxValue < 600) {
        setMaxValueY(600);
    } else {
        setMaxValueY(1000);
    }
}

/* 绘制想要的波形 */
void MyAxis::drawWanted(QPainter &painter)
{
    switch (m_nDrawType) {
    case TYPE_WAVE:
        getNewdata();
        drawColorWave(painter);
        break;
    case TYPE_EJECT_TIMES:
        drawEjectTimes(painter);
        break;
    default:
        break;
    }
}

/* 绘制彩色波形 */
void MyAxis::drawColorWave(QPainter &painter)
{
    bool bIM01 = gFlow.bIsIMSerialCam(struGsh.nUnit);

    if(!bIM01){
        drawSingleWave(painter, WAVE_BLUE);
        drawSingleWave(painter, WAVE_GREEN);
        drawSingleWave(painter, WAVE_RED);
        drawSingleWave(painter, WAVE_INFRARED);
    }else {
        if( 0 == struGsh.nWaveIndex){//! 绘制所有波形
            drawSingleWave(painter, WAVE_BLUE);
            drawSingleWave(painter, WAVE_GREEN);
            drawSingleWave(painter, WAVE_RED);
            drawSingleWave(painter, WAVE_INFRARED);


            drawSingleWave(painter, WAVE_INFRARED1);
            drawSingleWave(painter, WAVE_INFRARED2);

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[struGsh.nUnit] == 1){
                drawSingleWave(painter, WAVE_INFRARED3);
                drawSingleWave(painter, WAVE_INFRARED4);
                drawSingleWave(painter, WAVE_INFRARED5);
                drawSingleWave(painter, WAVE_INFRARED6);
                drawSingleWave(painter, WAVE_INFRARED7);
                drawSingleWave(painter, WAVE_INFRARED8);
            }
        }else{//! 绘制单条波形
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[struGsh.nUnit] == 0){
                if(struGsh.nWaveIndex>6){
                    return;
                }
            }
            drawSingleWave(painter, struGsh.nWaveIndex-1+WAVE_RED);
        }
    }
}

/**
 * @brief MyAxis::砻谷机控制数据
 * @param painter
 */
void MyAxis::drawMachineWave(QPainter &painter)
{
    drawMachineData(painter, WAVE_RED);
    drawMachineData(painter, WAVE_BLUE);
}

/* 根据标志位显示单一的波形 */
void MyAxis::drawSingleWave(QPainter &painter, int flag)
{
    uchar *pData;
    int quo = 1;

    QPen pen;
    pen.setWidthF(1);
    switch(flag){
    case WAVE_RED:
        pData = struGsh.sRowRed;
        pen.setColor(Qt::red);
        break;
    case WAVE_GREEN:
        pData = struGsh.sRowGreen;
        pen.setColor(Qt::green);
        break;
    case WAVE_BLUE:
        pData = struGsh.sRowBlue;
        pen.setColor(Qt::blue);
        break;
    case WAVE_INFRARED:
        pData = struGsh.sRowNInf;
        pen.setColor(Qt::yellow);
        break;
    case WAVE_INFRARED1:
        pData = struGsh.sRowNInf1;
        pen.setColor(Qt::darkCyan);
        break;
    case WAVE_INFRARED2:
        pData = struGsh.sRowNInf2;
        pen.setColor(Qt::darkGray);
        break;
    case WAVE_INFRARED3:
        pData = struGsh.sRowNInf3;
        pen.setColor(Qt::darkBlue);
        break;
    case WAVE_INFRARED4:
        pData = struGsh.sRowNInf4;
        pen.setColor(Qt::darkMagenta);
        break;
    case WAVE_INFRARED5:
        pData = struGsh.sRowNInf5;
        pen.setColor(Qt::darkYellow);
        break;
    case WAVE_INFRARED6:
        pData = struGsh.sRowNInf6;
        pen.setColor(Qt::magenta);
        break;
    case WAVE_INFRARED7:
        pData = struGsh.sRowNInf7;
        pen.setColor(Qt::lightGray);
        break;
    case WAVE_INFRARED8:
        pData = struGsh.sRowNInf8;
        pen.setColor(Qt::color1);
        break;
    default:
        break;
    }

    bool isFirstpoint = true;
    QRect rect = this->rect();
    rect.adjust(leftBorder,upBorder,-rightBorder,-downBorder);
    painter.setClipRect(rect);
    painter.setPen(pen);

    QPointF currentPoint, lastPoint;
    for (int i = 0; i < maxValueX; i++) {
        if (isFirstpoint) {
            lastPoint.setX(leftBorder+i/valueXPerPixel);
            lastPoint.setY(rect.height()+upBorder-pData[i/quo]/valueYPerPixel);
            isFirstpoint = false;
        } else {
            currentPoint.setX(leftBorder+i/valueXPerPixel);
            currentPoint.setY(rect.height()+upBorder-pData[i/quo]/valueYPerPixel);
            painter.drawLine(lastPoint,currentPoint);
            lastPoint = currentPoint;
        }
    }
}

/**
 * @brief MyAxis::drawMachineData
 * @param painter
 * @param flag
 */
void MyAxis::drawMachineData(QPainter &painter, int flag)
{
    int *pData;
    int quo = 1;

    QPen pen;
    pen.setWidthF(2);
    switch(flag){
    case WAVE_RED:
        pData = sCaliRed;
        pen.setColor(Qt::red);
        break;
    case WAVE_BLUE:
        pData = sCaliBlue;
        pen.setColor(Qt::blue);
        break;
    default:
        break;
    }

    bool isFirstpoint = true;
    QRect rect = this->rect();
    rect.adjust(leftBorder,upBorder,-rightBorder,-downBorder);
    painter.setClipRect(rect);
    painter.setPen(pen);

    QPointF currentPoint, lastPoint;
    for (int i = 0; i < maxValueX; i++) {
        if (isFirstpoint) {
            lastPoint.setX(leftBorder+i/valueXPerPixel);
            lastPoint.setY(rect.height()+upBorder-pData[i/quo]/valueYPerPixel);
            isFirstpoint = false;
        } else {
            currentPoint.setX(leftBorder+i/valueXPerPixel);
            currentPoint.setY(rect.height()+upBorder-pData[i/quo]/valueYPerPixel);
            painter.drawLine(lastPoint,currentPoint);
            lastPoint = currentPoint;
        }
    }
}

/*********
 *
 * 绘制波形类
 *
 ***************************************************************/
MyCurve::MyCurve(int width, int height, int index, QWidget *parent) :
    QWidget(parent)
{
    m_bMultiWave = false;

    /*   画坐标轴   */
    axisWidget = new MyAxis(index);
    axisWidget->setFixedSize(width, height);

    axisPageHBLayout = new QHBoxLayout;
    axisPageHBLayout->addWidget(axisWidget);
    //axisPageHBLayout->setContentsMargins(0, 0, 5, 15);
    setLayout(axisPageHBLayout);
}

MyCurve::~MyCurve()
{
}

void MyCurve::setAnalyzeIndex(int analyze, int machine)
{
    axisWidget->setAnalyzeIndex(analyze, machine);
}

/* 响应刷新图像的信号 */
void MyCurve::onUpdatePixmapSlt()
{
    axisWidget->update();

    if(!m_bMultiWave) {
        return;
    }

    int nLeft = axisWidget->getLeftBorder();
    int nRight = axisWidget->getRightBorder();
    int nUp = axisWidget->getUpBorder();
    int nDown = axisWidget->getDownBorder();

    m_pixmap = QPixmap::grabWidget(axisWidget, nLeft, nUp, axisWidget->width()-nLeft-nRight, axisWidget->height()-nUp-nDown);
    axisWidget->setPixmap(m_pixmap);
}

/* 设置是否显示边界线 */
void MyCurve::setBorderShow(bool bFlag, int showBorderIndex)
{
    axisWidget->setBorderShow(bFlag, showBorderIndex);
}

void MyCurve::setBorderValue(int left, int right)
{
    axisWidget->setBorderValue(left, right);
}

/* 清空绘制重叠波形时保存的图像数据 */
void MyCurve::clearPixmap()
{
    axisWidget->setPixmap(QPixmap());
    axisWidget->update();
}

void MyCurve::setDrawType(int type)
{
    axisWidget->setDrawType(type);
}
