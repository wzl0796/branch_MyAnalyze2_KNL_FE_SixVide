#include "statisticswidget.h"
#include <QCalendarWidget>
#include <QStandardItemModel>
#include <QListWidget>
#include <QHeaderView>
#include <QFile>
#include <QDebug>
#include "common/mymessagebox.h"
#include "common/setexporttypewidget.h"
#include "core/globalflow/sqliteinter.h"
#include "core/globalflow/globalflow.h"

StatisticsWidget::StatisticsWidget(int anaIndex, int machineIndex, QWidget *parent) :
    m_analyzeIndex(anaIndex), m_machineIndex(machineIndex), QWidget(parent)
{
    initParams();

    createPages();

    connectSigAndSlot();
}

/**
 * @brief StatisticsWidget::初始化参数
 */
void StatisticsWidget::initParams()
{
    m_nIndexOfPage = 0;
    nRecordNum = 0;
    m_scrollIndex = 0;
    m_headerSecIndex = 0;
    m_analyzeGlobal = gFlow.getSpecifyAnalyzeGlobal(m_analyzeIndex);
    machineBtnMap = new QSignalMapper();
    m_schemeIndex = struCnfg.nCntScheme;

    //! 初始化跨页选择相关变量
    m_crossPageSelectedRecords.clear();
    m_globalSelectedSet.clear();

}

/**
 * @brief StatisticsWidget::界面搭建
 */
void StatisticsWidget::createPages()
{
    //! 主页面
    createmainpage();
    laymainpage();

    //! 详细页面
    createdetailpage();
    laydetailpage();

    //! 分析页面
    createanalyzePage();
    layanalyzepage();

    m_stackwid = new QStackedWidget();
    m_stackwid->addWidget(m_widget);
    m_stackwid->addWidget(m_detailwid);
    m_stackwid->addWidget(m_analywid);
    m_stackwid->setCurrentIndex(0);

    //! 主布局
    QVBoxLayout* vlaymain = new QVBoxLayout();
    vlaymain->addWidget(m_stackwid);
    this->setLayout(vlaymain);

    refreshPage();
}

/*!
 * \brief 初始化首页
 */
void StatisticsWidget::createmainpage()
{
    m_widget = new QWidget();
    m_widget->setFixedSize(WIN_WID-165, WIN_HEI-190);
    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    //! top
    m_titleLbl = new QLabel(myLan.schme);      //! 目标方案
    m_titleLbl->hide();
    m_selSchmBox = new QComboBox();
    m_selSchmBox->setFixedSize(300, BTN_HEI-10);
    QListWidget *listWidget1 = new QListWidget();
    m_selSchmBox->setModel(listWidget1->model());
    m_selSchmBox->setView(listWidget1);
    m_selSchmBox->setMaxVisibleItems(8);

    m_from = new QLabel(myLan.from); //! 日期
    m_from->setFixedSize(70, BTN_HEI);

    QDateTime dateTime = QDateTime::currentDateTime();
    dateTimeEditFrom = new QDateTimeEdit();
    dateTimeEditFrom->setFixedSize(BTN_WID*1.5, BTN_HEI+30);
    dateTimeEditFrom->setDisplayFormat("yyyy-MM-dd");
    dateTimeEditFrom->setCalendarPopup(true);
    dateTimeEditFrom->setDateTime(dateTime);

    QCalendarWidget *calendarWidgetFrom = dateTimeEditFrom->calendarWidget();
    calendarWidgetFrom->setGridVisible(true);
    calendarWidgetFrom->setStyleSheet("background-color: rgb(255,250,250);");

    m_to = new QLabel(myLan.to);
    m_to->setFixedSize(50, BTN_HEI);

    dateTimeEditTo = new QDateTimeEdit();
    dateTimeEditTo->setFixedSize(BTN_WID*1.5, BTN_HEI+30);
    dateTimeEditTo->setDisplayFormat("yyyy-MM-dd");
    dateTimeEditTo->setCalendarPopup(true);
    dateTimeEditTo->setDateTime(dateTime);

    QCalendarWidget *calendarWidgetTo = dateTimeEditTo->calendarWidget();
    calendarWidgetTo->setGridVisible(true);
    calendarWidgetTo->setStyleSheet("background-color: rgb(250,250,250);");

    initDateTime();

    //! 打印
    m_printBtn = new QToolButton();
    m_printBtn->setText(myLan.print);
    m_printBtn->setIcon(QIcon(":/res/png/action_print.png"));
    m_printBtn->setFixedSize(BTN_WID+30, BTN_HEI);
    m_printBtn->setIconSize(QSize(50, 50));
    m_printBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_printBtn->setStyleSheet(QString("QToolButton{background-color: %1;border:0px;font: 24px;}").arg(g_ColorLightGray3));

    //! 打印统计组选择
    m_selstagpDlg = new QDialog(this);
    m_selstagpDlg->setFixedSize(800, 400);
    m_selstagpDlg->setWindowFlags(Qt::FramelessWindowHint);
    m_selstagpDlg->setModal(true);
    m_selstagpDlg->setStyleSheet("QDialog{background-color: #F1F0F5;border-radius:10px;}");
    m_selstagpDlg->hide();

    m_selPrintStagp = new QGroupBox(myLan.select_print_staisticgp);

    for(int i=0; i<6; i++){
        m_labenchk[i] = new MyCheckBox("");
        m_labenchk[i]->setCheckBtnSize(60,60);
    }
    
    m_okdlgBtn = new QPushButton(myLan.ok);
    m_okdlgBtn->setFixedSize(BTN_WID+20, BTN_HEI);

    m_canceldlgBtn = new QPushButton(myLan.cancel);
    m_canceldlgBtn->setFixedSize(BTN_WID+20, BTN_HEI);

    //! mid
    tableView = new QTableView;
    tableView->setFixedHeight(690);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView->verticalHeader()->hide();
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    initTableView();

    //! down
    m_delSingleBtn = new QPushButton(myLan.del);    //! 删除
    m_delSingleBtn->setFixedSize(BTN_WID+30, BTN_HEI);
    m_delSingleBtn->setStyleSheet(g_disableBtnStyle);
    m_delSingleBtn->setEnabled(false);
    m_delSingleBtn->setIcon(QIcon(":/res/png/del.png"));
    m_delSingleBtn->setIconSize(QSize(50, 50));

    m_delAllBtn = new QPushButton(myLan.del_all);         //! 删除全部
    m_delAllBtn->setFixedSize(BTN_WID, BTN_HEI-10);
    m_delAllBtn->setStyleSheet(g_enableBtnStyle);
    m_delAllBtn->hide();

    m_exportBtn = new QPushButton(myLan.exporting);    //! 导出数据
    m_exportBtn->setFixedSize(BTN_WID+30, BTN_HEI);
    m_exportBtn->setStyleSheet(g_enableBtnStyle);
    m_exportBtn->setIcon(QIcon(":/res/png/export.png"));
    m_exportBtn->setIconSize(QSize(50, 50));

    m_delDbBtn = new QPushButton(myLan.del_db);        //! 删除数据库
    m_delDbBtn->setFixedSize(BTN_WID+70, BTN_HEI);
    m_delDbBtn->setStyleSheet(g_enableBtnStyle);
    m_delDbBtn->setIcon(QIcon(":/res/png/action_deldb.png"));
    m_delDbBtn->setIconSize(QSize(50, 50));
    m_delDbBtn->hide();

    m_indexLbl = new QLabel(QString("%1:%2").arg(myLan.currentpage).arg(m_nIndexOfPage+1));
    m_indexLbl->setAlignment(Qt::AlignCenter);
    m_indexLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_preBtn = new QPushButton();
    m_preBtn->setIcon(QIcon(":/res/png/Arrow_Left.png"));
    m_preBtn->setIconSize(QSize(55, 55));
    m_preBtn->setFixedSize(55, 55);
    m_preBtn->setStyleSheet(g_disableBtnStyle);
    m_preBtn->setEnabled(false);

    m_nextBtn = new QPushButton();
    m_nextBtn->setIcon(QIcon(":/res/png/Arrow_Right.png"));
    m_nextBtn->setIconSize(QSize(55, 55));
    m_nextBtn->setFixedSize(55, 55);
    m_nextBtn->setStyleSheet(g_disableBtnStyle);
    m_nextBtn->setEnabled(false);

    m_clearSelectionBtn = new QPushButton(myLan.clear_selection);    //! 清除选中项
    m_clearSelectionBtn->setFixedSize(BTN_WID+50, BTN_HEI);
    m_clearSelectionBtn->setStyleSheet(g_disableBtnStyle);
    m_clearSelectionBtn->setEnabled(false);
    m_clearSelectionBtn->setIcon(QIcon(":/res/png/action_clear.png"));
    m_clearSelectionBtn->setIconSize(QSize(50, 50));

    m_selectedCountLbl = new QLabel(QString("%1: 0").arg(myLan.selected_item)); //! 选中项个数
    m_selectedCountLbl->setAlignment(Qt::AlignCenter);
    m_selectedCountLbl->setFixedSize(BTN_WID+30, BTN_HEI);
    m_selectedCountLbl->setStyleSheet("QLabel{background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 5px;}");
    m_selectedCountLbl->hide();

    m_detailbtn = new QPushButton(myLan.detail);
    m_detailbtn->setFixedSize(BTN_WID+30, BTN_HEI);
    m_detailbtn->setStyleSheet(g_enableBtnStyle);
    m_detailbtn->setIcon(QIcon(":/res/png/Edit_Detail.png"));
    m_detailbtn->setIconSize(QSize(50, 50));

    //! 数据分析
    m_recordBtn = new QPushButton(myLan.clear);
    m_recordBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_recordBtn->setStyleSheet(g_enableBtnStyle);

    m_analyzeBtn = new QPushButton(myLan.data_analyze);
    m_analyzeBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_analyzeBtn->setStyleSheet(g_enableBtnStyle);

    m_AnltypeComb = new QComboBox();
    m_AnltypeComb->setFixedSize(BTN_WID+30, BTN_HEI);
    QListWidget *anltyupelstwid = new QListWidget();
    m_AnltypeComb->setModel(anltyupelstwid->model());
    m_AnltypeComb->setView(anltyupelstwid);
    m_AnltypeComb->insertItem(0,myLan.pie_chart);
    m_AnltypeComb->insertItem(1,myLan.bar_chart);


    m_recordBtn->hide();
    m_analyzeBtn->hide();
    m_AnltypeComb->hide();

}

