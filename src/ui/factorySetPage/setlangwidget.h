/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        setlangwidget.h
 * \brief       设置语言界面头文件
 * \date        2019.10.30
 */
#ifndef SETLANGWIDGET_H
#define SETLANGWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include "core/globalflow/mylanguage.h"

class setLangWidget : public QWidget
{
    Q_OBJECT
public:
    explicit setLangWidget(QWidget *parent = 0);

    void createPage();
    void initLan();
    void initLangParams();
    void updateLangSetList();

    void setLangBtnViewEna(bool);

private:
    void createWidgetChoose();

signals:
    void sigBackToSetPage();

public slots:
    void onApplyBtnClicked();
    void onLangListRowChangedSlt();
    void onLangSetSlt();
    void onChgLanguageSlt();

    void onUpdateLangList();

    void onUpdateBtnClicked();
    void onRestoreBtnClicked();

private:
    QWidget     *m_widget, *m_widgetChoose;
    QListWidget *langList;
    int nLangIndex[MAX_LANG];
    QPushButton *m_updateBtn, *m_restoreBtn;
    QMap<quint32, QString> m_LangIdAndQmMap;

    int        m_currentIndex;

    //! 词根
    QString change_lang_sure;
    QString switch_lang_success;
    QString update;
    QString restore;
    QString qm_file;
    QString invalid_qm;
    QString qm_not_exist;
    QString lan_apply;
    QString lan_tips;
    QString lan_fail;
    QString lan_success;
};

#endif // SETLANGWIDGET_H
