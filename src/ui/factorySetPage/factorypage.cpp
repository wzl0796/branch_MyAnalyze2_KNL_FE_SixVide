#include "factorypage.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"


factoryPage::factoryPage(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief factoryPage::初始化参数
 */
void factoryPage::initParams()
{
    lan_signal_set = myLan.signal_debug;
    lan_image_get  = myLan.capture;
    lan_simulate = myLan.label_params; //! 仿真
    lan_author_card = myLan.engineer;
    lan_lang_set    = myLan.language_set;
    lan_machine_set = myLan.machine_params;
    lan_pepher = myLan.periph_device;
    lan_calibrate = myLan.calibrate;
    lab_sortercom = myLan.sorter_debug;
}

/**
 * @brief factoryPage::界面搭建
 */
void factoryPage::createPage()
{
    QStringList materialList, iconList;
    materialList <<lan_signal_set<<lan_machine_set
                 <<lan_pepher<<lan_lang_set
                 <<lan_author_card<<lan_calibrate
                 <<myLan.tick;

    iconList << ":/res/png/Chart_Line.png"
             << ":/res/png/Sorter.png"
             << ":/res/png/Device_Chip.png"
             << ":/res/png/tab_language_normal.png"
             << ":/res/png/User_Engineer.png"
             << ":/res/png/model_parse.png"
             << ":/res/png/Sorter_Ejector.png";



    listSetWig = new QListWidget();
    listSetWig->setMovement(QListView::Static);
    listSetWig->setViewMode(QListView::IconMode);
    listSetWig->setFixedSize(BTN_WID+15, WIN_HEI-180);
    listSetWig->setIconSize(QSize(BTN_HEI, BTN_HEI-15));
    listSetWig->setStyleSheet(QString("QListWidget{border:1px solid lightgray; "
                                     "border-radius:10px;outline:0px;"
                                     "padding:30px,0px,0px,0px;"
                                     "background-color:white;"
                                     "font:25px;}"
                                     "QListWidget::Item:selected{"
                                     "border-right:5px solid #43CD80}"));



    for (int i = 0; i < PAGE_ALL; i++) {
        QListWidgetItem *listItem = new QListWidgetItem(materialList.at(i));
        listItem->setSizeHint(QSize(BTN_WID+13,90));
        listItem->setTextAlignment(Qt::AlignCenter);
        listItem->setIcon(QIcon(iconList.at(i)));
        listSetWig->addItem(listItem);
    }

    listSetWig->setCurrentRow(0);


    m_stack = new QStackedWidget();
    m_stack->setFixedSize(WIN_WID-315, WIN_HEI-180);
    m_stack->setStyleSheet("background-color:white; border-radius:10px;");

    //! 信号
    pageCameraSignal = new cameraSignalPage();
    m_stack->addWidget(pageCameraSignal);

    //! 机型
    pageMachinewid = new pageMachine();
    m_stack->addWidget(pageMachinewid);

    //! 外设页面
    perpherwid = new perpheralPage();
    m_stack->addWidget(perpherwid);

    //! 设置语言界面
    pageSetLang = new setLangWidget;
    m_stack->addWidget(pageSetLang);

    //! 工程师
    pageEngineer = new engineerSetPage();
    m_stack->addWidget(pageEngineer);

    //! 标定页面
    pageCalib = new  pagecalibrate();
    m_stack->addWidget(pageCalib);

//    pageGetImage = new getImagePage();
//    m_stack->addWidget(pageGetImage);

    pagetickSet = new ticksetting();
    m_stack->addWidget(pagetickSet);

    m_stack->setCurrentIndex(0);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(listSetWig);
    mainLayout->addWidget(m_stack);
}

/**
 * @brief factoryPage::信号槽绑定
 */
void factoryPage::connectSigAndSlot()
{
    connect(listSetWig,     SIGNAL(currentRowChanged(int)), this,       SLOT(onListSetWidRowChangedSlt()));
    connect(pageEngineer,   SIGNAL(updateLangList()),       pageSetLang,SLOT(onUpdateLangList()));
    connect(this,           SIGNAL(sigChangeScheme()),      pageCameraSignal, SLOT(onChangeScheme()));
    connect(this,           SIGNAL(sigIM01SenTempture(int)),pageCameraSignal, SLOT(onIM01SensorTempure(int)));
    connect(perpherwid,    SIGNAL(sigrefreshBeltStatus()),  this ,SIGNAL(sigrefreshBeltSta()));
    connect(perpherwid,    &perpheralPage::sigEjectorEnChg,  this , &factoryPage::refreshDisplay);

    connect(pageMachinewid, SIGNAL(sigInitWeb(bool)), this, SIGNAL(sigInitwebSocket(bool)));
    connect(pageMachinewid, SIGNAL(sigInitRemoteSorter()), this, SIGNAL(sigInitRemoteSorter()));
    connect(pageMachinewid, SIGNAL(sigautosampleEnsta(int,bool)), this, SIGNAL(sigautosampleEnstatus(int,bool)));
    connect(pageMachinewid, SIGNAL(siguploadparmas(QString,int)), this, SIGNAL(sigUploadParamsbyCloud(QString,int)));
    connect(pageMachinewid, SIGNAL(sigMachineTypechg(bool)), this, SLOT(onProcessMachineChgSlt(bool)));
    connect(pageMachinewid, &pageMachine::sigApplyMachine,pageCameraSignal,&cameraSignalPage::onApplyMachineSlt);
    connect(pageMachinewid, SIGNAL(sigSimulateEn(bool)), this, SIGNAL(sigSimulateEnChg(bool)));
}

/**
 * @brief factoryPage::刷新界面显示
 */
void factoryPage::refreshDisplay()
{
    onListSetWidRowChangedSlt();
    onProcessMachineChgSlt(struCnfg.nMachineType == nMachine_Bottle ? true : false);
}

/*!
 * \brief 获取当前页面下标
 */
int factoryPage::getpageIdx()
{
    return m_stack->currentIndex();
}

/**
 * @brief factoryPage::列表切换
 */
void factoryPage::onListSetWidRowChangedSlt()
{
    int index = listSetWig->currentRow();
    if (index != PAGE_SIGNAL_SET) {//! 切校正波形
        for (int i = 0; i < struCnfg.analyzeNum; i++) {
            for(int j = 0; j < gFlow.getMachineCamNum(); j++)
                gFlow.sendCamSignalMode(i, j, 1);
        }
    }

    g_curveDataThread->stopWave();

    if(struCnfg.nMachineType ==nMachine_Bottle
            && index==PAGE_CALIBRATE){
        return;
    }

    m_stack->setCurrentIndex(index);
//    lisetWigIconChanged(index);
    emit sigEnfeeder((index ==PAGE_CALIBRATE || index == PAGE_TICK_SET) ? true : false);

    switch (index) {
    case PAGE_SIGNAL_SET:
        pageCameraSignal->refreshPageDisplay();
        break;
//    case PAGE_GET_IMAGE:
//        pageGetImage->refreshPageDisplay();
//        break;
    case PAGE_ENGINEER_SET:
        pageEngineer->refreshDisplay();
        break;
    case PAGE_LANG_SET:
        break;
    case PAGE_MACHINE_SET:
        pageMachinewid->refreshPagemachine();
        break;
    case PAGE_PERPHER:
        perpherwid->refreshPage();
        break;
    case PAGE_CALIBRATE:
        pageCalib->refreshpageCalibrate();
        break;
    case PAGE_TICK_SET:
        pagetickSet->refreshPage();
        break;
    default:
        break;
    }
}

/*!
 * \brief 槽: 处理机型更改
 */
void factoryPage::onProcessMachineChgSlt(bool bBottle)
{
    if(bBottle){
        listSetWig->item(PAGE_CALIBRATE)->setHidden(true);
        listSetWig->item(PAGE_TICK_SET)->setHidden(true);
    }else{
        listSetWig->item(PAGE_CALIBRATE)->setHidden(false);
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal){
            listSetWig->item(PAGE_TICK_SET)->setHidden(false);
        }else{
            listSetWig->item(PAGE_TICK_SET)->setHidden(true);
        }
    }

    perpherwid->refreshPage();

    emit sigMachineTypeChg(bBottle);
}

