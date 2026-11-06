#include "sortercompage.h"

sortercompage::sortercompage(QWidget *parent) : QWidget(parent),m_nSortidx(0)
{
    createpage();
    laypage();
    connectAndSlt();
}

void sortercompage::createpage()
{
    setStyleSheet("background-color:white; border-radius:10px;");

    m_wid = new QWidget();

    //! 主页面
    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        m_maxSortNumtileBtn[i] = new QPushButton();
        m_maxSortNumtileBtn[i]->setFixedSize(180,60);
    }

    m_StaicGpBtn = new QPushButton(); //! 相机id
    m_StaicGpBtn->setFixedSize(BTN_WID+50,BTN_HEI);
    m_StaicGpBtn->setIconSize(QSize(50,50));
    m_StaicGpBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));
    
    m_gb = new QGroupBox();
    m_labscroarea = new QScrollArea();
    m_labscroarea->setWidgetResizable(true);
    m_labscroarea->setWidget(m_gb);

    QStringList titlelst;
    titlelst<<myLan.label<<myLan.lower_limit<<myLan.up_limit<<myLan.target;
    for(int i=0; i<titlelst.size(); i++){
        m_titlelb[i] = new QLabel(titlelst.at(i));
    }
    m_titlelb[3]->hide();

    for(int i=0; i<MAX_LABELS; i++){
        m_labchk[i] = new MyCheckBox("");
        m_lowthdle[i] = new MyLineEdit();
        m_lowthdle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_lowthdle[i]->setFixedWidth(120);
        m_higthdle[i] = new MyLineEdit();
        m_higthdle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_higthdle[i]->setFixedWidth(120);
        m_tarthdle[i] = new MyLineEdit();
        m_tarthdle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_tarthdle[i]->setFixedWidth(120);
        m_tarthdle[i]->hide();
    }

    m_showadjpageBtn = new QPushButton(QString("%1").arg(myLan.adj_page));
    m_showadjpageBtn->setFixedSize(BTN_WID+50,BTN_HEI);
    m_showadjpageBtn->setIcon(QIcon(":/res/png/MiddleWare.png"));
    m_showadjpageBtn->setIconSize(QSize(50,50));

    m_renameBtn = new QPushButton(myLan.rename);
    m_renameBtn->setFixedSize(BTN_WID+50, BTN_HEI);
    m_renameBtn->setIcon(QIcon(":/res/png/Edit_Rename.png"));
    m_renameBtn->setIconSize(QSize(50,50));


    //! 参数设置
    createparamspage();
   
    //! 形选标签页面
    QStringList shapetitlelst;
    shapetitlelst<<myLan.label<<QString("%1%2").arg(myLan.lower_limit).arg("(%)")
                  <<QString("%1%2").arg(myLan.up_limit).arg("(%)");
    m_shape_gb = new QGroupBox();
    for(int i=0; i<3; i++){
        m_shapetitlelb[i] = new QLabel(shapetitlelst.at(i));
    }
    for(int i=0; i<MAX_LABELS; i++){
        m_shapechk[i] = new MyCheckBox("");
        m_shapeLowthdle[i] = new MyLineEdit();
        m_shapeLowthdle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_shapeLowthdle[i]->setFixedWidth(120);
        m_shapeHigthdle[i] = new MyLineEdit();
        m_shapeHigthdle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_shapeHigthdle[i]->setFixedWidth(120);
    }
    m_switchPageBtn = new QPushButton(myLan.switch_to_shape);
    m_switchPageBtn->setFixedSize(BTN_WID+50,BTN_HEI);
    m_switchPageBtn->setIcon(QIcon(":/res/png/Action_Switch.png"));
    m_switchPageBtn->setIconSize(QSize(50,50));


    m_switchParamsBtn = new QPushButton(myLan.setting);
    m_switchParamsBtn->setFixedSize(BTN_WID+50,BTN_HEI);
    m_switchParamsBtn->setIcon(QIcon(":/res/png/Action_Configure.png"));
    m_switchParamsBtn->setIconSize(QSize(50,50));
}

/*!
 * \brief sortercompage::createparamspage
 */
void sortercompage::createparamspage()
{
    m_paramsWid = new QWidget();

    //! 取样口参数
    m_params_gb = new QGroupBox();
    m_params_gb->setMaximumHeight(400);
   
    m_EnSortlb = new QLabel(myLan.samplechnelEb);
    m_EnSortlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_EnSortlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_EnSortclass = new MyCheckBox("");
    m_EnSortclass->setCheckBtnSize(BTN_WID,BTN_HEI);

    m_EnAccModelb = new QLabel(myLan.Acc_Mode);
    m_EnAccModelb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_EnAccModelb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_EnAccChk = new MyCheckBox("");
    m_EnAccChk->setCheckBtnSize(BTN_WID,BTN_HEI);

    m_sourcelb = new QLabel(myLan.samplechnelProty);
    m_sourcelb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_sourcelb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_sourcecomb = new QComboBox();
    QListWidget *sourcelistwid = new QListWidget();
    m_sourcecomb->setModel(sourcelistwid->model());
    m_sourcecomb->setView(sourcelistwid);
    m_sourcecomb->setFixedSize(BTN_WID,BTN_HEI);
    m_sourcecomb->insertItem(0,myLan.finished_product);
    m_sourcecomb->insertItem(1,myLan.waste_material);
    m_sourcecomb->insertItem(2,myLan.raw_material);

    m_maxSortNumlb = new QLabel(myLan.max_channel);
    m_maxSortNumlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_maxSortNumlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_maxSortNumle = new MyLineEdit();
    m_maxSortNumle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_maxSortNumle->setFixedSize(BTN_WID+40,BTN_HEI);

    m_minNumlb = new QLabel(QString("%1").arg(myLan.threshold));
    m_minNumlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_minNumlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_minNumle = new MyLineEdit();
    m_minNumle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_minNumle->setFixedSize(BTN_WID+40,BTN_HEI);

    m_maxTimespanlb = new QLabel(QString("%1(min)").arg(myLan.time_threshold));
    m_maxTimespanlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_maxTimespanlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_maxTimespanle = new MyLineEdit();
    m_maxTimespanle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_maxTimespanle->setFixedSize(BTN_WID+40,BTN_HEI);

    m_maxAdjtimelb = new QLabel(QString("%1(%2)").arg(myLan.sigle_channel_time).arg(myLan.times));
    m_maxAdjtimelb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_maxAdjtimelb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_maxAdjtimele = new MyLineEdit();
    m_maxAdjtimele->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_maxAdjtimele->setFixedSize(BTN_WID+40,BTN_HEI);

    m_maxAdjwaitlb = new QLabel(QString("%1(s)").arg(myLan.detect_delay));
    m_maxAdjwaitlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_maxAdjwaitlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_maxAdjwaitle = new MyLineEdit();
    m_maxAdjwaitle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_maxAdjwaitle->setFixedSize(BTN_WID+40,BTN_HEI);

    m_samplespanlb = new QLabel(QString("%1(s)").arg(myLan.sample_interval));
    m_samplespanlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_samplespanlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_samplespanle = new MyLineEdit();
    m_samplespanle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_samplespanle->setFixedSize(BTN_WID+40,BTN_HEI);


    m_localsocketstaLb = new QLabel();
    m_localsocketstaLb->setMidLineWidth(190);

    //! 其他参数
    m_otherparamsgb = new QGroupBox();

    m_Loopspanlb = new QLabel(QString("%1(min)").arg(myLan.Cycle_time));
    m_Loopspanlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_Loopspanlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_Loopspanle = new MyLineEdit();
    m_Loopspanle->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_Loopspanle->setFixedSize(BTN_WID+40,BTN_HEI);

    m_snvopenlb = new QLabel(myLan.Normal_open_valve);
    m_snvopenlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_snvopenlb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_snvopencomb = new QComboBox();
    QListWidget *svnopenlstwid = new QListWidget();
    m_snvopencomb->setModel(svnopenlstwid->model());
    m_snvopencomb->setView(svnopenlstwid);
    m_snvopencomb->setFixedSize(BTN_WID+40,BTN_HEI);

    m_reSetPIDBtn = new QPushButton(QString("%1").arg(myLan.reset_coeff));
    m_reSetPIDBtn->setFixedSize(BTN_WID,BTN_HEI);
    m_reSetPIDBtn->hide();

    m_autoSampleEnlb = new QLabel(myLan.auto_sample);
    m_autoSampleEnlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_autoSampleEnSw = new mySwitchButton();
    m_autoSampleEnSw->setFixedSize(BTN_WID+30,BTN_HEI);


    m_scheduleEnlb = new QLabel(myLan.scheduledfeedEn);  //! 定时吸料
    m_scheduleEnlb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_scheduleEnSw = new mySwitchButton();
    m_scheduleEnSw->setFixedSize(BTN_WID+30,BTN_HEI);

    m_scheTimelb = new QLabel(QString("%1(s)").arg(myLan.scheduledfeedtime));
    m_scheTimelb->setFixedSize(BTN_WID+40,BTN_HEI);
    m_scheTimelb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_scheTimele = new MyLineEdit();
    m_scheTimele->setStyleSheet("background-color:rgba(220,220,220,100)");
    m_scheTimele->setFixedSize(BTN_WID+40,BTN_HEI);

}

