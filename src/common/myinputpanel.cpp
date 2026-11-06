/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myinputpanel.h.cpp
 * \brief       数字输入框控件源文件
 * \date        2015.01.14
 */
#include "myinputpanel.h"

myInputPanel::myInputPanel(inputType type,int nMin, int nMax, double nDisp, QString title, QWidget *parent)
    : QDialog(parent)
{
    nMinNum  = nMin;
    nMaxNum  = nMax;
    nDispNum = nDisp;
    nValueDisp = nDisp;
    sTitle = title;
    nFlagMode  = type;
    b_FlagCLear = true;
    m_bJudgeEn = true;
    m_bInputNegSign = false;

    font.setPixelSize(30);
    setFont(font);

    createButtons();

    signalMapper = new QSignalMapper(this);
    for (int i=0; i<12; i++) {
        signalMapper->setMapping(button[i], i);
        connect(button[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sendChar(int)));

    //! 信号槽
    connect(buttonC1, SIGNAL(pressed()), this, SLOT(buttonC1Function()));
    connect(buttonC2, SIGNAL(pressed()), this, SLOT(buttonC2Function()));
    connect(buttonC3, SIGNAL(pressed()), this, SLOT(buttonC3Function()));
    connect(minusBtn, SIGNAL(pressed()), this, SLOT(onMinusBtnClickedSlt()));
    connect(addBtn  , SIGNAL(pressed()), this, SLOT(onAddBtnClickedSlt()));
    connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onValueSliderChangeSlt()));
}

myInputPanel::~myInputPanel()
{
    
}
/**
 * @brief myInputPanel::createButtons
 * 初始化页面，创建按钮
 */
