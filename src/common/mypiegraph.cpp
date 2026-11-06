#include "mypiegraph.h"

myPieGraph::myPieGraph(QWidget *parent)
    : QWidget(parent)
{
    m_sTitle = "";
    m_nMatType = MATERIAL_RICE;
}

void myPieGraph::setMatType(int nMatType)
{
    m_nMatType = nMatType;
}

void myPieGraph::setValue(double fValue1, double fValue2, double fValue3)
{
    m_fValue1 = fValue1;
    m_fValue2 = fValue2;
    m_fValue3 = fValue3;
}

void myPieGraph::resetTypeName()
{
    strType.clear();
    switch (m_nMatType) {
    case MATERIAL_RICE:
//        if(gs_consoleGlobal.nLang == LANG_CHS || gs_consoleGlobal.nLang == LANG_CHT)
        if(1)
            strType << "成品" << "杂质" << "整米" << "碎米";
        else
            strType << "成品" << "杂质" << "整" << "碎";
        break;
    case MATERIAL_BROWN:
    case MATERIAL_PADDY:
//        if(gs_consoleGlobal.nLang == LANG_CHS || gs_consoleGlobal.nLang == LANG_CHT)
        if(1)
            strType << "糙米" << "稻谷" << "整糙米" << "碎糙米";
        else
            strType << "糙米" << "稻谷" << "整糙米" << "碎糙米";
        break;
    default:
//        if(gs_consoleGlobal.nLang == LANG_CHS || gs_consoleGlobal.nLang == LANG_CHT)
        if(1)
            strType << "成品" << "杂质" << "整米" << "碎米";
        else
            strType << "成品" << "杂质" << "整" << "碎";
        break;
    }
}

void myPieGraph::paintEvent(QPaintEvent *ev)
{
    QPixmap pixmap(size());
    QPainter painter(&pixmap);

    QPen pen(Qt::black, 1);
    QPen pen2 = QPen(Qt::transparent, 1);
    QPen penGreen  = QPen(QBrush(g_GraphGreen), 1);
    QPen penYellow = QPen(QBrush(g_GraphYellow), 1);
    painter.setPen(pen2);

    //绘制背景颜色
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height()));
    linearGrad.setColorAt(0, Qt::white);
    linearGrad.setColorAt(1, Qt::white);

    painter.setBrush(QBrush(linearGrad));
    QRect paintRect = ev->rect();
    painter.drawRect(paintRect);

    // 上方数据字体
    QFont chUpFont = QFont("Microsoft YaHei", 20, 1);
    chUpFont.setPixelSize(20);

    // 下方数据字体
    QFont chDownFont = QFont("Microsoft YaHei", 23, 1);
    chDownFont.setPixelSize(23);

    int titleHeight = 100;      //标题高度
    int channelOutWidth = 50;   //柱状矩形宽度
    int pieWidth = 220;         //饼状宽度/高度
    double fRatio = 0.7;

    // 重置分类名称
    resetTypeName();

    QRect pieRect1 = QRect(40, titleHeight, pieWidth, pieWidth);
    QRect pieRect2 = QRect(width()-pieWidth-190, titleHeight, pieWidth, pieWidth);
    QPointF firstPoiint;

    // 成品饼状图
    painter.setBrush(QBrush(g_GraphGreen));
    painter.drawPie(pieRect1, 0*16, (100-m_fValue1)*3.6*16);

    // 整米饼状图
    if (m_nMatType == MATERIAL_RICE)
        painter.drawPie(pieRect2, 0*16, (100-m_fValue2-m_fValue3)*3.6*16);
    else if (m_nMatType == MATERIAL_BROWN || m_nMatType == MATERIAL_PADDY)
        painter.drawPie(pieRect2, 0*16, (100-m_fValue2)*3.6*16);

    // 杂质饼状图
    painter.setBrush(QBrush(g_GraphYellow));
    painter.drawPie(pieRect1, (100-m_fValue1)*3.6*16, m_fValue1*3.6*16);

    // 互混饼状图
    painter.setBrush(QBrush(g_cali_color_3));
    if (m_nMatType == MATERIAL_RICE)
        painter.drawPie(pieRect2, (100-m_fValue2-m_fValue3)*3.6*16, m_fValue3*3.6*16);

    // 碎米饼状图
    painter.setBrush(QBrush(g_GraphYellow));
    painter.drawPie(pieRect2, (100-m_fValue2)*3.6*16, m_fValue2*3.6*16);

    // 成品/杂质分割线
    painter.setPen(pen);
    firstPoiint.setX((qreal)pieRect1.right());
    firstPoiint.setY((qreal)pieRect1.top()+pieWidth/2);
    painter.drawLine(firstPoiint, firstPoiint + QPointF(pieWidth*fRatio,0));

    // 分割线数据
    QRect lineRect1, lineRect2, lineRect3, lineRect4, lineRect5;
    painter.setFont(chUpFont);
    lineRect1 = QRect(firstPoiint.x()+5, firstPoiint.y()-35, pieWidth*fRatio, 27);
    painter.drawText(lineRect1, Qt::AlignLeft, QString("%1:%2%").arg(strType[0]).arg(QString::number(100-m_fValue1, 'f', 1)));

    lineRect2 = QRect(firstPoiint.x()+5, firstPoiint.y()+5, pieWidth*fRatio, 27);
    painter.drawText(lineRect2, Qt::AlignLeft, QString("%1:%2%").arg(strType[1]).arg(QString::number(m_fValue1, 'f', 1)));

    // 整米/碎米分割线
    firstPoiint.setX((qreal)pieRect2.right());
    firstPoiint.setY((qreal)pieRect2.top()+pieWidth/2);
    painter.drawLine(firstPoiint, firstPoiint + QPointF(pieWidth*fRatio,0));

    lineRect3 = QRect(firstPoiint.x()+5, firstPoiint.y()-35, pieWidth*fRatio, 27);
    lineRect4 = QRect(firstPoiint.x()+5, firstPoiint.y()+5, pieWidth*fRatio, 27);
    if(m_fValue3 > 0.0) {
        lineRect5 = QRect(firstPoiint.x()+5, firstPoiint.y()+37, pieWidth*fRatio, 27);
    }