void sortercompage::layparamspage()
{
    QVBoxLayout *vlaycol = new QVBoxLayout();
    vlaycol->setContentsMargins(10,10,10,10);
    vlaycol->setSpacing(30);

    auto addControlPair = [](QHBoxLayout* hlay, QWidget* label, QWidget* control) {
        hlay->addWidget(label);
        hlay->addSpacing(20);
        hlay->addWidget(control);
        hlay->addStretch();
    };

    //! 第一列
    QHBoxLayout *hlaysour = new QHBoxLayout();
    QHBoxLayout *hlayen = new QHBoxLayout();
    QHBoxLayout *hlayacc = new QHBoxLayout();


    addControlPair(hlaysour, m_sourcelb, m_sourcecomb);
    addControlPair(hlayen, m_EnSortlb, m_EnSortclass);
    addControlPair(hlayacc, m_EnAccModelb, m_EnAccChk);


    vlaycol->addLayout(hlaysour);
    vlaycol->addLayout(hlayen);
    vlaycol->addLayout(hlayacc);
    vlaycol->addStretch();

    //! 第二列
    QVBoxLayout *vlaycol2 = new QVBoxLayout();
    vlaycol2->setContentsMargins(10,10,10,10);
    vlaycol2->setSpacing(20);

    QHBoxLayout *hlaychnel = new QHBoxLayout();
    QHBoxLayout *hlaynum = new QHBoxLayout();
    QHBoxLayout *hlaytime = new QHBoxLayout();

    addControlPair(hlaychnel, m_maxSortNumlb, m_maxSortNumle);
    addControlPair(hlaynum, m_minNumlb, m_minNumle);
    addControlPair(hlaytime, m_maxTimespanlb, m_maxTimespanle);

    vlaycol2->addLayout(hlaychnel);
    vlaycol2->addLayout(hlaynum);
    vlaycol2->addLayout(hlaytime);
    vlaycol2->addStretch();

    //! 第三列
    QVBoxLayout *vlaycol3 = new QVBoxLayout();
    vlaycol3->setContentsMargins(10,10,10,10);
    vlaycol3->setSpacing(20);

    QHBoxLayout *hlaysigtime = new QHBoxLayout();
    QHBoxLayout *hlaydelay = new QHBoxLayout();
    QHBoxLayout *hlayspan = new QHBoxLayout();

    addControlPair(hlaysigtime, m_maxAdjtimelb, m_maxAdjtimele);
    addControlPair(hlaydelay, m_maxAdjwaitlb, m_maxAdjwaitle);
    addControlPair(hlayspan, m_samplespanlb, m_samplespanle);

    vlaycol3->addLayout(hlaysigtime);
    vlaycol3->addLayout(hlaydelay);
    vlaycol3->addLayout(hlayspan);
    vlaycol3->addStretch();

    //! 第四列
    QVBoxLayout *vlaycol4 = new QVBoxLayout();
    vlaycol4->setContentsMargins(10,10,10,10);
    vlaycol4->setSpacing(30);

    vlaycol4->addWidget(m_reSetPIDBtn, 0, Qt::AlignRight);
    vlaycol4->addStretch();

    QHBoxLayout *vlayparams = new QHBoxLayout(m_params_gb);
    vlayparams->addLayout(vlaycol);
    vlayparams->addSpacing(60);
    vlayparams->addLayout(vlaycol2);
    vlayparams->addSpacing(60);
    vlayparams->addLayout(vlaycol3);
    vlayparams->addSpacing(100);
    vlayparams->addLayout(vlaycol4);


    //! 其他参数布局
    QHBoxLayout *hlayopen = new QHBoxLayout();
    addControlPair(hlayopen, m_snvopenlb, m_snvopencomb);

    QHBoxLayout *hlayloop = new QHBoxLayout();
    addControlPair(hlayloop, m_Loopspanlb, m_Loopspanle);

    QHBoxLayout *hlayauto = new QHBoxLayout();
    addControlPair(hlayauto, m_autoSampleEnlb, m_autoSampleEnSw);

    QHBoxLayout *hlayschduEn = new QHBoxLayout();
    addControlPair(hlayschduEn, m_scheduleEnlb, m_scheduleEnSw);

    QHBoxLayout *hlayschdutime = new QHBoxLayout();
    addControlPair(hlayschdutime, m_scheTimelb, m_scheTimele);


    //! 底部状态标签布局
    QHBoxLayout *hlaydown = new QHBoxLayout();
    hlaydown->addStretch();
    hlaydown->addWidget(m_localsocketstaLb, 0, Qt::AlignCenter);
    hlaydown->addStretch();

    QHBoxLayout *laytop = new QHBoxLayout();
    laytop->addLayout(hlayloop);
    laytop->addLayout(hlayopen);
    laytop->addLayout(hlayauto);

    QHBoxLayout *laymid = new QHBoxLayout();
    laymid->addLayout(hlayschduEn);
    laymid->addLayout(hlayschdutime);
    laymid->addStretch();

    QVBoxLayout *vlayother = new  QVBoxLayout(m_otherparamsgb);
    vlayother->addLayout(laytop);
    vlayother->addLayout(laymid);
    vlayother->addSpacing(15);
    vlayother->addLayout(hlaydown);

    //! 页面主布局
    QVBoxLayout *vlaymain = new QVBoxLayout(m_paramsWid);
    vlaymain->addWidget(m_params_gb);
    vlaymain->addSpacing(40);
    vlaymain->addWidget(m_otherparamsgb);
}

