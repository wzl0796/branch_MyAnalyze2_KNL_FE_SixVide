#include "traditionarith.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QStringList>
#include <QtMath>
#include <QVBoxLayout>
#include <QWidget>

#include "common/mycheckbox.h"
#include "common/myinputpanel.h"
#include "common/mylineedit.h"
#include "core/globalflow/globalflow.h"
#include "params/define.h"
#include "params/globalparams.h"

namespace {
const int kAlgorithmIds[] = {
    ARITH_GREY_A,
    ARITH_GREY_B,
    ARITH_DISCOLOR_A,
    ARITH_DISCOLOR_B
};

QString selectedButtonStyle()
{
    return QStringLiteral("QPushButton { background-color: #007454; color: white; }");
}

QString normalButtonStyle()
{
    return QStringLiteral("QPushButton { background-color: white; color: #333333; }");
}

QString algorithmDisplayName(int index)
{
    switch (index) {
    case 0: return myLan.gray_a;
    case 1: return myLan.gray_b;
    case 2: return myLan.discolor_a;
    case 3: return myLan.discolor_b;
    default: return QString();
    }
}

QStringList algorithmModeItems(int index)
{
    if (index < 2) {
        return QStringList() << myLan.red << myLan.green << myLan.blue;
    }
    return QStringList() << (myLan.red + "/" + myLan.green)
                         << (myLan.red + "/" + myLan.blue)
                         << (myLan.green + "/" + myLan.blue)
                         << myLan.nr << myLan.ng << myLan.nb;
}

QStringList retainModeItems(int algorithmIndex, int retainIndex)
{
    if (algorithmIndex < 2) {
        return QStringList() << myLan.red << myLan.green << myLan.blue;
    }
    if (retainIndex == 0) {
        return QStringList() << (myLan.red + "/" + myLan.green)
                             << (myLan.red + "/" + myLan.blue)
                             << (myLan.green + "/" + myLan.blue)
                             << myLan.nr << myLan.ng << myLan.nb;
    }
    return QStringList() << myLan.red << myLan.green << myLan.blue << QStringLiteral("N");
}
}

traditionArith::traditionArith(QWidget *parent)
    : QWidget(parent),
      m_labelCameraIndex(0),
      m_cameraIndex(0),
      m_selectedAlgorithm(0),
      m_nameLabel(nullptr),
      m_nameTitleLabel(nullptr),
      m_sensitivityTitleLabel(nullptr),
      m_modeTitleLabel(nullptr),
      m_rowCountTitleLabel(nullptr),
      m_badPointTitleLabel(nullptr),
      m_passTitleLabel(nullptr),
      m_lowerLimitTitleLabel(nullptr),
      m_sensitivityEdit(nullptr),
      m_modeCombo(nullptr),
      m_brightButton(nullptr),
      m_darkButton(nullptr),
      m_rowCountEdit(nullptr),
      m_badPointEdit(nullptr),
      m_passOneButton(nullptr),
      m_passTwoButton(nullptr),
      m_cutEdgeCheck(nullptr),
      m_independentDelayCheck(nullptr),
      m_lowerLimitEdit(nullptr),
      m_parameterBox(nullptr),
      m_retainBox(nullptr),
      m_algorithmGroup(nullptr),
      m_lightGroup(nullptr),
      m_passGroup(nullptr)
{
    cratePage();
    layoutPage();
    connectAndSlt();
    refreshPage();
}