//    if(gs_consoleGlobal.nLang == LANG_CHS || gs_consoleGlobal.nLang == LANG_CHT) {
    if(1) {
        painter.drawText(lineRect3, Qt::AlignLeft, QString("%1:%2%").arg(strType[2]).arg(QString::number(100-m_fValue2, 'f', 1)));
        painter.drawText(lineRect4, Qt::AlignLeft, QString("%1:%2%").arg(strType[3]).arg(QString::number(m_fValue2, 'f', 1)));
        if (m_nMatType == MATERIAL_RICE)
            painter.drawText(lineRect5, Qt::AlignLeft, QString("%1:%2%").arg("互混米").arg(QString::number(m_fValue3, 'f', 1)));
    }
    else {
        painter.drawText(lineRect3, Qt::AlignLeft, QString("%1:%2%").arg(strType[2]).arg(QString::number(100-m_fValue2, 'f', 1)));
        painter.drawText(lineRect4, Qt::AlignLeft, QString("%1:%2%").arg(strType[3]).arg(QString::number(m_fValue2, 'f', 1)));
        if (m_nMatType == MATERIAL_RICE)
            painter.drawText(lineRect5, Qt::AlignLeft, QString("%1:%2%").arg("互混").arg(QString::number(m_fValue3, 'f', 1)));
    }

    // title
    QFont chTitleFont = QFont("Microsoft YaHei", 28, 1);
    chTitleFont.setPixelSize(28);
    QRect titleTxtRect(10, 10, 280, titleHeight);
    painter.setFont(chTitleFont);
    painter.drawText(titleTxtRect, Qt::AlignHCenter | Qt::AlignTop, QString("%1(%)").arg("主要含杂数据"));

    QRect rectLabel1, rectLabel2, rectLabel3, rectLabel4, rectLabel5;
    QRect rect1, rect2, rect3, rect4, rect5;

    // 左侧下标数据
    if(m_nMatType == MATERIAL_RICE) {
        painter.setFont(chDownFont);
        painter.setBrush(QBrush(g_GraphGreen));
        rectLabel1 = QRect(50, height()-25, channelOutWidth+10, 22);
        painter.drawRect(rectLabel1);

        painter.setBrush(QBrush(g_GraphYellow));
        rectLabel2 = QRect(pieWidth/2+75, height()-25, channelOutWidth+10, 22);
        painter.drawRect(rectLabel2);

        rect1 = QRect(rectLabel1.x()+channelOutWidth, rectLabel1.y(), channelOutWidth+55, 22);
        painter.drawText(rect1, Qt::AlignCenter, strType[0]);

        rect2 = QRect(rectLabel2.x()+channelOutWidth, rectLabel2.y(), channelOutWidth+55, 22);
        painter.drawText(rect2, Qt::AlignCenter, strType[1]);

        // 右侧下标数据
        painter.setBrush(QBrush(g_GraphGreen));
        rectLabel3 = QRect(width()-pieWidth-160, height()-25, channelOutWidth+10, 22);
        painter.drawRect(rectLabel3);

        painter.setBrush(QBrush(g_GraphYellow));
        rectLabel4 = QRect(width()-pieWidth/2-130, height()-25, channelOutWidth+10, 22);
        painter.drawRect(rectLabel4);

        rect3 = QRect(rectLabel3.x()+channelOutWidth, rectLabel3.y(), channelOutWidth+50, 22);
        rect4 = QRect(rectLabel4.x()+channelOutWidth, rectLabel4.y(), channelOutWidth+50, 22);
//        if(gs_consoleGlobal.nLang == LANG_CHS || gs_consoleGlobal.nLang == LANG_CHT) {
        if(1) {
            painter.drawText(rect3, Qt::AlignCenter, strType[2]);
            painter.drawText(rect4, Qt::AlignCenter, strType[3]);
        }
        else {
            painter.drawText(rect3, Qt::AlignCenter, strType[2]);
            painter.drawText(rect4, Qt::AlignCenter, strType[3]);
        }

        if(m_fValue3 > 0.0) {
            painter.setBrush(QBrush(g_cali_color_3));
            rectLabel5 = QRect(width()-130, height()-25, channelOutWidth+10, 22);
            painter.drawRect(rectLabel5);

            rect5 = QRect(rectLabel5.x()+channelOutWidth, rectLabel5.y(), channelOutWidth+50, 22);
//            if(gs_consoleGlobal.nLang == LANG_CHS || gs_consoleGlobal.nLang == LANG_CHT)
            if(1)
                painter.drawText(rect5, Qt::AlignCenter, "互混米");
            else
                painter.drawText(rect5, Qt::AlignCenter, "互混");
        }
    }
    else if(m_nMatType == MATERIAL_BROWN || m_nMatType == MATERIAL_PADDY) {
        chDownFont.setBold(true);
        painter.setFont(chDownFont);
        painter.setBrush(QBrush(linearGrad));
        rect1 = QRect(pieRect1.left()+pieWidth/2-70, height()-83, 140, 80);
        painter.drawRect(rect1);
        painter.drawText(rect1, Qt::AlignCenter, QString("%1\n%2%").arg("脱壳率").arg(QString::number(m_fValue3, 'f', 1)));

        rect3 = QRect(pieRect2.left()+pieWidth/2-70, height()-83, 140, 80);
        painter.drawRect(rect3);
//        if(gs_consoleGlobal.nLang != LANG_CHS && gs_consoleGlobal.nLang != LANG_CHT)
        if(1)
            painter.drawText(rect3, Qt::AlignCenter, QString("%1\n%2%").arg("碎").arg(QString::number(m_fValue2, 'f', 1)));
        else
            painter.drawText(rect3, Qt::AlignCenter, QString("%1\n%2%").arg("糙碎率").arg(QString::number(m_fValue2, 'f', 1)));
    }

    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
}

