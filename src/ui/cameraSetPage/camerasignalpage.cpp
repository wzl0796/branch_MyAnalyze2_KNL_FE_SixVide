#include "camerasignalpage.h"
#include "common/mycurve.h"
#include "common/myinputmethod.h"
#include "core/io/udp/myudp.h"

#define BTN_WIDTH 160
#define BTN_HEIGHT 50

#define ICON_WIDTH 32
#define ICON_HEIGHT 32

#define LCD_WIDTH 1920
#define LCD_HEIGHT 1080

WaveSelectWgt::WaveSelectWgt(QWidget *parent) : QDialog(parent) {
  // setWindowModality(Qt::ApplicationModal);

  setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
  setFixedSize(BTN_WIDTH * 3 + ICON_WIDTH * 2, LCD_HEIGHT / 2 + BTN_HEIGHT);
  this->move(QPoint((LCD_WIDTH - (BTN_WIDTH * 3 + ICON_WIDTH * 2)) / 2,
                    LCD_HEIGHT / 4 - BTN_HEIGHT / 2));

  this->setModal(true);
  this->setStyleSheet(QString(
      "{background-color:rgb(128,128,128);border-radius:10px;outline:0px;}"));
  initUI();
  initLayout();
  initSigSlots();
}

void WaveSelectWgt::setSelectedRows(QVector<int> &rows) {
  m_waveCnt = rows.size();
  for (int i = 0; i < rows.size(); i++) {
    m_leftListWgt->setItemHidden(m_leftListWgt->item(rows.at(i) - 1), true);
    m_rightListWgt->setItemHidden(m_rightListWgt->item(rows.at(i) - 1), false);
  }
  if (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->struCaParams->nWaveSeg[struGsh.nUnit] == 0) { //! 6波段
    m_toRightBtn->setEnabled((rows.size() < 6) ? true : false);
    m_okBtn->setEnabled((rows.size() == 6) ? true : false);
  } else {
    m_toRightBtn->setEnabled((rows.size() < 12) ? true : false);
    m_okBtn->setEnabled((rows.size() == 12) ? true : false);
  }
}

QVector<int> WaveSelectWgt::getSelectedRows() {
  QVector<int> rows;

  for (int i = 0; i < 512; i++) {
    if (!m_rightListWgt->item(i)->isHidden()) {
      rows.append(i + 1);
    }
  }

  return rows;
}

void WaveSelectWgt::setIMSensorType(int nType) {
  n_nSensorType = nType;
  if (nType == SENSOR_IM01) {
    for (int i = 256; i < 512; i++) {
      m_leftListWgt->item(i)->setHidden(true);
    }
  }
}

void WaveSelectWgt::onToRightBtnClickedSlt() {
  int row = m_leftListWgt->currentRow();
  m_leftListWgt->setItemHidden(m_leftListWgt->item(row), true);
  m_rightListWgt->setItemHidden(m_rightListWgt->item(row), false);
  QVector<int> rows = getSelectedRows();
  m_toRightBtn->setEnabled((rows.size() < m_waveCnt) ? true : false);
  m_okBtn->setEnabled((rows.size() == m_waveCnt) ? true : false);
}

void WaveSelectWgt::onToLeftBtnClickedSlt() {
  int row = m_rightListWgt->currentRow();
  m_rightListWgt->setItemHidden(m_rightListWgt->item(row), true);
  m_leftListWgt->setItemHidden(m_leftListWgt->item(row), false);
  QVector<int> rows = getSelectedRows();
  m_toRightBtn->setEnabled((rows.size() < m_waveCnt) ? true : false);
  m_okBtn->setEnabled((rows.size() == m_waveCnt) ? true : false);
}

void WaveSelectWgt::onOkBtnClickedSlt() { accept(); }

void WaveSelectWgt::onCancleBtnClickedSlt() { reject(); }

void WaveSelectWgt::initUI() {
  m_mainLay = new QVBoxLayout(this);
  m_tipLabl = new QLabel("提示：提取的波段必须在4个开窗区间");
  m_tipLabl->hide();
  m_leftListWgt = new QListWidget;
  m_rightListWgt = new QListWidget;
  m_toRightBtn = new QPushButton("=>");
  m_toLeftBtn = new QPushButton("<=");
  m_okBtn = new QPushButton(myLan.ok);
  m_cancleBtn = new QPushButton(myLan.cancel);

  m_tipLabl->setFixedHeight(50);
  m_toRightBtn->setFixedSize(160 / 2, 50);
  m_toLeftBtn->setFixedSize(160 / 2, 50);
  m_okBtn->setFixedSize(160 / 2, 50);
  m_cancleBtn->setFixedSize(160 / 2, 50);

  m_leftListWgt->setIconSize(QSize(32, 32));
  m_leftListWgt->setMovement(QListView::Static);
  m_leftListWgt->setFixedWidth(160 + 32);

  m_rightListWgt->setIconSize(QSize(32, 32));
  m_rightListWgt->setMovement(QListView::Static);
  m_rightListWgt->setFixedWidth(160 + 32);

  for (int i = 0; i < 512; i++) {
    QListWidgetItem *item =
        new QListWidgetItem(QIcon(), QString("%1").arg(i + 1));
    m_leftListWgt->addItem(item);

    QListWidgetItem *item2 =
        new QListWidgetItem(QIcon(), QString("%1").arg(i + 1));
    m_rightListWgt->addItem(item2);
    m_rightListWgt->setItemHidden(item2, true);
  }
}

void WaveSelectWgt::initLayout() {
  QHBoxLayout *topLay = new QHBoxLayout;
  QVBoxLayout *midLay = new QVBoxLayout;
  QHBoxLayout *btmLay = new QHBoxLayout;
  midLay->addWidget(m_toRightBtn);
  midLay->addWidget(m_toLeftBtn);

  topLay->addWidget(m_leftListWgt);
  topLay->addLayout(midLay);
  topLay->addWidget(m_rightListWgt);

  btmLay->addWidget(m_okBtn, 0, Qt::AlignLeft);
  btmLay->addWidget(m_cancleBtn, 0, Qt::AlignRight);

  m_mainLay->addWidget(m_tipLabl);
  m_mainLay->addLayout(topLay);
  m_mainLay->addLayout(btmLay);
}

void WaveSelectWgt::initSigSlots() {
  connect(m_toRightBtn, SIGNAL(clicked(bool)), this,
          SLOT(onToRightBtnClickedSlt()));
  connect(m_toLeftBtn, SIGNAL(clicked(bool)), this,
          SLOT(onToLeftBtnClickedSlt()));
  connect(m_okBtn, SIGNAL(clicked(bool)), this, SLOT(onOkBtnClickedSlt()));
  connect(m_cancleBtn, SIGNAL(clicked(bool)), this,
          SLOT(onCancleBtnClickedSlt()));
}

/*!
 * \brief 信号调试构造函数
 * \param parent
 */
cameraSignalPage::cameraSignalPage(QWidget *parent) : QWidget(parent) {
  initParams();

  createPage();

  connectSigAndSlot();
}

/**
 * @brief cameraSignalPage::初始化参数
 */
void cameraSignalPage::initParams() {
  ori_wave = myLan.ori_wave;        // 原始波形;
  adjust_wave = myLan.adj_wave;     // 校正波形;
  adjust_signal = myLan.adj_signal; // 信号捕捉;
  adjust_target = myLan.adj_target; //"校正目标";
  auto_adjust = myLan.light_adj;    // "自动校正";
  dark_adjust = myLan.dark_adj;     // "暗校正";
  fre_adjust = myLan.frequence_adj; // "行频调节";
  fre_value = QString("%1(x1Hz)").arg(myLan.frequence);
  gain_adjust = myLan.gain_adj;                //"增益调节";
  fast_adjust = myLan.fast_adjust;             // "粗调";
  slow_adjust = myLan.fine_adjust;             // "细调";
  lamp_adjust = myLan.lamp_adjust;             //"灯光调节";
  mat_lamp = myLan.mat_lamp;                   //"物料灯";
  back_lamp = myLan.back_lamp;                 //"背景灯";
  inf_lamp = myLan.inf_lamp;                   // "红外灯";
  back_params = myLan.background_params;       //"背景参数";
  range_parmas = myLan.variation_range;        //"波动范围";
  thread_params = myLan.threshold_params;      //"触发下限";
  adjust_warn = myLan.confirm_calibrate;       // "确定校正当前相机?";
  pixel_channel = myLan.pixel_dived;           //"像元划分";
  begin_pixel = myLan.begin_pixel;             //"起始像元";
  end_pixel = myLan.end_pixel;                 //"终止像元";
  auto_pixel = myLan.auto_dived;               //"自动划分";
  resolution = myLan.resolution;               //"分辨率";
  sensor_type = myLan.type;                    //"类型";
  expose_value = myLan.exposure_value;         //"曝光值";
  Noneexpose_value = myLan.Non_exposure_value; //"非曝光值";
  m_strExportWaves = myLan.spectral_band;      //"波段";
  infra_bias = myLan.infra_bias;               //"红外偏置";

  initCameraParmas();
}

/**
 * @brief cameraSignalPage::初始化相机调试参数
 */
void cameraSignalPage::initCameraParmas() {
  n_wave_type = WAVE_ORI;
  n_view = FRONT_VIEW;
  nCalibrateID = PIXEL_2048;
  //    n_analyzeNum = struCnfg.analyzeNum;

  m_bManualChange = true;

  waveTypeMap = new QSignalMapper();
  adjustTargetMap = new QSignalMapper();
  fastAdjustMap = new QSignalMapper();
  slowAdjustMap = new QSignalMapper();
  frequencyMap = new QSignalMapper();
  sensorMap = new QSignalMapper();
  exposeMap = new QSignalMapper();
  exportWavesMapper = new QSignalMapper();
  NoneexposeMap = new QSignalMapper();
  tempreqMapper = new QSignalMapper();
  sensorWavemap = new QSignalMapper();

  lampLblAdjustMap = new QSignalMapper();
  pluseLblAdjustMap = new QSignalMapper();

  lampValueAdjustMap = new QSignalMapper();
  lampEnmap = new QSignalMapper();
  pluseValueAdjustMap = new QSignalMapper();
  pluseEnmap = new QSignalMapper();
  pixelValueAdjustMap = new QSignalMapper();
  ejectorDiveMap = new QSignalMapper();
}

/**
 * @brief cameraSignalPage::刷新显示
 */