/**
 * @brief factoryPage::lisetWigIconChanged
 * @param index
 */
void factoryPage::lisetWigIconChanged(int index)
{
    QStringList unPressedIcon, pressedIcon;
    unPressedIcon << ":/res/png/tab_adjust_normal.png"
                  << ":/res/png/tab_valve_normal.png"
                  << ":/res/png/tab_adjust_normal.png"
                  << ":/res/png/author_card.png"
                  << ":/res/png/tab_language_normal.png"
                  << ":/res/png/Sorter.png"
                  << ":/res/png/tab_setting_normal.png"
                  << ":/res/png/Sorter.png"
                  << ":/res/png/model_parse.png"
                  << ":/res/png/Sorter_Ejector.png";



    pressedIcon << ":/res/png/tab_adjust_pressed.png"
                << ":/res/png/tab_valve_pressed.png"
                << ":/res/png/tab_adjust_pressed.png"
                << ":/res/png/author_card_pressed.png"
                << ":/res/png/tab_language_pressed.png"
                << ":/res/png/Sorter.png"
                << ":/res/png/tab_setting_normal.png"
                << ":/res/png/Sorter.png"
                << ":/res/png/model_parse.png"
                << ":/res/png/Sorter_Ejector.png";

    for (int i = 0; i < PAGE_ALL; i++) {
        listSetWig->item(i)->setIcon(QIcon(unPressedIcon.at(i)));
    }
    listSetWig->item(index)->setIcon(QIcon(pressedIcon.at(index)));
}