/*!
 * \brief 初始化详细页面
 */
void StatisticsWidget::createdetailpage()
{
    m_detailwid = new QWidget();
    m_detailwid->setFixedSize(WIN_WID-165, WIN_HEI-190);
    m_detailwid->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    //! 详细页面
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_totalpurity[i] = new QLabel();
    }
    m_metalnumLb = new QLabel(QString("%1: ").arg(myLan.metal));
    m_metalnumLb->setFixedSize(200,BTN_HEI-10);
    m_metalnumLb->hide();

    m_frame = new QFrame();
    m_frame->setFixedHeight(2);
    m_frame->setFrameShape(QFrame::HLine);
    m_frame->setFrameShadow(QFrame::Sunken);
    m_frame->setStyleSheet(QString("background-color:%1;").arg(g_ColorLightGray));


    QString trestyle = QString("QTreeWidget::item{height:50px;text-align:left;padding-left:4px}"
                               "QHeaderView::section{text-align:left;padding-left:4px}"
                               "QTreeWidget {alternate-background-color: #E0E0E0;}"
                               "QTreeView::indicator{width:40px;height:50px}"
                               "QTreeView::branch{width:40px;height:50px}");
    
    m_moretreewid = new QTreeWidget();
    m_moretreewid->header()->setFixedHeight(50);
    m_moretreewid->setAlternatingRowColors(true);
    m_moretreewid->setStyleSheet(trestyle);
    m_moretreewid->setIndentation(40);

    //! 形选table
    QStringList shapetitlelst;
    shapetitlelst<<myLan.category<<myLan.little<<myLan.middle<<myLan.big;
    m_shapetableView = new QTableView();
    m_shapetableView->hide();
    m_shapetableView->setShowGrid(false);
    m_shapetableView->verticalHeader()->hide();
    m_shapedetailtabModl = new QStandardItemModel(8,4);
    for(int i=0; i<shapetitlelst.size(); i++){
        m_shapedetailtabModl->setHeaderData(i, Qt::Horizontal, shapetitlelst.at(i));
    }
    m_shapetableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_shapetableView->horizontalHeader()->setSectionsClickable(false);
    m_shapetableView->setModel(m_shapedetailtabModl);
    m_shapetableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_shapetableView->setSelectionMode(QAbstractItemView::NoSelection);
    m_shapetableView->horizontalHeader()->setStretchLastSection(true);
    m_shapetableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    m_bckbtn = new QPushButton(myLan.back);
    m_bckbtn->setFixedSize(BTN_WID+30,BTN_HEI);
    m_bckbtn->setStyleSheet(g_enableBtnStyle);
    m_bckbtn->setIcon(QIcon(":/res/png/Action_Back.png"));
    m_bckbtn->setIconSize(QSize(50, 50));

}

/*!
 * \brief 创建分析页面
 */
void StatisticsWidget::createanalyzePage()
{
    m_analywid = new QWidget();
    m_analywid->setFixedSize(WIN_WID-165, WIN_HEI-190);
    m_analywid->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    m_cattypeBtn = new QPushButton();
//    m_cattypeBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));
//    m_cattypeBtn->setIconSize(QSize(50,50));
    m_cattypeBtn->setFixedWidth(160);

    m_stagbBtn = new QPushButton();
    //    m_stagbBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));
    //    m_stagbBtn->setIconSize(QSize(50,50));
    m_stagbBtn->setFixedWidth(160);

    m_chart = new QChart();
    m_axisX = new QBarCategoryAxis();
    m_axisY = new QValueAxis();

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);


    //! 饼状图
    m_pieGb = new QGroupBox();
    for(int i=0; i<MAX_CAM_NUM; i++){
        m_piechart[i] = new QChart();
        m_piesers[i] = new QPieSeries();
        m_piechart[i]->addSeries(m_piesers[i]);
        m_piechart[i]->legend()->setVisible(true);
        m_piechart[i]->legend()->setAlignment(Qt::AlignRight);
        QFont font = m_piechart[i]->legend()->font();
        font.setPointSize(14);
        m_piechart[i]->legend()->setFont(font);
        m_pieChartView[i] = new QChartView(m_piechart[i]);
        m_pieChartView[i]->setRenderHint(QPainter::Antialiasing);
    }

    m_backBtn = new QPushButton(myLan.back);
    m_backBtn->setFixedSize(120,66);

}


/*!
 * \brief 布局主页面
 */
void StatisticsWidget::laymainpage()
{
    //! 标签使能对话框使能
    QGridLayout *laylabgd = new QGridLayout();
    laylabgd->setContentsMargins(150,20,50,20);
    for(int i=0; i<6; i++){
        laylabgd->addWidget(m_labenchk[i], i/2, i%2,1,1,Qt::AlignLeft);
    }

    QHBoxLayout *laybackbtn = new QHBoxLayout();
    laybackbtn->addStretch();
    laybackbtn->addWidget(m_okdlgBtn);
    laybackbtn->addSpacing(260);
    laybackbtn->addWidget(m_canceldlgBtn);
    laybackbtn->addStretch();

    QVBoxLayout *vlaylabgd = new QVBoxLayout(m_selPrintStagp);
    vlaylabgd->setContentsMargins(30,50,30,10);
    vlaylabgd->addStretch();
    vlaylabgd->addLayout(laylabgd);
    vlaylabgd->addStretch();
    vlaylabgd->addLayout(laybackbtn);

    QHBoxLayout* hlaystagp = new QHBoxLayout();
    hlaystagp->addWidget(m_selPrintStagp);
    m_selstagpDlg->setLayout(hlaystagp);

    QHBoxLayout *qTextControlLayout = new QHBoxLayout;
    qTextControlLayout->addStretch();
    qTextControlLayout->addWidget(m_from,0,Qt::AlignCenter);
    qTextControlLayout->addWidget(dateTimeEditFrom,0,Qt::AlignCenter);
    qTextControlLayout->addSpacing(50);
    qTextControlLayout->addWidget(m_to,0,Qt::AlignCenter);
    qTextControlLayout->addWidget(dateTimeEditTo,0,Qt::AlignCenter);
    qTextControlLayout->addStretch();

    QHBoxLayout *upLay = new QHBoxLayout;
    upLay->setMargin(10);
    upLay->addWidget(m_titleLbl);
    upLay->addWidget(m_selSchmBox);
    upLay->addStretch(2);
    upLay->addLayout(qTextControlLayout);
    upLay->addStretch(3);
    upLay->addWidget(m_printBtn);
    upLay->addSpacing(8);
    upLay->addWidget(m_AnltypeComb);
    upLay->addSpacing(3);


    QHBoxLayout *downLay = new QHBoxLayout;
    downLay->addWidget(m_exportBtn);
    downLay->addSpacing(10);
    downLay->addWidget(m_delSingleBtn);
    downLay->addSpacing(10);
    downLay->addWidget(m_delAllBtn);
    downLay->addSpacing(10);
    downLay->addWidget(m_delDbBtn);
    downLay->addStretch();
    downLay->addWidget(m_preBtn);
    downLay->addSpacing(20);
    downLay->addWidget(m_indexLbl);
    downLay->addSpacing(20);
    downLay->addWidget(m_nextBtn);
    downLay->addStretch();
    downLay->addWidget(m_selectedCountLbl);
    downLay->addSpacing(10);
    downLay->addWidget(m_clearSelectionBtn);
    downLay->addSpacing(10);
    downLay->addWidget(m_detailbtn);
    downLay->addSpacing(10);
    downLay->addWidget(m_recordBtn);
    downLay->addSpacing(10);
    downLay->addWidget(m_analyzeBtn);
    downLay->addSpacing(10);


    /** 布局 */
    QVBoxLayout *statisticLay = new QVBoxLayout();
    statisticLay->addLayout(upLay);
    statisticLay->addWidget(tableView);
    statisticLay->addLayout(downLay);
    statisticLay->addSpacing(12);

    m_widget->setLayout(statisticLay);

}

void StatisticsWidget::laydetailpage()
{
    QHBoxLayout* hlaydetailtop = new QHBoxLayout();
    hlaydetailtop->setContentsMargins(5,0,5,5);
    for(int i=0;i<MAX_CAM_NUM;i++){
        hlaydetailtop->addWidget(m_totalpurity[i]);
    }
    hlaydetailtop->addWidget(m_metalnumLb);

    QHBoxLayout* hlaydetailmid = new QHBoxLayout();
    hlaydetailmid->addWidget(m_moretreewid);
    hlaydetailmid->addWidget(m_shapetableView);


    QHBoxLayout* hlaydetaildown = new QHBoxLayout();
    hlaydetaildown->addStretch();
    hlaydetaildown->addWidget(m_bckbtn);
    hlaydetaildown->addSpacing(30);

    QVBoxLayout* vlaydetail = new QVBoxLayout(m_detailwid);
    vlaydetail->addLayout(hlaydetailtop);
    vlaydetail->addWidget(m_frame);
    vlaydetail->addLayout(hlaydetailmid);
    vlaydetail->addLayout(hlaydetaildown);
    vlaydetail->addSpacing(10);

}

/*!
 * \brief 布局分析页面
 */
