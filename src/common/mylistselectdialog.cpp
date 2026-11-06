/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylistselectdialog.cpp
 * \brief       列表选择对话框控件实现文件
 * \date        2025.12.23
 */
#include "mylistselectdialog.h"

myListSelectDialog::myListSelectDialog(const QString &title, const QStringList &items, QWidget *parent)
    : QDialog(parent)
    , m_title(title)
    , m_items(items)
    , m_listWidget(nullptr)
    , m_okBtn(nullptr)
    , m_cancelBtn(nullptr)
    , m_dialogWidth(500)
    , m_dialogHeight(600)
{
    createUI();
    connectSignals();
}

myListSelectDialog::~myListSelectDialog()
{
}

QString myListSelectDialog::getSelectedText() const
{
    QListWidgetItem *currentItem = m_listWidget->currentItem();
    if (currentItem) {
        return currentItem->text();
    }
    return QString();
}

int myListSelectDialog::getSelectedIndex() const
{
    return m_listWidget->currentRow();
}

void myListSelectDialog::setDialogSize(int width, int height)
{
    m_dialogWidth = width;
    m_dialogHeight = height;
    setFixedSize(m_dialogWidth, m_dialogHeight);
}

void myListSelectDialog::setDefaultSelection(int index)
{
    if (index >= 0 && index < m_listWidget->count()) {
        m_listWidget->setCurrentRow(index);
    }
}

void myListSelectDialog::createUI()
{
    setWindowTitle(m_title);
    setFixedSize(m_dialogWidth, m_dialogHeight);
    setStyleSheet("QDialog{background-color:white; border-radius:10px;}");

    //! 创建列表控件
    m_listWidget = new QListWidget(this);
    m_listWidget->setStyleSheet(QString("QListWidget{border:1px solid lightgray; "
                                        "background-color:white; font-size:18px;}"
                                        "QListWidget::Item:hover{background-color:lightgray;}"
                                        "QListWidget::Item:selected{"
                                        "background-color:rgb(0,150,136); color:white;}"));
    m_listWidget->addItems(m_items);
    
    //! 默认选中第一项
    if (m_listWidget->count() > 0) {
        m_listWidget->setCurrentRow(0);
    }

    //! 创建按钮
    m_okBtn = new QPushButton(myLan.ok, this);
    m_okBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_okBtn->setStyleSheet(g_enableBtnStyle);

    m_cancelBtn = new QPushButton(myLan.cancel, this);
    m_cancelBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_cancelBtn->setStyleSheet(g_enableBtnStyle);

    //! 按钮布局
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->addStretch();
    btnLay->addWidget(m_okBtn);
    btnLay->addSpacing(20);
    btnLay->addWidget(m_cancelBtn);
    btnLay->addStretch();

    //! 主布局
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addWidget(m_listWidget);
    mainLay->addLayout(btnLay);
}

void myListSelectDialog::connectSignals()
{
    connect(m_okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &QDialog::accept);
}
