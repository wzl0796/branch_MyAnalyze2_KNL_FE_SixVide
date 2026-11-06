#ifndef ANALYZETESTDLG_H
#define ANALYZETESTDLG_H

#include <QWidget>
#include <QtMath>
#include <QLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QMovie>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QGroupBox>
#include <QPlainTextEdit>
#include "params/define.h"
#include "common/mybargraph.h"
#include "common/mypiegraph.h"
#include "common/myinputpanel.h"
#include "core/globalflow/mylanguage.h"
#include "common/mymessagebox.h"

class AnalyzeTestDlg : public QWidget
{
    Q_OBJECT
public:
    explicit AnalyzeTestDlg(int analyze, QWidget *parent = 0);

    void createPage(void);

    void connectSigAndSlot(void);

    void refreshStatusItem(bool bReset = false);

    void refreshMainpageItem(bool bStatus);

    void refreshInferResult();

signals:
    void closeAnalyzeDlgSig();

    void StatsGpidChgSig(int);

public slots:
    void onRefreshStatsGpRes();
    void onRefreshSimRes(QString str);
    void onMergerColorMatChgSlt(bool checked);

private:
    QWidget        *m_widget;
    QWidget        *m_realWidget;
    QFrame         *m_Hline;
    QLabel         *m_detectLbl;
    QLabel         *m_metaititleLb;    //!金属检测
    QPushButton    *m_viewIdBtn;      //! 统计组
    QPushButton    *m_mergerColorMaterialBtn; //! 颜色材质合并
    QTreeWidget    *m_labtreewid;

    QStackedWidget  *m_staticwidChgstck;

    //! 仿真功能
    QWidget        *m_camStatContainer;
    QScrollArea    *m_camStatScroll;
    QVBoxLayout    *m_camStatLayout;
    QPlainTextEdit *m_planEdit;

private:
    void createRealWidget();

    void createSimWidget();

    void refreshBarView();
    void refreshBarViewMerger();

    void refreshMatStaRes();

    void refreshLabStaRes();
    void refreshLabStaResMerger();

    void refreshSimulateRes(bool bReset=false);

    int m_StatusGpId = 0;  //! 统计组Idx
    int m_nLowlevlNum = 0; //! 次一级

    QList<QString> m_lbnamelst;

};

#endif // ANALYZETESTDLG_H
