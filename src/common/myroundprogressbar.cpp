#include "myroundprogressbar.h"
#include <QtGui/QPainter>

MyRoundProgressBar::MyRoundProgressBar(QWidget *parent) :
    QWidget(parent),
    m_min(0), m_max(100),
    m_value(25),
    m_nullPosition(PositionTop),
    m_barStyle(StyleDonut),
    m_outlinePenWidth(1),
    m_dataPenWidth(1),
    m_rebuildBrush(false),
    m_format("%p%"),
    m_decimals(0),
    m_selfPoints("100"),
    m_updateFlags(UF_PERCENT),
    m_bIsImage(false)
{
}

void MyRoundProgressBar::setRange(double min, double max)
{
    m_min = min;
    m_max = max;

    if (m_max < m_min)
        qSwap(m_max, m_min);

    if (m_value < m_min)
        m_value = m_min;
    else if (m_value > m_max)
        m_value = m_max;

    if (!m_gradientData.isEmpty())
        m_rebuildBrush = true;

    update();
}

void MyRoundProgressBar::setMinimum(double min)
{
    setRange(min, m_max);
}

void MyRoundProgressBar::setMaximum(double max)
{
    setRange(m_min, max);
}

void MyRoundProgressBar::setValue(double val)
{
    if (m_value != val)
    {
        if (val < m_min)
            m_value = m_min;
        else if (val > m_max)
            m_value = m_max;
        else
            m_value = val;

        update();
    }
}

void MyRoundProgressBar::setValue(int val)
{
    setValue(double(val));
}

void MyRoundProgressBar::setNullPosition(double position)
{
    if (position != m_nullPosition)
    {
        m_nullPosition = position;

        if (!m_gradientData.isEmpty())
            m_rebuildBrush = true;

        update();
    }
}

void MyRoundProgressBar::setBarStyle(MyRoundProgressBar::BarStyle style)
{
    if (style != m_barStyle)
    {
        m_barStyle = style;

        update();
    }
}

void MyRoundProgressBar::setOutlinePenWidth(double penWidth)
{
    if (penWidth != m_outlinePenWidth)
    {
        m_outlinePenWidth = penWidth;

        update();
    }
}

void MyRoundProgressBar::setDataPenWidth(double penWidth)
{
    if (penWidth != m_dataPenWidth)
    {
        m_dataPenWidth = penWidth;

        update();
    }
}

void MyRoundProgressBar::setDataColors(const QGradientStops &stopPoints)
{
    if (stopPoints != m_gradientData)
    {
        m_gradientData = stopPoints;
        m_rebuildBrush = true;

        update();
    }
}

void MyRoundProgressBar::setFormat(const QString &format)
{
    if (format != m_format) {
        m_format = format;
        valueFormatChanged();
    }
}

void MyRoundProgressBar::resetFormat()
{
    m_format = QString();
    valueFormatChanged();
}

void MyRoundProgressBar::setDecimals(int count)
{
    if (count >= 0 && count != m_decimals)
    {
        m_decimals = count;

        valueFormatChanged();
    }
}

void MyRoundProgressBar::paintEvent(QPaintEvent* /*event*/)
{
    double outerRadius = qMin(width(), height());
    QRectF baseRect(1, 1, outerRadius-2, outerRadius-2);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // data brush
    rebuildDataBrushIfNeeded();

    // background
    drawBackground(p, rect());

    double innerRadius(0);
    QRectF innerRect;
    calculateInnerRect(baseRect, outerRadius, innerRect, innerRadius);

    // data circle
    double arcStep = 360.0 / (m_max - m_min) * m_value;

    // base circle
    drawBase(p, baseRect, innerRect);


    drawValue(p, baseRect, m_value, arcStep, innerRect, innerRadius);

    // center circle

    drawInnerBackground(p, innerRect);

    // text
    if (m_bIsImage) {
        drawImage(p, innerRect, outerRadius, innerRadius, m_selfPoints);
    } else {
        drawText(p, innerRect, innerRadius, m_selfPoints);
    }
    // finally draw the bar
    p.end();
}

void MyRoundProgressBar::drawBackground(QPainter &p, const QRectF &baseRect)
{ 
    p.fillRect(baseRect, palette().background());
}

void MyRoundProgressBar::drawBase(QPainter &p, const QRectF &baseRect, const QRectF &innerRect)
{
    switch (m_barStyle)
    {
    case StyleDonut:{
        QPainterPath dataPath;
        dataPath.setFillRule(Qt::OddEvenFill);
        dataPath.moveTo(baseRect.center());
        dataPath.addEllipse(baseRect);
        dataPath.addEllipse(innerRect);
        p.setPen(QPen(palette().shadow().color(), m_outlinePenWidth));
        p.setBrush(palette().base());
        p.drawEllipse(baseRect);
        }
        break;

    case StylePie:
        p.setPen(QPen(palette().base().color(), m_outlinePenWidth));
        p.setBrush(palette().base());
        p.drawEllipse(baseRect);
        break;

    case StyleLine:
        p.setPen(QPen(palette().base().color(), m_outlinePenWidth));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(baseRect.adjusted(m_outlinePenWidth/2, m_outlinePenWidth/2, -m_outlinePenWidth/2, -m_outlinePenWidth/2));
        break;

    default:;
    }

}

