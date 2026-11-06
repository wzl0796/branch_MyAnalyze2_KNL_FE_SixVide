#include "mybargraph.h"

myBarGraph::myBarGraph(QWidget *parent)
    : QWidget(parent)
{
    m_maxValue = 5.0;
    m_sTitle = "";
}

void myBarGraph::setValue(QList<double> targetMinValue, QList<double> targetMaxValue, QList<double> realValue, QStringList strList)
{
    m_fTargetMinValue = targetMinValue;
    m_fTargetMaxValue = targetMaxValue;
    m_fRealValue = realValue;
    m_sType = strList;
}

void myBarGraph::paintEvent(QPaintEvent *ev)
{
    int channelNum = m_fTargetMaxValue.size();
    if(channelNum <= 0 || m_fRealValue.size() != m_fTargetMaxValue.size() || m_fRealValue.size() != m_sType.size())
        return;

    QPixmap pixmap(size());
    QPainter painter(&pixmap);

    QPen pen(Qt::black, 1);
    QPen pen2 = QPen(Qt::transparent, 1);
    painter.setPen(pen2);

    //绘制背景颜色
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height()));
    linearGrad.setColorAt(0, Qt::white);
    linearGrad.setColorAt(1, Qt::white);

    painter.setBrush(QBrush(linearGrad));
    QRect paintRect = ev->rect();
    painter.drawRect(paintRect);

    // 柱状矩形上方数据字体'
    QFont chUpFont = QFont("Microsoft YaHei", 20, 1);
    chUpFont.setPixelSize(20);

    // 柱状矩形下方数据字体
    QFont chDownFont = QFont("Microsoft YaHei", 23, 1);
    chDownFont.setPixelSize(22);

    int unitWidth = width()/channelNum;
    int titleHeight = 100;      //标题高度
    int channelOutWidth = 50;   //柱状矩形宽度
    int channelOutHeight = 30;  //柱状矩形上方数据高度
    int channelNoHeight = 50;   //柱状矩形下方数据高度
    int unitHeight =  height() - titleHeight - channelOutHeight - channelNoHeight - 30;

    for(int index = 0; index < channelNum; ++index) {

        painter.setPen(pen);
        int nCurRectX = (unitWidth-channelOutWidth*2)/2+index*unitWidth;                // 矩形X坐标
        int nCurRectY = unitHeight+(titleHeight+channelOutHeight);       // 矩形Y坐标
        int nRealHeight = 0.8*unitHeight*m_fRealValue.at(index)/m_maxValue;
        int nTargetMaxHeight = 0.8*unitHeight*m_fTargetMaxValue.at(index)/m_maxValue;
        int nTargetMinHeight = 0.8*unitHeight*m_fTargetMinValue.at(index)/m_maxValue;

        QRect rectReal, rectMinTarget, rectMaxTarget;
        QRect rectRealUp, rectTargetUp;

        // 目标值矩形
        painter.setBrush(QBrush(g_GraphGreen));
        rectMaxTarget = QRect(nCurRectX, nCurRectY-nTargetMaxHeight, channelOutWidth, nTargetMaxHeight>1?nTargetMaxHeight:1);
        painter.drawRect(rectMaxTarget);

        painter.setBrush(QBrush(QColor(255,255,255)));
        rectMinTarget = QRect(nCurRectX, nCurRectY-nTargetMinHeight, channelOutWidth, nTargetMinHeight>1?nTargetMinHeight:1);
        painter.drawRect(rectMinTarget);

        // 实际值矩形
        painter.setBrush(QBrush(g_GraphYellow));
        rectReal = QRect(nCurRectX+channelOutWidth+5, nCurRectY-nRealHeight, channelOutWidth, nRealHeight>1?nRealHeight:1);
        painter.drawRect(rectReal);

        // 实际值数据
        painter.setFont(chUpFont);
        rectRealUp = QRect(rectReal.x(), rectReal.y()-channelOutHeight, channelOutWidth, channelOutHeight);
        QString realValue = QString::number(m_fRealValue.at(index), 'f', 2);
        painter.drawText(rectRealUp, Qt::AlignCenter, QString("%1").arg(realValue));

        // 目标值数据
        painter.setFont(chUpFont);
        rectTargetUp = QRect(rectMaxTarget.x()-30, rectMaxTarget.y()-channelOutHeight, channelOutWidth+80, channelOutHeight);
        QString targetMinValue = QString::number(m_fTargetMinValue.at(index), 'f', 2);
        QString targetMaxValue = QString::number(m_fTargetMaxValue.at(index), 'f', 2);
        painter.drawText(rectTargetUp, Qt::AlignCenter, QString("%1-%2").arg(targetMinValue).arg(targetMaxValue));

        // 柱状矩形下方数据
        QRect rectDown(index*unitWidth, unitHeight+titleHeight+channelOutHeight, unitWidth, channelNoHeight);
        painter.setFont(chDownFont);
        painter.drawText(rectDown, Qt::AlignCenter, QString("%1").arg(m_sType.at(index)));
    }

    // title
    QFont chTitleFont = QFont("Microsoft YaHei", 30, 1);
//    chTitleFont.setPixelSize(30);
    QRect titleTxtRect(10, 10, 250, titleHeight);
    chTitleFont.setPixelSize(28);
    painter.setFont(chTitleFont);
    painter.drawText(titleTxtRect, Qt::AlignHCenter | Qt::AlignTop, QString("%1(%)").arg("主要含杂数据"));

//    QRect schmRect(0, 10, 400, titleHeight);
//    painter.setFont(chTitleFont);
//    painter.drawText(schmRect, Qt::AlignHCenter | Qt::AlignTop, m_sTitle);

    // 最下方标注目标数据
    QRect rectLabel1, rectLabel2;
    painter.setBrush(QBrush(g_GraphGreen));
    rectLabel1 = QRect(width()/3+30, height()-25, channelOutWidth, 20);
    painter.drawRect(rectLabel1);

    // 最下方标注实际数据
    painter.setBrush(QBrush(g_GraphYellow));
    rectLabel2 = QRect(width()*2/3-channelOutWidth*2+20/*-20*/, height()-25, channelOutWidth, 20);
    painter.drawRect(rectLabel2);

    QRect rect1, rect2;
    painter.setFont(chUpFont);
    rect1 = QRect(rectLabel1.x()+channelOutWidth+5, rectLabel1.y()-5, channelOutWidth+80, 30);
    painter.drawText(rect1, /*Qt::AlignCenter*/Qt::AlignLeft|Qt::AlignVCenter, "目标值");

    rect2 = QRect(rectLabel2.x()+channelOutWidth+5, rectLabel2.y()-5, channelOutWidth+80, 30);
    painter.drawText(rect2, /*Qt::AlignCenter*/Qt::AlignLeft|Qt::AlignVCenter, "实际值");

    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
}

void myBarGraph::setMaxValue(double maxValue)
{
    m_maxValue = maxValue;
}

void myBarGraph::setTitle(QString title)
{
    m_sTitle = title;
}