void cameraSignalPage::refreshPageDisplay() {
  onNodeBoxChangedSlt(struGsh.nAnalyzeIndex);

  for (int i = 0; i < MAX_CAM_NUM; i++) {
    if (i < gFlow.getMachineCamNum()) {
      m_camBtn[i]->show();
      m_camBtn[i]->setText(
          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
    } else {
      m_camBtn[i]->hide();
    }
  }

  if (n_wave_type == WAVE_SIGNAL) {
    n_wave_type = WAVE_ORI;
  }

  if (!g_curveDataThread->getRunningState()) {
    g_curveDataThread->startWave();
  }

  //! 刷新相机参数页面相关参数
  refreshCamParam();

  //! 刷新灯光值页面相关参数
  refreshLightParams();

  //! 刷新传感器页面相关参数
  refreshFreValue();
}

/**
 * @brief cameraSignalPage::界面构建
 */
void cameraSignalPage::createPage() {
  m_pagecamSet = new QWidget();
  m_pageGetImage = new getImagePage();

  createCurvePage();
  createParamSetPage();

  QHBoxLayout *mainLayout = new QHBoxLayout(m_pagecamSet);
  mainLayout->setMargin(0);
  mainLayout->addWidget(curveGBox);
  mainLayout->addWidget(paramsGBox);

  m_stacklaytopPage = new QStackedLayout(this);
  m_stacklaytopPage->addWidget(m_pagecamSet);
  m_stacklaytopPage->addWidget(m_pageGetImage);

  m_stacklaytopPage->setCurrentIndex(0);
}

/*!
 * \brief 波形页面
 */
void cameraSignalPage::createCurvePage() {
  QStringList waveList;
  waveList << ori_wave << adjust_wave << adjust_signal;

  curveGBox = new QGroupBox();
  curveGBox->setFixedSize(WIN_WID / 2 + 140, WIN_HEI - 200);
  curveGBox->setStyleSheet(
      "QWidget{background-color:white; border-radius:10px;}");

  QHBoxLayout *viewHLayout = new QHBoxLayout();
  m_camBtnMap = new QSignalMapper(this); //! 相机tab
  for (int i = 0; i < MAX_CAM_NUM; i++) {
    m_camBtn[i] = new QPushButton(
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
    m_camBtn[i]->setFixedSize(BTN_WID, BTN_HEI);

    m_camBtnMap->setMapping(m_camBtn[i], i);
    connect(m_camBtn[i], SIGNAL(clicked(bool)), m_camBtnMap, SLOT(map()));

    viewHLayout->addWidget(m_camBtn[i]);
  }
  viewHLayout->addStretch();

  myAxis = new MyAxis(0, curveGBox); //! 波形图
  myAxis->setFixedSize(GROUP_WID * 3 - 140, GROUP_HEI * 2.3);
  myAxis->update();

  QHBoxLayout *waveLayout = new QHBoxLayout(); //! 原始信号 or 校正信号
  for (int i = 0; i < WAVE_TYPE; i++) {
    waveBtn[i] =
        new QPushButton(); //! 波形页面中 原始信号  校正信号  校正波形 按钮
    waveBtn[i]->setText(waveList.at(i));
    waveBtn[i]->setFixedSize(BTN_WID + 10, BTN_HEI - 10);
    waveLayout->addWidget(waveBtn[i]);

    waveTypeMap->setMapping(waveBtn[i], i);
    connect(waveBtn[i], SIGNAL(pressed()), waveTypeMap, SLOT(map()));
  }

  QVBoxLayout *curveLayout = new QVBoxLayout(curveGBox);
  curveLayout->addLayout(viewHLayout);
  curveLayout->addWidget(myAxis, 0, Qt::AlignCenter);
  curveLayout->addLayout(waveLayout);
}

/*!
 * \brief 参数页面
 */
void cameraSignalPage::createParamSetPage() {
  createCamSubPage();
  createLightSubPage();
  createSensorSubPage();

  m_tabWidget = new QTabWidget;
  m_tabWidget->setStyleSheet(
      "QTabWidget::pane { border-top:1px solid #C2C7CB;}"
      "QTabBar::tab{background-color:white;color:black;border:2px solid "
      "#dfdfdf;"
      "min-width: 110px;"
      "border-top-left-radius:10px;border-top-right-radius:10px;font: 22px;"
      "padding-left:10px;padding-top:16px;padding-right:10px;padding-bottom:"
      "16px;}"
      "QTabBar::tab::selected{color:white;background-color: #007454;}");
  m_tabWidget->addTab(m_camSubPage, QString("%1").arg(myLan.camer));
  m_tabWidget->addTab(m_lightSubPage, QString("%1").arg(myLan.lighting));
  m_tabWidget->addTab(m_sensorSubPage, myLan.sensor);
  m_tabWidget->setCurrentIndex(0);

  paramsGBox = new QGroupBox();
  paramsGBox->setFixedSize(480, WIN_HEI - 200);
  paramsGBox->setStyleSheet("font-size:22px;");

  QVBoxLayout *paramsLayout = new QVBoxLayout(paramsGBox);
  paramsLayout->addWidget(m_tabWidget);
}

/**
 * @brief cameraSignalPage::相机参数
 */
void cameraSignalPage::createCamSubPage() {
  QStringList gainList, colorList, colorNameList, pixelList, ejectorList;
  gainList << gain_adjust << fast_adjust << slow_adjust;
  colorList << ":/res/png/Mark_Red.png" << ":/res/png/Mark_Green.png"
            << ":/res/png/Mark_Blue.png" << ":/res/png/Mark_N.png";
  colorNameList << "R" << "G" << "B" << "N";
  pixelList << begin_pixel << end_pixel;
  ejectorList << myLan.start_ejc << myLan.end_ejc;

  //! 校正目标
  adjustTargetLbl = new QLabel();
  adjustTargetLbl->setFixedSize(BTN_WID, BTN_HEI - 30);
  adjustTargetLbl->setText(adjust_target);
  adjustTargetLbl->setStyleSheet("font:bold;");

  QGridLayout *adjustLayout = new QGridLayout;
  adjustLayout->setVerticalSpacing(5);
  adjustLayout->addWidget(adjustTargetLbl, 0, 0);

  for (int i = 0; i < COLOR_TYPE_NUM; i++) { //! 校正值
    adjustTargetTypeLbl[i] = new QLabel();
    adjustTargetTypeLbl[i]->setAlignment(Qt::AlignCenter);
    adjustTargetTypeLbl[i]->setText(colorNameList.at(i));
    adjustTargetTypeLbl[i]->setFixedSize(BTN_WID, BTN_HEI - 25);

    adjustTargetTypetBtn[i] = new QPushButton();
    adjustTargetTypetBtn[i]->setFixedSize(BTN_WID, BTN_HEI - 25);

    adjustLayout->addWidget(adjustTargetTypeLbl[i], i + 1, 0, Qt::AlignRight);
    adjustLayout->addWidget(adjustTargetTypetBtn[i], i + 1, 1, Qt::AlignLeft);

    adjustTargetMap->setMapping(adjustTargetTypetBtn[i], i);
    connect(adjustTargetTypetBtn[i], SIGNAL(pressed()), adjustTargetMap,
            SLOT(map()));
  }

  adjustAutoBtn = new QPushButton();
  adjustAutoBtn->setText(auto_adjust);
  adjustAutoBtn->setFixedSize(BTN_WID, BTN_HEI - 20);
  adjustLayout->addWidget(adjustAutoBtn, 1, 2, COLOR_TYPE_NUM, 1,
                          Qt::AlignCenter);

  darkAdjustBtn = new QPushButton();
  darkAdjustBtn->setText(dark_adjust);
  darkAdjustBtn->setFixedSize(BTN_WID, BTN_HEI - 20);
  adjustLayout->addWidget(darkAdjustBtn, COLOR_TYPE_NUM + 1, 2, 1, 1,
                          Qt::AlignCenter);

  //! 增益调节
  QGridLayout *gainLayout = new QGridLayout;
  gainLayout->setVerticalSpacing(10);
  for (int i = 0; i < ADJUST_TYPE; i++) { //! title
    adjustTypeLbl[i] = new QLabel();
    adjustTypeLbl[i]->setText(gainList.at(i));

    if (i > 0) {
      adjustTypeLbl[i]->setAlignment(Qt::AlignCenter);
      gainLayout->addWidget(adjustTypeLbl[i], 0, i, Qt::AlignCenter);
    } else {
      adjustTypeLbl[i]->setStyleSheet("font:bold;");
      gainLayout->addWidget(adjustTypeLbl[i], 0, i);
    }
  }

  QHBoxLayout *colorTypeLayout[COLOR_TYPE_NUM];
  for (int i = 0; i < COLOR_TYPE_NUM; i++) {
    colorTypeLbl[i] = new QLabel();
    colorTypeLbl[i]->setPixmap(QPixmap(colorList.at(i)));
    colorTypeLbl[i]->setFixedSize(30, 30);
    colorTypeLbl[i]->setAlignment(Qt::AlignCenter);

    colorTypeBtn[i] = new QLabel();
    colorTypeBtn[i]->setText(colorNameList.at(i));
    colorTypeBtn[i]->setFixedSize(BTN_HEI, BTN_HEI - 30);
    colorTypeBtn[i]->setAlignment(Qt::AlignCenter);

    colorTypeLayout[i] = new QHBoxLayout();
    colorTypeLayout[i]->addWidget(colorTypeLbl[i], 0, Qt::AlignCenter);
    colorTypeLayout[i]->addSpacing(5);
    colorTypeLayout[i]->addWidget(colorTypeBtn[i], 0, Qt::AlignCenter);

    fastAdjustNum[i] = new QPushButton();
    fastAdjustNum[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);
    fastAdjustNum[i]->setText(QString("%1").arg(1));

    fastAdjustMap->setMapping(fastAdjustNum[i], i);
    connect(fastAdjustNum[i], SIGNAL(pressed()), fastAdjustMap, SLOT(map()));

    slowAdjustNum[i] = new QPushButton();
    slowAdjustNum[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);
    slowAdjustNum[i]->setText(QString("%1").arg(50));

    slowAdjustMap->setMapping(slowAdjustNum[i], i);
    connect(slowAdjustNum[i], SIGNAL(pressed()), slowAdjustMap, SLOT(map()));

    gainLayout->addLayout(colorTypeLayout[i], i + 1, 0, Qt::AlignCenter);
    gainLayout->addWidget(fastAdjustNum[i], i + 1, 1, Qt::AlignCenter);
    gainLayout->addWidget(slowAdjustNum[i], i + 1, 2, Qt::AlignCenter);
  }
  colorTypeLbl[COLOR_TYPE_N]->hide();
  colorTypeBtn[COLOR_TYPE_N]->hide();
  fastAdjustNum[COLOR_TYPE_N]->hide();
  slowAdjustNum[COLOR_TYPE_N]->hide();

  //! 划分像元
  pixelTitleLbl = new QLabel();
  pixelTitleLbl->setText(pixel_channel);
  pixelTitleLbl->setFixedSize(BTN_WID, BTN_HEI - 30);
  pixelTitleLbl->setStyleSheet("font:bold;");

  QVBoxLayout *midPixelLeftLayout = new QVBoxLayout();
  midPixelLeftLayout->setSpacing(5);

  QHBoxLayout *pixelHLayout[PIXEL_TYPE];
  for (int i = 0; i < PIXEL_TYPE; i++) {
    beginChannelLbl[i] = new QLabel();
    beginChannelLbl[i]->setText(pixelList.at(i));
    beginChannelLbl[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

    beginChanelBtn[i] = new QPushButton();
    beginChanelBtn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

    pixelValueAdjustMap->setMapping(beginChanelBtn[i], i);
    connect(beginChanelBtn[i], SIGNAL(pressed()), pixelValueAdjustMap,
            SLOT(map()));

    pixelHLayout[i] = new QHBoxLayout();
    pixelHLayout[i]->addWidget(beginChannelLbl[i]);
    pixelHLayout[i]->addSpacing(55);
    pixelHLayout[i]->addWidget(beginChanelBtn[i]);
    pixelHLayout[i]->addStretch();
    midPixelLeftLayout->addLayout(pixelHLayout[i]);
  }

  //    auto layitem =[this](QBoxLayout *hlay, QWidget* wid, auto&&... widgets){
  //        hlay->addWidget(wid);
  //        hlay->addSpacing(25);
  //        (hlay->addWidget(widgets), ...);
  //    };

  QStringList ejectorlst;
  ejectorlst << myLan.ejector_divide << myLan.ejector_index
             << myLan.ejector_piexl;

  QGridLayout *glayejector = new QGridLayout();
  glayejector->setSpacing(5);

  for (int i = 0; i < 3; i++) {
    ejectorSetTitleLbl[i] = new QLabel(ejectorlst.at(i));
    ejectorSetTitleLbl[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

    if (i > 0) {
      ejectorSetTitleLbl[i]->setAlignment(Qt::AlignCenter);
      glayejector->addWidget(ejectorSetTitleLbl[i], 0, i, Qt::AlignRight);
    } else {
      ejectorSetTitleLbl[i]->setStyleSheet("font:bold;");
      glayejector->addWidget(ejectorSetTitleLbl[i], 0, i);
    }
  }

  for (int i = 0; i < 2; i++) {
    diveEjectorLb[i] = new QLabel();
    diveEjectorLb[i]->setText(ejectorList.at(i));
    diveEjectorLb[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

    diveEjectorBtn[i] = new QPushButton();
    diveEjectorBtn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

    diveEjectorPiexlBtn[i] = new QPushButton();
    diveEjectorPiexlBtn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

    ejectorDiveMap->setMapping(diveEjectorBtn[i], 2 * i);
    ejectorDiveMap->setMapping(diveEjectorPiexlBtn[i], 2 * i + 1);

    connect(diveEjectorBtn[i], SIGNAL(pressed()), ejectorDiveMap, SLOT(map()));
    connect(diveEjectorPiexlBtn[i], SIGNAL(pressed()), ejectorDiveMap,
            SLOT(map()));

    glayejector->addWidget(diveEjectorLb[i], i + 1, 0, Qt::AlignLeft);
    glayejector->addWidget(diveEjectorBtn[i], i + 1, 1, Qt::AlignRight);
    glayejector->addWidget(diveEjectorPiexlBtn[i], i + 1, 2, Qt::AlignRight);
  }

  QHBoxLayout *autoDivLayout = new QHBoxLayout();
  autoDivBtn = new QPushButton();
  autoDivBtn->setText(auto_pixel);
  autoDivBtn->setFixedSize(BTN_WID, BTN_HEI - 20);
  autoDivLayout->addWidget(autoDivBtn, 0, Qt::AlignCenter);

  //    QHBoxLayout *midPixelHLayout = new QHBoxLayout();
  //    midPixelHLayout->addStretch(0);
  //    midPixelHLayout->addLayout(midPixelLeftLayout);
  //    midPixelHLayout->addStretch(0);
  //    midPixelHLayout->addLayout(autoDivLayout);
  //    midPixelHLayout->addStretch(0);

  QVBoxLayout *pixelVLayout = new QVBoxLayout();
  pixelVLayout->setSpacing(5);
  pixelVLayout->addWidget(pixelTitleLbl, 0, Qt::AlignLeft);
  pixelVLayout->addLayout(midPixelLeftLayout);

  //! 红外偏置
  infraBiasLbl = new QLabel(infra_bias);
  infraBiasLbl->setFixedSize(BTN_WID, BTN_HEI - 30);
  infraBiasLbl->setStyleSheet("font:bold;");

  QStringList tmpLst;
  tmpLst << QString("%1A").arg(myLan.channel)
         << QString("%1B").arg(myLan.channel)
         << QString("%1C").arg(myLan.channel)
         << QString("%1D").arg(myLan.channel);
  channelTabBar = new QTabBar;
  for (int i = 0; i < 4; i++)
    channelTabBar->addTab(QIcon(colorList.at(i)), tmpLst[i]);
  channelTabBar->setCurrentIndex(0);
  channelTabBar->setStyleSheet(
      "QTabBar::tab{background-color: white;color:black;border:2px solid "
      "#dfdfdf;width:80px;"
      "border-top-left-radius:2px;border-top-right-radius:2px;font: 16px;"
      "padding-left:3px;padding-top:3px;padding-right:3px;padding-bottom:3px;}"
      "QTabBar::tab::selected{color:white;background-color: #007454;}");

  oddChannelLbl = new QLabel(QString("%1").arg(myLan.odd));
  oddChannelLbl->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

  oddChannelBtn = new QPushButton;
  oddChannelBtn->setFixedSize(BTN_WID, BTN_HEI - 25);

  QHBoxLayout *hLayOdd = new QHBoxLayout;
  hLayOdd->addWidget(oddChannelLbl, 0, Qt::AlignRight);
  hLayOdd->addSpacing(130);
  hLayOdd->addWidget(oddChannelBtn);
  hLayOdd->addSpacing(10);

  infraBiasWidget = new QWidget;

  waveLbl = new QLabel(myLan.spectral_band);
  waveLbl->setFixedSize(BTN_WID - 30, BTN_HEI - 25);

  QStringList str;
  str << myLan.all;
  for (int i = 0; i < 12; i++) {
    str << QString("%1%2").arg(myLan.spectral_band).arg(i + 1);
  }
  waveBox = new QComboBox;
  waveBox->setFixedSize(BTN_WID, BTN_HEI - 25);
  waveBox->addItems(str);
  waveBox->setCurrentIndex(struGsh.nWaveIndex);

  QHBoxLayout *hLayWave = new QHBoxLayout;
  hLayWave->addWidget(waveLbl, 0, Qt::AlignRight);
  hLayWave->addSpacing(130);
  hLayWave->addWidget(waveBox);
  hLayWave->addSpacing(10);

  QVBoxLayout *infraBiaVLayout = new QVBoxLayout(infraBiasWidget);
  infraBiaVLayout->setSpacing(5);
  infraBiaVLayout->addWidget(infraBiasLbl, 0, Qt::AlignLeft);
  infraBiaVLayout->addWidget(channelTabBar);
  infraBiaVLayout->addLayout(hLayOdd);
  infraBiaVLayout->addSpacing(10);
  infraBiaVLayout->addLayout(hLayWave);

  m_captureBtn = new QPushButton();
  m_captureBtn->setFixedSize(BTN_WID - 15, BTN_HEI - 10);
  m_captureBtn->setIcon(QIcon(":/res/png/Action_Camera.png"));
  m_captureBtn->setIconSize(QSize(BTN_WID - 15, BTN_HEI - 10));

  QHBoxLayout *laybtn = new QHBoxLayout();
  laybtn->addStretch();
  laybtn->addWidget(m_captureBtn);
  laybtn->addSpacing(1);

  //! 主布局
  m_camSubPage = new QWidget;
  QVBoxLayout *camLayout = new QVBoxLayout(m_camSubPage);
  camLayout->setContentsMargins(20, 0, 20, 0);
  camLayout->setSpacing(10);
  camLayout->addLayout(adjustLayout);
  camLayout->addLayout(gainLayout);
  camLayout->addLayout(pixelVLayout);
  camLayout->addLayout(glayejector);
  camLayout->addWidget(infraBiasWidget);
  camLayout->addStretch(2);
  camLayout->addLayout(laybtn);
  camLayout->addStretch();
}

/**
 * @brief cameraSignalPage::灯光控制
 */
void cameraSignalPage::createLightSubPage() {
  auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

  //! 背景参数
  lampTitleLbl = new QLabel();
  lampTitleLbl->setText("");
  lampTitleLbl->setFixedSize(BTN_WID, BTN_HEI - 30);
  lampTitleLbl->setStyleSheet("font:bold;");
  lampTitleLbl->hide();

  QVBoxLayout *backLayout = new QVBoxLayout();
  backLayout->setSpacing(20);
  QHBoxLayout *lampHLayout[LAMP_TYPE];
  for (int i = 0; i < LAMP_TYPE; i++) {
    lampLbl[i] = new QPushButton();
    lampLbl[i]->setText(pProfile->nLampName[i]);
    lampLbl[i]->setFixedSize(BTN_WID + 30, BTN_HEI - 20);

    lampLblAdjustMap->setMapping(lampLbl[i], i);
    connect(lampLbl[i], SIGNAL(pressed()), lampLblAdjustMap, SLOT(map()));

    lampValuetBtn[i] = new QPushButton();
    lampValuetBtn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 20);

    lampValueAdjustMap->setMapping(lampValuetBtn[i], i);
    connect(lampValuetBtn[i], SIGNAL(pressed()), lampValueAdjustMap,
            SLOT(map()));

    m_lampEn[i] = new MyCheckBox("");
    m_lampEn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 20);
    m_lampEn[i]->setCheckBtnSize(50, 50);
    lampEnmap->setMapping(m_lampEn[i], i);
    connect(m_lampEn[i], SIGNAL(pressed()), lampEnmap, SLOT(map()));

    lampHLayout[i] = new QHBoxLayout();
    lampHLayout[i]->setSpacing(30);
    lampHLayout[i]->addWidget(m_lampEn[i], 0, Qt::AlignLeft);
    lampHLayout[i]->addWidget(lampLbl[i], 0, Qt::AlignRight);
    lampHLayout[i]->addWidget(lampValuetBtn[i], 0, Qt::AlignLeft);
    backLayout->addLayout(lampHLayout[i]);
  }

  //! 恒流源
  QVBoxLayout *pluseLayout = new QVBoxLayout();
  pluseLayout->setSpacing(20);
  QHBoxLayout *pluseHLayout[PLUSE_TYPE];
  for (int i = 0; i < PLUSE_TYPE; i++) {
    pluseLbl[i] = new QPushButton();
    pluseLbl[i]->setText(pProfile->nPluseName[i]);
    pluseLbl[i]->setFixedSize(BTN_WID + 30, BTN_HEI - 20);

    pluseLblAdjustMap->setMapping(pluseLbl[i], i);
    connect(pluseLbl[i], SIGNAL(pressed()), pluseLblAdjustMap, SLOT(map()));

    pluseValuetBtn[i] = new QPushButton();
    pluseValuetBtn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 20);

    pluseValueAdjustMap->setMapping(pluseValuetBtn[i], i);
    connect(pluseValuetBtn[i], SIGNAL(pressed()), pluseValueAdjustMap,
            SLOT(map()));

    m_pluseEn[i] = new MyCheckBox("");
    m_pluseEn[i]->setFixedSize(BTN_WID - 30, BTN_HEI - 20);
    m_pluseEn[i]->setCheckBtnSize(50, 50);
    pluseEnmap->setMapping(m_pluseEn[i], i);
    connect(m_pluseEn[i], SIGNAL(pressed()), pluseEnmap, SLOT(map()));

    pluseHLayout[i] = new QHBoxLayout();
    pluseHLayout[i]->setSpacing(30);
    pluseHLayout[i]->addWidget(m_pluseEn[i], 0, Qt::AlignLeft);
    pluseHLayout[i]->addWidget(pluseLbl[i], 0, Qt::AlignRight);
    pluseHLayout[i]->addWidget(pluseValuetBtn[i], 0, Qt::AlignLeft);
    pluseLayout->addLayout(pluseHLayout[i]);
  }

  halogenLampLbl = new QLabel(myLan.halogen); //! 卤素灯
  halogenLampLbl->setFixedSize(BTN_WID, BTN_HEI + 20);

  hologenSwitchBtn = new mySwitchButton;
  hologenSwitchBtn->setFixedSize(BTN_WID, BTN_HEI - 15);

  QHBoxLayout *hologenHLayout = new QHBoxLayout();
  hologenHLayout->addWidget(halogenLampLbl, 0, Qt::AlignLeft);
  hologenHLayout->addWidget(hologenSwitchBtn, 0, Qt::AlignLeft);
  hologenHLayout->addSpacing(80);

  m_lightSubPage = new QWidget;
  QVBoxLayout *lampVLayout = new QVBoxLayout(m_lightSubPage);

  lampVLayout->addWidget(lampTitleLbl, 0, Qt::AlignLeft);
  lampVLayout->addSpacing(10);
  lampVLayout->addLayout(backLayout);
  lampVLayout->addSpacing(20);
  lampVLayout->addLayout(pluseLayout);
  lampVLayout->addSpacing(40);
  lampVLayout->addLayout(hologenHLayout);
  lampVLayout->addStretch();
}

/**
 * @brief cameraSignalPage::传感器
 */
void cameraSignalPage::createSensorSubPage() {
  m_sensorSubPage = new QWidget;

  QVBoxLayout *vLay = new QVBoxLayout(m_sensorSubPage);
  vLay->setSpacing(20);

  for (int i = 0; i < MAX_CAM_NUM; i++) {
    sensorGBox[i] = new QGroupBox();
    sensorGBox[i]->setFixedWidth(440);
    sensorGBox[i]->setStyleSheet("font-size:22px;");

    //! 行频调节
    freValueLbl[i] = new QLabel();
    freValueLbl[i]->setFixedSize(BTN_WID, BTN_HEI - 25);
    freValueLbl[i]->setText(fre_value);

    freValueBtn[i] = new QPushButton();
    freValueBtn[i]->setFixedSize(BTN_WID, BTN_HEI - 25);

    sensorTypeLbl[i] = new QLabel();
    //        sensorTypeLbl[i]->setAlignment(Qt::AlignCenter);
    sensorTypeLbl[i]->setFixedSize(BTN_WID, BTN_HEI - 25);
    sensorTypeLbl[i]->setText(sensor_type);

    QStringList sensorLst, senwaveLst;
    sensorLst << "VL05" << "IM01" << "VL08" << "IM04";
    senwaveLst << "6" << "12";
    sensorTypeCombox[i] = new QComboBox;
    sensorTypeCombox[i]->setFixedSize(BTN_WID, BTN_HEI - 25);
    sensorTypeCombox[i]->addItems(sensorLst);
    sensorWaveComb[i] = new QComboBox();
    sensorWaveComb[i]->setFixedSize(110, BTN_HEI - 25);
    sensorWaveComb[i]->addItems(senwaveLst);
    sensorWaveComb[i]->hide();

    exposeValueLbl[i] = new QLabel(); //! 曝光值
    exposeValueLbl[i]->setFixedSize(BTN_WID, BTN_HEI - 25);
    exposeValueLbl[i]->setText(expose_value);

    exposeValueBtn[i] = new QPushButton();
    exposeValueBtn[i]->setFixedSize(BTN_WID, BTN_HEI - 25);

    NoneexposeValueLbl[i] = new QLabel(); //! 非曝光值
    NoneexposeValueLbl[i]->setFixedSize(BTN_WID, BTN_HEI - 25);
    NoneexposeValueLbl[i]->setText(Noneexpose_value);

    NoneexposeValueBtn[i] = new QPushButton();
    NoneexposeValueBtn[i]->setFixedSize(BTN_WID, BTN_HEI - 25);

    exportWaveBtn[i] = new QPushButton(m_strExportWaves); //! 波段
    exportWaveBtn[i]->setFixedSize(110, 50);

    wavesLabels[i] = new QLabel(""); //! 波段
    wavesLabels[i]->setFixedSize(270, 50);
    wavesLabels[i]->setWordWrap(true);

    tempurelb[i] = new QLabel(); //! 温度值
    tempurebtn[i] = new QPushButton(myLan.temperature);
    tempurebtn[i]->setFixedSize(110, 50);

    QGridLayout *sensorLayout = new QGridLayout(sensorGBox[i]);
    sensorLayout->setContentsMargins(30, 5, 20, 5);
    sensorLayout->addWidget(freValueLbl[i], 0, 0, Qt::AlignRight);
    sensorLayout->addWidget(freValueBtn[i], 0, 1, Qt::AlignLeft);
    sensorLayout->addWidget(sensorTypeLbl[i], 1, 0, Qt::AlignRight);
    sensorLayout->addWidget(sensorTypeCombox[i], 1, 1, Qt::AlignLeft);
    sensorLayout->addWidget(sensorWaveComb[i], 1, 2, Qt::AlignRight);
    sensorLayout->addWidget(exposeValueLbl[i], 2, 0, Qt::AlignRight);
    sensorLayout->addWidget(exposeValueBtn[i], 2, 1, Qt::AlignLeft);
    sensorLayout->addWidget(NoneexposeValueLbl[i], 3, 0, Qt::AlignRight);
    sensorLayout->addWidget(NoneexposeValueBtn[i], 3, 1, Qt::AlignLeft);
    sensorLayout->addWidget(tempurebtn[i], 4, 0, Qt::AlignLeft);
    sensorLayout->addWidget(tempurelb[i], 4, 1, Qt::AlignLeft);
    sensorLayout->addWidget(exportWaveBtn[i], 5, 0, Qt::AlignLeft);
    sensorLayout->addWidget(wavesLabels[i], 5, 1, Qt::AlignLeft);

    vLay->addWidget(sensorGBox[i]);

    frequencyMap->setMapping(freValueBtn[i], i);
    connect(freValueBtn[i], SIGNAL(pressed()), frequencyMap, SLOT(map()));

    sensorMap->setMapping(sensorTypeCombox[i], i);
    connect(sensorTypeCombox[i], SIGNAL(currentIndexChanged(int)), sensorMap,
            SLOT(map()));

    sensorWavemap->setMapping(sensorWaveComb[i], i);
    connect(sensorWaveComb[i], SIGNAL(currentIndexChanged(int)), sensorWavemap,
            SLOT(map()));

    exposeMap->setMapping(exposeValueBtn[i], i);
    connect(exposeValueBtn[i], SIGNAL(pressed()), exposeMap, SLOT(map()));

    NoneexposeMap->setMapping(NoneexposeValueBtn[i], i);
    connect(NoneexposeValueBtn[i], SIGNAL(pressed()), NoneexposeMap,
            SLOT(map()));

    exportWavesMapper->setMapping(exportWaveBtn[i], i);
    connect(exportWaveBtn[i], SIGNAL(clicked()), exportWavesMapper,
            SLOT(map()));

    tempreqMapper->setMapping(tempurebtn[i], i);
    connect(tempurebtn[i], SIGNAL(pressed()), tempreqMapper, SLOT(map()));
  }

  backGBox = new QGroupBox(QString("%1").arg(myLan.background_set));
  backGBox->setFixedWidth(440);
  backGBox->setStyleSheet("font-size:22px;");

  threadLbl = new QLabel(QString("%1").arg(myLan.background_threshold));
  threadLbl->setFixedSize(BTN_WID, BTN_HEI - 25);
  threadBtn = new QPushButton();
  threadBtn->setFixedSize(BTN_WID, BTN_HEI - 25);

  upLimitLbl = new QLabel(QString("%1").arg(myLan.background_uplimit));
  upLimitLbl->setFixedSize(BTN_WID, BTN_HEI - 25);
  upLimitBtn = new QPushButton();
  upLimitBtn->setFixedSize(BTN_WID, BTN_HEI - 25);

  rangeLbl = new QLabel();
  rangeLbl->setText(range_parmas);
  rangeLbl->setFixedSize(BTN_WID, BTN_HEI - 25);

  rangeValueBtn = new QPushButton();
  rangeValueBtn->setFixedSize(BTN_WID, BTN_HEI - 25);

  //! 背景条件
  m_backgb = new QGroupBox(QString("%1").arg(myLan.background_conditon));
  m_backgb->setFixedWidth(440);

  QStringList bcklist;
  bcklist << myLan.black_bg << myLan.red_bg << myLan.green_bg << myLan.yellow_bg
          << myLan.blue_bg << myLan.purple_bg << myLan.cyan_bg
          << myLan.white_bg;

  m_backlb = new QLabel(myLan.background);
  m_bckcomb = new QComboBox();
  m_bckcomb->setFixedWidth(180);
  QListWidget *bcklstwid = new QListWidget();
  m_bckcomb->setModel(bcklstwid->model());
  m_bckcomb->setView(bcklstwid);
  for (int i = 0; i < bcklist.size(); i++) {
    m_bckcomb->insertItem(i, bcklist.at(i));
  }

  m_bckthdlb = new QLabel(myLan.threshold);
  m_bckle = new MyLineEdit();
  m_bckle->setFixedWidth(180);
  m_bckle->setStyleSheet("background-color:rgba(220,220,220,100)");
  m_bckthdlb->hide();
  m_bckle->hide();

  m_bckratiolb = new QLabel(myLan.ratio);
  m_bckratiole = new MyLineEdit();
  m_bckratiole->setFixedWidth(180);
  m_bckratiole->setStyleSheet("background-color:rgba(220,220,220,100)");
  m_bckratiole->hide();
  m_bckratiolb->hide();

  QHBoxLayout *hlaybck = new QHBoxLayout();
  hlaybck->addWidget(m_backlb);
  hlaybck->addWidget(m_bckcomb);

  QHBoxLayout *hlaythd = new QHBoxLayout();
  hlaythd->addWidget(m_bckthdlb);
  hlaythd->addWidget(m_bckle);

  QHBoxLayout *hlayratio = new QHBoxLayout();
  hlayratio->addWidget(m_bckratiolb);
  hlayratio->addWidget(m_bckratiole);

  QVBoxLayout *vlaybck = new QVBoxLayout(m_backgb);
  vlaybck->setContentsMargins(0, 30, 0, 0);
  vlaybck->addLayout(hlaybck);
  vlaybck->addLayout(hlaythd);
  vlaybck->addLayout(hlayratio);

  QGridLayout *backLayout = new QGridLayout(backGBox);
  backLayout->setContentsMargins(30, 40, 30, 20);
  backLayout->addWidget(threadLbl, 0, 0, Qt::AlignRight);
  backLayout->addWidget(threadBtn, 0, 1, Qt::AlignLeft);

  backLayout->addWidget(upLimitLbl, 1, 0, Qt::AlignRight);
  backLayout->addWidget(upLimitBtn, 1, 1, Qt::AlignLeft);

  backLayout->addWidget(rangeLbl, 2, 0, Qt::AlignRight);
  backLayout->addWidget(rangeValueBtn, 2, 1, Qt::AlignLeft);

  vLay->addWidget(backGBox);
  vLay->addWidget(m_backgb);
  //    vLay->addStretch();
}

/**
 * @brief cameraSignalPage::信号槽绑定
 */
void cameraSignalPage::connectSigAndSlot() {
  connect(m_camBtnMap, SIGNAL(mapped(int)), this, SLOT(onCamViewChanged(int)));

  connect(adjustAutoBtn, SIGNAL(pressed()), this,
          SLOT(onAutoAdjustBtnPressedSlt()));
  connect(darkAdjustBtn, SIGNAL(pressed()), this,
          SLOT(onDarkAdjustBtnPressedSlt()));
  connect(m_tabWidget, SIGNAL(currentChanged(int)), this,
          SLOT(onTabWidgetChangedSlt(int)));
  connect(m_captureBtn, &QPushButton::pressed, this,
          &cameraSignalPage::onCaptureBtnPressedSlt);
  connect(m_pageGetImage, &getImagePage::sigbackBtnpressed, [this]() {
    this->refreshPageDisplay();
    m_stacklaytopPage->setCurrentIndex(0);
  });
  connect(adjustTargetMap, SIGNAL(mapped(int)), this,
          SLOT(onAdjustTargetBtnPressedSlt(int)));
  connect(waveTypeMap, SIGNAL(mapped(int)), this,
          SLOT(onWaveTypeBtnPressedSlt(int)));
  connect(fastAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onFastGainBtnPressedSlt(int)));
  connect(slowAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onSlowGainBtnPressedSlt(int)));

  connect(frequencyMap, SIGNAL(mapped(int)), this,
          SLOT(onFreValueBtnPressedSlt(int)));
  connect(sensorMap, SIGNAL(mapped(int)), this,
          SLOT(onSensorTypeChangedSlt(int)));
  connect(sensorWavemap, SIGNAL(mapped(int)), this,
          SLOT(onSensorWaveChangedSlt(int)));
  connect(exposeMap, SIGNAL(mapped(int)), this,
          SLOT(onExposeValueBtnPresseSlt(int)));
  connect(NoneexposeMap, SIGNAL(mapped(int)), this,
          SLOT(onNoneExposeValueBtnPresseSlt(int)));
  connect(exportWavesMapper, SIGNAL(mapped(int)), this,
          SLOT(onExportWaveBtnPresseSlt(int)));
  connect(tempreqMapper, SIGNAL(mapped(int)), this,
          SLOT(onTemptureReqSlt(int)));

  connect(lampLblAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onLampLblBtnPressedSlt(int)));
  connect(pluseLblAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onPluseLblBtnPressedSlt(int)));

  connect(lampValueAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onLampValueBtnPressedSlt(int)));
  connect(pluseValueAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onPluseValueBtnPressedSlt(int)));
  connect(lampEnmap, SIGNAL(mapped(int)), this, SLOT(onLampEnchgSlt(int)));
  connect(pluseEnmap, SIGNAL(mapped(int)), this, SLOT(onPluseEnchgSlt(int)));

  connect(hologenSwitchBtn, SIGNAL(checkedChanged(bool)), this,
          SLOT(onHologenSwitchBtnSlt(bool)));

  connect(rangeValueBtn, SIGNAL(pressed()), this,
          SLOT(onBGParamsValuePressedSlt()));
  connect(pixelValueAdjustMap, SIGNAL(mapped(int)), this,
          SLOT(onPixelChannelBtnPressedSlt(int)));
  connect(ejectorDiveMap, SIGNAL(mapped(int)), this,
          SLOT(onEjectorDiveBtnPressedSlt(int)));

  connect(channelTabBar, SIGNAL(currentChanged(int)), this,
          SLOT(onChannelTabChangedSlt(int)));
  connect(oddChannelBtn, SIGNAL(pressed()), this,
          SLOT(onOddChannelBtnPressedSlt()));

  connect(g_curveDataThread, SIGNAL(sReceiveData()), myAxis, SLOT(update()));

  connect(waveBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onWaveBoxChangedSlt(int)));
  connect(threadBtn, SIGNAL(pressed()), this, SLOT(onIM01ThreadPressedSlt()));
  connect(upLimitBtn, SIGNAL(pressed()), this, SLOT(onIM01LimitPressedSlt()));

  connect(m_bckcomb, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onbackGroundIdxSlt(int)));
  connect(m_bckle, SIGNAL(pressed()), this, SLOT(onbackGroundValueSlt()));
  connect(m_bckratiole, SIGNAL(pressed()), this, SLOT(onbackGroundRatioSlt()));
}