void sortercompage::laypage()
{
    QHBoxLayout *hlaysorterBtn = new QHBoxLayout();
    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        hlaysorterBtn->addWidget(m_maxSortNumtileBtn[i]);
    }
    hlaysorterBtn->addStretch();
    hlaysorterBtn->addWidget(m_StaicGpBtn);

    m_glaylab = new QGridLayout();
    for(int i=0; i<max_titleNum; i++){
        if(i==0){
            m_glaylab->addWidget(m_titlelb[i], 0, i, Qt::AlignLeft);
        }else{
            m_glaylab->addWidget(m_titlelb[i], 0, i, Qt::AlignCenter);
        }
    }
    for(int i=0; i<MAX_LABELS; i++){
        m_glaylab->addWidget(m_labchk[i],  i+1, 0,  Qt::AlignLeft);
        m_glaylab->addWidget(m_lowthdle[i], i+1, 1, Qt::AlignCenter);
        m_glaylab->addWidget(m_higthdle[i], i+1, 2, Qt::AlignCenter);
        m_glaylab->addWidget(m_tarthdle[i], i+1, 3, Qt::AlignCenter);
    }

    QVBoxLayout*vlaymid = new QVBoxLayout();
    vlaymid->addLayout(m_glaylab);
    vlaymid->addStretch();
    m_gb->setLayout(vlaymid);

    QHBoxLayout *hlaymdown= new QHBoxLayout();
    hlaymdown->addSpacing(5);
    hlaymdown->addWidget(m_switchPageBtn);
    hlaymdown->addSpacing(10);
    hlaymdown->addWidget(m_showadjpageBtn);
    hlaymdown->addStretch();
    hlaymdown->addWidget(m_renameBtn);
    hlaymdown->addSpacing(10);
    hlaymdown->addWidget(m_switchParamsBtn);
    hlaymdown->addSpacing(5);

    //! 创建参数设置布局
    layparamspage();

    //! 创建形选参数布局
    QGridLayout *m_glayshapelab = new QGridLayout();
    for(int i=0; i<3; i++){
        if(i==0){
            m_glayshapelab->addWidget(m_shapetitlelb[i], 0, i, Qt::AlignLeft);
        }else{
            m_glayshapelab->addWidget(m_shapetitlelb[i], 0, i, Qt::AlignCenter);
        }
    }
    for(int i=0; i<MAX_LABELS; i++){
        m_glayshapelab->addWidget(m_shapechk[i],  i+1, 0,  Qt::AlignLeft);
        m_glayshapelab->addWidget(m_shapeLowthdle[i], i+1, 1, Qt::AlignCenter);
        m_glayshapelab->addWidget(m_shapeHigthdle[i], i+1, 2, Qt::AlignCenter);
    }

    QVBoxLayout*vlayshapemid = new QVBoxLayout();
    vlayshapemid->addLayout(m_glayshapelab);
    vlayshapemid->addStretch();
    m_shape_gb->setLayout(vlayshapemid);

    //! 主布局
    m_stackedLayout = new QStackedLayout();
    m_stackedLayout->addWidget(m_labscroarea);      //! 第0页：原标签参数
    m_stackedLayout->addWidget(m_shape_gb);         //! 第1页：形选参数
    m_stackedLayout->addWidget(m_paramsWid);        //! 第2页：参数设置
    m_stackedLayout->setCurrentIndex(0);
    
    QVBoxLayout *vlaywid = new QVBoxLayout(m_wid);
    vlaywid->addLayout(hlaysorterBtn);
    vlaywid->addLayout(m_stackedLayout);
    vlaywid->addSpacing(10);
    vlaywid->addLayout(hlaymdown);
    vlaywid->addSpacing(5);

    QVBoxLayout *vlaymain = new QVBoxLayout(this);
    vlaymain->addWidget(m_wid);
}

