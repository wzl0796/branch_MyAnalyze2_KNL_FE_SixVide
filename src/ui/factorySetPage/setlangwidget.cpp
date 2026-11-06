#include "setlangwidget.h"
#include "common/selectfiledialog.h"
#include "common/mymessagebox.h"
//#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"
//#include "core/globalflow/mylanguage.h"
#include <QTranslator>

setLangWidget::setLangWidget(QWidget *parent) :
    QWidget(parent)
{
    initLan();

    initLangParams();

    createPage();
}

/*!
 * \brief 初始化词根
 */
void setLangWidget::initLan()
{
//    if(struCnfg.IsChineseLang()) {
        change_lang_sure = myLan.confirm_switch_lang;
        switch_lang_success = myLan.switch_lang_sucess;
        update = myLan.update;
        restore = myLan.restore;
        qm_file = QString("qm%1").arg(myLan.file);
        invalid_qm = myLan.invalid_file;
        qm_not_exist = myLan.no_find_file;
        lan_apply = myLan.application;
        lan_tips = myLan.notice;
        lan_fail = myLan.failed;
        lan_success = myLan.sucess;
//    }
//    else {
//        change_lang_sure = "Language Switch";
//        switch_lang_success = "Language Switched,computer reboot in 2s";
//        update = "update";
//        restore = "restore";
//        qm_file = "qm file";
//        invalid_qm = "invalid qm file";
//        qm_not_exist = "qm file no exist";
//    }
}

/*!
 * \brief 初始化qm文件
 */
void setLangWidget::initLangParams()
{
    m_LangIdAndQmMap[LANG_CHS] = QString("zh-cn.qm");
    m_LangIdAndQmMap[LANG_ENG] = QString("en.qm");
    m_LangIdAndQmMap[LANG_RUS] = QString("ru.qm");
    m_LangIdAndQmMap[LANG_VIET] = QString("vi.qm");
    m_LangIdAndQmMap[LANG_THAILAND] = QString("th.qm");
    m_LangIdAndQmMap[LANG_SPAN] = QString("es.qm");
    m_LangIdAndQmMap[LANG_CHT] = QString("zh-tw.qm");
    m_LangIdAndQmMap[LANG_TURKEY] = QString("tr.qm");
    m_LangIdAndQmMap[LANG_AFARSI] = QString("fa.qm");
    m_LangIdAndQmMap[LANG_AFRENCH] = QString("fr.qm");
    m_LangIdAndQmMap[LANG_UYGHUR] = QString("ug.qm");
    m_LangIdAndQmMap[LANG_AKOREAN] = QString("ko.qm");
    m_LangIdAndQmMap[LANG_AARABIC] = QString("ar.qm");
    m_LangIdAndQmMap[LANG_ABULGARIAN] = QString("bg.qm");
    m_LangIdAndQmMap[LANG_ALOVAK] = QString("sk.qm");
    m_LangIdAndQmMap[LANG_APORTUGUESE] = QString("pt.qm");
    m_LangIdAndQmMap[LANG_MYANMAR] = QString("my.qm");
    m_LangIdAndQmMap[LANG_ABENGALI] = QString("bn.qm");
    m_LangIdAndQmMap[LANG_INDONESIA] = QString("id.qm");
    m_LangIdAndQmMap[LANG_POLAND] = QString("pl.qm");
    m_LangIdAndQmMap[LANG_ITALY] = QString("it.qm");
    m_LangIdAndQmMap[LANG_COMBODIA] = QString("cb.qm");
    m_LangIdAndQmMap[LANG_AGERMAN] = QString("de.qm");
    m_LangIdAndQmMap[LANG_AHUNGARIAN] = QString("hu.qm");
    m_LangIdAndQmMap[LANG_JAVANESE] = QString("jv.qm");
}

/*!
 * \brief 创建页面
 */
void setLangWidget::createPage()
{
    m_widgetChoose = new QWidget();
    createWidgetChoose();
    onUpdateLangList();

    QVBoxLayout *Lay = new QVBoxLayout(this);
    Lay->setMargin(0);
    Lay->addWidget(m_widgetChoose, 0, Qt::AlignCenter);

    connect(langList,     SIGNAL(itemSelectionChanged()), this, SLOT(onLangListRowChangedSlt()));
    connect(m_updateBtn,  SIGNAL(clicked(bool)),          this, SLOT(onUpdateBtnClicked()));
    connect(m_restoreBtn, SIGNAL(clicked(bool)),          this, SLOT(onRestoreBtnClicked()));
}

/*!
 * \brief 创建语言选择页面
 */