void traditionArith::cratePage()
{
    m_tabBar = new QTabBar();
    m_tabBar->addTab(myLan.lab_set);
    m_tabBar->addTab(myLan.params_set);
    m_tabBar->setExpanding(false);

    m_tabBar->setStyleSheet(
        "QTabBar::tab {min-width: 150px; padding: "
        "8px;min-height:45px;margin-right:5px }"
        "QTabBar::tab:selected { background-color: #007454; color: white; }"
        "QTabBar::tab:!selected { background-color: #f0f0f0; color: #333333; }");


    m_stackwid = new QStackedWidget();

    //! 标签设置  设置标签是否使能传统算法
    m_labsetPage = new QWidget();
    m_stackwid->addWidget(m_labsetPage);

    m_labsetBox = new QGroupBox();
    m_labsetBox->setFixedSize(1570, 780);
    m_labelCameraButton = new QPushButton();
    m_labelCameraButton->setFixedSize(BTN_WID + 60, BTN_HEI);
    m_labelCameraButton->setIconSize(QSize(50, 50));
    m_labelCameraButton->setIcon(QIcon(":/res/png/Action_Camera.png"));
    m_labelCameraButton->hide();
    m_labelScrollArea = new QScrollArea();
    m_labelScrollArea->setWidgetResizable(true);
    for (int i = 0; i < MAX_LABELS; ++i) {
        m_labelIds[i] = -1;
        m_labelName[i] = new QLabel();
        m_labelTraditionEnable[i] = new MyCheckBox(QString());
        m_labelTraditionEnable[i]->setCheckBtnSize(34, 34);
        m_labelTraditionAlgorithm[i] = new QComboBox();
        m_labelTraditionAlgorithm[i]->setMinimumHeight(BTN_HEI);
        m_labelTraditionAlgorithm[i]->addItems(
            QStringList() << myLan.gray_a << myLan.gray_b
                          << myLan.discolor_a << myLan.discolor_b);
    }


    //! 传统参数
    m_traditionParamsPage = new QWidget();
    m_stackwid->addWidget(m_traditionParamsPage);

    m_algorithmGroup = new QButtonGroup(this);
    m_algorithmGroup->setExclusive(true);
    QStringList namelst;
    namelst<<myLan.gray_a<<myLan.gray_b<<myLan.discolor_a<<myLan.discolor_b;
    for (int i = 0; i < ALGORITHM_COUNT; ++i) {
        m_algorithmEnable[i] = new MyCheckBox(QString());
        m_algorithmEnable[i]->setCheckBtnSize(34, 34);
        m_algorithmSelect[i] = new QPushButton();
        m_algorithmSelect[i]->setCheckable(true);
        m_algorithmSelect[i]->setMinimumSize(120, 48);
        m_algorithmSelect[i]->setText(namelst.at(i));
        m_algorithmGroup->addButton(m_algorithmSelect[i], i);
    }

    m_nameLabel = new QLabel();
    m_nameTitleLabel = new QLabel(myLan.name);
    m_sensitivityTitleLabel = new QLabel(myLan.sensitivity);
    m_modeTitleLabel = new QLabel(myLan.mode);
    m_rowCountTitleLabel = new QLabel(myLan.row_count);
    m_badPointTitleLabel = new QLabel(myLan.bad_point);
    m_passTitleLabel = new QLabel(myLan.infection);
    m_lowerLimitTitleLabel = new QLabel(myLan.lower_limit_thread);
    m_sensitivityEdit = new MyLineEdit();
    m_sensitivityEdit->setFixedWidth(130);
    m_modeCombo = new QComboBox();
    m_modeCombo->setMinimumHeight(BTN_HEI);
    m_brightButton = new QPushButton(myLan.bright);
    m_darkButton = new QPushButton(myLan.dark);
    m_lightGroup = new QButtonGroup(this);
    m_lightGroup->setExclusive(true);
    m_brightButton->setCheckable(true);
    m_darkButton->setCheckable(true);
    m_lightGroup->addButton(m_brightButton, 0);
    m_lightGroup->addButton(m_darkButton, 1);

    m_rowCountEdit = new MyLineEdit();
    m_rowCountEdit->setFixedWidth(100);
    m_badPointEdit = new MyLineEdit();
    m_badPointEdit->setFixedWidth(120);
    m_passOneButton = new QPushButton(myLan.infection_1);
    m_passTwoButton = new QPushButton(myLan.infection_2);
    m_passGroup = new QButtonGroup(this);
    m_passGroup->setExclusive(true);
    m_passOneButton->setCheckable(true);
    m_passTwoButton->setCheckable(true);
    m_passGroup->addButton(m_passOneButton, 0);
    m_passGroup->addButton(m_passTwoButton, 1);
    m_cutEdgeCheck = new MyCheckBox(myLan.cut_edge);
    m_independentDelayCheck = new MyCheckBox(myLan.independent_delay);
    m_lowerLimitEdit = new MyLineEdit();
    m_lowerLimitEdit->setFixedWidth(100);

    m_retainBox = new QGroupBox(myLan.more_retain);
    m_retainBox->setFixedHeight(780);
    m_retainBox->setEnabled(false);
    for (int i = 0; i < 2; ++i) {
        m_retainModeCombo[i] = new QComboBox();
        m_retainModeCombo[i]->setMinimumHeight(BTN_HEI);
        m_retainLightCombo[i] = new QComboBox();
        m_retainLightCombo[i]->setMinimumHeight(BTN_HEI);
        m_retainLightCombo[i]->addItems(QStringList() << myLan.bright << myLan.dark);
        m_retainSensitivityEdit[i] = new MyLineEdit();
        m_retainSensitivityEdit[i]->setFixedWidth(120);
    }
}