/**
 * @brief cameraSignalPage::波形类型切换
 * @param index
 */
void cameraSignalPage::onWaveTypeBtnPressedSlt(int index) {
  if (n_wave_type == index)
    return;

  n_wave_type = index;

  if (n_wave_type == WAVE_SIGNAL) {
    getAdjustSignal();
  } else {
    if (!g_curveDataThread->getRunningState()) {
      g_curveDataThread->startWave();
    }
  }

  refreshWaveBtnState();
}

/**
 * @brief cameraSignalPage::分析仪切换
 */
void cameraSignalPage::onNodeBoxChangedSlt(int index) {
  struGsh.nAnalyzeIndex = index;

  refreshFreValue();

  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

  for (int i = 0; i < LAMP_TYPE; i++) {
    lampValuetBtn[i]->setText(QString("%1").arg(pProfile->nLampValue[i]));
    lampLbl[i]->setText(QString("%1").arg(pProfile->nLampName[i]));
    m_lampEn[i]->setChecked(pProfile->bEnlamp[i]);
  }

  for (int i = 0; i < PLUSE_TYPE; i++) {
    pluseValuetBtn[i]->setText(QString("%1").arg(pProfile->nPluseValue[i]));
    pluseLbl[i]->setText(QString("%1").arg(pProfile->nPluseName[i]));
    m_pluseEn[i]->setChecked(pProfile->bEnPluse[i]);
  }

  hologenSwitchBtn->setChecked(pProfile->bHologenLampOn);

  refreshCamParam();

  if (n_wave_type == WAVE_SIGNAL) {
    getAdjustSignal();
  }
}

