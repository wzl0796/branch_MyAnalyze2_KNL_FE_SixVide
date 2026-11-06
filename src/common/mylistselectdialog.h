/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylistselectdialog.h
 * \brief       列表选择对话框控件头文件
 * \date        2025.12.23
 */
#ifndef MYLISTSELECTDIALOG_H
#define MYLISTSELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include "params/define.h"
#include "core/globalflow/mylanguage.h"

/*!
 * \brief 列表选择对话框控件类
 */
class myListSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit myListSelectDialog(const QString &title, const QStringList &items, QWidget *parent = 0);
    ~myListSelectDialog();

    /*!
     * \brief 获取选中的文本
     * \return 选中的文本，如果未选中则返回空字符串
     */
    QString getSelectedText() const;

    /*!
     * \brief 获取选中的索引
     * \return 选中的索引，如果未选中则返回-1
     */
    int getSelectedIndex() const;

    /*!
     * \brief 设置对话框大小
     * \param width 宽度
     * \param height 高度
     */
    void setDialogSize(int width, int height);

    /*!
     * \brief 设置默认选中项
     * \param index 默认选中的索引
     */
    void setDefaultSelection(int index);

private:
    void createUI();
    void connectSignals();

private:
    QString m_title;
    QStringList m_items;
    
    QListWidget *m_listWidget;
    QPushButton *m_okBtn;
    QPushButton *m_cancelBtn;
    
    int m_dialogWidth;
    int m_dialogHeight;
};

#endif // MYLISTSELECTDIALOG_H