void MyRoundProgressBar::drawValue(QPainter &p, const QRectF &baseRect, double value, double arcLength, const QRectF& innerRect, double innerRadius)
{
    Q_UNUSED(innerRadius)

    // nothing to draw
    if (value == m_min)
        return;

    // for Line style
    if (m_barStyle == StyleLine)
    {
        p.setPen(QPen(palette().highlight().color(), m_dataPenWidth));
        p.setBrush(Qt::NoBrush);
        p.drawArc(baseRect.adjusted(m_outlinePenWidth/2, m_outlinePenWidth/2, -m_outlinePenWidth/2, -m_outlinePenWidth/2),
                  m_nullPosition * 16,
                  -arcLength * 16);
        return;
    }

    // for Pie and Donut styles
    QPainterPath dataPath;
    dataPath.setFillRule(Qt::WindingFill);

    // pie segment outer
    dataPath.moveTo(baseRect.center());
    dataPath.arcTo(baseRect,  m_nullPosition-arcLength, -360);                          //! 先绘制外圆的弧长
    if (m_barStyle == StylePie) {
        dataPath.lineTo(baseRect.center());
        p.setPen(QPen(palette().shadow().color(), m_dataPenWidth));
    }
    if (m_barStyle == StyleDonut) {
        QPointF currentPoint = dataPath.currentPosition();                              //! 绘制完外圆弧长后，获取绘制完的位置
        currentPoint = baseRect.center() + (currentPoint - baseRect.center()) * 0.75;   //! 计算内圆的坐标点，0.75代表内圆半径是外圆半径的0.75倍
        dataPath.moveTo(baseRect.center());                                             //! 坐标点回到中心中心准备绘制内圆弧形
        dataPath.arcTo(innerRect,m_nullPosition-arcLength,-360);                        //! 绘制内圆的弧形
        p.setPen(Qt::NoPen);
    }

    p.setBrush(palette().highlight());
    p.drawPath(dataPath);
}

void MyRoundProgressBar::calculateInnerRect(const QRectF &/*baseRect*/, double outerRadius, QRectF &innerRect, double &innerRadius)
{
    // for Line style
    if (m_barStyle == StyleLine)
    {
        innerRadius = outerRadius - m_outlinePenWidth;
    }
    else    // for Pie and Donut styles
    {
        innerRadius = outerRadius * 0.75;
    }

    double delta = (outerRadius - innerRadius) / 2;
    innerRect = QRectF(delta, delta, innerRadius, innerRadius);
}

void MyRoundProgressBar::drawInnerBackground(QPainter &p, const QRectF &innerRect)
{
    if (m_barStyle == StyleDonut)
    {
        p.setBrush(palette().alternateBase());
        p.drawEllipse(innerRect);
    }
}

void MyRoundProgressBar::drawImage(QPainter& p, const QRectF& innerRect, double outRadius, double innerRadius, QString imgPath)
{
    Q_UNUSED(innerRect)

    if (m_format.isEmpty())
        return;
    QRectF tmpRect;
    innerRadius = outRadius * 0.5;
    double delta = (outRadius - innerRadius) / 2;
    tmpRect = QRectF(delta, delta, innerRadius, innerRadius);

    QRectF textRect(tmpRect);
//    p.setPen(palette().text().color());
//    p.drawText(textRect, Qt::AlignCenter, value);
    p.drawImage(textRect,QImage(imgPath));
}

void MyRoundProgressBar::drawText(QPainter &p, const QRectF &innerRect, double innerRadius, QString value)
{
    if (m_format.isEmpty())
        return;

    // !!! to revise
    QFont f(font());
    f.setPixelSize(innerRadius * qMax(0.05, (0.35 - (double)m_decimals * 0.08)) * 1.5);
    p.setFont(f);

    QRectF textRect(innerRect);
    p.setPen(palette().text().color());
    p.drawText(textRect, Qt::AlignCenter, value);
}

QString MyRoundProgressBar::valueToText(double value) const
{
    QString textToDraw(m_format);

    if (m_updateFlags & UF_VALUE)
        textToDraw.replace("%v", QString::number(value, 'f', m_decimals));

    if (m_updateFlags & UF_PERCENT)
    {
        double procent = (value - m_min) / (m_max - m_min) * 100.0;
        textToDraw.replace("%p", QString::number(procent, 'f', m_decimals));
    }

    if (m_updateFlags & UF_MAX)
        textToDraw.replace("%m", QString::number(m_max - m_min + 1, 'f', m_decimals));

    return textToDraw;
}

void MyRoundProgressBar::valueFormatChanged()
{
    m_updateFlags = 0;

    if (m_format.contains("%v"))
        m_updateFlags |= UF_VALUE;

    if (m_format.contains("%p"))
        m_updateFlags |= UF_PERCENT;

    if (m_format.contains("%m"))
        m_updateFlags |= UF_MAX;

    update();
}

void MyRoundProgressBar::rebuildDataBrushIfNeeded()
{
    if (m_rebuildBrush)
    {
//        m_rebuildBrush = false;

        double arcStep = 360.0 / (m_max - m_min) * m_value;
        QConicalGradient dataBrush;
        dataBrush.setCenter(0.5,0.5);
        dataBrush.setCoordinateMode(QGradient::StretchToDeviceMode);

        // invert colors
        for (int i = 0; i < m_gradientData.count(); i++)
        {
            dataBrush.setColorAt(1.0 - m_gradientData.at(i).first, m_gradientData.at(i).second);
        }

        // angle
        dataBrush.setAngle(m_nullPosition-arcStep);

        QPalette p(palette());
        p.setBrush(QPalette::Highlight, dataBrush);
        setPalette(p);
    }
}

void MyRoundProgressBar::setPoint(QString point)
{
    m_selfPoints = point;
}

void MyRoundProgressBar::setDrawImgOrText(bool flag)
{
    m_bIsImage = flag;
}