void traditionArith::layoutPage()
{
    //! 标签设置
    auto *labelGrid = new QGridLayout();
    labelGrid->setHorizontalSpacing(24);
    labelGrid->setVerticalSpacing(12);
    labelGrid->addWidget(new QLabel(myLan.name), 0, 0);
    labelGrid->addWidget(new QLabel(myLan.algorithm_enable), 0, 1);
    labelGrid->addWidget(new QLabel(myLan.tradition), 0, 2);
    for (int i = 0; i < MAX_LABELS; ++i) {
        labelGrid->addWidget(m_labelName[i], i + 1, 0);
        labelGrid->addWidget(m_labelTraditionEnable[i], i + 1, 1, Qt::AlignCenter);
        labelGrid->addWidget(m_labelTraditionAlgorithm[i], i + 1, 2);
        m_labelName[i]->hide();
        m_labelTraditionEnable[i]->hide();
        m_labelTraditionAlgorithm[i]->hide();
    }
    labelGrid->setColumnStretch(0, 2);
    labelGrid->setColumnStretch(1, 1);
    labelGrid->setColumnStretch(2, 2);

    auto *labelContent = new QWidget();
    labelContent->setLayout(labelGrid);
    m_labelScrollArea->setWidget(labelContent);

    auto *labelBoxLayout = new QVBoxLayout(m_labsetBox);
    labelBoxLayout->addWidget(m_labelScrollArea);
    labelBoxLayout->addStretch();


    auto *labelHeaderLayout = new QHBoxLayout();
    labelHeaderLayout->addWidget(m_labelCameraButton);
    labelHeaderLayout->addStretch();

    auto *labelPageLayout = new QVBoxLayout(m_labsetPage);
    labelPageLayout->addLayout(labelHeaderLayout);
    labelPageLayout->addWidget(m_labsetBox);
    labelPageLayout->addStretch();

    //! 传统参数
    auto *algorithmBox = new QGroupBox(myLan.algorithm_enable);
    algorithmBox->setFixedHeight(780);
    auto *algorithmGrid = new QGridLayout(algorithmBox);
    algorithmGrid->setContentsMargins(5, 80, 5, 5);
    algorithmGrid->setHorizontalSpacing(18);
    algorithmGrid->setVerticalSpacing(20);
    for (int i = 0; i < ALGORITHM_COUNT; ++i) {
        const int row = i / 2;
        const int column = (i % 2) * 2;
        algorithmGrid->addWidget(m_algorithmEnable[i], row, column);
        algorithmGrid->addWidget(m_algorithmSelect[i], row, column + 1);
    }
    algorithmGrid->setRowStretch(2, 1);

    m_parameterBox = new QGroupBox(myLan.algorithm_params);
    m_parameterBox->setFixedHeight(780);
    m_parameterBox->setEnabled(false);
    auto *form = new QGridLayout(m_parameterBox);
    form->setContentsMargins(5, 80, 5, 5);
    form->setHorizontalSpacing(10);
    form->setVerticalSpacing(20);
    form->addWidget(m_nameTitleLabel, 0, 0);
    form->addWidget(m_nameLabel, 0, 1, 1, 3);
    form->addWidget(m_sensitivityTitleLabel, 1, 0);
    form->addWidget(m_sensitivityEdit, 1, 1);
    form->addWidget(m_modeTitleLabel, 2, 0);
    form->addWidget(m_modeCombo, 2, 1);
    auto *lightLayout = new QHBoxLayout();
    lightLayout->addWidget(m_brightButton);
    lightLayout->addWidget(m_darkButton);
    form->addLayout(lightLayout, 2, 2, 1, 2);

    form->addWidget(m_rowCountTitleLabel, 3, 0);
    form->addWidget(m_rowCountEdit, 3, 1);
    form->addWidget(m_badPointTitleLabel, 3, 2);
    form->addWidget(m_badPointEdit, 3, 3);

    form->addWidget(m_passTitleLabel, 4, 0);
    auto *passLayout = new QHBoxLayout();
    passLayout->addWidget(m_passOneButton);
    passLayout->addWidget(m_passTwoButton);
    form->addLayout(passLayout, 4, 1, 1, 3);

    form->addWidget(m_cutEdgeCheck, 5, 0, 1, 2);
    form->addWidget(m_independentDelayCheck, 6, 0, 1, 2);
    form->addWidget(m_lowerLimitTitleLabel, 7, 0);
    form->addWidget(m_lowerLimitEdit, 7, 1);

    form->setRowStretch(8, 1);
    form->setColumnStretch(3, 1);

    auto *retainLayout = new QVBoxLayout(m_retainBox);
    retainLayout->setContentsMargins(5, 80, 5, 5);
    retainLayout->setSpacing(20);
    for (int i = 0; i < 2; ++i) {
        auto *retainGroup = new QGroupBox(QString("%1 %2").arg(myLan.retain).arg(i + 1));
        auto *retainForm = new QFormLayout(retainGroup);
        retainForm->setContentsMargins(5, 60, 5, 5);
        retainForm->addRow(myLan.mode, m_retainModeCombo[i]);
        retainForm->addRow(myLan.bright + "/" + myLan.dark, m_retainLightCombo[i]);
        retainForm->addRow(myLan.sensitivity, m_retainSensitivityEdit[i]);
        retainLayout->addWidget(retainGroup);
    }
    retainLayout->addStretch();


    auto *contentLayout = new QHBoxLayout(m_traditionParamsPage);
    contentLayout->setContentsMargins(6, 6, 6, 6);
    contentLayout->setSpacing(8);
    contentLayout->addWidget(algorithmBox, 9);
    contentLayout->addWidget(m_parameterBox, 12);
    contentLayout->addWidget(m_retainBox, 10);



    //! 总布局
    QHBoxLayout *hlaytop = new QHBoxLayout();
    hlaytop->addWidget(m_tabBar);
    hlaytop->addStretch();


    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->addLayout(hlaytop);
    mainLayout->addWidget(m_stackwid);
}