void sortercompage::connectAndSlt()
{
    connect(m_switchPageBtn, SIGNAL(pressed()), this, SLOT(onSwitchPageBtn()));
    connect(m_switchParamsBtn, SIGNAL(pressed()), this, SLOT(onSwitchParamsBtn()));

    QSignalMapper* sigSortclassbtnmap = new QSignalMapper();
    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        sigSortclassbtnmap->setMapping(m_maxSortNumtileBtn[i], i);
        connect(m_maxSortNumtileBtn[i], SIGNAL(pressed()), sigSortclassbtnmap, SLOT(map()));
    }
    connect(sigSortclassbtnmap,SIGNAL(mapped(int)),this,SLOT(onsorterclassBtnPressed(int)));

    connect(m_StaicGpBtn,SIGNAL(pressed()),this, SLOT(onStaicGpChg()));                      //!  相机视角
    connect(m_sourcecomb,SIGNAL(currentIndexChanged(int)),this,SLOT(onsourceindexchg(int))); //! 化验源
    connect(m_EnSortclass,SIGNAL(toggled(bool)),this,SLOT(onEnSorterclssChg(bool))); //! 次是否使能检测
    connect(m_EnAccChk,SIGNAL(toggled(bool)),this,SLOT(onEnAccChkChg(bool))); //! 次是否使能检测

    QSignalMapper* siglabchkmap = new QSignalMapper();
    for(int i=0; i<MAX_LABELS; i++){
        siglabchkmap->setMapping(m_labchk[i], i);
        connect(m_labchk[i], SIGNAL(pressed()), siglabchkmap, SLOT(map()));
    }
    connect(siglabchkmap,SIGNAL(mapped(int)),this,SLOT(onlabchkstatuschg(int)));

    QSignalMapper* siglablowthdmap = new QSignalMapper(); //! 下阈值
    for(int i=0; i<MAX_LABELS; i++){
        siglablowthdmap->setMapping(m_lowthdle[i], i);
        connect(m_lowthdle[i], SIGNAL(pressed()), siglablowthdmap, SLOT(map()));
    }
    connect(siglablowthdmap,SIGNAL(mapped(int)),this,SLOT(onlabLowthdChg(int)));


    QSignalMapper* siglabhigthdmap = new QSignalMapper(); //! 上阈值
    for(int i=0; i<MAX_LABELS; i++){
        siglabhigthdmap->setMapping(m_higthdle[i], i);
        connect(m_higthdle[i], SIGNAL(pressed()), siglabhigthdmap, SLOT(map()));
    }
    connect(siglabhigthdmap,SIGNAL(mapped(int)),this,SLOT(onlabHigthdChg(int)));

    QSignalMapper* siglabtarthdmap = new QSignalMapper(); //! 目标阈值
    for(int i=0; i<MAX_LABELS; i++){
        siglabtarthdmap->setMapping(m_tarthdle[i], i);
        connect(m_tarthdle[i], SIGNAL(pressed()), siglabtarthdmap, SLOT(map()));
    }
    connect(siglabtarthdmap,SIGNAL(mapped(int)),this,SLOT(onlabTarthdChg(int)));


    connect(m_maxSortNumle,SIGNAL(pressed()),this,SLOT(onmaxsortNumchg())); //! 最大调节组
    connect(m_minNumle,SIGNAL(pressed()),this,SLOT(onMinNumChg())); //! 最少调节阈值
    connect(m_maxTimespanle,SIGNAL(pressed()),this,SLOT(onMaxAdjspantimeChg()));
    connect(m_maxAdjtimele,SIGNAL(pressed()), this, SLOT(onMaxAdjtimesChg()));
    connect(m_renameBtn,   SIGNAL(pressed()),this,SLOT(onEnnamestatuschg()));
    connect(m_reSetPIDBtn,SIGNAL(pressed()),this,SLOT(onresetPIDparams()));
    connect(m_showadjpageBtn,SIGNAL(pressed()),this,SLOT(onshowadjPage()));//! 调节页面显示
    connect(m_maxAdjwaitle, SIGNAL(pressed()), this, SLOT(onMaxAutosamplewait()));
    connect(m_samplespanle, SIGNAL(pressed()), this, SLOT(onsamplespanChg()));
    connect(m_Loopspanle, SIGNAL(pressed()), this, SLOT(onLoopSpantimeChg()));
    connect(m_snvopencomb,SIGNAL(currentIndexChanged(int)), this, SLOT(onsvnopencombChg(int)));
    connect(m_autoSampleEnSw, SIGNAL(checkedChanged(bool)), this,SLOT(onAutosampleChg(bool)));
    connect(m_scheduleEnSw, SIGNAL(checkedChanged(bool)), this, SLOT(onScheduleEnChg(bool)));
    connect(m_scheTimele, SIGNAL(pressed()), this, SLOT(onScheduleTimeChg()));


    //! 形选参数信号槽连接
    QSignalMapper* sigshapechkmap = new QSignalMapper();
    for(int i=0; i<MAX_LABELS; i++){
        sigshapechkmap->setMapping(m_shapechk[i], i);
        connect(m_shapechk[i], SIGNAL(pressed()), sigshapechkmap, SLOT(map()));
    }
    connect(sigshapechkmap,SIGNAL(mapped(int)),this,SLOT(onshapechkstatuschg(int)));

    QSignalMapper* sigshapelowthdmap = new QSignalMapper(); //! 形选下阈值
    for(int i=0; i<MAX_LABELS; i++){
        sigshapelowthdmap->setMapping(m_shapeLowthdle[i], i);
        connect(m_shapeLowthdle[i], SIGNAL(pressed()), sigshapelowthdmap, SLOT(map()));
    }
    connect(sigshapelowthdmap,SIGNAL(mapped(int)),this,SLOT(onshapelowthdChg(int)));

    QSignalMapper* sigshapehigthdmap = new QSignalMapper(); //! 形选上阈值
    for(int i=0; i<MAX_LABELS; i++){
        sigshapehigthdmap->setMapping(m_shapeHigthdle[i], i);
        connect(m_shapeHigthdle[i], SIGNAL(pressed()), sigshapehigthdmap, SLOT(map()));
    }
    connect(sigshapehigthdmap,SIGNAL(mapped(int)),this,SLOT(onshapehigthdChg(int)));

}

/*!
 * \brief 刷新页面
 */
void sortercompage::refreshpage()
{
   m_nSortidx=0;
   m_nCamID = 0;

   m_StaicGpBtn->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nCamID]);

    //! 刷新title
   refreshtitleBtn(m_nSortidx);

   //! 刷新标签参数
   refreshlabparams(m_nSortidx);

   if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnshape){
        m_switchPageBtn->show();
   }else{
       m_switchPageBtn->hide();
   }
}

/*!
 * \brief 槽：切换分次
 * \param nidx
 */
void sortercompage::onsorterclassBtnPressed(int nidx)
{
    m_nSortidx = nidx;
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;

    if(m_stackedLayout->currentIndex() == 0 || m_stackedLayout->currentIndex() == 2){
        refreshtitleBtn(m_nSortidx);
        refreshlabparams(m_nSortidx);
        disconnect(m_sourcecomb,SIGNAL(currentIndexChanged(int)),this,SLOT(onsourceindexchg(int)));
        m_sourcecomb->setCurrentIndex(pProfile->nsourceIdx[m_nSortidx]);
        m_EnSortclass->setChecked(pProfile->bEnClass[m_nSortidx]);
        m_maxAdjtimele->setText(QString("%1").arg(pProfile->nAdjtimes[m_nSortidx]));
        m_maxAdjwaitle->setText(QString("%1").arg(pProfile->nAdjwaittime[m_nSortidx]));
        m_samplespanle->setText(QString("%1").arg(pProfile->nsamplespan[m_nSortidx]));
        m_EnAccChk->setChecked(pProfile->bEnACC[m_nSortidx]);
        m_minNumle->setText(QString("%1").arg(pProfile->nMinNumthd[m_nSortidx]));
        connect(m_sourcecomb,SIGNAL(currentIndexChanged(int)),this,SLOT(onsourceindexchg(int))); //! 化验源
    }else if(m_stackedLayout->currentIndex() == 1){
        refreshtitleBtn(m_nSortidx);
        refreshshapelabparams(m_nSortidx);
    }
   
}

/*!
 * \brief 槽：最大分次统计
 */
void sortercompage::onmaxsortNumchg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 8, m_maxSortNumle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_maxSortNumle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nmaxNum = n_sample;

        if(m_nSortidx > n_sample -1){
            m_nSortidx = 0;
        }

        refreshtitleBtn(m_nSortidx);
        refreshlabparams(m_nSortidx);
        refresnvopencomb();

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
            gFlow.sendSorterlabparams(false);
        }
    }
}

/*!
 * \brief 槽： 是否使能编辑名称
 */
void sortercompage::onEnnamestatuschg()
{
    myInputMethod input("", m_maxSortNumtileBtn[m_nSortidx]->text(), g_mainWidget);
    int ret = input.exec();
    if (ret == QDialog::Accepted) {
        QString tmp = input.getText().trimmed();
        QByteArray array = tmp.toLocal8Bit();
        if(array.size()>30){//! 字符过长无效
            infoWidget->raise();
            infoWidget->setLabelText(myLan.str_is_to_long);
            infoWidget->showSecs();
        }else{
            m_maxSortNumtileBtn[m_nSortidx]->setText(tmp);
            memset(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.sortclassname[m_nSortidx],0,30);
            memcpy(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.sortclassname[m_nSortidx],
                   array.data(),
                   array.size()
                   );
             if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
                gFlow.sendSorterlabparams(false);
             }
        }
    }
}

/*!
 * \brief 槽：状态使能
 * \param nIdx
 */
