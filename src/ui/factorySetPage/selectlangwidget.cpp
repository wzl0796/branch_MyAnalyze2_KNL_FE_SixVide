#include "selectlangwidget.h"
#include <QVBoxLayout>
#include "params/define.h"
#include "core/globalflow/mylanguage.h"

selectLangWidget::selectLangWidget(QWidget *parent) : QWidget(parent)
{
    m_langEnaWidget = new QWidget();
    m_langEnaWidget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    //! 支持语言列表
    m_allListWidget = new QListWidget();
    m_allListWidget->setFixedSize(GROUP_WID+80, WIN_HEI-400);
    m_allListWidget->setStyleSheet(QString("QListWidget{border:1px solid gray; border-radius:10px;outline:0px;"
                                    "background-color:white;font:30px;}")
                                    +QString("QListWidget::Item:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen)
                                    +QString("QListWidget::Item:first{border-radius:5px}"));

    m_allList = myLan.getLanguageList();
    for (int i = 0; i < m_allList.count(); i++) {
        int index = m_allList.at(i);
        m_allListWidget->addItem(myLan.getLanguageListName(index, false));
        m_allListWidget->item(i)->setSizeHint(QSize(GROUP_WID, BTN_HEI));
    }

    //! 选择语言列表
    m_selListWidget = new QListWidget();
    m_selListWidget->setFixedSize(GROUP_WID+80, WIN_HEI-400);
    m_selListWidget->setStyleSheet(QString("QListWidget{border:1px solid gray; border-radius:10px;outline:0px;"
                                    "background-color:white;font:30px;}")
                                    +QString("QListWidget::Item:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen)
                                    +QString("QListWidget::Item:first{border-radius:5px}"));

    //! 添加 & 删除
    m_addBtn = new QPushButton(myLan.add);
    m_addBtn->setIcon(QIcon(":/res/png/Arrow_Right.png"));
    m_addBtn->setStyleSheet("QPushButton{text-align:left;}");
    m_addBtn->setIconSize(QSize(BTN_HEI-5, BTN_HEI-5));
    m_addBtn->setFixedSize(QSize(BTN_WID+50, BTN_HEI));
    m_delBtn = new QPushButton(myLan.del);
    m_delBtn->setIcon(QIcon(":/res/png/Arrow_Left.png"));
    m_delBtn->setStyleSheet("QPushButton{text-align:left;}");
    m_delBtn->setIconSize(QSize(BTN_HEI-5, BTN_HEI-5));
    m_delBtn->setFixedSize(QSize(BTN_WID+50, BTN_HEI));

    //! 确定 & 取消
    m_sureBtn = new QPushButton(myLan.ok);
    m_sureBtn->setFixedSize(QSize(BTN_WID, BTN_HEI));
    m_cancelBtn = new QPushButton(myLan.cancel);
    m_cancelBtn->setFixedSize(QSize(BTN_WID, BTN_HEI));
    m_sureBtn->setStyleSheet(g_enableBtnStyle);
    m_cancelBtn->setStyleSheet(g_enableBtnStyle);

    QVBoxLayout *langSetLayout = new QVBoxLayout;
    langSetLayout->addStretch();
    langSetLayout->addWidget(m_addBtn);
    langSetLayout->addWidget(m_delBtn);
    langSetLayout->addStretch();

    QHBoxLayout *langListLayout = new QHBoxLayout;
    langListLayout->addWidget(m_allListWidget);
    langListLayout->addLayout(langSetLayout);
    langListLayout->addWidget(m_selListWidget);

    QHBoxLayout *langBoxLayout = new QHBoxLayout;
    langBoxLayout->addWidget(m_sureBtn);
    langBoxLayout->addStretch();
    langBoxLayout->addWidget(m_cancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_langEnaWidget);
    mainLayout->addLayout(langListLayout);
    mainLayout->addLayout(langBoxLayout);

    QVBoxLayout *Lay = new QVBoxLayout(this);
    Lay->setMargin(0);
    Lay->addWidget(m_langEnaWidget);

    connect(m_addBtn,    SIGNAL(clicked()), this, SLOT(onAddBtnClicked()));
    connect(m_delBtn,    SIGNAL(clicked()), this, SLOT(onDelBtnClicked()));
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(onCancelBtnClicked()));
    connect(m_sureBtn,   SIGNAL(clicked()), this, SLOT(onSureBtnClicked()));
}

void selectLangWidget::updateSelectWidget()
{
    //! 刷新选择语言列表
    m_selListWidget->clear();
    for (int i = 0; i < m_selList.count(); i++) {
        int langId = m_selList.at(i);
        m_selListWidget->addItem(myLan.getLanguageListName(langId, false));
        m_selListWidget->item(i)->setSizeHint(QSize(BTN_HEI, BTN_HEI));
    }
}

void selectLangWidget::refreshDisplay()
{
    m_selList.clear();
    for (int i = 0; i < myLan.nLangList[0]; i++) {
        m_selList.append(myLan.nLangList[i+1]);
    }
    //! 内置中文支持
    if (m_selList.count() == 0) {
        m_selList << LANG_CHS;
    }
    updateSelectWidget();
}

/**
 * @brief setLangWidget::onm_addBtnClicked
 * 响应语言设置界面增加功能按键
 */
void selectLangWidget::onAddBtnClicked()
{
    //! 确认语言列表非空
    if (m_allListWidget->count() <= 0)
    {
        return;
    }

    //! 确认是否选择语言
    if (m_allListWidget->currentRow() == -1)
    {
        return;
    }

    if (!m_allListWidget->currentItem()->isSelected())
    {
        return;
    }

    //! 确认语言是否存在
    int langId = m_allList.at(m_allListWidget->currentRow());
    if (m_selList.indexOf(langId) != -1)
    {
        return ;
    }

    //! 语言移至选择列表
    m_selList.append(langId);

    //! 刷新语言选择列表
    updateSelectWidget();
}

/**
 * @brief setLangWidget::onm_delBtnClicked
 * 响应语言设置界面移除功能按键
 */
void selectLangWidget::onDelBtnClicked()
{
    //! 确认选择列表非空
    if (m_selListWidget->count() <= 1)
        return;

    //! 确认是否选择语言
    if (m_selListWidget->currentRow() == -1)
        return;

    if (!m_selListWidget->currentItem()->isSelected())
        return;

    //! 锁定当前系统语言
    int langId = m_selList.at(m_selListWidget->currentRow());
    if (langId == myLan.nLang)
        return;

    //! 锁定简体中文支持
    if (langId == LANG_CHS)
        return;

    //! 语言移出选择列表
    m_selList.removeAt(m_selListWidget->currentRow());

    //! 刷新语言选择列表
    updateSelectWidget();
}

/**
 * @brief setLangWidget::onCancelBtnClicked
 * 响应语言设置界面取消功能按键
 */
void selectLangWidget::onCancelBtnClicked()
{
    m_selList.clear();
    for (int i = 0; i < myLan.nLangList[0]; i++) {
        m_selList.append(myLan.nLangList[i+1]);
    }

    //! 刷新语言选择列表
    updateSelectWidget();
    emit updateList();
}

/**
 * @brief setLangWidget::onSureBtnClicked
 * 响应语言设置界面确定功能按键
 */
void selectLangWidget::onSureBtnClicked()
{
    myLan.nLangList[0] = m_selList.count();
    for (int i = 0; i < m_selList.count(); i++) {
        myLan.nLangList[i+1] = m_selList.at(i);
    }
    myLan.saveLanguageSetting();
    emit updateList();
}