void traditionArith::connectAndSlt()
{
    connect(m_tabBar, &QTabBar::currentChanged, this, [this](int index){refreshPage();});

    //! 标签设置
    connect(m_labelCameraButton, &QPushButton::clicked, this, [this]() {
        const int cameraCount = qMin(gFlow.getMachineCamNum(), MAX_CAM_NUM);
        if (cameraCount <= 1) {
            return;
        }
        m_labelCameraIndex = (m_labelCameraIndex + 1) % cameraCount;
        refreshLabelset();
    });

    for (int i = 0; i < MAX_LABELS; ++i) {
        connect(m_labelTraditionEnable[i], &MyCheckBox::toggled, this, [this, i](bool enabled) {
            const int labelId = m_labelIds[i];
            if (labelId < 0 || labelId >= MAX_LABELS) {
                return;
            }
            auto *profile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
            profile->bTraditionLabelEn[m_labelCameraIndex][labelId] = enabled;
            gFlow.sendlaboftraditon2Soc();
        });

        connect(m_labelTraditionAlgorithm[i], QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this, i](int algorithm) {
            const int labelId = m_labelIds[i];
            if (labelId < 0 || labelId >= MAX_LABELS || algorithm < 0 ||
                algorithm >= TRADITION_ARITH_COUNT) {
                return;
            }
            auto *profile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
            profile->nTraditionLabelAlgorithm[m_labelCameraIndex][labelId] = algorithm;

            gFlow.sendlaboftraditon2Soc();
        });
    }


    //! 传统参数
    for (int i = 0; i < ALGORITHM_COUNT; ++i) {
        connect(m_algorithmEnable[i], &MyCheckBox::toggled, this, [this, i](bool enabled) { //! 算法使能
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.traditionArith[i].bEnable = enabled;
            if(i == ARITH_GREY_A || i ==ARITH_GREY_B){
                gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);
            }else if(i == ARITH_DISCOLOR_A || i ==ARITH_DISCOLOR_B){
                gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
            }

            refreshSelectedAlgorithm();
        });

        connect(m_algorithmSelect[i], &QPushButton::clicked, this, [this, i]() {
            m_selectedAlgorithm = i;
            refreshSelectedAlgorithm();
        });
    }

    connect(m_sensitivityEdit, &MyLineEdit::pressed, this, [this]() { //! 灵敏度
        editValue(m_sensitivityEdit, 0.0, 100.0, myLan.sensitivity,
                  [this](double value) {saveSensitivity(value); });
    });

    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) { //! 模式
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].mode = index;
        if(m_selectedAlgorithm == ARITH_GREY_A ||
            m_selectedAlgorithm == ARITH_GREY_B){
            gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

        }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                 m_selectedAlgorithm == ARITH_DISCOLOR_B){
            gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
        }
    });

    connect(m_brightButton, &QPushButton::clicked, this, [this]() {//! 亮暗
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].bright = true;
        refreshSelectedAlgorithm();

        if(m_selectedAlgorithm == ARITH_GREY_A ||
            m_selectedAlgorithm == ARITH_GREY_B){
            gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

        }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                 m_selectedAlgorithm == ARITH_DISCOLOR_B){
            gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
        }
    });

    connect(m_darkButton, &QPushButton::clicked, this, [this]() {//! 亮暗
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].bright = false;
        refreshSelectedAlgorithm();

        if(m_selectedAlgorithm == ARITH_GREY_A ||
            m_selectedAlgorithm == ARITH_GREY_B){
            gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

        }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                 m_selectedAlgorithm == ARITH_DISCOLOR_B){
            gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
        }
    });

    connect(m_rowCountEdit, &MyLineEdit::pressed, this, [this]() {//! 行数
        editValue(m_rowCountEdit, 1, 4096, myLan.row_count,
                  [this](int value) {
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.traditionArith[m_selectedAlgorithm].rowCount = value;

            m_rowCountEdit->setText(QString::number(value));

            if(m_selectedAlgorithm == ARITH_GREY_A ||
                m_selectedAlgorithm == ARITH_GREY_B){
                gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

            }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                     m_selectedAlgorithm == ARITH_DISCOLOR_B){
                gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
            }
        });
    });
    connect(m_badPointEdit, &MyLineEdit::pressed, this, [this]() {//! 病斑
        editValue(m_badPointEdit, 0, 65535, myLan.bad_point,
                  [this](int value) {
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.traditionArith[m_selectedAlgorithm].badPointCount = value;

            m_badPointEdit->setText(QString::number(value));

            if(m_selectedAlgorithm == ARITH_GREY_A ||
                m_selectedAlgorithm == ARITH_GREY_B){
                gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

            }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                     m_selectedAlgorithm == ARITH_DISCOLOR_B){
                gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
            }
        });
    });

    connect(m_passOneButton, &QPushButton::clicked, this, [this]() { //! 传染
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].pass = 0;

        refreshSelectedAlgorithm();

        if(m_selectedAlgorithm == ARITH_GREY_A ||
            m_selectedAlgorithm == ARITH_GREY_B){
            gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

        }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                 m_selectedAlgorithm == ARITH_DISCOLOR_B){
            gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
        }
    });

    connect(m_passTwoButton, &QPushButton::clicked, this, [this]() {//! 传染
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].pass = 1;

        refreshSelectedAlgorithm();

        if(m_selectedAlgorithm == ARITH_GREY_A ||
            m_selectedAlgorithm == ARITH_GREY_B){
            gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

        }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                 m_selectedAlgorithm == ARITH_DISCOLOR_B){
            gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
        }
    });

    connect(m_cutEdgeCheck, &MyCheckBox::toggled, this, [this](bool checked) { //! 切边缘
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].cutEdge = checked;

        if(m_selectedAlgorithm == ARITH_GREY_A ||
            m_selectedAlgorithm == ARITH_GREY_B){
            gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

        }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                 m_selectedAlgorithm == ARITH_DISCOLOR_B){
            gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
        }
    });

    connect(m_independentDelayCheck, &MyCheckBox::toggled, this, [this](bool checked) {
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
            ->analyzeProfile.traditionArith[m_selectedAlgorithm].independentDelay = checked;
    });

    connect(m_lowerLimitEdit, &MyLineEdit::pressed, this, [this]() {
        editValue(m_lowerLimitEdit, 0, 65535, myLan.lower_limit_thread,
                  [this](int value) {
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.traditionArith[m_selectedAlgorithm].lowerLimit = value;
            m_lowerLimitEdit->setText(QString::number(value));
        });
    });

    for (int i = 0; i < 2; ++i) {//! 保留
        connect(m_retainModeCombo[i], QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this, i](int index) {//! 模式
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.traditionArith[m_selectedAlgorithm].retain[i].mode = index;

            if(m_selectedAlgorithm == ARITH_GREY_A ||
                m_selectedAlgorithm == ARITH_GREY_B){
                gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

            }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                     m_selectedAlgorithm == ARITH_DISCOLOR_B){
                gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
            }
        });

        connect(m_retainLightCombo[i], QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this, i](int index) {//! 亮暗
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                ->analyzeProfile.traditionArith[m_selectedAlgorithm].retain[i].bright = index == 0;
            if(m_selectedAlgorithm == ARITH_GREY_A ||
                m_selectedAlgorithm == ARITH_GREY_B){
                gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

            }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                     m_selectedAlgorithm == ARITH_DISCOLOR_B){
                gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
            }
        });

        connect(m_retainSensitivityEdit[i], &MyLineEdit::pressed, this, [this, i]() {
            editValue(m_retainSensitivityEdit[i], 0.0, 100.0, myLan.sensitivity,
                      [this, i](double value) {
                
                const int decimalPlaces = i == 0 ? 2 : 1;
                m_retainSensitivityEdit[i]->setText(QStringLiteral("%1%").arg(value));

                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                    ->analyzeProfile.traditionArith[m_selectedAlgorithm].retain[i].sensitivity = value*pow(10, decimalPlaces);   
                   
                 if(m_selectedAlgorithm == ARITH_GREY_A ||
                    m_selectedAlgorithm == ARITH_GREY_B){
                    gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);
                }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
                        m_selectedAlgorithm == ARITH_DISCOLOR_B){
                    gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
                }    
                    
            }, i == 0 ? 2 : 1);
        });
    }
}

