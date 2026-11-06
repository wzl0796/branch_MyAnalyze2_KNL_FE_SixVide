/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mymessagebox.h
 * \brief       消息框控件头文件
 * \date        2015.01.14
 */
#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsItem>
#include <QTabBar>
#include <QTimer>
#include <QLayout>
#include <QDateTime>
#include <QComboBox>
#include <QListWidget>
#include <QCoreApplication>
#include "params/define.h"
#include "src/common/myinputmethod.h"
#include "core/globalflow/globalflow.h"
#include "core/globalflow/mylanguage.h"
#define MSG_QUES    0       //  询问操作
#define MSG_INFO    1       //  信息提示
#define MSG_WARN    2       //  警告操作
#define MSG_ABOUT   3       //  关于信息
#define MSG_TIPS    4       //  提示信息

class GraphicsView;

/*!
 * \brief 消息对话框控件类
 */
class myMessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit myMessageBox(int style, QString text, QString titletext = "", QWidget *parent = 0);
    ~myMessageBox();
    void setText(QString text);
    void setTextColor(int color);
    bool getSel();
    void setOKBtnText(QString text);
    void setCancelBtnText(QString text);
    void setautoClose(bool bclose);
    void setBatchBtntext(QString text);
    void setBatchBtnshow(bool bShow);
    void setShiftBtntext(QString text);
    void setShiftBtnshow(bool bShow);
    void setadjcontetshow(bool bShow, QStringList itemlst);
    int  getsorterschmeidx(void);
    int  getsortersourceidx(void);
    QString getBathBtntext();
    QString getshiftBtntext();
    void settextNotCenter();
    void setsortersourceidx(int nIdx);
    float getyield(){return yieldbtn->text().toFloat();}
    void setYieldInfovisible(bool bvisible){
        yieldlb->setVisible(bvisible);
        yieldbtn->setVisible(bvisible);
    }


signals:
    
public slots:
    void onOkBtnClicked();
    void onCancelBtnClicked();
    void onBatchBtnpressed();
    void onrefreshsortersorce(int nIdx);
    void onShiftBtnClicked();
    void onYieldBtnClicked();

private:
    QWidget        *widget;
    QLabel         *iconLabel;
    QPushButton    *okBtn;
    QPushButton    *cancelBtn;

    QPushButton    *batchBtn;   //! 批次
    QLabel         *textLabel;

    QLabel         *shiftLb;    //! 化验员
    QPushButton    *shiftBtn;

    QLabel          *yieldlb;
    QPushButton     *yieldbtn;

    QLabel         *m_adjtileLb; //! 联调
    QComboBox      *m_adjCob;
    QComboBox      *m_adjSourceComb;

    bool isChecked;

    QTimer *timer;
};

class myInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit myInfoWidget(QWidget *parent = 0);
    ~myInfoWidget();
    void setLabelText(QString text);
    void delayShow(void);
    void hideCentralWidget(void);
    void showSecs(int secs = 2);
    void delayHide(void);
    void setVisableCancelBtn(bool isShow = false);
    QString getLabelText();

public slots:
    void onCancelBtnClicked(void);

signals:
    void sCancelBtnClicked();

private:
    QFont        font;
    QLabel      *warnningLabel, *iconLabel;
    QWidget     *widget;
    QPushButton *cancelBtn;
};

class myBackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit myBackWidget(QWidget *parent = 0);
    ~myBackWidget();

private:
    QLabel *label;
};


extern myInfoWidget *infoWidget;
extern myBackWidget *backWidget;

#endif // MYMESSAGEBOX_H