void setLangWidget::createWidgetChoose()
{
    langList = new QListWidget();
    langList->setFixedSize(800, 700);
    langList->setIconSize(QSize(BTN_HEI, BTN_HEI-15));
    langList->setStyleSheet(QString("QListWidget{border:1px solid gray; border-radius:10px;outline:0px;"
                                    "background-color:white;font:30px;}")
                            +QString("QListWidget::Item:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen)
                            +QString("QListWidget::Item:first{border-radius:5px}"));
    langList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_updateBtn = new QPushButton(update);
    m_updateBtn->setIcon(QIcon(":/res/png/action_refresh.png"));
    m_updateBtn->setIconSize(QSize(BTN_HEI+30, BTN_HEI-5));
    m_updateBtn->setFixedSize(BTN_WID*2, BTN_HEI);
    m_updateBtn->setStyleSheet(g_enableBtnStyle);

    m_restoreBtn = new QPushButton(restore);
    m_restoreBtn->setIcon(QIcon(":/res/png/action_reset.png"));
    m_restoreBtn->setIconSize(QSize(BTN_HEI+30, BTN_HEI-5));
    m_restoreBtn->setFixedSize(BTN_WID*2, BTN_HEI);
    m_restoreBtn->setStyleSheet(g_enableBtnStyle);

    QVBoxLayout *upRightLay = new QVBoxLayout();
    upRightLay->setSpacing(10);
    upRightLay->addWidget(m_updateBtn);
    upRightLay->addWidget(m_restoreBtn);
    upRightLay->addStretch();

    QHBoxLayout *widgetChooseLay = new QHBoxLayout();
    widgetChooseLay->addWidget(langList);
    widgetChooseLay->addLayout(upRightLay);
    m_widgetChoose->setLayout(widgetChooseLay);
}

/*!
 * \brief 语言选择页面显示使能
 */
void setLangWidget::setLangBtnViewEna(bool viewEna)
{
    m_widgetChoose->show();
}

/*!
 * \brief 隐藏语言选择页面
 */
void setLangWidget::onLangSetSlt()
{
    m_widgetChoose->hide();
}

/*!
 * \brief 更新语言列表
 */
void setLangWidget::onUpdateLangList()
{
    langList->clear();
    QStringList strList;
    for (int i = 0; i < myLan.nLangList[0]; i++) {
        nLangIndex[i] = myLan.nLangList[i+1];
        strList << myLan.getLanguageListName(nLangIndex[i], false);
    }
    for(int i = 0; i < myLan.nLangList[0]; i++){
        QListWidgetItem *listItem = new QListWidgetItem(strList.at(i));
        listItem->setIcon(myLan.getLanguageListIcon(nLangIndex[i]));
        listItem->setSizeHint(QSize(langList->width()-20,85));

        //! item布局
        QWidget *itemWg = new QWidget;
        itemWg->setContentsMargins(0,0,41,0);
        itemWg->setStyleSheet("QWidget {background: transparent;}");
        QPushButton *itemBtn = new QPushButton(lan_apply);
        itemBtn->setFixedSize(BTN_WID, BTN_HEI);
        itemBtn->setStyleSheet(g_enableBtnStyle);
        itemBtn->setObjectName(QString("apply%1").arg(i));
        connect(itemBtn, SIGNAL(clicked(bool)), this, SLOT(onApplyBtnClicked()));

        if(nLangIndex[i] != myLan.nLang)
            itemBtn->hide();
        else {
            itemBtn->show();
            m_currentIndex = i;
        }
        QHBoxLayout *itemLay = new QHBoxLayout(itemWg);
        itemLay->addWidget(itemBtn, 0, Qt::AlignRight);

        langList->addItem(listItem);
        langList->setItemWidget(listItem, itemWg);
    }
    langList->setCurrentRow(m_currentIndex);
}

/*!
 * \brief 槽函数: 语言列表切换
 */
void setLangWidget::onLangListRowChangedSlt()
{
    for(int i = 0; i < langList->count(); ++i) {
        if(i != langList->currentRow()) {
            QPushButton *itemBtn = langList->itemWidget(langList->item(i))->findChild<QPushButton*>(QString("apply%1").arg(i));
            itemBtn->hide();
        }
        else {
            QPushButton *itemBtn = langList->itemWidget(langList->item(i))->findChild<QPushButton*>(QString("apply%1").arg(i));
            itemBtn->show();
        }
    }
}

/*!
 * \brief 槽函数: 更新语言qm文件
 */
void setLangWidget::onUpdateBtnClicked()
{
    if(langList->currentRow() == -1)
        return;

    QString str = update+qm_file;
    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(str), lan_tips, g_mainWidget);
    int ret = msgBox.exec();
    if(ret == QDialog::Accepted){
#ifdef Q_OS_UNIX
        if (!gFlow.mountUDisk()) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.no_found_udisk);
            infoWidget->showSecs();
            return;
        }