/**
 * @brief cameraSignalPage::前后视4相机切换
 * @param index
 */
void cameraSignalPage::onCamViewChanged(int index) {
  if (n_view == (FRONT_VIEW + index))
    return;

  n_view = FRONT_VIEW + index;
  struGsh.nUnit = FRONT_VIEW + index;

  refreshCamParam();
  refreshFreValue();
  refreshLightParams();

  if (n_wave_type == WAVE_SIGNAL) {
    getAdjustSignal();
  }
}

/**
 * @brief cameraSignalPage::校正目标设置
 * @param index
 */
void cameraSignalPage::onAdjustTargetBtnPressedSlt(int index) {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 255, adjustTargetTypetBtn[index]->text().toDouble(),
      adjust_target, g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->nAdjustTarget[n_view][index] = inputPanel->getValue();
    adjustTargetTypetBtn[index]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nAdjustTarget[n_view][index]));
  }
}

/**
 * @brief cameraSignalPage::自动校正
 */
void cameraSignalPage::onAutoAdjustBtnPressedSlt() {
  if (n_wave_type != WAVE_ORI) {
    infoWidget->raise();
    infoWidget->setLabelText(myLan.forbid_adjust_signal);
    infoWidget->showSecs();
  } else {
    myMessageBox msgBox(MSG_QUES, auto_adjust, adjust_warn, g_mainWidget);

    int ret = msgBox.exec();
    if (ret != QDialog::Accepted) {
      return;
    }

    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->delayShow();

    //! 1、校正系数获取
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, nCalibrateID, 1, n_view);

    gFlow.sleep(1);

    //! 2、校正系数保存
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, nCalibrateID, 3, n_view);

    infoWidget->hide();
  }
}