void sortercompage::onlabchkstatuschg(int nIdx)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    QString currentLabelName = m_labchk[nIdx]->text().trimmed();
    currentLabelName = currentLabelName.replace(QRegExp("^\\s+"), "");
    
    //! 获取标签合并映射表
    QMultiMap<QString, QString> mergeLabMap;
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vMegerlabmap.size() > m_nCamID) {
        mergeLabMap = pProfile->vMegerlabmap[m_nCamID];
    }else{
        return;
    }
    
    //! 2. 根据名称区分是父标签还是子标签
    bool isParent = mergeLabMap.contains(currentLabelName);
    bool isChecked = m_labchk[nIdx]->isChecked();
    
    if(isParent) { //! 父标签使能：子标签全部不使能，且禁用
        if(isChecked) {
            QList<QString> subLabels = mergeLabMap.values(currentLabelName);
            for(int i = 0; i < MAX_LABELS; i++) {
                if(m_labchk[i]->isVisible()) {
                    QString labelText = m_labchk[i]->text().trimmed().replace(QRegExp("^\\s+"), "");
                    if(subLabels.contains(labelText)) {
                        m_labchk[i]->setChecked(false);
                        m_labchk[i]->setEnabled(false);
                        m_lowthdle[i]->setEnabled(false);
                        m_higthdle[i]->setEnabled(false);
                        pProfile->strcSortclass.vlabParm[m_nSortidx][labelText].bEn = false;
                    }
                }
            }
        } else {//! 父标签不使能：子标签不禁用
            QList<QString> subLabels = mergeLabMap.values(currentLabelName);
            for(int i = 0; i < MAX_LABELS; i++) {
                if(m_labchk[i]->isVisible()) {
                    QString labelText = m_labchk[i]->text().trimmed().replace(QRegExp("^\\s+"), "");
                    if(subLabels.contains(labelText)) {
                        m_labchk[i]->setEnabled(true);
                        m_lowthdle[i]->setEnabled(true);
                        m_higthdle[i]->setEnabled(true);
                    }
                }
            }
        }
    } else {//! 如果是子标签
        QString parentLabel = "";
        for(auto it = mergeLabMap.begin(); it != mergeLabMap.end(); ++it) {
            if(it.value() == currentLabelName) {
                parentLabel = it.key();
                break;
            }
        }
        
        if(!parentLabel.isEmpty()) {
            int parentIdx = -1;
            for(int i = 0; i < MAX_LABELS; i++) {
                if(m_labchk[i]->isVisible()) {
                    QString labelText = m_labchk[i]->text().trimmed().replace(QRegExp("^\\s+"), "");
                    if(labelText == parentLabel) {
                        parentIdx = i;
                        break;
                    }
                }
            }
            
            if(parentIdx != -1) {
                if(isChecked) {//! 子标签使能：父标签不使能，且禁用
                    m_labchk[parentIdx]->setChecked(false);
                    m_labchk[parentIdx]->setEnabled(false);
                    m_lowthdle[parentIdx]->setEnabled(false);
                    m_higthdle[parentIdx]->setEnabled(false);
                    pProfile->strcSortclass.vlabParm[m_nSortidx][parentLabel].bEn = false;
                } else { //! 检查是否所有子标签都不使能
                    QList<QString> subLabels = mergeLabMap.values(parentLabel);
                    bool anySubLabelChecked = false;
                    for(int i = 0; i < MAX_LABELS; i++) {
                        if(m_labchk[i]->isVisible() && i != nIdx) {
                            QString labelText = m_labchk[i]->text().trimmed().replace(QRegExp("^\\s+"), "");
                            if(subLabels.contains(labelText) && m_labchk[i]->isChecked()) {
                                anySubLabelChecked = true;
                                break;
                            }
                        }
                    }
                    
                    //! 如果子标签都不使能：父标签不禁用
                    if(!anySubLabelChecked) {
                        m_labchk[parentIdx]->setEnabled(true);
                        m_lowthdle[parentIdx]->setEnabled(true);
                        m_higthdle[parentIdx]->setEnabled(true);
                    }
                }
            }
        }
    }
    
    //! 更新状态
    pProfile->strcSortclass.vlabParm[m_nSortidx][currentLabelName].bEn = m_labchk[nIdx]->isChecked();
    
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        gFlow.sendSorterlabparams(false);
    }
}

/*!
 * \brief 槽: 标签下阈值
 * \param nIdx
 */
void sortercompage::onlabLowthdChg(int nIdx)
{
    float nmax =100.0;
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    if(struCnfg.nMachineType == nMachine_Bottle
        && pProfile->nStaisticType ==0){
        nmax = 10000000.0;
    }

    myInputPanel *inputPanel = new myInputPanel(floatType, 0, nmax, m_lowthdle[nIdx]->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        QString value = QString::number(n_sample,'f',2);
     
        m_lowthdle[nIdx]->setText(QString("%1").arg(value.toFloat()));
        
        QString currentLabelName = m_labchk[nIdx]->text().trimmed();
        currentLabelName = currentLabelName.replace(QRegExp("^\\s+"), "");
        pProfile->strcSortclass.vlabParm[m_nSortidx][currentLabelName].nlowthd = value.toFloat();

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
            gFlow.sendSorterlabparams(false);
        }
    }
}

/*!
 * \brief 槽: 标签上阈值
 * \param nIdx
 */
void sortercompage::onlabHigthdChg(int nIdx)
{
    float nmax =100.0;
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    if(struCnfg.nMachineType == nMachine_Bottle
        && pProfile->nStaisticType ==0){
        nmax = 10000000.0;
    }

    myInputPanel *inputPanel = new myInputPanel(floatType, 0, nmax, m_higthdle[nIdx]->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        QString value = QString::number(n_sample,'f',2);

        m_higthdle[nIdx]->setText(QString("%1").arg(value.toFloat()));
        
        QString currentLabelName = m_labchk[nIdx]->text().trimmed();
        currentLabelName = currentLabelName.replace(QRegExp("^\\s+"), "");
        pProfile->strcSortclass.vlabParm[m_nSortidx][currentLabelName].nhigthd = value.toFloat();

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
            gFlow.sendSorterlabparams(false);
        }
    }
}

/*!
 * \brief 槽： 标签目前阈值
 * \param nIdx
 */
void sortercompage::onlabTarthdChg(int nIdx)
{
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 1, m_tarthdle[nIdx]->text().remove("%").toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        QString value = QString::number(n_sample,'g',2);
        m_tarthdle[nIdx]->setText(QString("%1").arg(value.toFloat()));
    }
}

/*!
 * \brief sortercompage::onresetPIDparams
 */
void sortercompage::onresetPIDparams()
{
    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_reset_coeff), "", g_mainWidget);
    int ret = msgBox.exec();
    if(ret == QDialog::Rejected)
        return;

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        gFlow.sendSorterlabparams(true);
    }
}

/*!
 * \brief 显示调节页面
 */
void sortercompage::onshowadjPage()
{
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        gFlow.sendSorterIPparams(true);
    }
}

/*!
 * \brief 更新localsocket状态
 * \param ncode
 */