void myPieGraph::setTitle(QString title)
{
    m_sTitle = title;
}






mySinglePieGraph::mySinglePieGraph(QWidget *parent)
    : QWidget(parent)
{
    m_sTitle = "";
    m_nMatType = MATERIAL_RICE;
}

bool mySinglePieGraph::setName(QStringList cStringList, QList<QColor> colorList)
{
    if(cStringList.size() == 0 || cStringList.size()>12){
        return false;
    }
    m_dataSize = cStringList.size();
    m_cStringList.clear();
    m_cStringList = cStringList;
    m_colorList.clear();
    m_colorList = colorList;
    return true;
}

void mySinglePieGraph::setMatType(int nMatType)
{
    m_nMatType = nMatType;
}

bool mySinglePieGraph::setValue(QList<double> fValueList)
{
    if(fValueList.size()>12){
        return false;
    }
    m_fValue = fValueList;
    return true;
}

void mySinglePieGraph::paintEvent(QPaintEvent *ev)
{
    if(m_dataSize == 0 || m_colorList.size() == 0) 
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

    // 上方数据字体
    QFont chUpFont = QFont("Microsoft YaHei", 20, 1);
    chUpFont.setPixelSize(20);

    // 下方数据字体
    QFont chFont = QFont("Microsoft YaHei", 20, 1);
    chFont.setPixelSize(20);

    int titleHeight = 100;      //标题高度
    int channelOutWidth = 50;   //柱状矩形chFont宽度
    int pieWidth = 250;         //饼状宽度/高度

    QRect pieRect2 = QRect(width()-pieWidth-280, titleHeight, pieWidth, pieWidth);
    QPointF firstPoiint;

    double startValue = 0;
    double impurity = 0;
    if(m_nMatType == MATERIAL_PADDY) {
        for(int i=0; i<m_dataSize; i++){
            painter.setBrush(QBrush(m_colorList.at(i)));
            if(i == m_dataSize-1)
                impurity = m_fValue.at(i);
            if(0 != i)
                startValue += m_fValue.at(i-1);
            painter.drawPie(pieRect2, startValue*3.6*16, m_fValue.at(i)*3.6*16);
        }
        double tmp = startValue+m_fValue.at(m_dataSize-1);
        if(tmp < 100)
            painter.drawPie(pieRect2, tmp*3.6*16, (100-tmp)*3.6*16);
    }
    else {
        for(int i=0; i<m_dataSize; i++){
            painter.setBrush(QBrush(m_colorList.at(i)));
            if(0 != i){
                startValue += m_fValue.at(i-1);
                impurity += m_fValue.at(i);
                painter.drawPie(pieRect2, startValue*3.6*16, m_fValue.at(i)*3.6*16);
            }else{
                painter.drawPie(pieRect2, 0, (100-impurity)*3.6*16);
            }
        }
    }
    if( impurity > 100.0 ) impurity = 100.0;

    // 分割线数据
    QRect lineRect1, lineRect2;
    painter.setPen(pen);
    painter.setFont(chUpFont);
    // 整米/碎米分割线
    firstPoiint.setX((qreal)pieRect2.right());
    firstPoiint.setY((qreal)pieRect2.top()+pieWidth/2);
    painter.drawLine(firstPoiint, firstPoiint + QPointF(pieWidth*0.5+7,0));

    lineRect1 = QRect(firstPoiint.x()+5, firstPoiint.y()-35, pieWidth*0.6, 27);
    lineRect2 = QRect(firstPoiint.x()+5, firstPoiint.y()+5, pieWidth*0.6, 27);
    if(m_nMatType == MATERIAL_PADDY) {
        painter.drawText(lineRect1, Qt::AlignLeft, QString("%1:%2%").arg("稻谷").arg(QString::number(100-impurity, 'f', 1)));
        painter.drawText(lineRect2, Qt::AlignLeft, QString("%1:%2%").arg("糙米").arg(QString::number(impurity, 'f', 1)));
    }
    else {
        painter.drawText(lineRect1, Qt::AlignLeft, QString("%1:%2%").arg(m_cStringList[0]).arg(QString::number(100-impurity, 'f', 1)));
        painter.drawText(lineRect2, Qt::AlignLeft, QString("%1:%2%").arg("其他").arg(QString::number(impurity, 'f', 1)));
    }

    // title
    QFont chTitleFont = QFont("Microsoft YaHei", 28, 1);
    chTitleFont.setPixelSize(28);
    QRect titleTxtRect(10, 10, 280, titleHeight);
    painter.setFont(chTitleFont);
    painter.drawText(titleTxtRect, Qt::AlignHCenter | Qt::AlignTop, QString("%1(%)").arg("样品指标"));

    int interval = (width()-60)*(/*gs_consoleGlobal.nLang == LANG_CHS*/1?0.25:0.333);
    int rowNum = /*gs_consoleGlobal.nLang == LANG_CHS*/1?4:3;
    // 右侧下标数据
    painter.setFont(chFont);
    for(int i=0; i<m_dataSize; i++){
        if(i<rowNum){
            painter.setBrush(QBrush(m_colorList.at(i)));
            QRect rectLabel = QRect(interval*i+30, height()-50, channelOutWidth, 20);
            painter.drawRect(rectLabel);

            QRect rect = QRect(rectLabel.x()+channelOutWidth, rectLabel.y(), channelOutWidth*(/*gs_consoleGlobal.nLang == LANG_CHS*/1?2.8:3.5), 20);
            if(m_nMatType == MATERIAL_PADDY)
                painter.drawText(rect, Qt::AlignCenter, QString("%1:%2%").arg(m_cStringList.at(i)).arg(QString::number(m_fValue.at(i), 'f', 1)));
            else
                painter.drawText(rect, Qt::AlignCenter, QString("%1:%2%").arg(m_cStringList.at(i)).arg(QString::number(0==i?(100-impurity):m_fValue.at(i), 'f', 1)));
        }else{
            painter.setBrush(QBrush(m_colorList.at(i)));
            QRect rectLabel = QRect(interval*(i%rowNum)+30, height()-23, channelOutWidth, 20);
            painter.drawRect(rectLabel);

            QRect rect = QRect(rectLabel.x()+channelOutWidth, rectLabel.y(), channelOutWidth*(/*gs_consoleGlobal.nLang == LANG_CHS*/1?2.8:3.5), 20);
            painter.drawText(rect, Qt::AlignCenter, QString("%1:%2%").arg(m_cStringList.at(i)).arg(QString::number(m_fValue.at(i), 'f', 1)));
        }
    }

    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
}

void mySinglePieGraph::setTitle(QString title)
{
    m_sTitle = title;
}