void traditionArith::refreshPage()
{
    if(m_tabBar->currentIndex() == PAGE_LAB_SET){
        refreshLabelset();
        m_stackwid->setCurrentIndex(0);
    }else if(m_tabBar->currentIndex() == PAGE_PARAMS_SET){
        refreshTradition();
        m_stackwid->setCurrentIndex(1);
    }
}


void traditionArith::refreshTradition()
{
    m_cameraIndex = 0;
    const int cameraCount = gFlow.getMachineCamNum();
    if (cameraCount <= 0) {
        m_parameterBox->setEnabled(false);
        m_retainBox->setEnabled(false);
        return;
    }
    m_cameraIndex = qBound(0, m_cameraIndex, qMin(cameraCount, MAX_CAM_NUM) - 1);

    auto *profile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    for (int i = 0; i < ALGORITHM_COUNT; ++i) {
        const QSignalBlocker blocker(m_algorithmEnable[i]);
        m_algorithmEnable[i]->setChecked(
            profile->traditionArith[i].bEnable);
    }
    refreshSelectedAlgorithm();
}

void traditionArith::refreshLabelset()
{
    const int cameraCount = qMin(gFlow.getMachineCamNum(), MAX_CAM_NUM);
    if (cameraCount <= 0) {
        m_labelCameraButton->setEnabled(false);
        for (int i = 0; i < MAX_LABELS; ++i) {
            m_labelIds[i] = -1;
            m_labelName[i]->hide();
            m_labelTraditionEnable[i]->hide();
            m_labelTraditionAlgorithm[i]->hide();
        }
        return;
    }

    m_labelCameraIndex = qBound(0, m_labelCameraIndex, cameraCount - 1);
    m_labelCameraButton->setEnabled(true);
    m_labelCameraButton->setText(
        struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[m_labelCameraIndex]);
    if (m_labelCameraIndex >= struGsh.vlabNamemap.size()) {
        for (int i = 0; i < MAX_LABELS; ++i) {
            m_labelIds[i] = -1;
            m_labelName[i]->hide();
            m_labelTraditionEnable[i]->hide();
            m_labelTraditionAlgorithm[i]->hide();
        }
        return;
    }

    auto *profile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    const QMap<int, QString> &labels = struGsh.vlabNamemap[m_labelCameraIndex];
    int row = 0;
    for (auto it = labels.constBegin(); it != labels.constEnd() && row < MAX_LABELS; ++it, ++row) {
        const int labelId = it.key();
        m_labelIds[row] = labelId;
        m_labelName[row]->setText(it.value());
        m_labelName[row]->show();
        {
            const QSignalBlocker blocker(m_labelTraditionEnable[row]);
            m_labelTraditionEnable[row]->setChecked(
                labelId >= 0 && labelId < MAX_LABELS &&
                profile->bTraditionLabelEn[m_labelCameraIndex][labelId]);
        }
        {
            const QSignalBlocker blocker(m_labelTraditionAlgorithm[row]);
            m_labelTraditionAlgorithm[row]->setCurrentIndex(
                labelId >= 0 && labelId < MAX_LABELS
                    ? qBound(0, profile->nTraditionLabelAlgorithm[m_labelCameraIndex][labelId],
                             TRADITION_ARITH_COUNT - 1)
                    : 0);
        }
        m_labelTraditionEnable[row]->show();
        m_labelTraditionAlgorithm[row]->show();
    }
    for (; row < MAX_LABELS; ++row) {
        m_labelIds[row] = -1;
        m_labelName[row]->hide();
        m_labelTraditionEnable[row]->hide();
        m_labelTraditionAlgorithm[row]->hide();
    }
}