//! 暗校正
void cameraSignalPage::onDarkAdjustBtnPressedSlt() {
  if (n_wave_type != WAVE_ORI) {
    infoWidget->raise();
    infoWidget->setLabelText(myLan.forbid_adjust_signal);
    infoWidget->showSecs();
  } else {
    myMessageBox msgBox(MSG_QUES, dark_adjust, adjust_warn, g_mainWidget);

    int ret = msgBox.exec();
    if (ret != QDialog::Accepted) {
      return;
    }

    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->delayShow();

    //! 1、校正系数获取
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, nCalibrateID, 0, n_view);

    gFlow.sleep(1);

    //! 2、校正系数保存
    gFlow.sendCalibratePara(struGsh.nAnalyzeIndex, nCalibrateID, 2, n_view);

    infoWidget->hide();
  }
}

/*!
 * \brief 槽: 参数标签切换
 * \param index
 */
void cameraSignalPage::onTabWidgetChangedSlt(int index) {
  if (0 == index) {
    refreshAdjustValue();
    refreshPixelValue();
    refreshEjectorValue();
    refreshInfraBiasValue();
  } else if (index == 1) {
    refreshLightParams();
  } else {
    refreshFreValue();
  }
}

/**
 * @brief cameraSignalPage::粗调增益\或IM01模拟增益
 * @param index
 */
void cameraSignalPage::onFastGainBtnPressedSlt(int index) {
  int max = 15;
  if (gFlow.bIsIMSerialCam(n_view))
    max = 63;
  if (SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                         ->struCaParams->sensorType[n_view]) {
    max = 511;
  }

  myInputPanel *inputPanel =
      new myInputPanel(intType, 0, max, fastAdjustNum[index]->text().toDouble(),
                       fast_adjust, g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    analyzeGlobal *pAnalyzeGlobal =
        gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    pProfile->nGainFast[n_view][index] = inputPanel->getValue();
    fastAdjustNum[index]->setText(
        QString("%1").arg(pProfile->nGainFast[n_view][index]));

    gFlow.sendGainPara(struGsh.nAnalyzeIndex, n_view);
  }
}

/**
 * @brief cameraSignalPage::细调增益
 * @param index
 */
void cameraSignalPage::onSlowGainBtnPressedSlt(int index) {
  myInputPanel *inputPanel =
      new myInputPanel(intType, 0, 255, slowAdjustNum[index]->text().toDouble(),
                       slow_adjust, g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    analyzeGlobal *pAnalyzeGlobal =
        gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    pProfile->nGainSlow[n_view][index] = inputPanel->getValue();
    slowAdjustNum[index]->setText(
        QString("%1").arg(pProfile->nGainSlow[n_view][index]));

    gFlow.sendGainPara(struGsh.nAnalyzeIndex, n_view);
  }
}

void cameraSignalPage::onLampLblBtnPressedSlt(int index) {
  myInputMethod input(myLan.lighting_name, lampLbl[index]->text(),
                      g_mainWidget);
  int ret = input.exec();
  if (ret == QDialog::Accepted) {
    QString tmp = input.getText().trimmed();

    analyzeGlobal *pAnalyzeGlobal =
        gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    pProfile->nLampName[index] = tmp;
    lampLbl[index]->setText(tmp);
  }
}

void cameraSignalPage::onPluseLblBtnPressedSlt(int index) {
  myInputMethod input(myLan.lighting_name, pluseLbl[index]->text(),
                      g_mainWidget);
  int ret = input.exec();
  if (ret == QDialog::Accepted) {
    QString tmp = input.getText().trimmed();

    analyzeGlobal *pAnalyzeGlobal =
        gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    pProfile->nPluseName[index] = tmp;
    pluseLbl[index]->setText(tmp);
  }
}

/**
 * @brief 背景灯灯光参数调节
 * @param index
 */
void cameraSignalPage::onLampValueBtnPressedSlt(int index) {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 3000, lampValuetBtn[index]->text().toDouble(),
      lampLbl[index]->text(), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    analyzeGlobal *pAnalyzeGlobal =
        gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    pProfile->nLampValue[index] = inputPanel->getValue();
    lampValuetBtn[index]->setText(
        QString("%1").arg(pProfile->nLampValue[index]));

    gFlow.sendLampPara(struGsh.nAnalyzeIndex);
  }
}

/**
 * @brief 物料灯参数调节
 * @param index
 */
void cameraSignalPage::onPluseValueBtnPressedSlt(int index) {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 3000, pluseValuetBtn[index]->text().toDouble(),
      pluseLbl[index]->text(), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    analyzeGlobal *pAnalyzeGlobal =
        gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
    struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

    pProfile->nPluseValue[index] = inputPanel->getValue();
    pluseValuetBtn[index]->setText(
        QString("%1").arg(pProfile->nPluseValue[index]));

    gFlow.sendPlusePara(struGsh.nAnalyzeIndex);
  }
}

