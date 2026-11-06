#include "mylistwidget.h"

myListWidget::myListWidget(QWidget *parent) : QListWidget(parent)
{
    setFixedHeight(54);
    setFlow(QListView::LeftToRight);
    setViewMode(QListView::ListMode);
    setMovement(QListWidget::Static);
    setSpacing(0);
    setFocusPolicy(Qt::TabFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QString style = QString("QListWidget{border:0px solid gray;outline:0px;font:26px \"Microsoft YaHei\";}"
                            "QListWidget::item{height:26px;margin-left:0px;margin-right:0px;margin-top:8px;margin-bottom:8px;"
                            "padding-left:20px;padding-right:20px;padding-top:0px;padding-bottom:0px;"
                            "background:white;color:black;border:0px;border-left:2px solid %1;}"
                            "QListWidget::item:selected{color:%2;}").arg(g_ColorLightGray2).arg(g_ColorMYGreen);
    setStyleSheet(style);
}