#endif
        quint32 LangIdx = nLangIndex[langList->currentRow()];
        QStringList strList;
        strList << m_LangIdAndQmMap[LangIdx];

        SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget);
        dialog->setDir(MOUNT_UDISK_PATH, strList);
        if (dialog->exec() == QDialog::Accepted) {
            QString updateName = QString("%1%2").arg(MOUNT_UDISK_PATH).arg(dialog->getFileName());
            QString oldName = QString("%1%2").arg(LANG_DIR).arg(m_LangIdAndQmMap[LangIdx]);
            QString bakName = QString("%1bak_%2").arg(LANG_DIR).arg(m_LangIdAndQmMap[LangIdx]);

            QTranslator  translate;
            if(translate.load(updateName)){
                //! 备份文件
                QFile bakFile(bakName);
                QFile oldFile(oldName);
                if(!bakFile.exists()){
                    QFile::copy(oldName,bakName);
                }
                oldFile.remove();
                //! 替换文件
                if(!QFile::copy(updateName,oldName)){
                    QFile::copy(bakName,oldName);
                    infoWidget->raise();
                    infoWidget->setLabelText(myLan.update_lang_failed);
                    infoWidget->showSecs();
                }else{
                    infoWidget->raise();
                    infoWidget->setLabelText(myLan.update_lang_sucess);
                    infoWidget->showSecs();
                }
            }else{
                infoWidget->raise();
                infoWidget->setLabelText(invalid_qm);
                infoWidget->showSecs();
            }

            gFlow.umountUDisk(MOUNT_UDISK_PATH);
        }
    }
}

/*!
 * \brief 槽函数: 恢复语言qm文件
 */
void setLangWidget::onRestoreBtnClicked()
{
    if(langList->currentRow() == -1)
        return;

    QString str = restore+qm_file;
    myMessageBox msgBox(MSG_QUES, str, lan_tips, g_mainWidget);
    int ret = msgBox.exec();
    if(ret == QDialog::Accepted){
        quint32 LangIdx = nLangIndex[langList->currentRow()];
        QString curName = QString("%1%2").arg(LANG_DIR).arg(m_LangIdAndQmMap[LangIdx]);
        QString bakName = QString("%1bak_%2").arg(LANG_DIR).arg(m_LangIdAndQmMap[LangIdx]);

        QFile bakFile(bakName);
        QFile curFile(curName);
        if(bakFile.exists()){
            //! 删除文件
            curFile.remove();

            //! 还原文件
            if(!QFile::copy(bakName,curName)){
                infoWidget->raise();
                infoWidget->setLabelText(myLan.restore_lang_failed);
                infoWidget->showSecs();
            }else{
                infoWidget->raise();
                infoWidget->setLabelText(myLan.restore_lang_sucess);
                infoWidget->showSecs();
            }
        }else{
            infoWidget->raise();
            infoWidget->setLabelText(qm_not_exist);
            infoWidget->showSecs();
        }
    }
}

/*!
 * \brief 槽函数: 设置语言
 */
void setLangWidget::onApplyBtnClicked()
{
    if(m_currentIndex != langList->currentRow()){
        myMessageBox mesBox(MSG_QUES, QString("%1?").arg(change_lang_sure), lan_tips,g_mainWidget);
        int ret = mesBox.exec();
        if(ret == QDialog::Accepted){
            m_currentIndex = langList->currentRow();
            myLan.nLang = nLangIndex[m_currentIndex];
            qDebug("apply lag id =%d",myLan.nLang);
//            g_processData.saveAllGlobal();
            gFlow.saveGlobalParams(CNF_GLOBAL);

            infoWidget->raise();
            infoWidget->setLabelText(myLan.switch_lang_sucess);
            infoWidget->showSecs(5);

            myLan.saveLanguageSetting();
            system("sync");

            gFlow.sendSorterIPparams(false);

            gFlow.msleep(3000);
            system("killall mystart.sh; killall myrestart.sh; cd /app && ./myrestart.sh &");
        }
    }
}

/*!
 * \brief 槽函数: 切换语言
 */
void setLangWidget::onChgLanguageSlt()
{
    QStringList strList;
    for (int i = 0; i < myLan.nLangList[0]; i++) {
        nLangIndex[i] = myLan.nLangList[i+1];
        strList << myLan.getLanguageListName(nLangIndex[i], false);
    }
    for(int i = 0; i < langList->count(); ++i) {
        langList->item(i)->setText(strList.at(i));
        QPushButton *itemBtn = langList->itemWidget(langList->item(i))->findChild<QPushButton*>(QString("apply%1").arg(i));
        itemBtn->setText(lan_apply);
    }
}