void sortercompage::onupdateLocalsocketstatus(int ncode)
{
    switch (ncode) {
    case -2://! 连接服务器失败
        m_localsocketstaLb->setText(myLan.connect_server_sucess);
        break;
    case -1:
        m_localsocketstaLb->setText(myLan.connect_server_sucess);
        break;
    case 0:
        m_localsocketstaLb->setText(myLan.commutication_sucess);
        break;
    case 1:
        m_localsocketstaLb->setText(myLan.commutication_failed);
        break;
    case 2:
        m_localsocketstaLb->setText(myLan.aimodel_abnormal);
        break;
    case 3:
        m_localsocketstaLb->setText(myLan.sorter_jointdebug_failed);
        break;
    default:
        break;
    }
}

/*!
 * \brief 槽：化验来源改变
 * \param nidx
 */
void sortercompage::onsourceindexchg(int nidx)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nsourceIdx[m_nSortidx] = nidx;
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass){
        gFlow.sendSorterlabparams(false);
    }
}

/*!
 * \brief 槽：次调节使能
 * \param bsta
 */
void sortercompage::onEnSorterclssChg(bool bsta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.bEnClass[m_nSortidx] = bsta;
    gFlow.sendSorterlabparams(false);
}

void sortercompage::onEnAccChkChg(bool bsta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.bEnACC[m_nSortidx] = bsta;
}

/*!
 * \brief 槽：最少个数
 */
void sortercompage::onMinNumChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 900000000, m_minNumle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_minNumle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nMinNumthd[m_nSortidx] = n_sample;
    }
}

/*!
 * \brief 最大调节时间
 */
void sortercompage::onMaxAdjspantimeChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 10000, m_maxTimespanle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_maxTimespanle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nMaxSpantime = n_sample;
    }
}

/*!
 * \brief 单通道调节次
 */
void sortercompage::onMaxAdjtimesChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 100, m_maxAdjtimele->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_maxAdjtimele->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nAdjtimes[m_nSortidx] = n_sample;
    }
}

/*!
 * \brief sortercompage::onMaxAutosamplewait
 */
void sortercompage::onMaxAutosamplewait()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 100000, m_maxAdjwaitle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_maxAdjwaitle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nAdjwaittime[m_nSortidx] = n_sample;
    }
}

/*!
 * \brief 取样通道间隔
 */
void sortercompage::onsamplespanChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 100000, m_samplespanle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_samplespanle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nsamplespan[m_nSortidx] = n_sample;
    }
}

/*!
 * \brief 长吸口序号变更
 * \param nIdx
 */
void sortercompage::onsvnopencombChg(int nIdx)
{
    int nvalue = 0;
    if(nIdx !=0){
       nvalue = m_snvopencomb->currentText().toInt();
    }
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nChannel = nvalue;

}
/*!
 * \brief 轮取样间隔
 */
void sortercompage::onLoopSpantimeChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, 100000, m_Loopspanle->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_Loopspanle->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nLooptimes = n_sample;
    }
}

/*!
 * \brief 槽：相机视角变更
*/
void sortercompage::onStaicGpChg()
{
    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaticGroup <= 1){
        return;
    }

    m_nCamID += 1;
    if(m_nCamID >= gFlow.getMachineCamNum()){
        m_nCamID = 0;
    }

    m_StaicGpBtn->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nCamID]);

    refreshlabparams(m_nSortidx);
}

/*!
 * \brief 刷新分级按钮
 * \param nCurIdx
 */
void sortercompage::refreshtitleBtn(int nCurIdx)
{
    for(int i=0; i<MAX_SORTERCLASS_NUM; i++){
        if(i < struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nmaxNum){
            m_maxSortNumtileBtn[i]->show();
            m_maxSortNumtileBtn[i]->setText(QString::fromLocal8Bit(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.sortclassname[i]));
        }else{
            m_maxSortNumtileBtn[i]->hide();
        }

        QString strPressed = QString("color:white;background-color:#007454;");
        QString strUnPressed = QString("background-color:white;");

        if(i == nCurIdx){
            m_maxSortNumtileBtn[i]->setStyleSheet(strPressed);
        }else{
            m_maxSortNumtileBtn[i]->setStyleSheet(strUnPressed);
        }
    }
}

/*!
 * \brief 刷新标签参数
 * \param nCurIdx：取样口ID
 */