void StatisticsWidget::layanalyzepage()
{
    m_stalayout = new QStackedLayout();
    QHBoxLayout *laypieGb = new QHBoxLayout(m_pieGb);
    for(int i=0; i<MAX_CAM_NUM; i++){
        laypieGb->addWidget(m_pieChartView[i]);
    }
    m_stalayout->addWidget(m_pieGb);
    m_stalayout->addWidget(m_chartView);
    m_stalayout->setCurrentIndex(0);

    QVBoxLayout* vlaybtn = new QVBoxLayout();
    vlaybtn->addStretch(1);
    vlaybtn->addWidget(m_cattypeBtn);
    vlaybtn->addSpacing(20);
    vlaybtn->addWidget(m_stagbBtn);
    vlaybtn->addStretch(3);

    QHBoxLayout *laydown = new QHBoxLayout();
    laydown->addWidget(m_backBtn);
    laydown->addStretch();

    QHBoxLayout *hlaypage = new QHBoxLayout();
    hlaypage->addLayout(m_stalayout);
    hlaypage->addLayout(vlaybtn);

    QVBoxLayout *laypage = new QVBoxLayout();
    laypage->addLayout(hlaypage);
    laypage->addLayout(laydown);
    laypage->addSpacing(15);


    m_analywid->setLayout(laypage);
}

/**
 * @brief StatisticsWidget::信号槽绑定
 */
void StatisticsWidget::connectSigAndSlot()
{
    connect(m_selSchmBox,                  SIGNAL(currentIndexChanged(int)),     this,     SLOT(onSchmBoxChangedSlt(int)));
    connect(m_printBtn,                    SIGNAL(clicked(bool)),                this,     SLOT(onPrintBtnClicked()));
    connect(m_canceldlgBtn,                SIGNAL(clicked()),                    this,     SLOT(onBackdlgBtnClicked()));
    connect(m_okdlgBtn,                    SIGNAL(pressed()),                    this,     SLOT(onprintHistory()));

    connect(dateTimeEditFrom,              SIGNAL(dateTimeChanged(QDateTime)),   this,     SLOT(onDateTimeEditFromChanged(QDateTime)));
    connect(dateTimeEditTo,                SIGNAL(dateTimeChanged(QDateTime)),   this,     SLOT(onDateTimeEditToChanged(QDateTime)));
    
    connect(m_delSingleBtn,                SIGNAL(clicked(bool)),                this,     SLOT(onDelSingleBtnClicked()));
    connect(m_delAllBtn,                   SIGNAL(clicked(bool)),                this,     SLOT(onDelSchmBtnClicked()));
    connect(m_delDbBtn,                    SIGNAL(clicked(bool)),                this,     SLOT(onDelDbBtnClicked()));
    connect(m_exportBtn,                   SIGNAL(clicked(bool)),                this,     SLOT(onExportBtnClicked()));
    connect(m_clearSelectionBtn,           SIGNAL(clicked(bool)),                this,     SLOT(onClearSelectionBtnClicked()));
    connect(m_preBtn,                      SIGNAL(clicked(bool)),                this,     SLOT(onPreBtnClicked()));
    connect(m_nextBtn,                     SIGNAL(clicked(bool)),                this,     SLOT(onNextBtnClicked()));

    connect(tableView->selectionModel(),   SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableViewItemChg(QModelIndex,QModelIndex)));
    connect(tableView,                     SIGNAL(clicked(const QModelIndex &)), this,     SLOT(onTableViewClicked(const QModelIndex &)));
    connect(tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),          this,     SLOT(onHeaderViewClicked(int)));
    connect(this,                          SIGNAL(showTip()),                    this,     SLOT(onShowTip()), Qt::QueuedConnection);
    connect((QAbstractSlider*)tableView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrollBarMoveSlt(int)));
    
    connect(m_detailbtn,                    SIGNAL(pressed()),                  this,      SLOT(onDetailBtnPressedSlt()));
    connect(m_bckbtn,                       SIGNAL(pressed()),                  this,      SLOT(onBckBtnPressedSlt()));
    connect(m_backBtn,                      SIGNAL(pressed()),                   this,     SLOT(onBckBtnPressedSlt()));

    QSignalMapper *m_labCheckMapper = new QSignalMapper();
    for(int i=0; i<6; i++){
        connect(m_labenchk[i], SIGNAL(toggled(bool)), m_labCheckMapper, SLOT(map()));
        m_labCheckMapper->setMapping(m_labenchk[i], i);
    }
    connect(m_labCheckMapper, SIGNAL(mapped(int)), this, SLOT(onLabCheckBoxChanged(int)));
 

    //! 数据分析页面槽
    connect(m_recordBtn,                    SIGNAL(pressed()),                  this,      SLOT(onRefreshRecordBtn()));
    connect(m_analyzeBtn,                   SIGNAL(pressed()),                  this,      SLOT(onSkip2AnalyzePage()));
    connect(m_AnltypeComb,                  SIGNAL(currentIndexChanged(int)),   this,      SLOT(onAnalyTypeChg(int)));
    connect(m_cattypeBtn,                   SIGNAL(pressed()),                  this,      SLOT(onRefresAnalyPageDataByCat()));
    connect(m_stagbBtn,                     SIGNAL(pressed()),                  this,      SLOT(onRefresAnalyPageDataByGbid()));

    for(int i=0; i<MAX_CAM_NUM; i++){
        connect(m_piesers[i],               SIGNAL(pressed(QPieSlice*)),        this ,     SLOT(onRefreshPieSlice(QPieSlice*)));
    }
}

/**
 * @brief StatisticsWidget::界面刷新
 */
void StatisticsWidget::refreshPage()
{
    if(struGsh.nAuthority == AUTHORITY_ENGINEER){
        m_delDbBtn->show();
    }else{
        m_delDbBtn->hide();
    }

    //! 刷新方案
    m_schemeIndex = struCnfg.nCntScheme;
    updateSchmCbx();

    onSearchChanged(false);

    //! 重新关联信号槽（删除之前的model）
    connect(tableView->selectionModel(),   SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableViewItemChg(QModelIndex,QModelIndex)));

}

void StatisticsWidget::onLabSelectBtnClicked()
{
    m_nStagbId = 0;
    refreshLabelCheckBoxStates();

    QPoint parentPos = this->mapToGlobal(QPoint(0, 0));
    int x = parentPos.x() + (this->width() - m_selstagpDlg->width()) / 3;
    int y = parentPos.y() + (this->height() - m_selstagpDlg->height()) / 3;
    m_selstagpDlg->move(x, y);


    m_selstagpDlg->raise();
    m_selstagpDlg->show();
}

void StatisticsWidget::onBackdlgBtnClicked()
{
    m_selstagpDlg->hide();
}

void StatisticsWidget::onLabCheckBoxChanged(int index)
{
    Q_UNUSED(index);

    m_vStagpId.clear();

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    for(int i=0; i<pProfile->nStaticGroup; i++){
        if(m_labenchk[i]->isChecked()){
            m_vStagpId.push_back(i);
        }
    }
}

/*!
 * \brief 刷新统计组
 */
void StatisticsWidget::refreshLabelCheckBoxStates()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    int nStagp = pProfile->nStaticGroup;

    for(int i=0; i<6; i++){
        if(i<nStagp && i<pProfile->vUnitaddrlist.size()){
            const int nCamId = QString(pProfile->vUnitaddrlist[i].at(0)).toInt();
            m_labenchk[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nCamId]);
            m_labenchk[i]->setChecked(true);
            m_labenchk[i]->show();
        }else{
            m_labenchk[i]->setChecked(false);
            m_labenchk[i]->hide();
        }
    }

    m_vStagpId.clear();
    for(int i=0; i<nStagp; i++){
        m_vStagpId.push_back(i);
    }
}

/*!
 * \brief 打印历史数据
 */
