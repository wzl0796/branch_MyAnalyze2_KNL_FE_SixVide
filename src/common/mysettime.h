/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mysettime.h
 * \brief       设置时间控件头文件
 * \date        2015.01.14
 */
#ifndef MYSETTIME_H
#define MYSETTIME_H

#include <QDialog>

#include <QApplication>
#include <sys/time.h>
#include <time.h>

#include "myinputpanel.h"

/*!
 * \brief 设置时间控件类
 */
class mySetTime : public QDialog
{
    Q_OBJECT
public:
    explicit mySetTime(QWidget *parent = 0);

    QSignalMapper *timeSignalMapper;
    QLabel       *warning;
    QLabel       *dateLabel[6];
    QPushButton    *dateLbe[6];
    QDateTimeEdit *dateTimeEdit;
    QSpacerItem   *horizontalSpacer;
    QPushButton  *setDateBtn;
    QGridLayout   *gridLayout;
    QHBoxLayout   *dateHBLayout;
    QSpacerItem   *horizontalSpacer_2;
    int            data[6];
    QPushButton  *nextBtn;
signals:
    
public slots:
    void getTimeIndex(int index);

private:
    //! 词根
    QString year;
    QString month;
    QString day;
    QString hour;
    QString minute;
    QString seconds;
    QString verify_sys_time;
    QString back;
};

#endif // MYSETTIME_H