void myInputPanel::createButtons(void)
{
    setWindowFlags(Qt::FramelessWindowHint);
    this->setMinimumSize(50,50);

    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setStyleSheet("background-color: rgb(0, 0, 0, 100);");
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    widget = new QWidget(this);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setGeometry(WIN_WID/3+80, WIN_HEI/4, WIN_WID*0.25, WIN_HEI*0.5);
    widget->setStyleSheet(QString("background-color:%1;").arg(g_ColorLightGray2));
    widget->show();

    for(int i=0; i < 12; i++) {
        QString str;

        if (i == 9) {
            button.append(new QPushButton("0",widget));
        } else if (i == 10) {
            button.append(new QPushButton(".", widget));
        } else if (i == 11) {
            button.append(new QPushButton("-", widget));
        }else {
            str.sprintf("%d",i+1);
            button.append(new QPushButton(str,widget));
        }
        button[i]->setFocusPolicy(Qt::NoFocus);
        button[i]->setSizePolicy(sizePolicy);
        button[i]->setStyleSheet("background-color:white;");
    }

    titleLabel = new QLineEdit(sTitle, widget);
    titleLabel->setSizePolicy(sizePolicy);
    titleLabel->setStyleSheet("background-color: white;border:0px;font:30px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setReadOnly(true);
    textLbe = new QLineEdit(widget);
    textLbe->setStyleSheet("background-color: white;border:0px;font:60px;");
    textLbe->setAlignment(Qt::AlignCenter);
    textLbe->setSizePolicy(sizePolicy);


    buttonC1 = new QPushButton(myLan.del,widget);
    buttonC1->setFocusPolicy(Qt::NoFocus);
    buttonC1->setSizePolicy(sizePolicy);
    buttonC2 = new QPushButton(myLan.ok, widget);
    buttonC2->setFocusPolicy(Qt::NoFocus);
    buttonC2->setSizePolicy(sizePolicy);
    buttonC2->setStyleSheet("background-color:white;");
    buttonC3 = new QPushButton(myLan.cancel, widget);
    buttonC3->setFocusPolicy(Qt::NoFocus);
    buttonC3->setSizePolicy(sizePolicy);
    buttonC3->setStyleSheet("background-color:white;");

    minusBtn = new QPushButton("-");
    minusBtn->setMaximumWidth(40+8);
    minusBtn->setMaximumHeight(40+8);
    minusBtn->setSizePolicy(sizePolicy);
    addBtn = new QPushButton("+");
    addBtn->setMaximumWidth(40+8);
    addBtn->setMaximumHeight(40+8);
    addBtn->setSizePolicy(sizePolicy);

    valueSlider  = new QSlider(Qt::Horizontal);
    valueSlider->setRange(nMinNum,nMaxNum);
    valueSlider->setValue(nValueDisp);
    valueSlider->setSizePolicy(sizePolicy);
    valueSlider->setStyleSheet("QSlider::groove:horizontal {"
                               "border: 1px solid #999999;"
                               "height: 45px;"
                               "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
                               "margin: 2px 0;}"

                               "QSlider::handle:horizontal {"
                               "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
                               "border: 1px solid #5c5c5c;"
                               "width: 45px;"
                               "margin: -2px 0;"
                               "border-radius: 3px; }");

    mainVerticalLayout = new QVBoxLayout(widget);
    mainVerticalLayout->setSpacing(3);
    mainVerticalLayout->setMargin(3);

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->addWidget(titleLabel);
    verticalLayout->addWidget(textLbe);
    mainVerticalLayout->addLayout(verticalLayout);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(3);
    horizontalLayout_2->addWidget(minusBtn);
    horizontalLayout_2->addWidget(valueSlider);
    horizontalLayout_2->addWidget(addBtn);
//    mainVerticalLayout->addLayout(horizontalLayout_2);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setSpacing(3);
    horizontalLayout_3->addWidget(button[0]);
    horizontalLayout_3->addWidget(button[1]);
    horizontalLayout_3->addWidget(button[2]);
    mainVerticalLayout->addLayout(horizontalLayout_3);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setSpacing(3);
    horizontalLayout_4->addWidget(button[3]);
    horizontalLayout_4->addWidget(button[4]);
    horizontalLayout_4->addWidget(button[5]);
    mainVerticalLayout->addLayout(horizontalLayout_4);

    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->setSpacing(3);
    horizontalLayout_5->addWidget(button[6]);
    horizontalLayout_5->addWidget(button[7]);
    horizontalLayout_5->addWidget(button[8]);
    mainVerticalLayout->addLayout(horizontalLayout_5);

    gridLayout_6 = new QGridLayout();
    gridLayout_6->setSpacing(3);
    gridLayout_6->addWidget(button[9], 0, 0);
    gridLayout_6->addWidget(button[10], 0, 1);
    button[11]->hide();
    gridLayout_6->addWidget(button[11], 0, 2);
    gridLayout_6->setColumnStretch(0, 2);
    gridLayout_6->setColumnStretch(1, 1);
    mainVerticalLayout->addLayout(gridLayout_6);

    gridLayout_7 = new QGridLayout();
    gridLayout_7->setSpacing(3);
    gridLayout_7->addWidget(buttonC3,0,2);
    gridLayout_7->addWidget(buttonC1,0,1);
    gridLayout_7->addWidget(buttonC2,0,0);
    mainVerticalLayout->addLayout(gridLayout_7);

    QString str = QString("%1").arg(nDispNum);
    textLbe->setText(str);

    if (nFlagMode == floatType) {
        if (str[str.length()-1] == '.') {
            button[10]->setEnabled(false);
        } else {
            button[10]->setEnabled(true);
        }
    } else if (nFlagMode == intType){
        button[10]->setEnabled(false);
    } else if (nFlagMode == passwdType){
        button[10]->setEnabled(false);
        minusBtn->hide();
        addBtn->hide();
        valueSlider->hide();
        textLbe->setText("");
        nMinNum = -1;
        nMaxNum = 10000000;
        textLbe->setEchoMode(QLineEdit::Password);
    } else if (nFlagMode == textType) {
        button[10]->setEnabled(true);
        minusBtn->hide();
        addBtn->hide();
        valueSlider->hide();
        textLbe->setText("");
        nMinNum = -1;
        nMaxNum = 10000000;
    }

    return;
}
/*
 * 各个字符槽函数，1，2，3...
 */
void myInputPanel::sendChar(int indexOfCharToSend)
{
    checkDotBtn();
    if(m_bInputNegSign) {
        button[11]->setEnabled(false);
    }

    if (nFlagMode == floatType) {
        button[10]->setEnabled(true);
    }

    if (nFlagMode == passwdType || nFlagMode == textType) {
        if (b_FlagCLear) {
            b_FlagCLear = false;
            textLbe->setText("");
        }
        textLbe->insert(button[indexOfCharToSend]->text());
    }
    else {
        if (b_FlagCLear) {
            b_FlagCLear = false;
            nDispNum = 0;
            nValueDisp = nDispNum;
            valueSlider->setValue(nValueDisp);
            textLbe->setText("");
        }

        textLbe->insert(button[indexOfCharToSend]->text());


        nDispNum = textLbe->text().toDouble();
        if (nDispNum >= nMaxNum && m_bJudgeEn) {
            nDispNum = nMaxNum;
            QString str = QString("%1").arg(nDispNum);
            textLbe->setText(str);
        }
        // nValueDisp = nDispNum;
        // valueSlider->setValue(nValueDisp);

        checkDotBtn();
    }

    return;
}
/**
 * @brief myInputPanel::buttonC1Function
 * 删除上次输入
 */
void myInputPanel::buttonC1Function(void)
{
    // 屏蔽第一次清空参数
    b_FlagCLear = false;
    if(m_bInputNegSign) {
        button[11]->setEnabled(true);
    }
    if (nFlagMode == textType || nFlagMode == passwdType) {
        QString str;
        str = textLbe->text();

        str.remove(str.length()-1,str.length());
        textLbe->setText(str);
    }
    else {
        nDispNum = 0;
        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);
        textLbe->setText("");

        checkDotBtn();
        if (nFlagMode == floatType) {
            button[10]->setEnabled(true);
        }
    }

    return;
}
/**
 * @brief myInputPanel::buttonC2Function
 * 确定输入
 */