void StatisticsWidget::onprintHistory()
{
    m_selstagpDlg->hide();

    infoWidget->raise();
    infoWidget->setLabelText(QString("%1......").arg(myLan.printing_and_wait));
    infoWidget->show();

    QModelIndexList itemlst = tableView->selectionModel()->selectedRows();
    int row = itemlst.constFirst().row();
    QStandardItemModel *model = (QStandardItemModel*)tableView->model();
    QString  schcode = struCnfg.schemeList[m_schemeIndex].code;
    QString date = model->data(model->index(row, 0)).toString(); //! 时间


    QString querycolor = QString("SELECT * FROM '%1' WHERE LOGTIME = '%2'")
                             .arg(schcode).arg(date);
    m_sqlquery = g_sqliteInter->executeCmd(querycolor);


    QStringList tailList;
    QStringList tileLst;
    QList<QStringList> labLst;
    QList<QStringList> labtitleLst;
    QVector<QStringList> vRowinfo;

    //! 客户厂家名称
    //! 方案名称  批次信息 检验员 化验重量
    //! 化验时间  设备名称  机器编号
    //! 公司名称
    if(m_sqlquery.first()){
        tailList <<myLan.detect_time<<date
                 <<struCnfg.gDeviceNmae <<struCnfg.master_device_code;

        do{
            QString strscheme = m_sqlquery.value(5).toString();
            QString strbatch = m_sqlquery.value(1).toString();
            QString strdetector = m_sqlquery.value(2).toString();
            QString stryiled = m_sqlquery.value(4).toString();

            tileLst <<myLan.yield<<stryiled <<myLan.batch<<strbatch
                    <<myLan.detector<<strdetector<<myLan.schme<<strscheme;

            QString valstr = m_sqlquery.value(6).toString();
            QJsonObject json = gFlow.qstr2Json(valstr);

            if(struCnfg.nMachineType == nMachine_Bottle &&
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect){
                QStringList metallst;
                if(json.contains("metal")){
                    metallst<<QString(myLan.metal)
                             <<QString::number(json["metal"].toInt());
                    tileLst<<metallst;
                }
            }

            if(json["data"].isArray()){
                QJsonArray  jsonarray = json["data"].toArray();
                for(int k=0; k<jsonarray.size(); k++){
                    if(!jsonarray[k].isObject()) continue;

                    int num = -2;
                    QStringList keylst = jsonarray[k].toObject().keys();
                    QString key = keylst.first();
                    QRegularExpression re(R"(\d+)");
                    QRegularExpressionMatch match = re.match(key);
                    if (match.hasMatch()) {
                        num = match.captured(0).toInt();
                    }
                    if(!m_vStagpId.contains(num)){//! 统计组过滤
                        continue;
                    }

                    if(jsonarray[k].isObject()){
                        QStringList matinfolst;
                        QJsonObject labjson  = jsonarray[k].toObject();

                        //! 打印物料统计信息
                        if(labjson.contains(QString("stagp_%1_mat").arg(num))){
                            QJsonObject matjson = labjson[QString("stagp_%1_mat").arg(num)].toObject();
                            for(QString matkey: matjson.keys()){
                                matinfolst<<QString("%1: ").arg(matkey);
                                matinfolst<<matjson[matkey].toString();
                            }
                            tileLst<<matinfolst;
                        }

                        //! 打印标签统计信息
                        QStringList slabtile, labtitle;
                        QJsonArray labary = labjson[QString("stagp_%1_lab").arg(num)].toArray();
                        if(labary.size()>0){
                            labtitle = labary[0].toObject().keys();
                            labtitle.removeOne("child");
                            labtitle.removeOne("name");
                            labtitle.removeOne("stat");
                        }
                        slabtile<<myLan.category<<labtitle<<myLan.status;
                        labtitleLst.push_back(slabtile);
                        vRowinfo.push_back(slabtile);

                        QStringList slablst;
                        for(int m=0; m<labary.size(); m++){
                            QStringList rowlablst;
                            QJsonObject datajson = labary[m].toObject();
                            slablst<<QString("%1").arg(datajson["name"].toString());
                            rowlablst<<QString("%1").arg(datajson["name"].toString());

                            for(QString key : labtitle){//! 只打印第一个统计类别
                                if(labtitle.indexOf(key)>1) break;
                                slablst<<datajson[key].toString();
                                rowlablst<<datajson[key].toString();
                            }
                            slablst<<(datajson["stat"].toBool()?myLan.abnormal:myLan.normal);
                            rowlablst<<(datajson["stat"].toBool()?myLan.abnormal:myLan.normal);
                            vRowinfo.push_back(rowlablst);
                        }
                        labLst.push_back(slablst);
                    }
                }
            }
        }while(m_sqlquery.next());
    }

    if(!(tileLst.isEmpty() && labLst.isEmpty() && labtitleLst.isEmpty())){
        int nCol = labtitleLst.at(0).size();
        // gFlow.PrintByrow(nCol,vRowinfo,tileLst,tailList);
         QFuture<void> future = QtConcurrent::run(&gFlow,
                                                 &GlobalFlow::PrintByrow,
                                                 nCol,
                                                 vRowinfo,
                                                 tileLst,
                                                 tailList);
        QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
            watcher->deleteLater();
            infoWidget->hide();
        });
        watcher->setFuture(future);
    }
}



/**
 * @brief StatisticsWidget::时间选择槽函数
 */
void StatisticsWidget::onShowTip()
{
    infoWidget->raise();
    infoWidget->setLabelText(QString("%1!").arg(myLan.endtime_lessthan_starttime));
    infoWidget->showSecs();
}

/**
 * @brief StatisticsWidget::打印数据槽函数
 */
void StatisticsWidget::onPrintBtnClicked()
{
    QModelIndexList itemlst = tableView->selectionModel()->selectedRows();
    if(itemlst.size() == 0){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_item));
        infoWidget->showSecs(3);
    }else if(itemlst.size()>1){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.not_support_select_multi_item));
        infoWidget->showSecs(3);
    }else{
        int row = itemlst.constFirst().row();
        QStandardItemModel *model = (QStandardItemModel*)tableView->model();
        QString date = model->data(model->index(row, 0)).toString(); //! 时间
        if(date.compare("") == 0){
            infoWidget->setLabelText(myLan.invalid_data);
            infoWidget->showSecs();
        }else{
            auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
            int nStagp = pProfile->nStaticGroup;

            if(nStagp>1){
                onLabSelectBtnClicked();
            }else{
                m_vStagpId.clear();
                m_vStagpId.push_back(0);
                onprintHistory();
            }
        }
    }
}

/**
 * @brief StatisticsWidget::起始时间
 * @param dateTime
 */
void StatisticsWidget::onDateTimeEditFromChanged(QDateTime dateTime)
{
    if(!initDateTime()) {
        return;
    }
    onSearchChanged();
}

/**
 * @brief StatisticsWidget::截止时间
 * @param dateTime
 */
void StatisticsWidget::onDateTimeEditToChanged(QDateTime dateTime)
{
    if(!initDateTime()) {
        return;
    }
    onSearchChanged();
}

/**
 * @brief StatisticsWidget::数据库搜索
 */
void StatisticsWidget::onSearchChanged(bool show)
{
    if(show){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1...").arg(myLan.require_data_and_wait));
        infoWidget->delayShow();
    }

    //! 清除跨页选择状态（新的搜索条件）
    clearCrossPageSelection();

    m_nIndexOfPage = 0;
    initTableView();
    getData(m_sqlquery);
    refreshTableView(m_sqlquery);

    infoWidget->hide();
}

/**
 * @brief StatisticsWidget::删除一条记录
 */
void StatisticsWidget::onDelSingleBtnClicked()
{
    QString schmcode = struCnfg.schemeList[m_schemeIndex].code;
    QStandardItemModel *model = (QStandardItemModel*)tableView->model();
    QModelIndexList itemlst = tableView->selectionModel()->selectedRows();

    if(itemlst.size()<=0){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_item));
        infoWidget->showSecs();
    }else{
        QString str = QString("%1?").arg(myLan.confirm_del);
        myMessageBox msgBox(MSG_QUES, str, myLan.notice, g_mainWidget);
        int ret = msgBox.exec();
        if(ret != QDialog::Accepted){
            return;
        }

        for(QModelIndex idx:itemlst){
            int row = idx.row();
            if(row >= 0 && row < model->rowCount()) {
                QString date = model->data(model->index(row, 0)).toString();
                if(date.compare("") == 0)
                    return;


                QString query = QString("DELETE FROM '%1' WHERE LOGTIME = '%2'")
                        .arg(schmcode).arg(date);
                g_sqliteInter->executeCmd(query);
            }
        }

        onSearchChanged();
    }
}

/**
 * @brief StatisticsWidget::删除单个方案下的所有记录
 */
void StatisticsWidget::onDelSchmBtnClicked()
{
    QString str = QString("%1?").arg(myLan.confirm_del);
    myMessageBox msgBox(MSG_QUES, str, myLan.notice, g_mainWidget);
    int ret = msgBox.exec();
    if(ret != QDialog::Accepted){
        return;
    }

    QString schmcode = struCnfg.schemeList[m_schemeIndex].code;
    QString query = QString("DROP TABLE '%1'")
            .arg(schmcode);
    g_sqliteInter->executeCmd(query);

    gFlow.msleep(3000);
    g_sqliteInter->createTable(struCnfg.schemeList[m_schemeIndex].code);

    onSearchChanged();
}

/**
 * @brief StatisticsWidget::删除所有记录
 */
void StatisticsWidget::onDelAllBtnClicked()
{
//    QString str = QString("%1%?").arg(myLan.confirm_del);
//    myMessageBox msgBox(MSG_QUES, str, myLan.notice, g_mainWidget);
//    int ret = msgBox.exec();
//    if(ret != QDialog::Accepted){
//        return;
//    }

//    QString tableName = "STATISTICDATA";
//    QString batchName = "BATCHDATA";
//    QString machineCode = QString(struCnfg.analyzeInfo[m_analyzeIndex]->analyzeModel.sModelCode);
//    QString query = QString("DELETE FROM %1 WHERE matno = '%2' AND machinecode = '%3'")
//                           .arg(tableName).arg(m_analyzeGlobal->nMaterial).arg(machineCode);
//    g_sqliteInter->executeCmd(query);

//    query = QString("DELETE FROM %1 WHERE matno = '%2' AND machinecode = '%3'")
//                   .arg(batchName).arg(m_analyzeGlobal->nMaterial).arg(machineCode);
//    g_sqliteInter->executeCmd(query);

//    onSearchChanged();
}

/**
 * @brief StatisticsWidget::删除数据库按钮槽函数
 */
void StatisticsWidget::onDelDbBtnClicked()
{
    if(struGsh.nFeederState){
        infoWidget->raise();
        infoWidget->setLabelText(myLan.please_stop_detect);
        infoWidget->showSecs();
    }else{
        if(removeStadb()){
            gFlow.saveGlobalParams(CNF_GLOBAL);

            infoWidget->raise();
            infoWidget->setLabelText(myLan.operate_sucess_system_reboot);
            infoWidget->showSecs();

            system("sync");
            gFlow.msleep(2000);
            system("killall mystart.sh; killall myrestart.sh; cd /app && ./myrestart.sh &");
        }
    }
}

/**
 * @brief StatisticsWidget::导出数据
 */
void StatisticsWidget::onExportBtnClicked()
{
    setExportTypeWidget dlg(g_mainWidget);
    dlg.setarrayBtnshow();
    int ret = dlg.exec();
    if(ret == QDialog::Rejected) {
        return;
    }

    infoWidget->raise();
    infoWidget->setLabelText(QString("%1....").arg(myLan.export_data_and_wait));
    infoWidget->delayShow();

    m_exArrayType = dlg.getArraytype();
    QString textData;
    if(dlg.getSelIndex() == 0) {//! 导出表格中所有选中项数据
        textData = exportSelectedRecordsToUSB();
    } else { //! 导出当前时间段
        getData(m_sqlquery);

        m_sqlquery.last();
        int sqlsize = m_sqlquery.at() + 1;
        if (sqlsize <= 0) {
            infoWidget->setLabelText(QString("%1!").arg(myLan.no_check_valid_data));
            infoWidget->showSecs();
            return;
        }

        textData = ProcessExportData();
    }


    if(textData.length()>16){
        QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        QString name = QString("%1%2.csv").arg(APP_DIR).arg(dateTime);
        QFile csvFile(name);
        if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QTextStream out(&csvFile);
            out.setGenerateByteOrderMark(true);
            out.setCodec("GB18030");
            out << textData;
            csvFile.close();
        }

        //! 检查U盘
        if (!gFlow.mountUDisk()) {
            infoWidget->setLabelText(QString("%1!").arg(myLan.no_find_udisk));
            infoWidget->showSecs();
            csvFile.remove();
            return;
        }

        bool bCopyOk = true;
        if(!QFile::copy(QString("%1%2.csv").arg(APP_DIR).arg(dateTime),
                         QString("%1%2.csv").arg(MOUNT_UDISK_PATH).arg(dateTime)))
            bCopyOk = false;

        gFlow.sleep(2);
        gFlow.umountUDisk(MOUNT_UDISK_PATH);
        gFlow.sleep(2);
        csvFile.remove();

        if (bCopyOk) {
            infoWidget->setLabelText(QString("%1!").arg(myLan.export_sucess));
        } else {
            infoWidget->setLabelText(QString("%1!").arg(myLan.export_failed));
        }
        infoWidget->showSecs();
    }else{
        infoWidget->setLabelText(QString("%1!").arg(myLan.export_failed));
        infoWidget->showSecs();
    }
}