void traditionArith::refreshSelectedAlgorithm()
{
    const int index = qBound(0, m_selectedAlgorithm, ALGORITHM_COUNT - 1);
    m_selectedAlgorithm = index;
    for (int i = 0; i < ALGORITHM_COUNT; ++i) {
        m_algorithmSelect[i]->setChecked(i == index);
        m_algorithmSelect[i]->setStyleSheet(i == index ? selectedButtonStyle() : normalButtonStyle());
    }

    m_nameLabel->setText(algorithmDisplayName(index));
    const auto &options = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                              ->analyzeProfile.traditionArith[index];
    const bool algorithmEnabled = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                      ->analyzeProfile.traditionArith[m_selectedAlgorithm].bEnable;
    m_parameterBox->setEnabled(algorithmEnabled);
    m_retainBox->setEnabled(algorithmEnabled);
    {
        const QSignalBlocker blocker(m_modeCombo);
        m_modeCombo->clear();
        m_modeCombo->addItems(algorithmModeItems(index));
        m_modeCombo->setCurrentIndex(qBound(0, options.mode, m_modeCombo->count() - 1));
    }
    m_brightButton->setChecked(options.bright);
    m_darkButton->setChecked(!options.bright);
    m_brightButton->setStyleSheet(options.bright ? selectedButtonStyle() : normalButtonStyle());
    m_darkButton->setStyleSheet(options.bright ? normalButtonStyle() : selectedButtonStyle());
    m_rowCountEdit->setText(QString::number(options.rowCount));
    m_badPointEdit->setText(QString::number(options.badPointCount));
    m_passOneButton->setChecked(options.pass == 0);
    m_passTwoButton->setChecked(options.pass == 1);
    m_passOneButton->setStyleSheet(options.pass == 0 ? selectedButtonStyle() : normalButtonStyle());
    m_passTwoButton->setStyleSheet(options.pass == 1 ? selectedButtonStyle() : normalButtonStyle());
    {
        const QSignalBlocker blocker(m_cutEdgeCheck);
        m_cutEdgeCheck->setChecked(options.cutEdge);
    }
    {
        const QSignalBlocker blocker(m_independentDelayCheck);
        m_independentDelayCheck->setChecked(options.independentDelay);
    }
    m_lowerLimitEdit->setText(QString::number(options.lowerLimit));
    m_lowerLimitEdit->setVisible(index >= 2);
    m_lowerLimitTitleLabel->setVisible(index >= 2);

    const int sensitivity = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                ->analyzeProfile.traditionArith[index].nSens;
    const bool isGrayAlgorithm = index == ARITH_GREY_A || index == ARITH_GREY_B;
    const double scale = isGrayAlgorithm ? 10.0 : 100.0;
    const int decimalPlaces = isGrayAlgorithm ? 1 : 2;
    m_sensitivityEdit->setText(
        QString::number(sensitivity / scale, 'f', decimalPlaces) + QLatin1Char('%'));
    refreshRetainSettings();
}

