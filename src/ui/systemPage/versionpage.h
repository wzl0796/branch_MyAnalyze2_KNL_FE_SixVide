#ifndef VERSIONPAGE_H
#define VERSIONPAGE_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QStackedWidget>
#include "params/define.h"
#include "common/mymessagebox.h"
#include "softwareversionpage.h"
#include "upgradefpgawidget.h"
#include "core/globalflow/mylanguage.h"
#include "core/mythread/netcheckthread.h"

class versionPage : public QWidget
{
    Q_OBJECT
public:
    explicit versionPage(QWidget *parent = 0);

    void refreshDisplay(void);

signals:


public slots:
    void onRefreshBtnPressedSlt(void);

    void onUpgradeBtnPressedSlt(void);

    void onBackBtnPressedSlt(void);

    void onFactoryBtnClicked();

    void onUserBtnClicked();

    void setBtnState(bool state);

private:
    enum {
        PGAE_SOFT_WARE,
        PAGE_UPGRADE
    };

    QWidget             *m_widget;
    softwareVersionPage *pageSoftWare;
    upgradeFPGAwidget   *pageFpgaUpgrade;
    QStackedWidget      *m_stack;

    QPushButton     *refreshBtn;
    QPushButton     *backBtn;
    QPushButton     *updateBtn;

    QPushButton *m_userBtn, *m_factoryBtn;
    QWidget *m_modeWgt;

private:
    void createPage(void);

    void connectSigAndSlot(void);
};


#endif // VERSIONPAGE_H