/**
 * @brief StatisticsWidget::上一页
 */
void StatisticsWidget::onPreBtnClicked()
{
    m_nextBtn->setStyleSheet(g_enableBtnStyle);
    m_nextBtn->setEnabled(true);

    if (m_nIndexOfPage > 0) {
        m_nIndexOfPage--;

        refreshTableView(m_sqlquery);

        //! 更新选中项数量显示
        updateSelectedCountDisplay();
    }
}

/**
 * @brief StatisticsWidget::下一页
 */
void StatisticsWidget::onNextBtnClicked()
{
    m_preBtn->setStyleSheet(g_enableBtnStyle);
    m_preBtn->setEnabled(true);

    initTableView();

    m_nIndexOfPage++;
    refreshTableView(m_sqlquery);

    //! 更新选中项数量显示
    updateSelectedCountDisplay();
}

/*!
 * \brief 槽函数: 表格点击事件
 */
void StatisticsWidget::onTableViewClicked(const QModelIndex &index)
{
    m_delSingleBtn->setStyleSheet(g_enableBtnStyle);
    m_delSingleBtn->setEnabled(true);
    m_printBtn->setEnabled(true);

    QStandardItemModel *model = (QStandardItemModel*)tableView->model();
    int row = index.row();
    QString date = model->data(model->index(row, 0)).toString();

    if(date.isEmpty()) {
        return;
    }

    bool bsel = tableView->selectionModel()->isSelected(index);

    //! 更新跨页选择状态
    if(bsel) {
        if(!m_globalSelectedSet.contains(date)) {
            m_globalSelectedSet.insert(date);
            m_crossPageSelectedRecords.append(date);
        }
    } else {
        if(m_globalSelectedSet.contains(date)) {
            m_globalSelectedSet.remove(date);
            m_crossPageSelectedRecords.removeAll(date);
        }
    }

    //! 更新原有的分析记录列表（保持向后兼容）
    if(m_dateRecordlst.contains(date)){
        if(!bsel){
            m_dateRecordlst.removeOne(date);
        }
    }else{
        if(bsel){
            m_dateRecordlst.append(date);
        }
    }

    //! 更新UI显示
    updateSelectedCountDisplay();
}

void StatisticsWidget::onTableViewItemChg(QModelIndex curItem, QModelIndex preItem)
{
    if(curItem.row() >=0){
        onTableViewClicked(curItem);
    }

    if(preItem.row() >=0){
        onTableViewClicked(preItem);
    }
}

/*!
 * \brief 槽函数: 表头点击事件
 */
void StatisticsWidget::onHeaderViewClicked(int index)
{
    m_headerSecIndex = index;
    if(index < 3 || index >= tableView->model()->columnCount()-1)return;

    QRect rect = tableView->horizontalHeader()->geometry();
    int x = rect.x();
    for(int i = m_scrollIndex; i < m_headerSecIndex; ++i) {
        x += tableView->columnWidth(i);
    }
}




/**
 * @brief StatisticsWidget::方案切换
 */
void StatisticsWidget::onSchmBoxChangedSlt(int index)
{
    if(index >= 0 && index < struCnfg.nSchemTotal){
        m_schemeIndex = index;
        onSearchChanged();
    }
}

/**
 * @brief StatisticsWidget::onSrcBoxChangedSlt
 * @param index
 */
void StatisticsWidget::onSrcBoxChangedSlt(int index)
{
    onSearchChanged();
}

/**
 * @brief StatisticsWidget::onScrollBarMoveSlt
 * @param index
 */
void StatisticsWidget::onScrollBarMoveSlt(int index)
{
    QAbstractSlider *slider = (QAbstractSlider*)tableView->horizontalScrollBar();
    if(slider->maximum() == index)
        return;

    m_scrollIndex = index;
}
/*!
 * \brief 槽：详细
 */
void StatisticsWidget::onDetailBtnPressedSlt()
{
    QModelIndexList itemlst = tableView->selectionModel()->selectedRows();
    if(itemlst.size()==0){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_item));
        infoWidget->showSecs(2);
    }else if(itemlst.size()>1){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.not_support_select_multi_item));
        infoWidget->showSecs(2);
    }else{
        m_moretreewid->clear();
        m_moretreewid->setColumnCount(1);
        m_moretreewid->setHeaderLabels(QStringList() << myLan.label);

        for(int i=1; i<MAX_CAM_NUM; i++){
            m_totalpurity[i]->hide();
        }

        int n = itemlst.constFirst().row();
        QStandardItemModel *mdel = (QStandardItemModel*)tableView->model();
        QString datestr =   mdel->data(mdel->index(n,0)).toString();  //! 时间
        QString schme =     struCnfg.schemeList[m_schemeIndex].code;


        QString querycolor = QString("SELECT * FROM '%1' WHERE LOGTIME = '%2'")
                                 .arg(schme).arg(datestr);

        m_sqlquery = g_sqliteInter->executeCmd(querycolor);

        QStringList matlst,metallst;
        if(m_sqlquery.first()){
            do{
                QString     contstr = m_sqlquery.value(6).toString();
                QJsonObject json =  gFlow.qstr2Json(contstr);
                QJsonArray  jsonarray = json["data"].toArray();

                //! 统计金属
                if(struCnfg.nMachineType == nMachine_Bottle &&
                    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect){
                    if(json.contains("metal")){
                        metallst<<QString("%1: ").arg(myLan.metal)
                                 <<QString::number(json["metal"].toInt())
                                 <<(json["metalsta"].toBool()?myLan.abnormal:myLan.normal);
                    }
                }

                for(int i=0; i<jsonarray.size(); i++){
                    if(jsonarray[i].isObject()){
                        QJsonObject json  = jsonarray[i].toObject();
                         
                        QStringList keylst = json.keys();
                        for(QString key : keylst){
                            if(key.contains("mat")){//! 统计物料
                                QJsonObject matobj = json[key].toObject();
                                for(QString matkey : matobj.keys()){
                                    matlst<<matkey<<matobj[matkey].toString();
                                }
                            }else if(key.contains("lab")){//! 统计标签
                                QJsonArray labjsonary = json[key].toArray();
                                QStringList labtitle;
                                if(labjsonary.size() > 0){
                                    labtitle = labjsonary[0].toObject().keys();
                                    labtitle.removeOne("child");
                                    labtitle.removeOne("name");
                                    labtitle.removeOne("stat");

                                    QStringList headerlst;
                                    headerlst<<myLan.label;
                                    if(labtitle.size()>0){
                                        headerlst.append(labtitle);
                                        headerlst.append(myLan.status);
                                        m_moretreewid->setHeaderLabels(headerlst);

                                        for(int i=0; i<headerlst.size(); i++){
                                            int nwid = (WIN_WID-300)/headerlst.size();
                                            m_moretreewid->setColumnWidth(i,nwid);
                                        }
                                    }
                                }

                                for(int k=0; k<labjsonary.size(); k++){
                                    QTreeWidgetItem* topitem = new QTreeWidgetItem();
                                    QJsonObject labjson = labjsonary[k].toObject();
                                    int nIdx = 0;
                                    QJsonArray childary =labjson["child"].toArray();
                                    for(int m =0; m<childary.size(); m++){
                                        QTreeWidgetItem* childitem = new QTreeWidgetItem();

                                        int ncol = 0;
                                        QJsonObject childjson = childary[m].toObject();
                                        childitem->setData(ncol++,Qt::DisplayRole,childjson["name"].toString());

                                        for(QString catstr : labtitle){
                                            childitem->setData(ncol++, Qt::DisplayRole, childjson[catstr].toString());
                                        }

                                        bool bsta = childjson["stat"].toBool();
                                        if(bsta){
                                            childitem->setData(ncol++,Qt::DecorationRole,QIcon(":/res/png/action_circle_red.png"));
                                        }else{
                                            childitem->setData(ncol++,Qt::DecorationRole,QIcon(":/res/png/action_circle.png"));
                                        }
                                        topitem->addChild(childitem);
                                    }
                                    topitem->setData(nIdx++,Qt::DisplayRole,labjson["name"].toString());
                                    for(QString catstr : labtitle){
                                        topitem->setData(nIdx++, Qt::DisplayRole, labjson[catstr].toString());
                                    }
                                    bool bsta = labjson["stat"].toBool();
                                    if(bsta){
                                        topitem->setData(nIdx++,Qt::DecorationRole,QIcon(":/res/png/action_circle_red.png"));
                                    }else{
                                        topitem->setData(nIdx++,Qt::DecorationRole,QIcon(":/res/png/action_circle.png"));
                                    }
                                    m_moretreewid->addTopLevelItem(topitem);
                                }
                            }
                        }
                    }
                }

                // if(struCnfg.nMachineType == nMachine_Bottle){
                //     if(json.contains("metal")){
                //         int value = json["metal"].toInt();
                //         m_metalnumLb->setText(QString("%1: %2").arg(myLan.metal).arg(value));
                //         m_metalnumLb->show();
                //     }
                // }else{
                //     m_metalnumLb->hide();
                // }
            }while(m_sqlquery.next());
        }

       QString titlestr;
       for(int n =0; n<matlst.size(); n++){
            if((n!=0)&& (n%2==0)){
                titlestr +="  ";
            }

            titlestr +=matlst.at(n);
        }

       if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnmetalDetect){
            for(QString metalvar :  metallst){
                titlestr +=metalvar;
                titlestr+=" ";
            }
        }

       m_totalpurity[0]->setText(titlestr);

       m_stackwid->setCurrentIndex(1);
    }
}
/*!
 * \brief 槽：返回
 */