void myInputPanel::buttonC2Function(void)
{
    if (nFlagMode != textType && nFlagMode != passwdType && m_bJudgeEn) {
        nDispNum = textLbe->text().toDouble();
        if (textLbe->text() == "" || nDispNum < nMinNum) {
            nDispNum = nMinNum;
        } else if (nDispNum > nMaxNum) {
            nDispNum = nMaxNum;
        }

        textLbe->setText(QString("%1").arg(nDispNum));
    }
    accept();

}
/**
 * @brief myInputPanel::buttonC3Function
 * 取消输入
 */
void myInputPanel::buttonC3Function(void)
{
    reject();
}
/**
 * @brief myInputPanel::onValueSliderChangeSlt
 * 滑动框输入
 */
void myInputPanel::onValueSliderChangeSlt()
{

    nValueDisp = valueSlider->value();
    nDispNum   = nValueDisp+nDispNum - (int)nDispNum;
    if (nDispNum >= nMaxNum && m_bJudgeEn){
        nDispNum = nMaxNum;
    }
    if (nDispNum < nMinNum){
        nDispNum = nMinNum;
    }
    valueSlider->setValue(nValueDisp);
    QString str = QString("%1").arg(nDispNum);
    textLbe->setText(str);


    checkDotBtn();

}
/**
 * @brief myInputPanel::onAddBtnClickedSlt
 * 手动增加数据
 */