void sortercompage::refreshlabparams(int nCurIdx)
{
    //! 刷新title
    QString extrastr =QString("(%)");
    if(struCnfg.nMachineType == nMachine_Bottle
        && struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.nStaisticType ==0){
        extrastr = QString("(PPM)");
    }

    m_titlelb[1]->setText(QString("%1%2").arg(myLan.lower_limit).arg(extrastr));
    m_titlelb[2]->setText(QString("%1%2").arg(myLan.up_limit).arg(extrastr));


    //! 刷新标签
    int nMin = qMin(struGsh.vlabNamemap.size(),struGsh.vMegreLablst.size());
    if(nMin >= m_nCamID){
        auto *pstrSorterclass  = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;
        QStringList namelist = struGsh.vMegreLablst[m_nCamID];

        //! 获取标签合并映射表
        QMultiMap<QString, QString> mergeLabMap;
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vMegerlabmap.size() > m_nCamID) {
            mergeLabMap = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.vMegerlabmap[m_nCamID];
        }
        
        QStringList displayLabels; //! 实际要显示的标签列表
        QList<bool> isParentLabel; //! 标记: true父标签 false 子标签
        
        //! 按照vMegreLablst集合进行遍历
        for(const QString &label : namelist) {
            if(mergeLabMap.contains(label)) {//! 标记为父标签
                displayLabels.append(label);
                isParentLabel.append(true);
                QList<QString> subLabels = mergeLabMap.values(label);
                for(const QString &subLabel : subLabels) {//! 标记为子标签
                    displayLabels.append(subLabel);
                    isParentLabel.append(false);
                }
            } else {//! 无子标签的也按父标签处理
                displayLabels.append(label);
                isParentLabel.append(true);
            }
        }
        

        //! 填充数据布局排布
        int ntotal = displayLabels.size();
        for(int i=0; i<MAX_LABELS; i++){
            if(i < ntotal){
                m_labchk[i]->show();
                m_lowthdle[i]->show();
                m_higthdle[i]->show();
                m_tarthdle[i]->hide();

                QString strlab = displayLabels.at(i);

                if(pstrSorterclass->vlabParm.size() >nCurIdx &&
                    pstrSorterclass->vlabParm[nCurIdx].contains(strlab)){
                    m_lowthdle[i]->setText(QString("%1").arg(pstrSorterclass->vlabParm[nCurIdx][strlab].nlowthd));
                    m_higthdle[i]->setText(QString("%1").arg(pstrSorterclass->vlabParm[nCurIdx][strlab].nhigthd));
                    m_labchk[i]->setChecked(pstrSorterclass->vlabParm[nCurIdx][strlab].bEn);

                }else{
                    m_lowthdle[i]->setText(QString("%1").arg(0));
                    m_higthdle[i]->setText(QString("%1").arg(0));
                    m_labchk[i]->setChecked(false);
                }

                //! 根据是否为父标签设置不同的显示方式: 父标签：布局靠左，无缩进  子标签：布局靠右，添加缩进
                if(isParentLabel[i]) {
                    m_labchk[i]->setText(displayLabels.at(i));
                    m_glaylab->setAlignment(m_labchk[i],Qt::AlignLeft);
                } else {
                    m_labchk[i]->setText(QString(" %1").arg(displayLabels.at(i))); //! 添加4个空格缩进
                    m_glaylab->setAlignment(m_labchk[i],Qt::AlignHCenter);
                }   
            }else{
                m_labchk[i]->hide();
                m_lowthdle[i]->hide();
                m_higthdle[i]->hide();
                m_tarthdle[i]->hide();
            }
        }
        
        //! 处理父子标签使能状态互斥关系(分三类处理)
        if(pstrSorterclass->vlabParm.size()<=nCurIdx){
            for(int i=0; i<MAX_LABELS; i++){
                m_labchk[i]->setChecked(false);
            }
            return;
        }

        for(int i = 0; i < ntotal; i++) {
            if(m_labchk[i]->isVisible()) {
                QString currentLabelName = displayLabels.at(i);
                bool bContains = pstrSorterclass->vlabParm[nCurIdx].contains(currentLabelName);
                if(!bContains){
                    m_labchk[i]->setChecked(false);

                    if(mergeLabMap.contains(currentLabelName)){
                        QList<QString> subLabels = mergeLabMap.values(currentLabelName);
                        for(const QString &subLabel : subLabels) {
                            int subIdx = displayLabels.indexOf(subLabel);
                            if(subIdx != -1 && subIdx < ntotal && !isParentLabel[subIdx]) {
                                m_labchk[subIdx]->setChecked(false);
                                m_labchk[subIdx]->setEnabled(true);
                            }
                        }
                    }

                    continue;
                }

                if(isParentLabel[i] && mergeLabMap.contains(currentLabelName)) {//! 类型1：父标签（有子标签）
                    if(pstrSorterclass->vlabParm[nCurIdx][currentLabelName].bEn) {//! 父标签使能：其所有子标签都不使能且禁用
                        QList<QString> subLabels = mergeLabMap.values(currentLabelName);
                        for(const QString &subLabel : subLabels) {
                            int subIdx = displayLabels.indexOf(subLabel);
                            if(subIdx != -1 && subIdx < ntotal && !isParentLabel[subIdx]) {
                                m_labchk[subIdx]->setChecked(false);
                                m_labchk[subIdx]->setEnabled(false);
                                m_lowthdle[subIdx]->setEnabled(false);
                                m_higthdle[subIdx]->setEnabled(false);
                                if(pstrSorterclass->vlabParm[nCurIdx].contains(subLabel)){
                                    pstrSorterclass->vlabParm[nCurIdx][subLabel].bEn = false;
                                }
                            }
                        }
                    } else {//! 父标签不使能：子标签不禁用
                        QList<QString> subLabels = mergeLabMap.values(currentLabelName);
                        for(const QString &subLabel : subLabels) {
                            int subIdx = displayLabels.indexOf(subLabel);
                            if(subIdx != -1 && subIdx < ntotal && !isParentLabel[subIdx]) {
                                m_labchk[subIdx]->setEnabled(true);
                                m_lowthdle[subIdx]->setEnabled(true);
                                m_higthdle[subIdx]->setEnabled(true);
                            }
                        }
                    }
                }else if(!isParentLabel[i]) { //! 类型2：子标签
                    QString parentLabel = "";
                    for(auto it = mergeLabMap.begin(); it != mergeLabMap.end(); ++it) {
                        if(it.value() == currentLabelName) {
                            parentLabel = it.key();
                            break;
                        }
                    }
                    
                    if(!parentLabel.isEmpty()) {//! 子标签有父标签
                        int parentIdx = displayLabels.indexOf(parentLabel);
                        if(parentIdx != -1 && parentIdx < ntotal && isParentLabel[parentIdx]) {
                            //! 检查该父标签下是否有任何子标签使能
                            QList<QString> subLabels = mergeLabMap.values(parentLabel);
                            bool anySubLabelEnabled = false;
                            for(const QString &subLabel : subLabels) {
                                int subIdx = displayLabels.indexOf(subLabel);
                                bool bContain = pstrSorterclass->vlabParm[nCurIdx].contains(subLabel);
                                if(subIdx != -1 && subIdx < ntotal && !isParentLabel[subIdx] && 
                                   bContain && pstrSorterclass->vlabParm[nCurIdx][subLabel].bEn) {
                                    anySubLabelEnabled = true;
                                    break;
                                }
                            }
                            
                            if(anySubLabelEnabled) {//! 只要有子标签使能，其对应父标签不使能且禁用
                                m_labchk[parentIdx]->setChecked(false);
                                m_labchk[parentIdx]->setEnabled(false);
                                m_lowthdle[parentIdx]->setEnabled(false);
                                m_higthdle[parentIdx]->setEnabled(false);
                                if(pstrSorterclass->vlabParm[nCurIdx].contains(parentLabel)){
                                    pstrSorterclass->vlabParm[nCurIdx][parentLabel].bEn = false;
                                }
                            } else {//! 所有子标签都不使能：父标签恢复可操作状态
                                m_labchk[parentIdx]->setEnabled(true);
                                m_lowthdle[parentIdx]->setEnabled(true);
                                m_higthdle[parentIdx]->setEnabled(true);
                                if(pstrSorterclass->vlabParm[nCurIdx].contains(parentLabel)){
                                     m_labchk[parentIdx]->setChecked(pstrSorterclass->vlabParm[nCurIdx][parentLabel].bEn);
                                }else{
                                    m_labchk[parentIdx]->setChecked(false);
                                }
                            }
                        }
                    } else {
                        //! 类型2b：子标签无父标签（独立子标签）
                        //! 保持正常使能状态
                        m_labchk[i]->setEnabled(true);
                        m_lowthdle[i]->setEnabled(true);
                        m_higthdle[i]->setEnabled(true);
                    }
                    if(pstrSorterclass->vlabParm[nCurIdx].contains(currentLabelName)){
                        m_labchk[i]->setChecked(pstrSorterclass->vlabParm[nCurIdx][currentLabelName].bEn);
                    }else{
                        m_labchk[i]->setChecked(false);
                    }
                }else {//! 类型3：父标签但无子标签（独立父标签）
                    m_labchk[i]->setEnabled(true);
                    m_lowthdle[i]->setEnabled(true);
                    m_higthdle[i]->setEnabled(true);
                    m_labchk[i]->setEnabled(true);
                }
            }
        }
    }
}

void sortercompage::refresnvopencomb()
{
    m_snvopencomb->blockSignals(true);

    m_snvopencomb->clear();
    m_snvopencomb->insertItem(0,myLan.empty);

    int n = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nmaxNum;
    int num =1;
    for(int i = n; i<MAX_SNV_CHANNEL; i++){
        m_snvopencomb->insertItem(num, QString("%1").arg(i+1));
        num += 1;
    }

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nChannel<n){
        m_snvopencomb->setCurrentIndex(0);
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nChannel = 0;
    }else{
        int nidx = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nChannel - n;
        m_snvopencomb->setCurrentIndex(nidx);
    }

    m_snvopencomb->blockSignals(false);
}