void StatisticsWidget::onBckBtnPressedSlt()
{
    m_stackwid->setCurrentIndex(0);
}

/*!
 * \brief 槽: 跳转数据分析对比页面
 */
void StatisticsWidget::onSkip2AnalyzePage()
{
    int nType = m_AnltypeComb->currentIndex();
    if(nType == 0){
        QModelIndexList itemlst = tableView->selectionModel()->selectedRows();
        if(itemlst.size() == 0){
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_item));
            infoWidget->showSecs(3);
        }else if(itemlst.size()>1){
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.not_support_select_multi_item));
            infoWidget->showSecs(3);
        }else{
            onRefreshPie();
            m_stalayout->setCurrentIndex(0);
            m_stackwid->setCurrentIndex(2);
        }
    }else if(nType ==1){
        refreshanalyzepage();
        m_stalayout->setCurrentIndex(1);
        m_stackwid->setCurrentIndex(2);
    }

    return;
}

/*!
 * \brief 槽：刷新数据对比勾选数据项使能
 */
void StatisticsWidget::onRefreshRecordBtn()
{
    m_dateRecordlst.clear();
}

/*!
 * \brief 动态刷新qpieslices
 * \param slice
 */
void StatisticsWidget::onRefreshPieSlice(QPieSlice *slice)
{
    QPieSeries *series = qobject_cast<QPieSeries*>(sender());
    if(!series)
        return;

    for(auto s : series->slices()){
        s->setExploded(s == slice);
        s->setExplodeDistanceFactor(s ==slice? 0.05:0);
        s->setLabelVisible(s == slice);
    }
}

/*!
 * \brief 刷新饼状图
 */
void StatisticsWidget::onRefreshPie()
{

    QModelIndexList itemlst = tableView->selectionModel()->selectedRows();
    QStandardItemModel *model = (QStandardItemModel*)tableView->model();
    int row = itemlst.constFirst().row();

    QString  schcode = struCnfg.schemeList[m_schemeIndex].code;
    QString date = model->data(model->index(row, 1)).toString(); //! 时间
    if(date.compare("") == 0){
        return;
    }

    m_vdataMap.clear();
    m_vcattypelst.clear();

    QString querycolor = QString("SELECT * FROM '%1' WHERE LOGTIME = '%2'")
                             .arg(schcode).arg(date);
    m_sqlquery = g_sqliteInter->executeCmd(querycolor);


    if(m_sqlquery.first()){
        do{
            QString valstr = m_sqlquery.value(3).toString();
            QJsonObject json = gFlow.qstr2Json(valstr);

            if(json["data"].isArray()){
                QJsonArray  jsonarray = json["data"].toArray();
                m_vcattypelst.resize(jsonarray.size());
                for(int k=0; k<jsonarray.size(); k++){
                    if(!m_vStagpId.contains(k)){
                        continue;
                    }

                    if(jsonarray[k].isObject()){
                        QStringList labtitle;
                        QJsonObject labjson  = jsonarray[k].toObject();
                        QJsonArray  labary = labjson[QString("stagp_%1_lab").arg(k)].toArray();
                        if(labary.size()>0){
                            labtitle = labary[0].toObject().keys();
                            labtitle.removeOne("child");
                            labtitle.removeOne("name");
                            labtitle.removeOne("stat");
                        }
                        m_vcattypelst[k] = labtitle;

                        QMap<QString,QMap<QString,float>> map;
                        for(int m=0; m<labary.size(); m++){
                            QJsonObject datajson = labary[m].toObject();
                            QString strname;
                            float nval = 0.0;
                            strname = datajson["name"].toString();

                            QMap<QString,float> datamap;
                            for(QString key : labtitle){
                                QString tmpkey = key;
                                 QString strval = datajson[tmpkey].toString();
                                if(strval.contains("%")){
                                    nval = strval.remove("%").toDouble();
                                }else{
                                    nval = strval.toDouble();
                                }
                                datamap[key] = nval;
                            }
                            map[strname] = datamap;
                        }
                        m_vdataMap.push_back(map);
                    }
                }
            }
        }while(m_sqlquery.next());
    }

    if(m_vcattypelst.size() >0 && m_vcattypelst[0].size()>0
        && m_vdataMap.size()>0){
        m_Gbid = 0;
        m_catIdx =0;
        m_cattypeBtn->setText(QString("%1").arg(m_vcattypelst[m_Gbid].at(m_catIdx)));
        m_stagbBtn->setText(QString("%1%2").arg(myLan.stagp).arg(m_Gbid));
        onRefreshDataOfPie(0,m_vcattypelst[0].first());
    }
}

/*!
 * \brief 刷新饼状图的数据源
 */
void StatisticsWidget::onRefreshDataOfPie(int nGid, QString strCat)
{
    for(int i=0; i<MAX_CAM_NUM; i++){
         m_piesers[i]->clear();
        if(i == nGid){
            m_pieChartView[i]->show();
        }else{
            m_pieChartView[i]->hide();
        }
    }

    float sum =0.0;
    QMap<QString,float> tmpMap;
    for(int n =0; n<m_vdataMap.size(); n++){
        if(n!=nGid){
            continue;
        }
        QMap<QString,QMap<QString,float>>::const_iterator itor = m_vdataMap[n].constBegin();
        while (itor != m_vdataMap[n].constEnd()) {
            QMap<QString,float> map = itor.value();
            if(map.contains(strCat)){
                float val = map[strCat];
                tmpMap[itor.key()] = val;
                sum +=val;
            }
            ++itor;
        }

        for(QString key : tmpMap.keys()){
            float nval = tmpMap[key]/sum;
            m_piesers[n]->append(QString("%1 %2%").arg(key).arg(QString::number(nval*100,'f',2)), nval*100);

        }
    }
}

/*!
 * \brief 统计方式修改
 * \param nIdx
 */
void StatisticsWidget::onAnalyTypeChg(int nIdx)
{
    m_recordBtn->setVisible(false);
    if(nIdx == 1){
        m_recordBtn->setVisible(true);
    }
}

/*!
 * \brief 刷新数据分析的数据源
 */
void StatisticsWidget::onRefresAnalyPageDataByCat()
{
    if(m_Gbid < m_vcattypelst.size()){
        m_catIdx +=1;
        if(m_catIdx >=m_vcattypelst[m_Gbid].size()){
            m_catIdx =0;
        }

        m_cattypeBtn->setText(QString("%1").arg(m_vcattypelst[m_Gbid].at(m_catIdx)));

        if(m_AnltypeComb->currentIndex() == 0){
            onRefreshDataOfPie(m_Gbid,m_vcattypelst[m_Gbid].at(m_catIdx));
        }else{
            onRefreshDateofBar(m_Gbid,m_vcattypelst[m_Gbid].at(m_catIdx));
        }
    }
}

void StatisticsWidget::onRefreshDateofBar(int nGgid, QString staCat)
{
    QStringList axisdateslst;
    QMultiMap<QString,float> labstamap;
    for(QString strkey : m_vDataBarMap.keys()){
        if(nGgid<m_vDataBarMap[strkey].size()){
            QMap<QString,QMap<QString,float>>::const_iterator itor = m_vDataBarMap[strkey][nGgid].constBegin();
            while (itor != m_vDataBarMap[strkey][nGgid].constEnd()) {
                QMap<QString,float> map = itor.value();
                if(map.contains(staCat)){
                    QString keyname = itor.key();
                    labstamap.insert(keyname,map[staCat]);
                }
                ++itor;
            }
            axisdateslst<<strkey;
        }
    }
    //! 重置chart
    QList<QString> keylst = labstamap.uniqueKeys();
    m_chart->removeAllSeries();
    int nmax = 0;
    QBarSeries *barseries = new QBarSeries();
    for(QString var: keylst){
        QBarSet *set = new QBarSet(var);
        for(int val : labstamap.values(var)){
            *set<<val;
            if(val > nmax){
                nmax = val;
            }
        }
        barseries->append(set);
    }
    m_chart->addSeries(barseries);

    m_axisX->clear();
    m_axisX->append(axisdateslst);
    m_axisX->setRange(axisdateslst.first(),axisdateslst.last());

    m_chart->setAxisX(m_axisX, barseries);
    m_chart->setAxisY(m_axisY, barseries);
    m_axisY->setRange(0, nmax);
}

void StatisticsWidget::onRefresAnalyPageDataByGbid()
{
    m_Gbid +=1;
    if(m_Gbid>=m_vcattypelst.size()){
        m_Gbid =0;
    }

    m_stagbBtn->setText(QString("%1%2").arg(myLan.stagp).arg(m_Gbid));

    if(m_Gbid<m_vcattypelst.size()){
        if(m_AnltypeComb->currentIndex() == 0){
            onRefreshDataOfPie(m_Gbid,m_vcattypelst[m_Gbid].at(m_catIdx));
        }else{
            onRefreshDateofBar(m_Gbid,m_vcattypelst[m_Gbid].at(m_catIdx));
        }
    }
}

/**
 * @brief StatisticsWidget::初始化查询时间
 * @return
 */
bool StatisticsWidget::initDateTime()
{
    dateTimeFrom = dateTimeEditFrom->dateTime();
    dateTimeTo   = dateTimeEditTo->dateTime();
    if(dateTimeFrom > dateTimeTo){
        emit showTip();

        dateTimeFrom = dateTimeTo;
        dateTimeEditFrom->setDateTime(dateTimeTo);
        return false;
    }
    return true;
}