/*!
 * \brief cameraSignalPage::onPluseEnchgSlt
 * \param nIdx
 */
void cameraSignalPage::onPluseEnchgSlt(int nIdx) {
  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

  pProfile->bEnPluse[nIdx] = m_pluseEn[nIdx]->isChecked();
  gFlow.sendPlusePara(struGsh.nAnalyzeIndex);
}

/*!
 * \brief cameraSignalPage::onLampEnchgSlt
 * \param nIdx
 */
void cameraSignalPage::onLampEnchgSlt(int nIdx) {
  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

  pProfile->bEnlamp[nIdx] = m_lampEn[nIdx]->isChecked();
  gFlow.sendLampPara(struGsh.nAnalyzeIndex);
}

/*!
 * \brief 卤素灯开关
 * \param bCheck
 */
void cameraSignalPage::onHologenSwitchBtnSlt(bool bCheck) {
  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

  pProfile->bHologenLampOn = bCheck;

  gFlow.sendLampPara(struGsh.nAnalyzeIndex);
}

/**
 * @brief cameraSignalPage::背景参数调节
 * @param index
 */
void cameraSignalPage::onBGParamsValuePressedSlt() {
  myInputPanel *inputPanel =
      new myInputPanel(intType, 0, 255, rangeValueBtn->text().toDouble(),
                       range_parmas, g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->nThreadValue[n_view] = inputPanel->getValue();
    rangeValueBtn->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nThreadValue[n_view]));
    gFlow.sendBackGroundValue(struGsh.nAnalyzeIndex, n_view);
  }
}

/**
 * @brief cameraSignalPage::行频
 */
void cameraSignalPage::onFreValueBtnPressedSlt(int index) {
  bool bIM01 = SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                  ->struCaParams->sensorType[index];
  bool bIM03 = SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                  ->struCaParams->sensorType[index];
  int max = bIM01 ? 3000 : 25000;
  int min = bIM01 ? /*492*/ 1 : 1;
  max = bIM03 ? 80000 : max;
  myInputPanel *inputPanel =
      new myInputPanel(intType, min, max, freValueBtn[index]->text().toDouble(),
                       fre_value, g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int tmp = inputPanel->getValue();
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->nFreQuence[index] = tmp;
    freValueBtn[index]->setText(QString("%1").arg(tmp));

    //        bool bIM01 = SENSOR_IM01 ==
    //        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[index];
    if (bIM01 || bIM03)
      gFlow.setIM01RowFreq(struGsh.nAnalyzeIndex, index);
    else
      gFlow.sendRowFreqPara(struGsh.nAnalyzeIndex, index, SENSOR_D04);

    if (tmp != 0) {
      struGsh.nlegthRatio =
          1000 * struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nMotorSpeed /
          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
              ->struCaParams->nFreQuence[0];
    }
  }
}

/*!
 * \brief 传感器类型设置
 * \param index: 相机地址
 */
void cameraSignalPage::onSensorTypeChangedSlt(int index) {
  auto *pCamParams = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;
  int tmp = sensorTypeCombox[index]->currentIndex();

  //! 设置传感器类型
  pCamParams->sensorType[index] = tmp;

  //! 刷新UI及参数
  if (gFlow.bIsIMSerialCam(index)) {
    exposeValueLbl[index]->show();
    exposeValueBtn[index]->show();

    if (SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                           ->struCaParams->sensorType[index]) {
      NoneexposeValueBtn[index]->show();
      NoneexposeValueLbl[index]->show();
      // sensorWaveComb[index]->show();
    }

    wavesLabels[index]->show();
    exportWaveBtn[index]->show();

    tempurelb[index]->show();
    tempurebtn[index]->show();

    gFlow.setIM01Waves(struGsh.nAnalyzeIndex, index);
    gFlow.setIM01Window(struGsh.nAnalyzeIndex, index);
    gFlow.setIM01RowFreq(struGsh.nAnalyzeIndex, index);

    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nPixel[index] = PIXEL_640;
  } else {
    exposeValueLbl[index]->hide();
    exposeValueBtn[index]->hide();

    NoneexposeValueBtn[index]->hide();
    NoneexposeValueLbl[index]->hide();

    sensorWaveComb[index]->hide();

    wavesLabels[index]->hide();
    exportWaveBtn[index]->hide();

    tempurelb[index]->hide();
    tempurebtn[index]->hide();

    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->nPixel[index] = PIXEL_2048;
  }

  refreshCamParam();

  refreshBackGroundParams();

  //! 设置SOC的工作模式
  if (struCnfg.nMachineType == nMachine_Bottle) {
    if (tmp == SENSOR_VL05) {
      if (index == 0 &&
          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bHasAssist) {
        pCamParams->nWorkMode[index] = 2;
      } else if (index == 2) {
        pCamParams->nWorkMode[index] = 1;
      } else {
        pCamParams->nWorkMode[index] = 0;
      }
    } else if (tmp == SENSOR_VL08) {
      pCamParams->nWorkMode[index] = 3;
    } else if (tmp == SENSOR_IM04 || tmp == SENSOR_IM01) {
      pCamParams->nWorkMode[index] = 0;
    } else {
      pCamParams->nWorkMode[index] = 0;
    }
  } else {
    pCamParams->nWorkMode[index] = 0;
  }

  gFlow.sendSocWorkMode(index);
}

/*!
 * \brief cameraSignalPage::onSensorWaveChangedSlt
 * \param index
 */
void cameraSignalPage::onSensorWaveChangedSlt(int index) {
  int tmp = sensorWaveComb[index]->currentIndex();
  for (int i = 0; i < gFlow.getMachineCamNum(); i++) {
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[i] =
        tmp;
    if (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->struCaParams->sensorType[i] == SENSOR_IM04) {
      gFlow.sendSocWorkMode(i);
    }
  }
}

/*!
 * \brief 槽：曝光值
 * \param index
 */
void cameraSignalPage::onExposeValueBtnPresseSlt(int index) {
  int max = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->struCaParams->nFreQuence[index] -
            492;
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, max, exposeValueBtn[index]->text().toDouble(), expose_value,
      g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int tmp = inputPanel->getValue();
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->exposeValue[index] = tmp;
    exposeValueBtn[index]->setText(QString("%1").arg(tmp));
    gFlow.setIM01RowFreq(struGsh.nAnalyzeIndex, index);
  }
}

/*!
 * \brief 非曝光值
 * \param index
 */
void cameraSignalPage::onNoneExposeValueBtnPresseSlt(int index) {
  int max = 65535;
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, max, NoneexposeValueBtn[index]->text().toInt(), "",
      g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int tmp = inputPanel->getValue();
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->NoneexposeValue[index] = tmp;
    NoneexposeValueBtn[index]->setText(QString("%1").arg(tmp));
    gFlow.setIM01RowFreq(struGsh.nAnalyzeIndex, index);
  }
}

/*!
 * \brief 槽: 波段设置
 * \param index
 */
void cameraSignalPage::onExportWaveBtnPresseSlt(int index) {
  QVector<int> rows;
  int nMaxwave = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                             ->struCaParams->nWaveSeg[index] == 0
                     ? 6
                     : 12;
  for (int m = 0; m < nMaxwave; m++) {
    rows << struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.n12WavesVal[index][m];
  }

  WaveSelectWgt waveSelectWgt;
  waveSelectWgt.setSelectedRows(rows);
  if (struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->struCaParams->sensorType[n_view] == SENSOR_IM01) {
    waveSelectWgt.setIMSensorType(SENSOR_IM01);
  } else {
    waveSelectWgt.setIMSensorType(SENSOR_IM04);
  }
  if (waveSelectWgt.exec() == QDialog::Accepted) {
    rows.clear();
    rows = waveSelectWgt.getSelectedRows();
    for (int i = 0; i < rows.size(); i++) {
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->analyzeProfile.n12WavesVal[index][i] = rows.at(i);
    }

    infoWidget->setLabelText(myLan.apply_and_wait);
    infoWidget->delayShow();
    QString str;
    for (int m = 0; m < nMaxwave; m++) {
      str += QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                 ->analyzeProfile.n12WavesVal[index][m]);
      str += " ";
      // if(m == 5){
      //     str +="\n";
      // }
    }

    wavesLabels[index]->setText(str);

    gFlow.setIM01Waves(struGsh.nAnalyzeIndex, index);
    gFlow.setIM01Window(struGsh.nAnalyzeIndex, index);
    gFlow.setIM01RowFreq(struGsh.nAnalyzeIndex, index);
    infoWidget->hide();
  }
}

/*!
 * \brief 查询相机温度传感器
 */
void cameraSignalPage::onTemptureReqSlt(int nIdx) {
  tempurelb[nIdx]->setText(QString("----"));
  gFlow.reqIM01SensorTempure(struGsh.nAnalyzeIndex, nIdx);
}

/**
 * @brief cameraSignalPage::像元设置
 * @param index
 */
void cameraSignalPage::onPixelChannelBtnPressedSlt(int index) {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 1, 2048, beginChanelBtn[index]->text().toDouble(),
      beginChannelLbl[index]->text(), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    if (index == PIXEL_BEGIN) {
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->struCaParams->nPixelBegin[n_view] = inputPanel->getValue();
    } else {
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
          ->struCaParams->nPixelEnd[n_view] = inputPanel->getValue();
    }

    beginChanelBtn[index]->setText(QString("%1").arg(inputPanel->getValue()));
    gFlow.sendCameraPixelChannel(struGsh.nAnalyzeIndex, n_view);
    gFlow.SocChannelinfo();
    gFlow.ResetPxOfEj(n_view);
  }
}

void cameraSignalPage::onChannelTabChangedSlt(int index) {
  int tmp = channelTabBar->currentIndex();
  oddChannelBtn->setText(
      QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                            ->struCaParams->infraBias[n_view][tmp]));
}

/*!
 * \brief 槽：红外偏置像元
 */
void cameraSignalPage::onOddChannelBtnPressedSlt() {
  int max = 255;
  if (SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                         ->struCaParams->sensorType[n_view]) {
    max = 7;
  }
  myInputPanel *inputPanel =
      new myInputPanel(intType, 0, max, oddChannelBtn->text().toDouble(),
                       oddChannelLbl->text(), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int tmp = channelTabBar->currentIndex();
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->infraBias[n_view][tmp] = inputPanel->getValue();

    oddChannelBtn->setText(QString("%1").arg(inputPanel->getValue()));
    gFlow.setInfBiasingParams(struGsh.nAnalyzeIndex, n_view);
  }
}

void cameraSignalPage::onWaveBoxChangedSlt(int index) {
  struGsh.nWaveIndex = index;
}

/*!
 * \brief 背景阈值
 */
void cameraSignalPage::onIM01ThreadPressedSlt() {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 255, threadBtn->text().toDouble(),
      QString("%1").arg(myLan.background_threshold), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->IM01_back_thread[n_view] = inputPanel->getValue();

    threadBtn->setText(QString("%1").arg(inputPanel->getValue()));
    gFlow.setBackgroundSensorIM01(struGsh.nAnalyzeIndex, n_view);
  }
}

