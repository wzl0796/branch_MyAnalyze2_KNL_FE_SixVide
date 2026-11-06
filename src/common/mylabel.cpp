#include "mylabel.h"

mylabel::mylabel(QWidget *parent) : QLabel(parent)
{

}

void mylabel::mouseReleaseEvent(QMouseEvent *event)
{
    int tmpX, tmpY;
    tmpX = event->pos().x();
    tmpY = event->pos().y();

    emit released();
}
