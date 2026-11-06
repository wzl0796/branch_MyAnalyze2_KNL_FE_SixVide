#include "myroundedprogress.h"
#include <QPainter>

MyRoundedProgress::MyRoundedProgress(QWidget *parent)
    : QWidget(parent)
    , m_sum(100)
    , m_cur(50)
    , m_legalMin(80.0)
    , m_legalMax(100.0)
{

}

void MyRoundedProgress::setSum(const int &sum)
{
    this->m_sum = sum;
    repaint();
}

void MyRoundedProgress::setCur(const int &cur)
{
    this->m_cur = cur;
    repaint();
}

void MyRoundedProgress::setLegalMin(const int &min)
{
    m_legalMin = min;
    repaint();
}

void MyRoundedProgress::setLegalMax(const int &max)
{
    m_legalMax = max;
    repaint();
}

void MyRoundedProgress::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this)   ;
    painter.fillRect(this->rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing);

    //! 绘制大圆形
    painter.setPen(Qt::gray);
    painter.setBrush(QBrush(QColor(230, 230, 230)));
    painter.drawEllipse(this->rect());

    //! 绘制有效区间
//    int angleStart = (90-m_legalMin*360/100)*16;
//    int span = -(m_legalMax-m_legalMin)*360/100*16;
//    painter.setBrush(Qt::gray);
//    painter.drawPie(this->rect(), angleStart, span);

    //! 绘制进度环
    if(m_sum > 0){
        QColor color = QColor(0, 116, 87);
        painter.setPen(color);
        painter.setBrush(QBrush(color));
        int span =  -m_cur*360/m_sum*16;
        painter.drawPie(this->rect(), 90*16, span);
    }

    //! 绘制小圆形
    QRect rectIn = this->rect();
    rectIn.adjust(10, 10, -10, -10);
    painter.setPen(Qt::gray);
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(rectIn);

    //! 写文本
    QFont font;
    font.setPixelSize(qMin(rectIn.width(), rectIn.height())/3);
    painter.setFont(font);
    painter.setPen(Qt::black);
    QString text = "0.0%";
    int min = m_legalMin*m_sum/100, max = m_legalMax*m_sum/100;
    if(m_sum > 0){
        text = QString("%1%").arg(m_cur*100.0/m_sum, 0, 'f', 1);
    }
    QPen pen = painter.pen();
    if(m_cur > max){
        pen.setColor(Qt::red);
    } else if(m_cur < min){
        pen.setColor(QColor(255, 127, 50));
    } else {
        pen.setColor(Qt::black);
    }
    painter.setPen(pen);
    painter.drawText(rectIn, Qt::AlignCenter, text);
}