void cameraSignalPage::onIM01LimitPressedSlt() {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 255, upLimitBtn->text().toDouble(),
      QString("%1").arg(myLan.background_uplimit), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->IM01_back_limit[n_view] = inputPanel->getValue();

    upLimitBtn->setText(QString("%1").arg(inputPanel->getValue()));
    gFlow.setBackgroundSensorIM01(struGsh.nAnalyzeIndex, n_view);
  }
}

//! 切换方案时，更新页面-增益、灯光
void cameraSignalPage::onChangeScheme() {
  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

  for (int i = 0; i < COLOR_TYPE_NUM; i++) {
    fastAdjustNum[i]->setText(
        QString("%1").arg(pProfile->nGainFast[n_view][i]));
    slowAdjustNum[i]->setText(
        QString("%1").arg(pProfile->nGainSlow[n_view][i]));
  }

  for (int i = 0; i < LAMP_TYPE; i++) {
    lampValuetBtn[i]->setText(QString("%1").arg(pProfile->nLampValue[i]));
    lampLbl[i]->setText(QString("%1").arg(pProfile->nLampName[i]));
    m_lampEn[i]->setChecked(pProfile->bEnlamp[i]);
  }

  for (int i = 0; i < PLUSE_TYPE; i++) {
    pluseValuetBtn[i]->setText(QString("%1").arg(pProfile->nPluseValue[i]));
    pluseLbl[i]->setText(QString("%1").arg(pProfile->nPluseName[i]));
    m_pluseEn[i]->setChecked(pProfile->bEnPluse[i]);
  }

  hologenSwitchBtn->setChecked(pProfile->bHologenLampOn);
}

/*!
 * \brief 背景序号
 * \param nIdx
 */
void cameraSignalPage::onbackGroundIdxSlt(int nIdx) {
  struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
      ->struCaParams->nBackgdIdx[struGsh.nUnit] = nIdx;
  gFlow.sendSocBackConditon();
}

/*!
 * \brief 背景值
 */
void cameraSignalPage::onbackGroundValueSlt() {
  myInputPanel *inputPanel = new myInputPanel(
      intType, 0, 255, m_bckle->text().toInt(), m_bckle->text(), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int n_sample = inputPanel->getValue();
    m_bckle->setText(QString("%1").arg(n_sample));
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->nBackgdValue[struGsh.nUnit] = n_sample;
    gFlow.sendSocBackConditon();
  }
}
/*!
 * \brief 背景比例
 * \param nIdx
 */
void cameraSignalPage::onbackGroundRatioSlt() {
  myInputPanel *inputPanel =
      new myInputPanel(intType, 0, 100, m_bckratiole->text().toInt(),
                       m_bckratiole->text(), g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    int n_sample = inputPanel->getValue();
    m_bckratiole->setText(QString("%1%2").arg(n_sample).arg("%"));
    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
        ->struCaParams->nBackgdRatio[struGsh.nUnit] = n_sample;
    gFlow.sendSocBackConditon();
  }
}

/*!
 * \brief 刷新传感器温度
 * \param tmp
 */
void cameraSignalPage::onIM01SensorTempure(int tmp) {
  //    float tempture=0.0;
  //    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[n_view]
  //    ==SENSOR_IM01){
  //        tempture =(tmp - 542.756)/3.1574;
  //    }else{
  //        tempture =(577.78176 - tmp)/1.51552;
  //    }
  tempurelb[n_view]->setText(QString::number(struGsh.nInfraTempure, 'f', 1));
}

void cameraSignalPage::onApplyMachineSlt() {
  if (n_wave_type == WAVE_ORI) {
    n_wave_type = WAVE_ADJUST;
    refreshWaveBtnState();
  }
}

void cameraSignalPage::onCaptureBtnPressedSlt() {
  if (n_wave_type != WAVE_ADJUST) {
    for (int i = 0; i < struCnfg.analyzeNum; i++) {
      for (int j = 0; j < gFlow.getMachineCamNum(); j++)
        gFlow.sendCamSignalMode(i, j, 1);
    }
  }

  g_curveDataThread->stopWave();

  m_pageGetImage->refreshPageDisplay();
  m_stacklaytopPage->setCurrentIndex(1);
}

void cameraSignalPage::onEjectorDiveBtnPressedSlt(int nIdx) {
  int nmax = EJ_NUM;
  if (nIdx % 2 != 0) {
    nmax = 2048;
  }

  myInputPanel *inputPanel =
      new myInputPanel(intType, 1, nmax, 1, "", g_mainWidget);
  int nRet = inputPanel->exec();
  if (nRet == QDialog::Accepted) {
    auto *p = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams;
    int nNewVal = (int)inputPanel->getValue();

    //! 用当前值作为基准，将要修改的那个字段替换为新值后进行校验
    int nEjBegin = p->nEjectorBegin[0];
    int nEjEnd = p->nEjectorEnd[0];
    int nEjPxBegin = p->nEjectorBeginPiexl[0];
    int nEjPxEnd = p->nEjectorEndPiexl[0];
    int nPixBegin = p->nPixelBegin[n_view];
    int nPixEnd = p->nPixelEnd[n_view];

    //! 先将新值赋给对应的临时变量
    if (nIdx == 0)
      nEjBegin = nNewVal;
    else if (nIdx == 1)
      nEjPxBegin = nNewVal;
    else if (nIdx == 2)
      nEjEnd = nNewVal;
    else if (nIdx == 3)
      nEjPxEnd = nNewVal;

    //! 校验：起始阀 < 终止阀
    //! 校验：起始阀像元 < 终止阀像元
    //! 校验：起始阀像元 <= nPixelBegin
    //! 校验：终止阀像元 >= nPixelEnd
    bool bValid = (nEjBegin < nEjEnd) && (nEjPxBegin < nEjPxEnd) &&
                  (nEjPxBegin <= nPixBegin) && (nEjPxEnd >= nPixEnd);

    if (bValid) {
      //! 校验通过，设置参数并更新UI
      if (nIdx == 0) {
        p->nEjectorBegin[0] = nNewVal;
        diveEjectorBtn[0]->setText(QString("%1").arg(nNewVal));
      } else if (nIdx == 1) {
        p->nEjectorBeginPiexl[0] = nNewVal;
        diveEjectorPiexlBtn[0]->setText(QString("%1").arg(nNewVal));
      } else if (nIdx == 2) {
        p->nEjectorEnd[0] = nNewVal;
        diveEjectorBtn[1]->setText(QString("%1").arg(nNewVal));
      } else if (nIdx == 3) {
        p->nEjectorEndPiexl[0] = nNewVal;
        diveEjectorPiexlBtn[1]->setText(QString("%1").arg(nNewVal));
      }

      gFlow.ResetPxOfEj(n_view);
    } else {
      //! 校验失败，弹出提示
      infoWidget->raise();
      infoWidget->show();
    }
  }
}

/**
 * @brief cameraSignalPage::刷新波形
 */
void cameraSignalPage::refreshWaveBtnState() {
  QString strPressed = QString("color:white;background-color:#007454;");
  QString strUnPressed = QString("background-color:white;");
  waveBtn[n_wave_type]->setStyleSheet(strPressed);
  paramsGBox->setEnabled(true);
  if (n_wave_type == WAVE_ORI) {
    waveBtn[WAVE_ADJUST]->setStyleSheet(strUnPressed);
    waveBtn[WAVE_SIGNAL]->setStyleSheet(strUnPressed);
    gFlow.sendCamSignalMode(struGsh.nAnalyzeIndex, n_view, 0);
  } else if (n_wave_type == WAVE_ADJUST) {
    waveBtn[WAVE_ORI]->setStyleSheet(strUnPressed);
    waveBtn[WAVE_SIGNAL]->setStyleSheet(strUnPressed);
    gFlow.sendCamSignalMode(struGsh.nAnalyzeIndex, n_view, 1);
    paramsGBox->setEnabled(false);
  } else {
    waveBtn[WAVE_ORI]->setStyleSheet(strUnPressed);
    waveBtn[WAVE_ADJUST]->setStyleSheet(strUnPressed);
    paramsGBox->setEnabled(false);
  }
}

/**
 * @brief cameraSignalPage::刷新前后视按钮显示
 */
void cameraSignalPage::refreshViewBtnState() {
  m_bManualChange = false;
  //    m_resoltionBox->setCurrentIndex(gFlow.bHighResolution(struGsh.nAnalyzeIndex)
  //    ? 1 : 0);

  QString strPressed = QString("color:white;background-color:#007454;");
  QString strUnPressed = QString("background-color:white;");

  for (int i = 0; i < MAX_CAM_NUM; i++) {
    if (i == n_view - FRONT_VIEW)
      m_camBtn[i]->setStyleSheet(strPressed);
    else
      m_camBtn[i]->setStyleSheet(strUnPressed);
  }

  m_bManualChange = true;
}

/**
 * @brief cameraSignalPage::刷新频率显示
 */
void cameraSignalPage::refreshFreValue() {
  for (int i = 0; i < MAX_CAM_NUM; i++) {
    if (i < gFlow.getMachineCamNum()) {
      freValueLbl[i]->show(); //! 传感器行频
      freValueBtn[i]->show();
      sensorTypeLbl[i]->show(); //! 传感器类型
      sensorTypeCombox[i]->show();
      freValueBtn[i]->setText(
          QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                ->struCaParams->nFreQuence[i]));
      sensorTypeCombox[i]->setCurrentIndex(
          struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
              ->struCaParams->sensorType[i]);

      if (gFlow.bIsIMSerialCam(i)) {
        exposeValueLbl[i]->show();
        exposeValueBtn[i]->show(); //! 曝光值
        NoneexposeValueLbl[i]->show();
        NoneexposeValueBtn[i]->show(); //! 非曝光
        // sensorWaveComb[i]->show();
        wavesLabels[i]->show(); //! 波段
        exportWaveBtn[i]->show();
        tempurebtn[i]->show(); //! 温度
        tempurelb[i]->show();

        exposeValueBtn[i]->setText(
            QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                  ->struCaParams->exposeValue[i]));
        NoneexposeValueBtn[i]->setText(
            QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                  ->struCaParams->NoneexposeValue[i]));
        sensorWaveComb[i]->setCurrentIndex(
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->struCaParams->nWaveSeg[i]);

        threadBtn->setText(
            QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                ->struCaParams->IM01_back_thread[n_view]));
        upLimitBtn->setText(
            QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                ->struCaParams->IM01_back_limit[n_view]));

        int nmaxwave = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                   ->struCaParams->nWaveSeg[i] == 0
                           ? 6
                           : 12;
        QString str;
        for (int m = 0; m < nmaxwave; m++) {
          str += QString::number(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                     ->analyzeProfile.n12WavesVal[i][m]);

          str += " ";
          // if(m == 5){
          //     str += "\n";
          // }
        }
        wavesLabels[i]->setText(str); //! 波段
      } else {
        exposeValueLbl[i]->hide();
        exposeValueBtn[i]->hide(); //! 曝光值
        NoneexposeValueLbl[i]->hide();
        NoneexposeValueBtn[i]->hide(); //! 非曝光
        sensorWaveComb[i]->hide();
        wavesLabels[i]->hide(); //! 波段
        exportWaveBtn[i]->hide();
        tempurebtn[i]->hide(); //! 温度
        tempurelb[i]->hide();
      }
    } else {
      freValueLbl[i]->hide();
      freValueBtn[i]->hide(); //! 行频
      sensorTypeLbl[i]->hide();
      sensorTypeCombox[i]->hide(); //! 传感器类型
      exposeValueLbl[i]->hide();
      exposeValueBtn[i]->hide(); //! 曝光值
      NoneexposeValueLbl[i]->hide();
      NoneexposeValueBtn[i]->hide(); //! 非曝光
      sensorWaveComb[i]->hide();
      wavesLabels[i]->hide(); //! 波段
      exportWaveBtn[i]->hide();
      tempurebtn[i]->hide(); //! 温度
      tempurelb[i]->hide();
    }
  }

  if (gFlow.bIsIMSerialCam(struGsh.nUnit)) {
    threadLbl->show();
    threadBtn->show(); //! 背景阈值和上限
    upLimitBtn->show();
    upLimitLbl->show();
  } else {
    threadLbl->hide();
    threadBtn->hide(); //! 背景阈值和上限
    upLimitBtn->hide();
    upLimitLbl->hide();
  }

  for (int i = 0; i < MAX_CAM_NUM; i++) { //! 传感器groupbox
    if (struGsh.nUnit == i) {
      sensorGBox[i]->show();
    } else {
      sensorGBox[i]->hide();
    }
  }

  refreshBackGroundParams(); //! 刷新背景条件
}

