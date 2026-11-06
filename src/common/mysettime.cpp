/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mysettime.cpp
 * \brief       设置时间控件源文件
 * \date        2015.01.14
 */
#include "mysettime.h"

mySetTime::mySetTime(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    resize(WIN_WID, WIN_HEI);

//    if(gs_consoleGlobal.IsChineseLang()) {
        year = "年";
        month = "月";
        day = "日";
        hour = "时";
        minute = "分";
        seconds = "秒";
        verify_sys_time = "请确认当前系统时间是否正确";
        back = "返回";
//    }
//    else {
//        year = "Year";
//        month = "Month";
//        day = "Day";
//        hour = "Hour";
//        minute = "Minute";
//        seconds = "Seconds";
//        verify_sys_time = "Please veryfy current system time";
//    }

    gridLayout = new QGridLayout(this);
    gridLayout->setHorizontalSpacing(50);

    timeSignalMapper = new QSignalMapper(this);
    warning = new QLabel(verify_sys_time);
    warning->setMaximumHeight(BTN_HEI);
    gridLayout->addWidget(warning,0,0,1,3);
    for (int i = 0; i < 6; i++) {
        dateLabel[i] = new QLabel("", this);
        dateLbe[i]   = new QPushButton("", this);
        switch(i){
        case 0:
            dateLabel[i]->setText(year);
            break;
        case 1:
            dateLabel[i]->setText(month);
            break;
        case 2:
            dateLabel[i]->setText(day);
            break;
        case 3:
            dateLabel[i]->setText(hour);
            break;
        case 4:
            dateLabel[i]->setText(minute);
            break;
        case 5:
            dateLabel[i]->setText(seconds);
            break;
        default:
            break;
        }

        dateLbe[i]->setMaximumHeight(BTN_HEI);
        if (i < 3){
            gridLayout->addWidget(dateLabel[i],i/3+1,i%3,1,1,Qt::AlignCenter);
            gridLayout->addWidget(dateLbe[i]  ,i/3+2,i%3,1,1);
        } else {
            gridLayout->addWidget(dateLabel[i],i/3+2,i%3,1,1,Qt::AlignCenter);
            gridLayout->addWidget(dateLbe[i]  ,i/3+3,i%3,1,1);
        }

        /* 消息栈 */
        timeSignalMapper->setMapping(dateLbe[i], i);
        connect(dateLbe[i], SIGNAL(pressed()), timeSignalMapper, SLOT(map()));
    }

    QString  str;
    QDateTime  dateTime = QDateTime::currentDateTime();

    data[0] = dateTime.date().year();
    data[1] = dateTime.date().month();
    data[2] = dateTime.date().day();
    data[3] = dateTime.time().hour();
    data[4] = dateTime.time().minute();
    data[5] = dateTime.time().second();

    for (int i = 0; i < 6; i++) {
        str = QString("%1").arg(data[i]);
        dateLbe[i]->setText(str);
    }

    nextBtn = new QPushButton(back, this);
    nextBtn->setMaximumSize(QSize(BTN_WID,BTN_HEI));

    dateHBLayout = new QHBoxLayout();
    dateHBLayout->addStretch();
    dateHBLayout->addWidget(nextBtn);

    gridLayout->addLayout(dateHBLayout, 5, 0, 1,3);

    /*槽函数*/
    connect(timeSignalMapper, SIGNAL(mapped(int)), this, SLOT(getTimeIndex(int)));
    connect(nextBtn , SIGNAL(clicked()), this, SLOT(accept()));
}

void mySetTime::getTimeIndex(int index)
{
    int ret;
    QString str;
    int nMin,nMax;
    switch(index) {
    case 0://year
        nMin = 0;
        nMax = 2049;
        break;
    case 1://month
        nMin = 1;
        nMax = 12;
        break;
    case 2://day
        nMin = 1;
        nMax = 31;
        break;
    case 3://hour
        nMin = 0;
        nMax = 59;
        break;
    case 4://mintue
        nMin = 0;
        nMax = 59;
        break;
    case 5://second
        nMin = 0;
        nMax = 59;
        break;
    }

    myInputPanel inputDlg(intType,nMin,nMax,data[index],"",g_mainWidget);
    ret = inputDlg.exec();

    if (ret == QDialog::Accepted) {
        data[index] = inputDlg.getValue();
        str = QString("%1").arg(data[index]);
        dateLbe[index]->setText(str);
#ifdef Q_OS_UNIX
        char time[64];
        sprintf(time, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"", data[0], data[1], data[2],
                data[3], data[4], data[5]);
        system(time);
        popen("hwclock -w","r");
#endif
    }
}