/*!
 * \brief 槽：页面切换（标签参数和形选参数之间）
 */
void sortercompage::onSwitchPageBtn()
{
    int currentIndex = m_stackedLayout->currentIndex();
    if(currentIndex == 0) {
        m_stackedLayout->setCurrentIndex(1); //! 切换到形选参数页面
        m_switchPageBtn->setText(myLan.switch_to_label);
        refreshshapelabparams(m_nSortidx);
    } else {
        m_stackedLayout->setCurrentIndex(0); //! 切换到标签参数页面
        m_switchPageBtn->setText(myLan.switch_to_shape);
    }
}

/*!
 * \brief 槽：参数设置页面切换
 */
void sortercompage::onSwitchParamsBtn()
{
    int currentIndex = m_stackedLayout->currentIndex();
    if(currentIndex == 2) {
        m_stackedLayout->setCurrentIndex(0);
        m_switchParamsBtn->setText(myLan.params);

        m_switchPageBtn->setDisabled(false);
        m_renameBtn->setDisabled(false);
        m_showadjpageBtn->setDisabled(false);
    } else {
        m_switchPageBtn->setDisabled(true);
        m_renameBtn->setDisabled(true);
        m_showadjpageBtn->setDisabled(true);

        refreshSampleChannelinfo();
        m_stackedLayout->setCurrentIndex(2);
        m_switchParamsBtn->setText(myLan.back);
    }
}

/*!
 * \brief 槽：形选参数使能状态改变
 * \param nIdx
 */
void sortercompage::onshapechkstatuschg(int nIdx)
{
    //! 获取标签合并映射表
    bool isChecked = m_shapechk[nIdx]->isChecked();
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;


    QString labname = m_shapechk[nIdx]->text();
    pProfile->vlabParm[m_nSortidx][labname].bshapeEn = isChecked;

    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass) {
        gFlow.sendSorterlabparams(false);
    }
}

/*!
 * \brief 槽：形选参数下阈值改变
 * \param nIdx
 */
void sortercompage::onshapelowthdChg(int nIdx)
{
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 100.0, m_shapeLowthdle[nIdx]->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        QString value = QString::number(n_sample,'f',2);
        m_shapeLowthdle[nIdx]->setText(QString("%1").arg(value.toFloat()));

        QString labname = m_shapechk[nIdx]->text();
        auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;

        pProfile->vlabParm[m_nSortidx][labname].nshapeLowthd = value.toFloat();

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass) {
            gFlow.sendSorterlabparams(false);
        }
    }
}

/*!
 * \brief 槽：形选参数上阈值改变
 * \param nIdx
 */
void sortercompage::onshapehigthdChg(int nIdx)
{
    myInputPanel *inputPanel = new myInputPanel(floatType, 0, 100.0, m_shapeHigthdle[nIdx]->text().toFloat(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        float n_sample = inputPanel->getValue();
        QString value = QString::number(n_sample,'f',2);
        m_shapeHigthdle[nIdx]->setText(QString("%1").arg(value.toFloat()));

        QString labname = m_shapechk[nIdx]->text();
        auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;

        pProfile->vlabParm[m_nSortidx][labname].nshapeHigthd = value.toFloat();

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnSorterClass) {
            gFlow.sendSorterlabparams(false);
        }
    }
}

/*!
 * \brief 槽：自动取样使能
 * \param bsta
 */
void sortercompage::onAutosampleChg(bool bsta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample = bsta;
}

/*!
 * \brief 槽：定时取料使能
 * \param bSta
 */
void sortercompage::onScheduleEnChg(bool bSta)
{
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.bScheduled = bSta;
}

/*!
 * \brief 槽：定时取料时间
 * \param bSta
 */
void sortercompage::onScheduleTimeChg()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 20, m_scheTimele->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_scheTimele->setText(QString("%1").arg(n_sample));
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass.nScheduletimes = n_sample;

    }
}

/*!
 * \brief 刷新形选参数
 * \param nCurIdx：取样口
 */
void sortercompage::refreshshapelabparams(int nCurIdx)
{
    auto *pstrSorterclass  = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;

    for(int n=0; n<MAX_LABELS; n++){
        if(n <struGsh.vShapeLab[nCurIdx].size()){
            QString name = struGsh.vShapeLab[nCurIdx].at(n);
            m_shapechk[n]->setText(name);

            if(pstrSorterclass->vlabParm[nCurIdx].contains(name)){
                m_shapeLowthdle[n]->setText(QString("%1").arg(pstrSorterclass->vlabParm[nCurIdx][name].nshapeLowthd));
                m_shapeHigthdle[n]->setText(QString("%1").arg(pstrSorterclass->vlabParm[nCurIdx][name].nshapeHigthd));
                m_shapechk[n]->setChecked(pstrSorterclass->vlabParm[nCurIdx][name].bshapeEn);
            }else{
                m_shapeLowthdle[n]->setText(QString("0"));
                m_shapeHigthdle[n]->setText(QString("0"));
                m_shapechk[n]->setChecked(false);
            }

            m_shapechk[n]->show();
            m_shapeLowthdle[n]->show();
            m_shapeHigthdle[n]->show();
        }else{
            m_shapechk[n]->hide();
            m_shapeLowthdle[n]->hide();
            m_shapeHigthdle[n]->hide();
        }
    }
}

/*!
 * \brief 刷新取样口相关参数
 */
void sortercompage::refreshSampleChannelinfo()
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;

    //! 刷新化验源
    m_sourcecomb->setCurrentIndex(pProfile->nsourceIdx[m_nSortidx]);

    //! 刷新次使能
    m_EnSortclass->setChecked(pProfile->bEnClass[m_nSortidx]);

    m_EnAccChk->setChecked(pProfile->bEnACC[m_nSortidx]);

    m_StaicGpBtn->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_nCamID]);

    //! 刷新分级次数
    m_maxSortNumle->setText(QString("%1").arg(pProfile->nmaxNum));

    //! 刷新最少个数
    m_minNumle->setText(QString("%1").arg(pProfile->nMinNumthd[m_nSortidx]));

    //! 最大调节时间
    m_maxTimespanle->setText(QString("%1").arg(pProfile->nMaxSpantime));

    //! 单通道调节次数
    m_maxAdjtimele->setText(QString("%1").arg(pProfile->nAdjtimes[m_nSortidx]));

    //! 取样等待
    m_maxAdjwaitle->setText(QString("%1").arg(pProfile->nAdjwaittime[m_nSortidx]));

    //! 取样间隔
    m_samplespanle->setText(QString("%1").arg(pProfile->nsamplespan[m_nSortidx]));

    //! 轮取样间隔
    m_Loopspanle->setText(QString("%1").arg(pProfile->nLooptimes));

    m_autoSampleEnSw->setChecked(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample);

    refresnvopencomb();

    m_scheduleEnSw->setChecked(pProfile->bScheduled);
    m_scheTimele->setText(QString("%1").arg(pProfile->nScheduletimes));

}
