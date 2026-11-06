/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymutex.cpp
 * \brief       互斥选择框控件源文件
 * \date        2015.01.14
 */
#include "mymutex.h"

myMutex::myMutex(Qt::Orientation flag, int selIndex, QWidget *parent)
    : QWidget(parent)
{
    createPage(flag);

    n_size = 0;

    curIndex = selIndex;
    btns[curIndex]->setIcon(QIcon(":/res/png/action_circle.png"));

    connectSigAndSlt();
}

/*!
 * \brief myMutex::createPage
 */
void myMutex::createPage(Qt::Orientation flag)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    QFont font;
    font.setPixelSize(20);
//    group = new QGroupBox(this);
//    QString groupstyle = QString("QGroupBox{background:transparent;border:2px solid rgb(190,190,190);border-radius:10px;margin-top:15px;outline:0px;}"
//                                 "QGroupBox:title{color:black;subcontrol-origin:margin;left:20px;}");
//    group->setStyleSheet(groupstyle);
//    group->setGeometry(0,0,width(),height());

    QString btnStyle = QString("QPushButton {"
                               "text-align: center;"
                               "outline: none;"
                               "background-color: #ffffff;"
                               "border: 1px solid #afafaf;"
                               "border-radius: 5;"
                               "padding: 3px;}"

                               "QPushButton:pressed {"
                               "padding-left: 5px;"
                               "padding-top: 5px;"
                               "background-color: #036eb8;}");

    for (int i = 0; i < MAX_NUM; i++) {
        btns[i] = new QPushButton();
        btns[i]->setFixedSize(QSize(BTN_HEI-25, BTN_HEI-25));
        btns[i]->setIconSize(QSize(BTN_HEI-25, BTN_HEI-25));
        btns[i]->setStyleSheet(btnStyle);

        labels[i] = new QLabel;
        labels[i]->setFont(font);
        labels[i]->setStyleSheet("QLabel{border-radius:0px;}");
    }

    if (flag == Qt::Horizontal) {
        QHBoxLayout *hLayout = new QHBoxLayout;
        for (int i = 0; i < MAX_NUM; i++) {
            hLayout->addWidget(btns[i]);
            hLayout->addWidget(labels[i]);
        }
        lay->addLayout(hLayout);
    }

    if (flag == Qt::Vertical) {
        QGridLayout *vLayout = new QGridLayout;
        for (int i = 0; i < MAX_NUM; i++) {
            vLayout->addWidget(btns[i], i, 0);
            vLayout->addWidget(labels[i], i, 1);
        }
        vLayout->setColumnMinimumWidth(0,60);
        lay->addLayout(vLayout);
    }
}

/*!
 * \brief 连接页面中的信号与槽
 */
void myMutex::connectSigAndSlt()
{
    m_sigMap = new QSignalMapper(this);
    for (int i = 0; i < MAX_NUM; i++) {
        m_sigMap->setMapping(btns[i], i);
        connect(btns[i], SIGNAL(pressed()), m_sigMap, SLOT(map()));
    }
    connect(m_sigMap, SIGNAL(mapped(int)), this, SLOT(onBtnsPressed(int)));
}

/* 设置组中的可选项 */
void myMutex::setNum(int num)
{
    if (num > MAX_NUM) {
        num = MAX_NUM;
    }

    for (int i = 0; i < num; i++)
        btns[i]->setEnabled(true);

    for (int i = num; i < MAX_NUM; i++) {
        btns[i]->hide();
        labels[i]->hide();
    }
}


/*!
 * \brief myMutex::setLabelText
 * \param str1
 * \param str2
 * \param str3
 */
void myMutex::setLabelText(QString str1, QString str2, QString str3)
{
    labels[0]->setText(str1);
    labels[1]->setText(str2);
    labels[2]->setText(str3);

    int nNum = 0;
    if (str2 == "") {
        nNum = 1;
    } else if (str3 == "") {
        nNum = 2;
    } else {
        nNum = 3;
    }

    for (int i = 0; i < nNum; i++) {
        btns[i]->show();
        labels[i]->show();
    }
    for (int i = nNum; i < MAX_NUM; i++) {
        btns[i]->hide();
        labels[i]->hide();
    }
}

/* 设置组中的标签的文本 */
void myMutex::setLabelText(QStringList strList)
{
    int nSize = strList.size();
    n_size = nSize;
    if (nSize > MAX_NUM) {
        nSize = MAX_NUM;
    }

    for (int i = 0; i < strList.size(); i++){
        labels[i]->setText(strList.at(i));
        btns[i]->show();
        labels[i]->show();
    }

    for (int i = strList.size(); i < MAX_NUM; i++) {
        labels[i]->hide();
        btns[i]->hide();
    }
}

/* 清空按钮组的图标 */
void myMutex::clearBtnsIcon()
{
    for (int i = 0; i < MAX_NUM; i++) {
        btns[i]->setIcon(QIcon());
    }
}

/* 获取当前被选择的索引号 */
int myMutex::getCurrentIndex()
{
    return curIndex;
}

/* 设置当前组别选择的索引号 */
void myMutex::setCurrentIndex(int index)
{
    clearBtnsIcon();
    curIndex = index;
    btns[index]->setIcon(QIcon(":/res/png/action_circle.png"));

//    if (curIndex == (n_size-1)) {
//        btns[index]->setIcon(QIcon(":/res/png/action_circle_red.png"));
//    } else {
//        btns[index]->setIcon(QIcon(":/res/png/action_circle.png"));
//    }
}

/* 设置索引号是否可按 */
void myMutex::setIndexEnabled(int index, bool enabled)
{
    btns[index]->setEnabled(enabled);
}

/*!
 * \brief 响应选择按钮按下
 * \param nIndex
 */
void myMutex::onBtnsPressed(int nIndex)
{
    clearBtnsIcon();
    btns[nIndex]->setIcon(QIcon(":/res/png/action_circle.png"));

//    if (nIndex == (n_size-1)) {
//        btns[nIndex]->setIcon(QIcon(":/res/png/action_circle_red.png"));
//    } else {
//        btns[nIndex]->setIcon(QIcon(":/res/png/action_circle.png"));
//    }

    curIndex = nIndex;
    emit indexChanged(nIndex);
}

/**
 * @brief myMutex::设置显示和隐藏
 * @param index
 * @param hiden
 */
void myMutex::setBtnHiden(int index, bool hiden)
{
    btns[index]->setHidden(hiden);
    labels[index]->setHidden(hiden);
}
