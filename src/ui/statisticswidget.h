#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTabBar>
#include <QDateTimeEdit>
#include <QTableView>
#include <QSqlQuery>
#include <QComboBox>
#include <QToolButton>
#include <QListWidget>
#include <QLayout>
#include <QStackedWidget>
#include <QSignalMapper>
#include <QStandardItemModel>
#include <QGroupBox>
#include "common/mylistwidget.h"
#include "params/globalparams.h"
#include "core/globalflow/mylanguage.h"
#include "src/common/mycheckbox.h"

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QTreeWidget>
#include <QStackedLayout>
QT_CHARTS_USE_NAMESPACE

/*!
 * \enum 枚举统计周期类型
 */
enum {
    STATISTICS_TODAY = 0,   /*! 按天统计 */
    STATISTICS_MONTH,       /*! 按月统计 */
    STATISTICS_YEAR         /*! 按年统计 */
};

#define LINE_WEBVIEW_WID    600 //! 定义曲线图的宽
#define HOURS_OF_DAY        24  //! 定义一天24个小时
#define DAYS_OF_MONTH       31  //! 定义一个月31天
#define MONTHS_OF_YEAR      12  //! 定义一年12个月

#define MAX_COUNT_PER_PAGE  100 //! 定义每页存放最多条数

class StatisticsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticsWidget(int anaIndex, int machineIndex, QWidget *parent = 0);

    void initParams(void);

    void createPages(void);

    void createmainpage();

    void createdetailpage();

    void createanalyzePage();

    void laymainpage();

    void laydetailpage();

    void layanalyzepage();

    void connectSigAndSlot(void);

    void refreshPage(void);

signals:
    void showTip();

public slots:
    void onShowTip(void);
    void onPrintBtnClicked();                           /*! 打印数据槽函数 */
    void onprintHistory();
    void onDateTimeEditFromChanged(QDateTime dateTime); /*! 起始时间 */
    void onDateTimeEditToChanged(QDateTime dateTime);   /*! 截止时间 */
    void onSearchChanged(bool show = true);                         /*! TabBar索引变化 */
    void onDelSingleBtnClicked();                       /*! 删除一条记录 */
    void onDelSchmBtnClicked();                         /*! 删除方案记录 */
    void onDelAllBtnClicked();                          /*! 删除所有记录 */
    void onDelDbBtnClicked();                           /*! 删除数据库按钮槽函数 */
    void onExportBtnClicked();                          /*! 导出数据 */
    void onPreBtnClicked();                             /*! 上一页 */
    void onNextBtnClicked();                            /*! 下一页 */
    void onTableViewClicked(const QModelIndex &);       /*! 表格点击事件 */
    void onTableViewItemChg(QModelIndex curItem,QModelIndex preItem);       /*! 表格点击事件 */
    void onHeaderViewClicked(int index);                /*! 表头点击事件 */
    void onSchmBoxChangedSlt(int index);
    void onSrcBoxChangedSlt(int index);
    void onScrollBarMoveSlt(int index);                 /*! 滑块移动事件 */
    void onDetailBtnPressedSlt(void);
    void onBckBtnPressedSlt(void);
    void onSkip2AnalyzePage(void);
    void onRefreshRecordBtn();
    void onRefreshPieSlice(QPieSlice* slice);
    void onRefreshPie();
    void onRefreshDataOfPie(int nGid, QString strCat);
    void onAnalyTypeChg(int nIdx);
    void onRefreshDateofBar(int nGgid, QString staCat);
    void onRefresAnalyPageDataByCat();
    void onRefresAnalyPageDataByGbid();
    void onClearSelectionBtnClicked();                      /*! 清除选中项按钮槽函数 */
    void onLabSelectBtnClicked();                          // 新增槽函数
    void onLabCheckBoxChanged(int index);               // 标签选择复选框改变槽函数
    void onBackdlgBtnClicked();                          // 返回按钮槽函数