void traditionArith::editValue(MyLineEdit *edit, int minimum, int maximum,
                               const QString &title, const std::function<void(int)> &save)
{
    editValueImpl(edit, minimum, maximum, 0, title,
                  [minimum, maximum, &save](double value) {
        save(qBound(minimum, qRound(value), maximum));
    });
}

void traditionArith::editValue(MyLineEdit *edit, double minimum, double maximum,
                               const QString &title, const std::function<void(double)> &save,
                               int decimalPlaces)
{
    editValueImpl(edit, minimum, maximum, decimalPlaces, title,
                  [minimum, maximum, &save](double value) {
        save(qBound(minimum, value, maximum));
    });
}

void traditionArith::editValueImpl(MyLineEdit *edit, double minimum, double maximum,
                                   int decimalPlaces, const QString &title,
                                   const std::function<void(double)> &save)
{
    const bool integer = decimalPlaces == 0;
    if (decimalPlaces < 0) {
        decimalPlaces = (m_selectedAlgorithm == ARITH_GREY_A ||
                         m_selectedAlgorithm == ARITH_GREY_B) ? 1 : 2;
    }

    QString currentText = edit->text();
    currentText.remove(QLatin1Char('%'));
    bool ok = false;
    double currentValue = currentText.toDouble(&ok);
    if (!ok) {
        currentValue = minimum;
    }
    currentValue = qBound(minimum, currentValue, maximum);
    if (integer) {
        currentValue = qRound(currentValue);
    }

    myInputPanel inputPanel(integer ? intType : floatType,
                            qFloor(minimum), qCeil(maximum), currentValue, title, this);
    if (inputPanel.exec() == QDialog::Accepted) {
        double value = qBound(minimum, inputPanel.getValue(), maximum);
        if (!integer) {
            const double scale = qPow(10.0, decimalPlaces);
            value = qRound64(value * scale) / scale;
        }
        save(value);
    }
}

