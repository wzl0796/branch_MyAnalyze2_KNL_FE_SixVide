/*!
 * \Copyright   Copyright (C) 2018 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setSorterCodewidget.h
 * \brief       设置数据导出类型界面类头文件
 * \date        2021.11.14
 */
#ifndef SETEXPORTTYPEWIDGET_H
#define SETEXPORTTYPEWIDGET_H

#include <QDialog>
#include <QSignalMapper>
#include <QRadioButton>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QButtonGroup>
#include "params/define.h"
#include "core/globalflow/mylanguage.h"

class setExportTypeWidget : public QDialog
{
    Q_OBJECT
public:
    explicit setExportTypeWidget(QWidget *parent = 0);

    void init(void);            /*! 初始化 */
    void createPage(void);      /*! 创建页面 */
    void connectSigAndSlt(void);/*! 连接信号&槽 */
    int  getSelIndex();         /*! 获取选择索引 */
    void settitleText(QString text);
    void setcontentText(QStringList lst);
    int  getArraytype(void);
    void setarrayBtnshow();
    void setarrayBtntext(QStringList lst);
    void setextraSelshow(){m_typeBtn[2]->show();m_typeLbl[2]->show();}
signals:

public slots:
    void onTypeBtnClicked(int);
    void onArrayBtnClicked(int index);
    void onOKBtnClicked();
    void onCancelBtnClicked();

private:
    QWidget       *m_widget;
    QButtonGroup  *m_typeBtnGroup, *m_arrayBtnGroup;
    QRadioButton  *m_typeBtn[3], *m_arrayBtn[2];
    QLabel        *m_titleLbl, *m_typeLbl[3],*m_arrayLbl[2];
    QPushButton   *m_okBtn, *m_cancelBtn;
    int            m_index,  m_arrayType;

};

#endif // SETEXPORTTYPEWIDGET_H