/**
 * @brief cameraSignalPage::刷新校正目标
 */
void cameraSignalPage::refreshAdjustValue() {
  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);

  if (gFlow.bIsIMSerialCam(n_view)) {
    for (int i = 0; i < COLOR_TYPE_NUM; i++) {
      if (i > 0) {
        adjustTargetTypeLbl[i]->hide(); //! 校正值
        adjustTargetTypetBtn[i]->hide();
      }
    }

    darkAdjustBtn->show(); //! 暗校正

    QStringList gainList, tmpLst; //! 增益调节
    tmpLst << QString("%1A").arg(myLan.channel)
           << QString("%1B").arg(myLan.channel)
           << QString("%1C").arg(myLan.channel)
           << QString("%1D").arg(myLan.channel);
    gainList << gain_adjust << myLan.analog_gain << myLan.digital_gain;
    for (int i = 0; i < ADJUST_TYPE; i++) {
      adjustTypeLbl[i]->setText(gainList.at(i));
    }

    for (int i = 0; i < COLOR_TYPE_NUM; i++) {
      colorTypeBtn[i]->setText(tmpLst[i]);
      if (SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                             ->struCaParams->sensorType[n_view]) {
        colorTypeLbl[i]->show();
        colorTypeBtn[i]->show();
        fastAdjustNum[i]->show();
        slowAdjustNum[i]->show();
      } else {
        if (i < 1) {
          colorTypeLbl[i]->show();
          colorTypeBtn[i]->show();
          fastAdjustNum[i]->show();
          slowAdjustNum[i]->show();
        } else {
          colorTypeLbl[i]->hide();
          colorTypeBtn[i]->hide();
          fastAdjustNum[i]->hide();
          slowAdjustNum[i]->hide();
        }
      }
    }

    //! 红外偏置
    for (int i = 0; i < COLOR_TYPE_NUM; i++) {
      if (SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                             ->struCaParams->sensorType[n_view]) {
        channelTabBar->setTabEnabled(i, true);
      } else {
        if (i < 1) {
          channelTabBar->setTabEnabled(i, true);
        } else {
          channelTabBar->setTabEnabled(i, false);
        }
      }
    }

    threadLbl->show();
    threadBtn->show();

    upLimitLbl->show();
    upLimitBtn->show();

    rangeLbl->hide();
    rangeValueBtn->hide();
  } else {
    for (int i = 0; i < COLOR_TYPE_NUM; i++) { //! 校正值
      adjustTargetTypeLbl[i]->show();
      adjustTargetTypetBtn[i]->show();
    }

    darkAdjustBtn->hide();

    QStringList colorNameList;
    colorNameList << "R" << "G" << "B" << "N";
    for (int i = 0; i < COLOR_TYPE_NUM; i++) {
      colorTypeBtn[i]->setText(colorNameList[i]);
      colorTypeLbl[i]->show();
      colorTypeBtn[i]->show();
      fastAdjustNum[i]->show();
      slowAdjustNum[i]->show();
    }

    if (pAnalyzeGlobal->struCaParams->sensorType[n_view] == SENSOR_VL05 ||
        pAnalyzeGlobal->struCaParams->sensorType[n_view] == SENSOR_VL08) {
      adjustTargetTypeLbl[COLOR_TYPE_N]->hide();
      adjustTargetTypetBtn[COLOR_TYPE_N]->hide();
      colorTypeBtn[COLOR_TYPE_N]->hide();
      colorTypeLbl[COLOR_TYPE_N]->hide();
      colorTypeBtn[COLOR_TYPE_N]->hide();
      fastAdjustNum[COLOR_TYPE_N]->hide();
      slowAdjustNum[COLOR_TYPE_N]->hide();
    }

    QStringList gainList;
    gainList << gain_adjust << fast_adjust << slow_adjust;
    for (int i = 0; i < ADJUST_TYPE; i++) {
      adjustTypeLbl[i]->setText(gainList.at(i));
    }

    threadLbl->hide();
    threadBtn->hide();

    upLimitLbl->hide();
    upLimitBtn->hide();

    rangeLbl->show();
    rangeValueBtn->show();
  }

  for (int i = 0; i < COLOR_TYPE_NUM; i++) {
    adjustTargetTypetBtn[i]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nAdjustTarget[n_view][i]));
    fastAdjustNum[i]->setText(
        QString("%1").arg(pProfile->nGainFast[n_view][i]));
    slowAdjustNum[i]->setText(
        QString("%1").arg(pProfile->nGainSlow[n_view][i]));
  }
}

/**
 * @brief cameraSignalPage::刷新波动范围
 */
void cameraSignalPage::refreshThreadValue() {
  rangeValueBtn->setText(
      QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                            ->struCaParams->nThreadValue[n_view]));
}

/**
 * @brief cameraSignalPage::刷新起始终止像元
 */
void cameraSignalPage::refreshPixelValue() {
  beginChanelBtn[PIXEL_BEGIN]->setText(
      QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                            ->struCaParams->nPixelBegin[n_view]));
  beginChanelBtn[PIXEL_END]->setText(
      QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                            ->struCaParams->nPixelEnd[n_view]));
}

void cameraSignalPage::refreshEjectorValue() {
  if (struCnfg.nMachineType == nMachine_Crop &&
      struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnVal && n_view == 0) {
    for (int i = 0; i < 3; i++) {
      ejectorSetTitleLbl[i]->show();
    }

    for (int i = 0; i < 2; i++) {
      diveEjectorLb[i]->show();
      diveEjectorBtn[i]->show();
      diveEjectorPiexlBtn[i]->show();

      if (n_view == 0) {
        diveEjectorBtn[i]->setDisabled(false);
        diveEjectorPiexlBtn[i]->setDisabled(false);
      } else {
        diveEjectorBtn[i]->setDisabled(true);
        diveEjectorPiexlBtn[i]->setDisabled(true);
      }
    }

    diveEjectorBtn[0]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nEjectorBegin[0]));
    diveEjectorBtn[1]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nEjectorEnd[0]));
    diveEjectorPiexlBtn[0]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nEjectorBeginPiexl[0]));
    diveEjectorPiexlBtn[1]->setText(
        QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->struCaParams->nEjectorEndPiexl[0]));
  } else {
    for (int i = 0; i < 3; i++) {
      ejectorSetTitleLbl[i]->hide();
    }

    for (int i = 0; i < 2; i++) {
      diveEjectorLb[i]->hide();
      diveEjectorBtn[i]->hide();
      diveEjectorPiexlBtn[i]->hide();
    }
  }
}

//! 红外偏置设置
void cameraSignalPage::refreshInfraBiasValue() {
  if (gFlow.bIsIMSerialCam(n_view)) {
    infraBiasWidget->show();
  } else {
    infraBiasWidget->hide();
  }
  int tmp = channelTabBar->currentIndex();
  oddChannelBtn->setText(
      QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                            ->struCaParams->infraBias[n_view][tmp]));
}

/*!
 * \brief 刷新背景条件参数
 */
void cameraSignalPage::refreshBackGroundParams() {
  if (gFlow.bIsIMSerialCam(n_view)) {
    m_bckratiole->hide();
    m_bckratiolb->hide();
    m_bckthdlb->hide();
    m_bckle->hide();
  } else {
    m_bckratiole->show();
    m_bckratiolb->show();
    m_bckthdlb->show();
    m_bckle->show();
  }

  m_bckratiole->setText(
      QString("%1%2")
          .arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                   ->struCaParams->nBackgdRatio[struGsh.nUnit])
          .arg("%"));

  m_bckle->setText(
      QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                            ->struCaParams->nBackgdValue[struGsh.nUnit]));

  m_bckcomb->setCurrentIndex(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                 ->struCaParams->nBackgdIdx[struGsh.nUnit]);
}

/**
 * @brief cameraSignalPage::获取校正波形
 */
void cameraSignalPage::getAdjustSignal() {
  infoWidget->setLabelText(myLan.apply_and_wait);
  infoWidget->show();
  if (g_curveDataThread->getRunningState()) {
    g_curveDataThread->stopWave();
    gFlow.sleep(1);
  }

  char mode = 0x02;

  gFlow.sendImgWaveReq(struGsh.nAnalyzeIndex, n_view, 0, mode, 0);

  gFlow.msleep(300);

  myAxis->update();

  infoWidget->hide();
}

/*!
 * \brief 刷新相机参数
 */
void cameraSignalPage::refreshCamParam() {
  refreshWaveBtnState();
  refreshViewBtnState();
  refreshAdjustValue();
  refreshThreadValue();
  refreshPixelValue();
  refreshEjectorValue();
  refreshInfraBiasValue();
  myAxis->update();
}

/*!
 * \brief 刷新灯光控制
 */
void cameraSignalPage::refreshLightParams() {
  analyzeGlobal *pAnalyzeGlobal =
      gFlow.getSpecifyAnalyzeGlobal(struGsh.nAnalyzeIndex);
  struProfile *pProfile = &(pAnalyzeGlobal->analyzeProfile);
  for (int i = 0; i < LAMP_TYPE; i++) {
    lampValuetBtn[i]->setText(QString("%1").arg(pProfile->nLampValue[i]));
    lampLbl[i]->setText(QString("%1").arg(pProfile->nLampName[i]));
    m_lampEn[i]->setChecked(pProfile->bEnlamp[i]);
  }
  for (int i = 0; i < PLUSE_TYPE; i++) {
    pluseValuetBtn[i]->setText(QString("%1").arg(pProfile->nPluseValue[i]));
    pluseLbl[i]->setText(QString("%1").arg(pProfile->nPluseName[i]));
    m_pluseEn[i]->setChecked(pProfile->bEnPluse[i]);

    if (struCnfg.nMachineType == nMachine_Bottle) {
      pluseValuetBtn[i]->hide();
      pluseLbl[i]->hide();
      m_pluseEn[i]->hide();
    } else {
      pluseValuetBtn[i]->show();
      pluseLbl[i]->show();
      m_pluseEn[i]->show();
    }
  }

  if (pAnalyzeGlobal->bHasAssist) {
    hologenSwitchBtn->setChecked(pProfile->bHologenLampOn);
    hologenSwitchBtn->show();
    halogenLampLbl->show();
  } else {
    hologenSwitchBtn->hide();
    halogenLampLbl->hide();
  }
}
