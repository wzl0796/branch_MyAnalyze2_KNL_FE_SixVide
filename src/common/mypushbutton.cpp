#include "mypushbutton.h"

myPushButton::myPushButton()
{
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()),  this, SLOT(timeOutSlt()));
    m_time = new QTime();
}

void myPushButton::mousePressEvent(QMouseEvent *e)
{
    m_timer->start(600);
    m_time->restart();
    nTimePress = m_time->elapsed();
}

void myPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_timer->stop();
    nTimeRelease = m_time->elapsed();
    if(nTimeRelease - nTimePress < 500)
        emit clicked();
}

void myPushButton::timeOutSlt()
{
    m_timer->stop();
    emit long_clicked();
}