/**
 * @brief StatisticsWidget::初始化表格数据
 */
void StatisticsWidget::initTableView()
{
    //! 删除旧的model
    if(QAbstractItemModel *oldModel = tableView->model()) {
        delete oldModel;
    }

    QStringList titleList;
    titleList << myLan.date << myLan.batch  << myLan.detector <<myLan.yield <<myLan.status;

    int row = MAX_COUNT_PER_PAGE;
    int column = titleList.size();
    QStandardItemModel *model = new QStandardItemModel(row, column, this);
    for(int i = 0; i < column; i++){
        model->setHorizontalHeaderItem(i, new QStandardItem(titleList[i]));
    }

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {//! 居中对齐
            model->setData(model->index(i, j), "");
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    tableView->setModel(model);

    for(int i = 0; i < row; i++) {
        tableView->setRowHeight(i,50);
    }
}

/**
 * @brief StatisticsWidget::刷新按钮状态
 * @param nCurPage
 * @param nRecordNum
 */
void StatisticsWidget::refreshBtnStatus(int nCurPage, int nRecordNum)
{
    int nTotalPages = nRecordNum%MAX_COUNT_PER_PAGE == 0 ? nRecordNum/MAX_COUNT_PER_PAGE : nRecordNum/MAX_COUNT_PER_PAGE + 1;

    if(0 == nTotalPages)
        nTotalPages = 1;

    //! 总数一页，按钮禁用
    if(nTotalPages <= 1) {
        m_preBtn->setStyleSheet(g_disableBtnStyle);
        m_preBtn->setEnabled(false);
        m_nextBtn->setStyleSheet(g_disableBtnStyle);
        m_nextBtn->setEnabled(false);
    }
    else {
        //! 总数超过一页，当前第一页
        if(nCurPage == 0) {
            m_preBtn->setStyleSheet(g_disableBtnStyle);
            m_preBtn->setEnabled(false);
            m_nextBtn->setStyleSheet(g_enableBtnStyle);
            m_nextBtn->setEnabled(true);
        }
        else {
            //! 当前中间页
            m_preBtn->setStyleSheet(g_enableBtnStyle);
            m_preBtn->setEnabled(true);
            if(nCurPage+1 < nTotalPages) {
                m_nextBtn->setStyleSheet(g_enableBtnStyle);
                m_nextBtn->setEnabled(true);
            }
            else {
                //! 当前最后页
                m_nextBtn->setStyleSheet(g_disableBtnStyle);
                m_nextBtn->setEnabled(false);
            }
        }
    }

    m_indexLbl->setText(QString("%1:%2/%3，%4%5").arg(myLan.currentpage).arg(nCurPage+1)
                                .arg(nTotalPages).arg(nRecordNum).arg(myLan.record));
}

/**
 * @brief StatisticsWidget::数据检索
 */
void StatisticsWidget::getData(QSqlQuery &sqlquery)
{
    QString sCurSchcode = struCnfg.schemeList[m_schemeIndex].code;

    QString query = QString("SELECT * FROM '%1' WHERE (LOGTIME BETWEEN '%2' AND '%3') ORDER BY LOGTIME DESC")
                                .arg(sCurSchcode)
                                .arg(dateTimeFrom.date().toString("yyyy-MM-dd")+QString(" 00:00:00"))
                                .arg(dateTimeTo.date().toString("yyyy-MM-dd")+QString(" 23:59:59"));
    sqlquery = g_sqliteInter->executeCmd(query);
}

/**
 * @brief StatisticsWidget::刷新表格显示
 * @param sqlquery
 */
void StatisticsWidget::refreshTableView(QSqlQuery sqlquery)
{

    QStandardItemModel *model = (QStandardItemModel*)tableView->model();
    int nModelRow = 0;

    //! 根据查询记录条数刷新按钮
    sqlquery.last();
    int sqlsize = sqlquery.at()+1;
    if(sqlsize < 0){
        sqlsize = 0;
        return;
    }

    refreshBtnStatus(m_nIndexOfPage, sqlsize);

    //! 获取起始地址
    int startIndex = m_nIndexOfPage*MAX_COUNT_PER_PAGE;
    int count = startIndex;
    sqlquery.first();
    sqlquery.seek(startIndex);

    do{
        QString date = sqlquery.value(0).toString();
        QString batch = sqlquery.value(1).toString();
        QString duty = sqlquery.value(2).toString();
        float   nYield = sqlquery.value(4).toFloat();
        int     nSta = sqlquery.value(3).toInt();

        if(batch.contains("%*")){
            batch.replace("%*","%");
        }

        model->setData(model->index(nModelRow, 0), date );
        model->setData(model->index(nModelRow, 1), batch);
        model->setData(model->index(nModelRow, 2), duty);
        model->setData(model->index(nModelRow, 3), QString::number(nYield,'f',2));
        model->setData(model->index(nModelRow, 4), nSta == 0 ? myLan.normal : myLan.abnormal);

        if(nSta != 0) {
            model->item(nModelRow, 4)->setForeground(QBrush(Qt::red));
        } else {
            model->item(nModelRow, 4)->setForeground(QBrush(Qt::black));
        }

        nModelRow++;
        count++;

    }while(count <(m_nIndexOfPage+1)*MAX_COUNT_PER_PAGE && sqlquery.next());

    //! 恢复跨页选择状态
    updateCrossPageSelection();

    infoWidget->hide();
}

/**
 * @brief StatisticsWidget::刷新方案下拉框
 */
void StatisticsWidget::updateSchmCbx()
{
    disconnect(m_selSchmBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSchmBoxChangedSlt(int)));
    m_selSchmBox->clear();

    QStringList schemeList;
    for(int i = 0; i < struCnfg.schemeList.size(); i++){
        schemeList << struCnfg.schemeList[i].name;
    }

    m_selSchmBox->addItems(schemeList);
    m_selSchmBox->setCurrentIndex(m_schemeIndex);
    connect(m_selSchmBox,    SIGNAL(currentIndexChanged(int)), this, SLOT(onSchmBoxChangedSlt(int)));
}


/*!
 * \brief 刷新分析页面
 */
void StatisticsWidget::refreshanalyzepage()
{
    if(m_dateRecordlst.size()==0){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1%2").arg(myLan.no_select_item));
        infoWidget->showSecs(3);
    }else{

        m_vcattypelst.clear();
        m_vDataBarMap.clear();

        int num = 0;
        QSqlQuery sqlquery;
        for(QString var : m_dateRecordlst){
            if(num >30){
                break;
            }

            QString schme = struCnfg.schemeList[m_schemeIndex].code;
            QString querycolor = QString("SELECT * FROM '%1' WHERE LOGTIME = '%2'")
                                     .arg(schme).arg(var);
            sqlquery = g_sqliteInter->executeCmd(querycolor);
            if(sqlquery.first()){
                do{
                    QString date = sqlquery.value(0).toString();
                    QString batch = QString("%1").arg(sqlquery.value(1).toString());
                    QString contstr = sqlquery.value(3).toString();
                    QJsonObject json = gFlow.qstr2Json(contstr);
                    if(json["data"].isArray()){
                        QVector<QMap<QString,QMap<QString,float>>> vlabResMap;
                        QJsonArray jsonarray = json["data"].toArray();
                        m_vcattypelst.resize(jsonarray.size());
                        for(int i=0; i<jsonarray.size(); i++){//! 统计组
                            if(!m_vStagpId.contains(i)){
                                continue;
                            }

                            if(jsonarray[i].isObject()){
                                QJsonObject labjson = jsonarray[i].toObject();
                                QJsonArray labary  = labjson[QString("stagp_%1_lab").arg(i)].toArray();

                                QStringList labtitle;
                                if(labary.size()>0){
                                    labtitle = labary[0].toObject().keys();
                                    labtitle.removeOne("child");
                                    labtitle.removeOne("name");
                                    labtitle.removeOne("stat");
                                }

                                m_vcattypelst[i] = labtitle;

                                QMap<QString,QMap<QString,float>> labMap;
                                for(int m=0; m<labary.size(); m++){
                                    QMap<QString,float> datamap;
                                    QJsonObject datajson = labary[m].toObject();

                                    for(QString key : labtitle){
                                        QString strval = datajson[key].toString();
                                        if(strval.contains("%")){
                                            strval = strval.remove("%");
                                        }
                                        datamap[key] =  strval.toDouble();
                                    }
                                    labMap[datajson["name"].toString()] = datamap;
                                }

                                vlabResMap.push_back(labMap);
                            }
                        }
                        m_vDataBarMap[date]=vlabResMap;
                    }
                    num++;
                }while(sqlquery.next());
            }
        }



        if(m_vcattypelst.size() >0 && m_vcattypelst[0].size()>0
            && m_vDataBarMap.size()>0 && m_vDataBarMap.first().size() >0){
            m_Gbid = 0;
            m_catIdx =0;
            m_cattypeBtn->setText(QString("%1").arg(m_vcattypelst[m_Gbid].at(m_catIdx)));
            m_stagbBtn->setText(QString("%1%2").arg(myLan.stagp).arg(m_Gbid));
            onRefreshDateofBar(0,m_vcattypelst[0].first());
        }
    }
}

/**
 * @brief StatisticsWidget::删除统计数据库文件
 */
bool StatisticsWidget::removeStadb()
{
    bool bSucess = false;
    QString dbPath = "/ssd/statisticDataLog.db";
    QFile file(dbPath);

    // 检查文件是否存在
    if (!file.exists()) {
        myMessageBox msgBox(MSG_TIPS, myLan.no_find_file, "", g_mainWidget);
        msgBox.exec();
        return false;
    }

    // 用户确认提示
    myMessageBox msgBox(MSG_QUES, myLan.confirm_del, "", g_mainWidget);
    int ret = msgBox.exec();

    if (ret != QDialog::Accepted) {
        // 用户取消删除操作
        return false;
    }

    // 执行删除操作
    if (file.remove()) {
        myMessageBox msgBox(MSG_TIPS, myLan.operate_sucess, "", g_mainWidget);
        msgBox.exec();
        bSucess = true;
    } else {
        myMessageBox msgBox(MSG_TIPS, myLan.operate_failed, "", g_mainWidget);
        msgBox.exec();
    }

    return bSucess;
}

