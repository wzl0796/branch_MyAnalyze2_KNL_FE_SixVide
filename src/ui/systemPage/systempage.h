#ifndef SYSTEMPAGE_H
#define SYSTEMPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "params/define.h"
#include "core/globalflow/mylanguage.h"
#include "versionpage.h"
#include "systemnetpage.h"
#include "systemstapage.h"

class SystemPage : public QWidget
{
    Q_OBJECT

public:
    enum {
        PAGE_SYSTEM,
        PAGE_VERSION,
        PAGE_Net,
        PAGE_ALL
    };
public:
    explicit SystemPage(QWidget *parent = nullptr);
    void cratePage();
    void layoutPage();
    void connectAndSlt();
    void refreshPage();
    void stopRefreshSysta(){m_sysStaPage->stopTimer();}

signals:
    void sigUpdateIotWid(int nCode);
    void sigUpdateVpnSta(bool bSta);

public slots:
    void    onListWidgetChgSlt(int nIdx);
    void    onPermissionChg();
    
private:
    QWidget *m_Wid;
    QStackedWidget *m_stackWid;
    QListWidget    *m_listWid;

    SystemStaPage  *m_sysStaPage;
    versionPage    *m_verSionPage;
    systemNetPage  *m_sysNetPage;

};

#endif // SYSTEMPAGE_H
