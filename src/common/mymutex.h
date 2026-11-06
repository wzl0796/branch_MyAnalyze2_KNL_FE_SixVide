/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymutex.h
 * \brief       互斥选择框控件头文件
 * \date        2015.01.14
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore>
#include <QtGui>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include "params/define.h"
#include "mypushbutton.h"


/* 该类用于实现二种或三种间的互斥选择 */
class myMutex : public QWidget
{
    Q_OBJECT

public:
    explicit myMutex(Qt::Orientation flag, int selIndex, QWidget* parent = 0);

    void createPage(Qt::Orientation flag);          //! 创建页面
    void connectSigAndSlt(void);        //! 连接页面中的信号与槽
    void setNum(int num);
    void setLabelText(QString str1 = "", QString str2 = "", QString str3 = "");
    void setLabelText(QStringList strList);
    void clearBtnsIcon(void);
    int  getCurrentIndex(void);
    void setCurrentIndex(int index);
    void setIndexEnabled(int index, bool enabled);
    void setBtnHiden(int index, bool hiden);
    int  getSize(void) {
        return n_size;
    }

signals:
    void indexChanged(int);

private slots:
    void onBtnsPressed(int nIndex);     //! 响应选择按钮按下

private:
    enum{
        MAX_NUM = 10
    };

    QGroupBox *group;
//    myPushButton *btns[MAX_NUM];
    QPushButton *btns[MAX_NUM];
    QLabel *labels[MAX_NUM];
    quint8 curIndex;
    QSignalMapper   *m_sigMap;
    int n_size;
};

#endif // GLOBAL_H
