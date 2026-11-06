/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        myinputmethod.h
 * \brief       中英文输入框控件头文件
 * \date        2015.01.14
 */
#ifndef MYINPUTMETHOD_H
#define MYINPUTMETHOD_H

#include <QDialog>
#include <QCheckBox>
#include <QSignalMapper>
#include <QVector>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include "mypinyin.h"
#include "core/globalflow/mylanguage.h"
#include "params/globalparams.h"

#define KB_NORM   0
#define KB_DC     1

enum inputMethod {
    chinese,
    english};

/*!
 * \brief 中英文输入框控件类
 */
class myInputMethod : public QDialog
{
    Q_OBJECT
    
public:
    myInputMethod(QString titleText, QString inputText ,QWidget *parent = 0, bool enaDelay = false);
    ~myInputMethod();
    QString getText(void);
    void setTitle(QString text);
    void setType(int type);
    void showEnableDelayCodeCbx(bool isChecked);
    int  getDelayCodeInfoValue();  /*!<< 加密码提醒使能标志*/
    void setPasswordMode(bool flag);
    void setChineseInputEn(bool flag);

private slots:
    void createButtons(void);   //创建按钮
    void displayAlphabet(void); //显示字母
    void displayNumber(void);   //显示数字
    void displaySpecialChar(void);//显示特殊符号
    void sendChar(int indexOfCharToSend);//打印所按的符号
    void changeInputMethod();  //更换输入法类型english or chinese
    void chineseSelectFont();  //汉字选择
    void pageUp();             //上一页
    void pageDown();           //下一页
    void clear(bool clearInputString);//清零
    void enableInputMethod();   //输入法使能

    void onDelBtnClicked(void); //删除字符
    void buttonC3Function(void);//字母与数字切换
    void buttonC4Function(void);//空格
    void buttonC5Function(void);//确定
    void buttonC6Function(void);//取消

    void onEnableDelayCode(void);

private:
    QWidget *parentWidget;
    QString     strTitle;
    QString     strInput;

    QLabel     *titleLabel; //文字显示区
    QPushButton *enableDelayCodeInfo;    //删除按钮
    bool enableDelay;

    QLineEdit   *textLbe;    //文字显示区
    QPushButton *delBtn;    //删除按钮

    QPushButton *listUp;    //上一页面
    QLineEdit   *pinyin;    //拼音显示区
    QPushButton *font_1;    //汉字1
    QPushButton *font_2;    //汉字2
    QPushButton *font_3;    //汉字3
    QPushButton *font_4;    //汉字4
    QPushButton *font_5;    //汉字5
    QPushButton *font_6;    //汉字6
    QPushButton *font_7;    //汉字7
    QPushButton *listDown;  //下一页
    QPushButton *methodBtn; //中英文切换按钮

    QString strPinyin;
    QString strText;
    QStringList inputCharList;
    QString transPy(QString pyInput);   //转换成汉字

    bool findNothing;                   //此字是否存在
    inputMethod method;                 //输入法模式
    int flagKey;
    int currentPage;                    //当前页面

    QStringList readyCharList;          //显示汉字列表
    void showPage(int index);           //显示页面
    int pageCount;                      //页面数

    /* Boutons et signalMapper */
    QSignalMapper *signalMapper;
    QSignalMapper *signalMapper2;

    QVector<QPushButton *> button;      //26个字母

    QPushButton *buttonC3;              //字母与拼音切换
    QPushButton *buttonC4;              //空格
    QPushButton *buttonC5;              //OK
    QPushButton *buttonC6;              //取消

    //布局界面
    QWidget *verticalLayoutWidget;
    QVBoxLayout *mainVerticalLayout;

    QHBoxLayout *horizontalLayout_0;
    QHBoxLayout *horizontalLayout_1;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_7;

    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;

    QWidget *widget;

    //! 词根
    QString encrypt_remind;
    QString unremind;
};

#endif // MYINPUTMETHOD_H