void myInputPanel::onAddBtnClickedSlt()
{
    qDebug()<<"onadd btn";
    if (nDispNum < nMaxNum)
    {
        if (nFlagMode == intType) {
            nDispNum++;
        }
        else if (nFlagMode == floatType){
            nDispNum += 0.01;

        }
        if (nDispNum >= nMaxNum && m_bJudgeEn){
            nDispNum = nMaxNum;
        }
        QString str = QString("%1").arg(nDispNum);
        textLbe->setText(str);
        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);
        checkDotBtn();
    }
}
/**
 * @brief myInputPanel::onMinusBtnClickedSlt
 * 手动减少数据
 */
void myInputPanel::onMinusBtnClickedSlt()
{
    qDebug()<<"on Min btn";

    if (nDispNum > nMinNum)
    {
        if (nFlagMode == intType) {
            nDispNum--;
        }
        else if (nFlagMode == floatType){
            nDispNum -= 0.01;
        }

        if (nDispNum < 0.1){
            nDispNum = nMinNum;
        }

        QString str = QString("%1").arg(nDispNum);
        textLbe->setText(str);

        nValueDisp = nDispNum;
        valueSlider->setValue(nValueDisp);
        checkDotBtn();
    }
}
/**
 * @brief myInputPanel::checkDotBtn
 * 检查点按钮输入使能
 */
void myInputPanel::checkDotBtn()
{
    QString str;
    str = textLbe->text();
    if (str.length()) {
        if (nFlagMode == floatType) {
            if (str[str.length()-1] == '.')
            {
                button[10]->setEnabled(false);
            }
        }
    }
}

/* 设置当前的值(浮点型) */
void myInputPanel::setValue(double value)
{
    QString str = QString("%1").arg(value);

    textLbe->setText(str);
    checkDotBtn();
}

/* 获取当前的值(浮点型) */
double myInputPanel::getValue()
{
    double valueTmp = textLbe->text().toDouble();
    return valueTmp;
}
/**
 * @brief myInputPanel::getText
 * @return
 */
QString myInputPanel::getText()
{
    return textLbe->text();
}
/**
 * @brief myInputPanel::setTitle
 * @param text
 */
void myInputPanel::setTitle(QString text)
{
    titleLabel->setText(text);
}

void myInputPanel::setText(QString text)
{
    textLbe->setText(text);
}

void myInputPanel::setInputType(inputType type)
{
    nFlagMode = type;
}

void myInputPanel::setValueMin(int nMin)
{
    nMinNum  = nMin;
}

void myInputPanel::setValueMax(int nMax)
{
    nMaxNum  = nMax;
}

/* 设置是否可输入负数 */
void myInputPanel::setInputNegEn(bool bFlag)
{
    m_bInputNegSign = bFlag;
    if(bFlag) {     // 可输入负数
        button[11]->show();
//        minusBtn->hide();
//        addBtn->hide();
//        valueSlider->hide();
        gridLayout_6->setColumnStretch(0, 0);
        gridLayout_6->setColumnStretch(1, 0);
    } else {        // 不可输入负数
        button[11]->hide();
//        minusBtn->show();
//        addBtn->show();
//        valueSlider->show();
        gridLayout_6->setColumnStretch(0, 2);
        gridLayout_6->setColumnStretch(1, 1);
    }
}

/* 设置是否判断最大值及最小值 */
void myInputPanel::setJudgeEn(bool bFlag)
{
    m_bJudgeEn = bFlag;
}

QString myInputPanel::formatDoubleNoExp(double v, int maxDec)
{
    if (!std::isfinite(v)) return QString::number(v);

    //! 如果接近整数则按整数显示，避免科学计数法
    double iv = qRound(v);
    if (qAbs(v - iv) < 1e-9) {
        //! 用 QString::number(long long) 避免指数形式
        return QString::number((long long)iv);
    }

    //! 否则以 fixed 格式输出并去掉多余的尾零
    QString s = QString::number(v, 'f', maxDec);
    if (s.contains('.')) {
        while (s.endsWith('0')) s.chop(1);
        if (s.endsWith('.')) s.chop(1);
    }
    return s;
}