private:
    QWidget       *m_widget;
    QWidget       *m_detailwid;
    QStackedWidget *m_stackwid;

    QLabel        *m_from, *m_to;
    QToolButton   *m_printBtn;
    QComboBox     *m_selSchmBox;
    QLabel        *m_titleLbl;
    QDateTimeEdit *dateTimeEditFrom;
    QDateTimeEdit *dateTimeEditTo;
    QDateTime     dateTimeFrom;
    QDateTime     dateTimeTo;

    QGroupBox     *m_selPrintStagp;
    QDialog       *m_selstagpDlg;  //! 标签选择对话框
    MyCheckBox    *m_labenchk[6]; //! 标签选择复选框数组
    QPushButton   *m_canceldlgBtn; //! 统计组选择对话框 取消按钮
    QPushButton   *m_okdlgBtn;   //! 统计组选择对话框 确认按钮

    QTableView    *tableView;
    QPushButton   *m_delSingleBtn, *m_delAllBtn;
    QPushButton   *m_delDbBtn;                              /*! 删除数据库按钮 */
    QPushButton   *m_exportBtn;
    QPushButton   *m_exportSelectedBtn;                     /*! 导出选中项按钮 */
    QPushButton   *m_clearSelectionBtn;                     /*! 清除选中项按钮 */
    QPushButton   *m_preBtn, *m_nextBtn;
    QLabel        *m_indexLbl;
    QLabel        *m_selectedCountLbl;                      /*! 显示选中项数量的标签 */

    QPushButton   *m_aiLabelCountBtn;
    QPushButton   *m_detailbtn;
    QPushButton   *m_recordBtn;     //!  记录选中的数据条数
    QPushButton   *m_analyzeBtn;    //!  数据分析
    QComboBox     *m_AnltypeComb;

    //! 详细页面
    QLabel              *m_totalpurity[MAX_CAM_NUM];
    QLabel              *m_metalnumLb; //!金属检测
    QFrame              *m_frame;
    QTreeWidget         *m_moretreewid;
    QTableView          *m_shapetableView;
    QStandardItemModel  *m_shapedetailtabModl;


    QPushButton         *m_bckbtn;

    //! 分析页面
    QWidget             *m_analywid;
    QChartView          *m_chartView;
    QChart              *m_chart;
    QBarCategoryAxis    *m_axisX;
    QValueAxis          *m_axisY;
    QPushButton         *m_backBtn;
    QStringList         m_dateRecordlst;
    QStringList         m_crossPageSelectedRecords;   // 跨页选中的记录列表（存储日期时间）
    QSet<QString>       m_globalSelectedSet;          // 全局选中项的集合，用于快速查找
    QStackedLayout      *m_stalayout;
    QPushButton         *m_cattypeBtn;
    QPushButton         *m_stagbBtn;
    int                 m_Gbid = 0;
    int                 m_catIdx =0;
    QVector<QStringList>   m_vcattypelst;
    QVector<QMap<QString,QMap<QString,float>>> m_vdataMap;  //! key 标签名称   Key:统计类别 val：值
    QMap<QString,QVector<QMap<QString,QMap<QString,float>>>> m_vDataBarMap; //!统计条目 vec 统计组 key 标签名称   Key:统计类别 val：值

    QGroupBox           *m_pieGb;
    QChartView          *m_pieChartView[MAX_CAM_NUM];
    QChart              *m_piechart[MAX_CAM_NUM];
    QPieSeries          *m_piesers[MAX_CAM_NUM];

private:
    int         m_analyzeIndex;
    int         m_machineIndex;
    analyzeGlobal *m_analyzeGlobal;
    int         m_schemeIndex;
    int         m_nprintsourId=0;
    QVector<int> m_vStagpId;

    int         nRecordNum;
    int         m_nIndexOfPage;
    int         m_scrollIndex;
    int         m_headerSecIndex;

    QSqlQuery   m_sqlquery;
    QSignalMapper *machineBtnMap;

    int         m_nStagbId = 0;

    int         m_exArrayType = 0;

private:
    bool initDateTime();
    void initTableView();                                    /*! 初始化表格数据 */
    void refreshBtnStatus(int nCurPage, int nRecordNum);     /*! 刷新按钮状态 */
    void getData(QSqlQuery &sqlquery);                       /*! 获取统计数据 */
    void refreshTableView(QSqlQuery sqlquery);               /*! 刷新表格数据 */
    void updateSchmCbx();                                    /*! 刷新方案下拉框*/
    void refreshanalyzepage();                              //! 刷新分析页面
    bool removeStadb();
    void updateCrossPageSelection();                        /*! 更新跨页选择状态 */
    void updateSelectedCountDisplay();                      /*! 更新选中项数量显示 */
    QString exportSelectedRecordsToUSB();                      /*! 导出选中记录到U盘 */
    void clearCrossPageSelection();                         /*! 清除跨页选择状态 */
    QString ProcessExportData();
    void refreshLabelCheckBoxStates();                      /*! 刷新标签选择复选框状态 */

};

#endif // STATISTICSWIDGET_H