/*!
 * \brief 更新跨页选择状态 - 恢复当前页面的选中状态
 */
void StatisticsWidget::updateCrossPageSelection()
{
    QStandardItemModel *model = (QStandardItemModel*)tableView->model();
    if (!model) {
        return;
    }

    //! 清除当前选择
    tableView->selectionModel()->clearSelection();

    //! 遍历当前页面的所有行，恢复选中状态
    for (int row = 0; row < model->rowCount(); ++row) {
        QString date = model->data(model->index(row, 1)).toString();
        if (!date.isEmpty() && m_globalSelectedSet.contains(date)) {
            //! 恢复选中状态
            tableView->selectionModel()->select(model->index(row, 0),
                                              QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
}

/*!
 * \brief 更新选中项数量显示
 */
void StatisticsWidget::updateSelectedCountDisplay()
{
    int selectedCount = m_crossPageSelectedRecords.size();
    m_selectedCountLbl->setText(QString("%1: %2").arg(myLan.selected_item).arg(selectedCount));

    //! 根据选中项数量控制按钮状态
    bool hasSelection = selectedCount > 0;
    m_clearSelectionBtn->setEnabled(hasSelection);

    if (hasSelection) {
        m_clearSelectionBtn->setStyleSheet(g_enableBtnStyle);
    } else {
        m_clearSelectionBtn->setStyleSheet(g_disableBtnStyle);
    }
}

/*!
 * \brief 清除跨页选择状态
 */
void StatisticsWidget::clearCrossPageSelection()
{
    //! 清除原有的记录列表
    m_crossPageSelectedRecords.clear();
    m_globalSelectedSet.clear();
    m_dateRecordlst.clear();

    //! 清除当前页面的选择
    if (tableView && tableView->selectionModel()) {
        tableView->selectionModel()->clearSelection();
    }

    //! 更新显示
    updateSelectedCountDisplay();
}


QString StatisticsWidget::ProcessExportData()
{
    QString textData;
    bool bFlag = false;
    if (m_sqlquery.first()) {
        do {
            QString date = m_sqlquery.value(0).toString();
            QString batch = m_sqlquery.value(1).toString();
            QString duty = m_sqlquery.value(2).toString();
            QString contstr = m_sqlquery.value(6).toString();
            QJsonObject json = gFlow.qstr2Json(contstr);

            if (json["data"].isArray()) {
                QJsonArray jsonarray = json["data"].toArray();
                for (int i = 0; i < jsonarray.size(); i++) {
                    if(m_exArrayType == 0){//! 横排
                        if(jsonarray[i].isObject()){
                            QJsonObject labjson = jsonarray[i].toObject();

                            QJsonObject matjson;
                            QJsonArray  labary;
                            QStringList keylst = labjson.keys();
                            for(QString key : keylst){
                                if(key.contains("mat")){
                                    matjson = labjson[key].toObject();
                                }else if(key.contains("lab")){
                                    labary = labjson[key].toArray();
                                }
                            }
                          
                            //! 标题
                            if(bFlag){
                                textData += QString("%1,").arg(myLan.date);
                                textData += QString("%1,").arg(myLan.batch);
                                textData += QString("%1,").arg(myLan.detector);
                                for(QString matkey : matjson.keys()){
                                    textData += QString("%1,").arg(matkey);
                                }

                                if(labary.size()>0){
                                    QStringList labkeys = labary[0].toObject().keys();
                                    for(QString val : labkeys){
                                        if(val != "child"){
                                            if(val == "name"){
                                                textData += QString("%1,").arg(myLan.label);
                                            }else if(val == "stat"){
                                                textData += QString("%1,").arg(myLan.status);
                                            }else{
                                                textData += QString("%1,").arg(val);
                                            }
                                        }
                                    }
                                }
                                if(struCnfg.nMachineType == nMachine_Bottle){
                                    textData += QString("%1,").arg(myLan.metal);
                                }
                                textData +="\n";
                                bFlag = false;
                            }

                            textData += QString("%1,").arg(date);
                            textData += QString("%1,").arg(batch);
                            textData += QString("%1,").arg(duty);

                            //! 统计物料
                            for(QString matkey : matjson.keys()){
                                textData += QString("%1,").arg(matjson[matkey].toString());
                            }

                            //! 统计标签
                            for(int k=0; k<labary.size(); k++){
                                QJsonObject labobj = labary[k].toObject();
                                QJsonArray childary = labobj["child"].toArray();

                                for(QString labkey: labobj.keys()){
                                    if(!labkey.contains("child")){
                                        if(labkey.contains("stat")){
                                            textData +=QString("%1,").arg((labobj[labkey].toBool()?myLan.abnormal:myLan.normal));
                                        }else{
                                            textData +=QString("%1,").arg(labobj[labkey].toString());
                                        }
                                    }
                                }
                            }
                        }
                        if(json.contains("metal")){
                            textData += QString("%1,").arg(json["metal"].toInt());
                        }
                        textData +="\n";
                    }else{//! 竖排
                        if(jsonarray[i].isObject()){
                            QJsonObject labjson = jsonarray[i].toObject();
                           
                            QJsonObject matjson;
                            QJsonArray labary;
                    
                            QStringList keylst = labjson.keys();
                            for(QString key : keylst){
                                if(key.contains("mat")){
                                    matjson = labjson[key].toObject();
                                }else if(key.contains("lab")){
                                    labary = labjson[key].toArray();
                                }
                            }

                            //! 统计物料
                            if(matjson.keys().size()>0){
                                textData += QString("%1,").arg(myLan.date);
                                textData += QString("%1,").arg(myLan.batch);
                                textData += QString("%1,").arg(myLan.detector);
                                for(QString matkey : matjson.keys()){
                                    textData += QString("%1,").arg(matkey);
                                }
                                textData +="\n";

                                textData += QString("%1,").arg(date);
                                textData += QString("%1,").arg(batch);
                                textData += QString("%1,").arg(duty);
                                for(QString matkey : matjson.keys()){
                                    textData += QString("%1,").arg(matjson[matkey].toString());
                                }
                                textData +="\n";
                            }

                            //! 统计标签
                            textData += QString("%1,").arg(myLan.date);
                            textData += QString("%1,").arg(myLan.batch);
                            textData += QString("%1,").arg(myLan.duty_shift);

                            if(labary.size()>0){
                                QStringList labkeys = labary[0].toObject().keys();
                                for(QString val : labkeys){
                                    if(val != "child"){
                                        if(val == "name"){
                                            textData += QString("%1,").arg(myLan.label);
                                        }else if(val == "stat"){
                                            textData += QString("%1,").arg(myLan.status);
                                        }else{
                                            textData += QString("%1,").arg(val);
                                        }
                                    }
                                }
                            }
                            textData +="\n";

                            for(int k=0; k<labary.size(); k++){
                                textData += QString("%1,").arg(date);
                                textData += QString("%1,").arg(batch);
                                textData += QString("%1,").arg(duty);

                                QJsonObject labobj = labary[k].toObject();
                                QJsonArray childary = labobj["child"].toArray();
                                for(QString labkey: labobj.keys()){
                                    if(!labkey.contains("child")){
                                        if(labkey.contains("stat")){
                                            textData +=QString("%1,").arg((labobj[labkey].toBool()?myLan.abnormal:myLan.normal));
                                        }else{
                                            textData +=QString("%1,").arg(labobj[labkey].toString());
                                        }
                                    }
                                }
                                textData +="\n";
                            }
                        }
                        textData +="\n";
                    }
                }
            }
            if(m_exArrayType !=0 && json.contains("metal")){
                textData += QString("%1,").arg(date);
                textData += QString("%1,").arg(batch);
                textData += QString("%1,").arg(duty);
                textData += QString("%1,%2,%3,").arg(myLan.metal)
                                .arg(json["metal"].toInt())
                                .arg(json[QString("%1sta").arg("metal")].toBool()?myLan.abnormal:myLan.normal);
                textData +="\n";
            }
        } while (m_sqlquery.next());
    }

    return textData;
}

/*!
 * \brief 清除选中项按钮槽函数
 */
void StatisticsWidget::onClearSelectionBtnClicked()
{
    if (m_crossPageSelectedRecords.isEmpty()) {
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_item));
        infoWidget->showSecs(2);
        return;
    }

    QString str = QString("%1?").arg(myLan.confirm_del);
    myMessageBox msgBox(MSG_QUES, str, myLan.notice, g_mainWidget);
    int ret = msgBox.exec();
    if (ret != QDialog::Accepted) {
        return;
    }

    clearCrossPageSelection();

    infoWidget->raise();
    infoWidget->setLabelText(QString("%1!").arg(myLan.operate_sucess));
    infoWidget->showSecs(2);
}

/*!
 * \brief 导出选中记录到U盘
 */
QString StatisticsWidget::exportSelectedRecordsToUSB()
{
    QString exportStr;

    if (m_crossPageSelectedRecords.isEmpty()) {
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_select_item));
        infoWidget->showSecs(3);
        return exportStr;
    }

    infoWidget->raise();
    infoWidget->setLabelText(QString("%1....").arg(myLan.export_data_and_wait));
    infoWidget->delayShow();
     
    //! 导出选中的记录
    for (const QString &selectedDate : m_crossPageSelectedRecords) {
        if (selectedDate.isEmpty()) {
            continue;
        }

        QString schme = struCnfg.schemeList[m_schemeIndex].code;
        QString querycolor = QString("SELECT * FROM '%1' WHERE LOGTIME = '%2'")
                                 .arg(schme).arg(selectedDate);
        m_sqlquery = g_sqliteInter->executeCmd(querycolor);

        exportStr +=ProcessExportData();
    }

    return exportStr;
}
