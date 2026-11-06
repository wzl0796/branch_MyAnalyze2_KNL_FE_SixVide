#ifndef SYSTEMNETPAGE_H
#define SYSTEMNETPAGE_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QGroupBox>
#include <QTabBar>
#include <QWindow>
#include "netconfigpage.h"
#include "remotectrlpage.h"


class systemNetPage : public QDialog
{
    Q_OBJECT
public:
    explicit systemNetPage(QWidget *parent = 0);

    void refreshDisplay(void);

signals:

public slots:
    void onBackBtnPressedSlt(void);

    void onTabIndexChangedSlt(int index);

    void onRefreshWebwindow(int nIdx);

    void onrefreshVPNStatus(bool bStatus);

private:
    enum{
        NET_LOCAL_PAGE =0,      /*! 本地主机 */
        NET_REMOTE_PAGE,      /*! 远程控制 */
        CLOUD_PAGE,
        PAGE_ALL
    };

    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    int m_nIndex;

    QString wechat;
    QString local_machine;
    QString remote_ctrl;
    QString cloud;

private:
    QWidget         *m_widget;
    QTabBar         *m_Tab;

    QStackedWidget  *m_stack;

    netConfigPage   *pageNetConfig;
    remoteCtrlPage  *pageRemote;
    QWidget         *pageCloud;

    bool            m_VPNStatus = false;

    QWindow         *m_win = nullptr;            //! 物联网窗口
    QWidget         *m_container =nullptr;
    int             m_wid =-1;
    QHBoxLayout     *m_hlaycloud;

};

#endif // SYSTEMNETPAGE_H