void traditionArith::saveSensitivity(double value)
{
     const int decimalPlaces =
                (m_selectedAlgorithm == ARITH_GREY_A ||
                 m_selectedAlgorithm == ARITH_GREY_B) ? 10 : 100;

    struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.traditionArith[m_selectedAlgorithm].nSens = value * decimalPlaces;
    m_sensitivityEdit->setText(QStringLiteral("%1%").arg(value));
    
    if(m_selectedAlgorithm == ARITH_GREY_A ||
        m_selectedAlgorithm == ARITH_GREY_B){
        gFlow.sendgrayAtraditoncmd(m_selectedAlgorithm);

    }else if(m_selectedAlgorithm == ARITH_DISCOLOR_A ||
             m_selectedAlgorithm == ARITH_DISCOLOR_B){
        gFlow.senddiscolorAtraditoncmd(m_selectedAlgorithm);
    }
}

void traditionArith::refreshRetainSettings()
{
    const int algorithm = qBound(0, m_selectedAlgorithm, ALGORITHM_COUNT - 1);
    for (int i = 0; i < 2; ++i) {
        const auto &retain = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]
                                 ->analyzeProfile.traditionArith[algorithm].retain[i];
        {
            const QSignalBlocker blocker(m_retainModeCombo[i]);
            m_retainModeCombo[i]->clear();
            m_retainModeCombo[i]->addItems(retainModeItems(algorithm, i));
            m_retainModeCombo[i]->setCurrentIndex(
                qBound(0, retain.mode, m_retainModeCombo[i]->count() - 1));
        }
        {
            const QSignalBlocker blocker(m_retainLightCombo[i]);
            m_retainLightCombo[i]->setCurrentIndex(retain.bright ? 0 : 1);
        }
        const double scale = i == 0 ? 100.0 : 10.0;
        const int decimalPlaces = i == 0 ? 2 : 1;
        m_retainSensitivityEdit[i]->setText(
            QStringLiteral("%1%").arg(retain.sensitivity / scale, 0, 'f', decimalPlaces));
    }
}
