 /*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myinputmethod.h.cpp
 * \brief       中英文输入框控件源文件
 * \date        2015.01.14
 */
#include "myinputmethod.h"

myInputMethod::myInputMethod(QString titleText, QString inputText, QWidget *parent, bool enaDelay)
    : QDialog(parent)
{
    strTitle = titleText;
    strInput = inputText;
    strPinyin = "";
    enableDelay = enaDelay;

    createButtons();
    signalMapper = new QSignalMapper(this);

    // 信号槽
    for (int i=0; i<37; i++){
        signalMapper->setMapping(button[i], i);
        connect(button[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sendChar(int)));
    connect(enableDelayCodeInfo, SIGNAL(pressed()),this,SLOT(onEnableDelayCode()));

    connect(buttonC3, SIGNAL(pressed()), this, SLOT(buttonC3Function()));
    connect(buttonC4, SIGNAL(pressed()), this, SLOT(buttonC4Function()));
    connect(buttonC5, SIGNAL(pressed()), this, SLOT(buttonC5Function()));
    connect(buttonC6, SIGNAL(pressed()), this, SLOT(buttonC6Function()));
    connect(delBtn, SIGNAL(pressed()),this,SLOT(onDelBtnClicked()));
    connect(methodBtn,SIGNAL(pressed()), this, SLOT(changeInputMethod()));
    connect(font_1,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(font_2,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(font_3,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(font_4,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(font_5,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(font_6,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(font_7,SIGNAL(pressed()),this,SLOT(chineseSelectFont()));
    connect(listUp,SIGNAL(pressed()),this,SLOT(pageUp()));
    connect(listDown,SIGNAL(pressed()),this,SLOT(pageDown()));
}

/**
 * @brief myInputMethod::~myInputMethod
 */
myInputMethod::~myInputMethod()
{
    
}

/**
 * @brief myInputMethod::getText
 * @return
 */
QString myInputMethod::getText()
{
    return textLbe->text();
}
/**
 * @brief myInputMethod::setTitle
 * @param text
 */
void myInputMethod::setTitle(QString text)
{
    titleLabel->setText(text);
}

void myInputMethod::setType(int type)
{
    if (type == KB_DC){
        method = english;
        listUp->setHidden(true);
        listDown->setHidden(true);
        methodBtn->setHidden(true);
        enableDelayCodeInfo->show();

        font_1->setHidden(true);
        font_2->setHidden(true);
        font_3->setHidden(true);
        font_4->setHidden(true);
        font_5->setHidden(true);
        font_6->setHidden(true);
        font_7->setHidden(true);
    }else {
        listUp->setHidden(false);
        listDown->setHidden(false);
        methodBtn->setHidden(false);
        enableDelayCodeInfo->hide();

        font_1->setHidden(false);
        font_2->setHidden(false);
        font_3->setHidden(false);
        font_4->setHidden(false);
        font_5->setHidden(false);
        font_6->setHidden(false);
        font_7->setHidden(false);
    }
}

/**
 * @brief myInputMethod::createButtons
 * 初始化
 */
void myInputMethod::createButtons(void)
{
    method = myLan.nLang == LANG_ENG?english:chinese;

    encrypt_remind = myLan.encrypt_remind;
    unremind = myLan.unremind;

    strText = strInput;
    setWindowFlags(Qt::FramelessWindowHint);

    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    this->setMinimumSize(50,50);
    this->setFocusPolicy(Qt::NoFocus);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    widget = new QWidget(this);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setGeometry(WIN_WID*0.15, 150, WIN_WID*0.7, WIN_HEI*0.68);
    widget->setStyleSheet("background-color: #D8D8D8;");
    widget->show();

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Laypout principal vertical
    mainVerticalLayout = new QVBoxLayout(widget);
    mainVerticalLayout->setSpacing(3);
    mainVerticalLayout->setMargin(3);

    // Layout Horizontal title -----------------------------------------------
    QHBoxLayout *horizontalLayout_0;
    titleLabel = new QLabel(strTitle, widget);
    titleLabel->setMaximumHeight(85);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(sizePolicy);
    titleLabel->setStyleSheet("background-color: #D8D8D8;border:0px;font:30px;");
    enableDelayCodeInfo = new QPushButton(encrypt_remind);
    enableDelayCodeInfo->setIconSize(QSize(BTN_HEI, BTN_HEI-10));
    enableDelayCodeInfo->setMaximumWidth(BTN_WID*2);
    enableDelayCodeInfo->setFixedHeight(60);
    enableDelayCodeInfo->setSizePolicy(sizePolicy);
    enableDelayCodeInfo->hide();
    horizontalLayout_0 = new QHBoxLayout();
    horizontalLayout_0->setSpacing(9);
    horizontalLayout_0->addWidget(titleLabel);
    horizontalLayout_0->addWidget(enableDelayCodeInfo,0,Qt::AlignVCenter);

    // Layout Horizontal 1 -----------------------------------------------
    delBtn = new QPushButton(myLan.del, widget);
    delBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    delBtn->setMaximumWidth(BTN_WID);
    delBtn->setFixedHeight(85);
    delBtn->setSizePolicy(sizePolicy);
    textLbe = new QLineEdit(strInput, widget);
    textLbe->setFixedHeight(85);
    textLbe->setAlignment(Qt::AlignCenter);
    textLbe->setSizePolicy(sizePolicy);
    textLbe->setStyleSheet("background-color: white;border:0px;font:60px;");

    horizontalLayout_1 = new QHBoxLayout();
    horizontalLayout_1->setSpacing(3);
    horizontalLayout_1->addWidget(textLbe);
    horizontalLayout_1->addWidget(delBtn);

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->addLayout(horizontalLayout_0);
    verticalLayout->addLayout(horizontalLayout_1);
    mainVerticalLayout->addLayout(verticalLayout);

    // Layout Horizontal 2 -----------------------------------------------
    listUp   = new QPushButton("", widget);
    listUp->setIcon(QIcon(":/res/png/Arrow_Left.png"));
    listUp->setIconSize(QSize(BTN_HEI, BTN_HEI));
    listUp->setFixedHeight(85);
    listUp->setSizePolicy(sizePolicy);
    listUp->setStyleSheet("background-color:white;");
    listDown = new QPushButton("", widget);
    listDown->setIcon(QIcon(":/res/png/Arrow_Right.png"));
    listDown->setIconSize(QSize(BTN_HEI, BTN_HEI));
    listDown->setSizePolicy(sizePolicy);
    listDown->setFixedHeight(85);
    listDown->setStyleSheet("background-color:white;");
    methodBtn = new QPushButton(myLan.Eng, widget);
//    methodBtn->setIcon(QIcon(":/res/png/Input_Language.png"));
//    methodBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    methodBtn->setSizePolicy(sizePolicy);
    methodBtn->setFixedHeight(85);
    methodBtn->setStyleSheet("background-color:white;");
    pinyin    = new QLineEdit(tr(""), widget);
    pinyin->hide();
    methodBtn->setFocusPolicy(Qt::NoFocus);
    delBtn->setFocusPolicy(Qt::NoFocus);
    textLbe->setFocusPolicy(Qt::NoFocus);
    listUp->setFocusPolicy(Qt::NoFocus);
    listDown->setFocusPolicy(Qt::NoFocus);
    pinyin->setFocusPolicy(Qt::NoFocus);

    font_1    = new QPushButton(tr(""), widget);
    font_1->setFixedHeight(85);
    font_1->setStyleSheet("background-color:white;");
    font_1->setSizePolicy(sizePolicy);
    font_2    = new QPushButton(tr(""), widget);
    font_2->setFixedHeight(85);
    font_2->setStyleSheet("background-color:white;");
    font_2->setSizePolicy(sizePolicy);
    font_3    = new QPushButton(tr(""), widget);
    font_3->setFixedHeight(85);
    font_3->setStyleSheet("background-color:white;");
    font_3->setSizePolicy(sizePolicy);
    font_4    = new QPushButton(tr(""), widget);
    font_4->setFixedHeight(85);
    font_4->setStyleSheet("background-color:white;");
    font_4->setSizePolicy(sizePolicy);
    font_5    = new QPushButton(tr(""), widget);
    font_5->setFixedHeight(85);
    font_5->setStyleSheet("background-color:white;");
    font_5->setSizePolicy(sizePolicy);
    font_6    = new QPushButton(tr(""), widget);
    font_6->setFixedHeight(85);
    font_6->setStyleSheet("background-color:white;");
    font_6->setSizePolicy(sizePolicy);
    font_7    = new QPushButton(tr(""), widget);
    font_7->setFixedHeight(85);
    font_7->setStyleSheet("background-color:white;");
    font_7->setSizePolicy(sizePolicy);

    font_1->setMinimumWidth(30);
    font_2->setMinimumWidth(30);
    font_3->setMinimumWidth(30);
    font_4->setMinimumWidth(30);
    font_5->setMinimumWidth(30);
    font_6->setMinimumWidth(30);
    font_7->setMinimumWidth(30);
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(3);
    horizontalLayout_2->addWidget(listUp);
    horizontalLayout_2->addWidget(font_1);
    horizontalLayout_2->addWidget(font_2);
    horizontalLayout_2->addWidget(font_3);
    horizontalLayout_2->addWidget(font_4);
    horizontalLayout_2->addWidget(font_5);
    horizontalLayout_2->addWidget(font_6);
    horizontalLayout_2->addWidget(font_7);
    horizontalLayout_2->addWidget(listDown);
    horizontalLayout_2->addWidget(methodBtn);
    mainVerticalLayout->addLayout(horizontalLayout_2);

    for(int i=0; i < 37; i++) {
        button.append(new QPushButton(".", widget));
        button[i]->setFixedHeight(85);
        button[i]->setFocusPolicy(Qt::NoFocus);
        button[i]->setSizePolicy(sizePolicy);
        button[i]->setStyleSheet("background-color:white;");
    }

    // Layout Horizontal 3-----------------------------------------------
    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setSpacing(3);

    for (int i = 0; i < 10; i++) {
        horizontalLayout_3->addWidget(button[i]);
    }

    mainVerticalLayout->addLayout(horizontalLayout_3);

    // Layout Horizontal 4 -----------------------------------------------
    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setSpacing(3);

    for(int i=10; i < 20; i++) {
        horizontalLayout_4->addWidget(button[i]);
    }

    mainVerticalLayout->addLayout(horizontalLayout_4);

    // Layout Horizontal 5 -----------------------------------------------
    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->setSpacing(3);
    horizontalLayout_5->setContentsMargins(button[0]->width()/2, 0, button[0]->width()/2, 0);

    for(int i=20; i < 29; i++)
    {
        horizontalLayout_5->addWidget(button[i]);
    }

    mainVerticalLayout->addLayout(horizontalLayout_5);

    // Layout Horizontal 6 -----------------------------------------------
    horizontalLayout_6 = new QHBoxLayout();
    horizontalLayout_6->setContentsMargins(button[0]->width()-20, 0, button[0]->width()-20, 0);
    horizontalLayout_6->setSpacing(3);

    horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout_6->addItem(horizontalSpacer);

    for(int i=29; i < 37; i++) {
        horizontalLayout_6->addWidget(button[i]);
    }

    horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    horizontalLayout_6->addItem(horizontalSpacer_2);

    mainVerticalLayout->addLayout(horizontalLayout_6);

    // Layout Horizontal 7 -----------------------------------------------
    horizontalLayout_7 = new QHBoxLayout();
    horizontalLayout_7->setSpacing(3);

    //确定按钮
    buttonC5 = new QPushButton(myLan.ok, widget);
    buttonC5->setFixedHeight(85);
//    buttonC5->setMaximumWidth(180);
    buttonC5->setStyleSheet("background-color:white;");
    buttonC5->setSizePolicy(sizePolicy);
    horizontalLayout_7->addWidget(buttonC5);

    //字母与符号切换
    buttonC3 = new QPushButton("", widget);
    buttonC3->setText(QString(".?123"));
    buttonC3->setFixedHeight(85);
//    buttonC3->setMaximumWidth(180);
    buttonC3->setStyleSheet("background-color:white;");
    buttonC3->setSizePolicy(sizePolicy);
    horizontalLayout_7->addWidget(buttonC3);

    //空格
    buttonC4 = new QPushButton(myLan.space, widget);
    buttonC4->setFixedHeight(85);
//    buttonC4->setMaximumWidth(180);
    buttonC4->setStyleSheet("background-color:white;");
    buttonC4->setSizePolicy(sizePolicy);
    horizontalLayout_7->addWidget(buttonC4);

    //! 取消按钮
    buttonC6 = new QPushButton(myLan.cancel, widget);
    buttonC6->setFixedHeight(85);
//    buttonC6->setMaximumWidth(180);
    buttonC6->setStyleSheet("background-color:white;");
    buttonC6->setSizePolicy(sizePolicy);
    horizontalLayout_7->addWidget(buttonC6);

    mainVerticalLayout->addLayout(horizontalLayout_7);

    displayAlphabet();

    enableInputMethod();
    currentPage = 0;
    pageCount = 0;
    findNothing = true;

    return;
}
/**
 * @brief myInputMethod::enableInputMethod
 * 输入法使能
 */
void myInputMethod::enableInputMethod()
{
    if (method == english) {
        pinyin->setEnabled(false);
        listUp->setEnabled(false);
        font_1->setEnabled(false);
        font_2->setEnabled(false);
        font_3->setEnabled(false);
        font_4->setEnabled(false);
        font_5->setEnabled(false);
        listDown->setEnabled(false);
        methodBtn->setEnabled(true);
        methodBtn->setText(myLan.Eng);
    }
    else if (method == chinese){
        pinyin->setEnabled(true);
        methodBtn->setEnabled(true);
        methodBtn->setText(myLan.chinese);
        font_1->setEnabled(true);
        font_2->setEnabled(true);
        font_3->setEnabled(true);
        font_4->setEnabled(true);
        font_5->setEnabled(true);
        listDown->setEnabled(true);
        listUp->setEnabled(true);
    }
}

/**
 * @brief myInputMethod::showEnableDelayCodeCbx
 * 使能加密提醒按钮
 */
void myInputMethod::showEnableDelayCodeCbx(bool isChecked)
{
    if(isChecked){
        enableDelayCodeInfo->setStyleSheet(g_enableBtnStyle);
        enableDelayCodeInfo->setText(unremind);
    }else{
        enableDelayCodeInfo->setStyleSheet(g_disableBtnStyle);
        enableDelayCodeInfo->setText(encrypt_remind);
    }
}


/**
 * @brief myInputMethod::getDelayCodeInfoValue
 * @return
 * 加密提醒功能使能获取
 */
int myInputMethod::getDelayCodeInfoValue()
{
    if (enableDelay)
    {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @brief myInputMethod::displayAlphabet
 * 显示大写字母
 */
void myInputMethod::displayAlphabet(void)
{
    unsigned char *key;

    unsigned char qwertyUpper[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0','Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '-'};
    key = qwertyUpper;


    for(int i=0; i < 37; i++)
    {
       button[i]->setText(QChar::fromLatin1(key[i]));
    }

    return;
}
/**
 * @brief myInputMethod::displayNumber
 * 显示字母
 */
void myInputMethod::displayNumber(void)
{
    unsigned short int number[] = {'[', ']', '{', '}', '#', '%', '^', '*', '|', '=', '_', '/', ':', ';', '(', ')', '+', '&', '@', '"', '.', ',', '?', '!', '\\', '%', '<', '>', '$',0x20ac,0x00B2,39,' ',' ',' ',' ',' '};

    for(int i=0; i < 37; i++)
    {
        if(number[i] != '&')
        {
            button[i]->setText(QChar(number[i]));
        }
        else
        {
            button[i]->setText(QString(QChar(number[i]))+QString(QChar(number[i])));
        }
    }

    return;
}
/**
 * @brief myInputMethod::displaySpecialChar
 * 显示特殊字符
 */
void myInputMethod::displaySpecialChar(void)
{
    unsigned char SpecialChar[] = {'[', ']', '{', '}', '#', '%', '^', '*', '+', '=', '_', '\\', '|', '~', '<', '>', '$', ' ', ' ', ' ', '.', ',', '?', '!', '\'', ' '};

    for(int i=0; i < 26; i++)
    {
        button[i]->setText(QChar::fromLatin1(SpecialChar[i]));
    }

    return;
}

/**
 * @brief myInputMethod::buttonC3Function
 * 字母与特殊字符的切换
 */
void myInputMethod::buttonC3Function(void)
{
    if(buttonC3->text() == QString(".?123"))
    {
        pinyin->setEnabled(false);
        listUp->setEnabled(false);
        font_1->setEnabled(false);
        font_2->setEnabled(false);
        font_3->setEnabled(false);
        font_4->setEnabled(false);
        font_5->setEnabled(false);
        listDown->setEnabled(false);
        methodBtn->setEnabled(false);

        buttonC3->setText(QString("ABC"));

        displayNumber();
    }
    else {
        enableInputMethod();
        buttonC3->setText(QString(".?123"));

        displayAlphabet();
    }

    return;
}
/**
 * @brief myInputMethod::buttonC4Function
 * 空格
 */
void myInputMethod::buttonC4Function(void)
{
    QString str;
    str = textLbe->text();
    str += " ";
    textLbe->setText(str);
}
/**
 * @brief myInputMethod::buttonC5Function
 * 确定按钮
 */
void myInputMethod::buttonC5Function(void)
{
    accept();
}
/**
 * @brief myInputMethod::buttonC6Function
 * 取消按钮
 */
void myInputMethod::buttonC6Function(void)
{
    reject();
}
/**
 * @brief myInputMethod::sendChar
 * @param indexOfCharToSend
 * 所按按钮上面的字符
 */
void myInputMethod::sendChar(int indexOfCharToSend)
{
    if(buttonC3->text() == QString(".?123"))
    {
        if (method == chinese) {
            if (indexOfCharToSend > 9) {
               pinyin->insert(button[indexOfCharToSend]->text().toLower());
               strPinyin = pinyin->text();
               transPy(strPinyin);
               textLbe->setText(strText+strPinyin);
            }
            else {
                textLbe->insert(button[indexOfCharToSend]->text());
                strText = textLbe->text();
            }
        }
        else {
            textLbe->insert(button[indexOfCharToSend]->text());
            strText = textLbe->text();
        }
    }
    else {
         textLbe->insert(button[indexOfCharToSend]->text());
         strText = textLbe->text();
    }

    return;
}
/**
 * @brief myInputMethod::onDelBtnClicked
 * 删除输入
 * 当中文输入法启用时，拼音内有输入时，删除拼音，否则删除汉字
 */
void myInputMethod::onDelBtnClicked()
{
    if (strPinyin == "") {
        strText.remove(strText.length()-1,strText.length());
        textLbe->setText(strText);
    }
    else {
        strPinyin = pinyin->text();
        strPinyin.remove(strPinyin.length()-1,strPinyin.length());
        pinyin->setText(strPinyin);
        textLbe->setText(strText+strPinyin);
        transPy(strPinyin);
    }
}

void myInputMethod::onEnableDelayCode(void)
{
    enableDelay = !enableDelay;
    if (enableDelay) {
        enableDelayCodeInfo->setStyleSheet(g_enableBtnStyle);
        enableDelayCodeInfo->setText(unremind);
    }
    else {
        enableDelayCodeInfo->setStyleSheet(g_disableBtnStyle);
        enableDelayCodeInfo->setText(encrypt_remind);
    }
}

/**
 * @brief myInputMethod::changeInputMethod
 * 输入法切换
 */
void myInputMethod::changeInputMethod()
{
   if(method == english)
   {
       method = chinese;
   }
   else {
       method = english;
       this->clear(true);
       textLbe->setText(strText+strPinyin);
       transPy(strPinyin);
   }
   enableInputMethod();
}
/**
 * @brief myInputMethod::pageUp
 * 汉字列表，上一页
 */
void myInputMethod::pageUp()
{
    currentPage--;
    if(currentPage > -1)
    {
        listDown->setEnabled(true);
        if(currentPage-- == 0)
            listUp->setDisabled(true);
        currentPage++;
        showPage(currentPage);
    }
    else
    {
        currentPage++;
        listUp->setDisabled(true);
    }
}
/**
 * @brief myInputMethod::pageDown
 * 汉字列表，下一页
 */
void myInputMethod::pageDown()
{
    currentPage++;
    if(currentPage < pageCount)
    {
         listUp->setEnabled(true);
         if(currentPage++ == pageCount-1)
            listDown->setDisabled(true);
         currentPage--;
         showPage(currentPage);
     }
    else
    {
        currentPage--;
        listDown->setDisabled(true);
     }
}
/**
 * @brief myInputMethod::chineseSelectFont
 * 选择汉字
 */
void myInputMethod::chineseSelectFont()
{
    QPushButton *btn = (QPushButton*)QObject::sender();
    strText += btn->text();
    textLbe->setText(strText);
    readyCharList.clear();
    this->clear(true);
}
/**
 * @brief myInputMethod::clear
 * @param clearInputString
 * 清除汉字列表
 */
void myInputMethod::clear(bool clearInputString)
{
    if(clearInputString) {
        pinyin->clear();
        strPinyin = pinyin->text();
    }
    font_1->setText("");
    font_2->setText("");
    font_3->setText("");
    font_4->setText("");
    font_5->setText("");
    font_6->setText("");
    font_7->setText("");

}
/**
 * @brief myInputMethod::transPy
 * @param pyInput
 * @return
 * 输入拼音，翻译成汉字
 */
QString myInputMethod::transPy(QString pyInput)
{
    int count = 0;
    QString py = pyInput;
    readyCharList.clear();

    readyCharList = struGsh.pinyin_map.values(pyInput);
    count = readyCharList.size();

    if(count%7 == 0)
    {
        pageCount = count/7;
    }
    else
    {
        pageCount = (count/7)+1;
    }

    if(count == 0)
    {
        this->clear(false);
        return NULL;
    }

    listUp->setEnabled(false);
    listDown->setEnabled(true);
    currentPage = 0;
    findNothing = true;
    showPage(currentPage);
    //currentPage = 0;
    return py;
}
/**
 * @brief myInputMethod::showPage
 * @param index
 * 汉字显示列表，每一页
 */
void myInputMethod::showPage(int index)
{
    int n =0;
    QString chinese[7];
    index *= 7;
    while((n < 7) && (readyCharList.count() > index))
    {
        chinese[n] = readyCharList[readyCharList.count()-index-1];
        n++;
        index++;
    }

    font_1->setText(tr(chinese[0].toUtf8()));
    font_2->setText(tr(chinese[1].toUtf8()));
    font_3->setText(tr(chinese[2].toUtf8()));
    font_4->setText(tr(chinese[3].toUtf8()));
    font_5->setText(tr(chinese[4].toUtf8()));
    font_6->setText(tr(chinese[5].toUtf8()));
    font_7->setText(tr(chinese[6].toUtf8()));
}

/*!
 * \brief myInputMethod::setPasswordMode
 * \param flag
 */
void myInputMethod::setPasswordMode(bool flag)
{
    if (flag) {
        textLbe->setEchoMode(QLineEdit::Password);

        method = english;
        enableInputMethod();
    } else {
        textLbe->setEchoMode(QLineEdit::Normal);
    }
}

/*!
 * \brief myInputMethod::setChineseInputEn
 * \param flag
 */
void myInputMethod::setChineseInputEn(bool flag)
{
    if (flag) {
        pinyin->setEnabled(true);
        methodBtn->setEnabled(true);
        font_1->setEnabled(true);
        font_2->setEnabled(true);
        font_3->setEnabled(true);
        font_4->setEnabled(true);
        font_5->setEnabled(true);
        listDown->setEnabled(true);
        listUp->setEnabled(true);
    } else {
        pinyin->setEnabled(false);
        listUp->setEnabled(false);
        font_1->setEnabled(false);
        font_2->setEnabled(false);
        font_3->setEnabled(false);
        font_4->setEnabled(false);
        font_5->setEnabled(false);
        listDown->setEnabled(false);
        methodBtn->setEnabled(false);
    }
}
