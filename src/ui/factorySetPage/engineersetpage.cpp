#include "engineersetpage.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"
#include "common/myinputmethod.h"
#include "common/myqrcode.h"
#include "common/mylistselectdialog.h"
#include "3rdparty/libgetpwd/getpwd.h"
#include <QCryptographicHash>
#include <QFile>
#include <QProcess>
#include "selectlangwidget.h"

engineerSetPage::engineerSetPage(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief engineerSetPage::初始化参数
 */
void engineerSetPage::initParams()
{
    lan_warn    = myLan.scan_by_MeyerAPP;
    lan_passwd  = myLan.password;
    lan_pwderr  = myLan.password_error;
    lan_deviceCodeErr = myLan.serial_num_error;

    lang_ena = myLan.language_enable;
    encryption = myLan.encryption;
}

/**
 * @brief engineerSetPage::搭建界面
 */
void engineerSetPage::createPage()
{
    setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    m_stackedWidget = new QStackedWidget();

    /** 二维码显示及Btn列表*/
    createQRcodeWidget();

    /** 语言、加解密、等子页面*/
    createSelectPage();

    m_stackedWidget->addWidget(m_QRcodeWidget);
    m_stackedWidget->addWidget(m_selectWidget);
    m_stackedWidget->addWidget(m_langSetWidget);
    m_stackedWidget->addWidget(m_factoryNmaeWid);
    m_stackedWidget->addWidget(m_staClassWid);
    m_stackedWidget->addWidget(m_timeSetWid);

    QVBoxLayout *Lay = new QVBoxLayout(this);
    Lay->setMargin(0);
    Lay->addWidget(m_stackedWidget);
}

/**
 * @brief engineerSetPage::信号槽绑定
 */
void engineerSetPage::connectSigAndSlot()
{
    connect(m_codeBtn,          SIGNAL(clicked(bool)),          this,  SLOT(onCodeBtnClicked()));
    connect(m_langSetWidget,    SIGNAL(updateList()),           this,  SLOT(onUpdateLangListSlot()));
    connect(&m_watcher,         SIGNAL(finished()),             this, SLOT(onFutureFinished()));
}

//! 二维码页面
void engineerSetPage::createQRcodeWidget()
{
    noticeLbl = new QLabel(lan_warn);
    noticeLbl->setAlignment(Qt::AlignCenter);
    noticeLbl->setFixedSize(380, 40);

    m_QRCodeLbl = new QLabel;
    m_QRCodeLbl->setFixedSize(350, 350);
    m_QRCodeLbl->setPixmap(QPixmap(":res/png/QRCode.png"));
    m_QRCodeLbl->setScaledContents(true);

    m_codeLbl = new QLabel(lan_passwd);

    m_codeBtn = new QPushButton();
    m_codeBtn->setStyleSheet("background-color:white;");
    m_codeBtn->setFixedSize(BTN_WID*3, BTN_HEI);

    QHBoxLayout *codeLay = new QHBoxLayout;
    codeLay->addStretch();
    codeLay->addWidget(m_codeLbl);
    codeLay->addSpacing(30);
    codeLay->addWidget(m_codeBtn);
    codeLay->addStretch();

    m_QRcodeWidget = new QWidget();
    m_QRcodeWidget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    QVBoxLayout* authLay = new QVBoxLayout(m_QRcodeWidget);
    authLay->addStretch();
    authLay->addWidget(noticeLbl, 0, Qt::AlignCenter);
    authLay->addSpacing(10);
    authLay->addWidget(m_QRCodeLbl, 0, Qt::AlignCenter);
    authLay->addStretch();
    authLay->addLayout(codeLay);
    authLay->addStretch();


    m_selectWidget = new QWidget();
    m_selectWidget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");

    m_EncryptBtn = new QPushButton(encryption);
    m_EncryptBtn->setIcon(QIcon(":/res/png/encrypt.png"));
    m_EncryptBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_EncryptBtn->setFixedSize(BTN_WID*2+40, BTN_HEI+10);
    m_EncryptBtn->setStyleSheet(g_enableBtnStyle);

    m_setLangBtn = new QPushButton(lang_ena);
    m_setLangBtn->setIcon(QIcon(":/res/png/lang_set.png"));
    m_setLangBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_setLangBtn->setFixedSize(BTN_WID*2+40, BTN_HEI+10);
    m_setLangBtn->setStyleSheet(g_enableBtnStyle);

    m_editFactoryNameBtn = new QPushButton(myLan.edit_factoryname);
    m_editFactoryNameBtn->setIcon(QIcon(":/res/png/rice.png"));
    m_editFactoryNameBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_editFactoryNameBtn->setFixedSize(BTN_WID*2+40, BTN_HEI+10);
    m_editFactoryNameBtn->setStyleSheet(g_enableBtnStyle);

    m_staClassBtn = new QPushButton(myLan.statisticclassEn);
    m_staClassBtn->setIcon(QIcon(":/res/png/syssta.png"));
    m_staClassBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_staClassBtn->setFixedSize(BTN_WID*2+40, BTN_HEI+10);
    m_staClassBtn->setStyleSheet(g_enableBtnStyle);

    //! 删除加密文件按钮
    m_deleteEncryptFilesBtn = new QPushButton(myLan.reset_device_code);
    m_deleteEncryptFilesBtn->setIcon(QIcon(":/res/png/tab_auxiliary_normal.png"));
    m_deleteEncryptFilesBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_deleteEncryptFilesBtn->setFixedSize(BTN_WID*2+40, BTN_HEI+10);
    m_deleteEncryptFilesBtn->setStyleSheet(g_enableBtnStyle);

    //! 修改时区按钮
    m_setTimezoneBtn = new QPushButton(myLan.set_timezone);
    m_setTimezoneBtn->setIcon(QIcon(":/res/png/tab_auxiliary_normal.png"));
    m_setTimezoneBtn->setIconSize(QSize(BTN_HEI, BTN_HEI));
    m_setTimezoneBtn->setFixedSize(BTN_WID*2+40, BTN_HEI+10);
    m_setTimezoneBtn->setStyleSheet(g_enableBtnStyle);

    QGridLayout* mainLay = new QGridLayout(m_selectWidget);
    mainLay->setVerticalSpacing(40);
    mainLay->addWidget(m_EncryptBtn, 0, 0, 1, 1, Qt::AlignCenter);
    mainLay->addWidget(m_setLangBtn, 1, 0, 1, 1, Qt::AlignCenter);
    mainLay->addWidget(m_editFactoryNameBtn, 0, 1, 1, 1, Qt::AlignCenter);
    mainLay->addWidget(m_staClassBtn, 1, 1, 1, 1, Qt::AlignCenter);
    mainLay->addWidget(m_deleteEncryptFilesBtn, 2, 1, 1, 1, Qt::AlignCenter);
    mainLay->addWidget(m_setTimezoneBtn, 2, 0, 1, 1, Qt::AlignCenter);


    connect(m_EncryptBtn,       SIGNAL(clicked(bool)),     this,  SLOT(onEncryptBtnClicked()));
    connect(m_setLangBtn,       SIGNAL(clicked(bool)),     this,  SLOT(onLangSetSlt()));
    connect(m_editFactoryNameBtn,   SIGNAL(clicked(bool)),  this,  SLOT(onEditfactorynamebtnSlt()));
    connect(m_staClassBtn,            &QPushButton::pressed, this, &engineerSetPage::onStaclasseditSlt);
    connect(m_deleteEncryptFilesBtn,   &QPushButton::clicked, this, &engineerSetPage::onDeleteEncryptFilesBtnClicked);
    connect(m_setTimezoneBtn,          &QPushButton::clicked, [this](){this->m_stackedWidget->setCurrentIndex(SYSTEM_TIME_WIDGET);});
}

void engineerSetPage::createSelectPage()
{
    //! 语言设置
    m_langSetWidget = new selectLangWidget();

    //! 厂家名称
    m_factoryNmaeWid = new QWidget();
    m_factoryNamelb = new QLabel(myLan.factoryname);
    m_factoryNamelb->setFixedSize(160+50,70);
    m_facNamele = new MyLineEdit();
    m_facNamele->setFixedSize(800,70);
    QFont font = m_factoryNamelb->font();
    font.setPointSize(20);
    m_factoryNamelb->setFont(font);
    m_facNamele->setFont(font);
    connect(m_facNamele, &MyLineEdit::pressed, this, &engineerSetPage::onEditnameleSlt);

    //! 设备名称
    m_deviceNamelb = new QLabel(myLan.device_name);
    m_deviceNamelb->setFixedSize(160+50,70);
    m_deviceNamele = new MyLineEdit();
    m_deviceNamele->setFixedSize(800,70);
    m_deviceNamelb->setFont(font);
    m_deviceNamele->setFont(font);
    connect(m_deviceNamele, &MyLineEdit::pressed, this, &engineerSetPage::onEditDeviceNameSlt);

    m_back2Btn = new QPushButton(myLan.back);
    m_back2Btn->setFixedSize(BTN_WID,BTN_HEI);
    m_back2Btn->setStyleSheet(g_enableBtnStyle);
    connect(m_back2Btn, &QPushButton::pressed, this, &engineerSetPage::onBackbtnPressedSlt);

    QHBoxLayout *hlaycenter = new QHBoxLayout();
    hlaycenter->addStretch();
    hlaycenter->addWidget(m_factoryNamelb);
    hlaycenter->addSpacing(10);
    hlaycenter->addWidget(m_facNamele);
    hlaycenter->addStretch();

    QHBoxLayout *hlaydev = new QHBoxLayout();
    hlaydev->addStretch();
    hlaydev->addWidget(m_deviceNamelb);
    hlaydev->addSpacing(10);
    hlaydev->addWidget(m_deviceNamele);
    hlaydev->addStretch();

    QHBoxLayout *laybtn = new QHBoxLayout();
    laybtn->addStretch();
    laybtn->addWidget(m_back2Btn);
    laybtn->addSpacing(10);

    QVBoxLayout *vlayfac = new QVBoxLayout(m_factoryNmaeWid);
    vlayfac->addStretch();
    vlayfac->addLayout(hlaycenter);
    vlayfac->addSpacing(120);
    vlayfac->addLayout(hlaydev);
    vlayfac->addStretch();
    vlayfac->addLayout(laybtn);

    //! 统计类别
    m_staClassWid = new QWidget();
    m_staclassgb = new QGroupBox();
    QGridLayout *glayitem = new QGridLayout(m_staclassgb);
    QSignalMapper *chkmap = new QSignalMapper();
    for(int i=0; i<MAX_STATISTIC_CLASS; i++){
        m_staClassEnchk[i] = new MyCheckBox("");
        m_staClassEnchk[i]->setCheckBtnSize(60,60);
        chkmap->setMapping(m_staClassEnchk[i],i);
        connect(m_staClassEnchk[i], &MyCheckBox::pressed, chkmap, QOverload<>::of(&QSignalMapper::map));

        glayitem->addWidget(m_staClassEnchk[i], i/2, i%2,1,1,Qt::AlignCenter);
    }
    connect(chkmap, QOverload<int>::of(&QSignalMapper::mapped), this, QOverload<int>::of(&engineerSetPage::onStaclasschkChgSlt));

    m_backBtn = new QPushButton(myLan.back);
    m_backBtn->setFixedSize(BTN_WID,BTN_HEI);
    m_backBtn->setStyleSheet(g_enableBtnStyle);
    connect(m_backBtn, &QPushButton::pressed, this, &engineerSetPage::onBackbtnPressedSlt);


    QHBoxLayout *laydown =new QHBoxLayout();
    laydown->addStretch();
    laydown->addWidget(m_backBtn);
    laydown->addSpacing(10);


    QVBoxLayout *laystapage = new QVBoxLayout(m_staClassWid);
    laystapage->addWidget(m_staclassgb);
    laystapage->addLayout(laydown);


    //! 系统时间
    m_timeSetWid = new QWidget();      //! 时间设置

    m_timezoneBtn = new QPushButton(myLan.set_timezone);
    m_timezoneBtn->setFixedSize(BTN_WID+40,BTN_HEI);
    m_timezoneBtn->setStyleSheet(g_PushButtonStyle);

    m_calSystimeBtn = new QPushButton(myLan.time_calibrate);
    m_calSystimeBtn->setFixedSize(BTN_WID+40,BTN_HEI);
    m_calSystimeBtn->setStyleSheet(g_PushButtonStyle);

    connect(m_timezoneBtn, &QPushButton::pressed,this, &engineerSetPage::onSetTimezoneBtnClicked);
    connect(m_calSystimeBtn, &QPushButton::pressed,this, &engineerSetPage::onCalibrateSysTimeBtnClicked);

    QHBoxLayout *hlayBtn = new QHBoxLayout();
    hlayBtn->addStretch(2);
    hlayBtn->addWidget(m_timezoneBtn);
    hlayBtn->addSpacing(240);
    hlayBtn->addWidget(m_calSystimeBtn);
    hlayBtn->addStretch(3);


    m_frame = new QFrame();
    m_frame->setFrameStyle(QFrame::HLine | QFrame::Plain);
    m_frame->setMinimumHeight(2);
    m_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    m_frame->setStyleSheet("background-color:gray;");

    QGridLayout *lyDown = new QGridLayout;
    lyDown->setContentsMargins(192, 32, 192, 32);
    for(int i=0; i<TIME_TYPE; i++){
        m_Timele[i] = new MyLineEdit();
        m_Timele[i]->setDisabled(true);
        lyDown->addWidget(m_Timele[i], i/3, (i%3)*2);
    }

    QStringList strList;
    strList << "-" << "-" << ":" << ":";
    for (int i = 0; i < DELIMITER_NUM; i++) {
        m_splitlb[i] = new QLabel(strList.at(i));
        m_splitlb[i]->setAlignment(Qt::AlignCenter);
        lyDown->addWidget(m_splitlb[i], i/2, (i%2)*2+1);
    }
    lyDown->setVerticalSpacing(60);


    m_back3Btn = new QPushButton(myLan.back);
    m_back3Btn->setFixedSize(BTN_WID,BTN_HEI);
    m_back3Btn->setStyleSheet(g_enableBtnStyle);
    connect(m_back3Btn, &QPushButton::pressed,[this](){this->m_stackedWidget->setCurrentIndex(HOME_WIDGET);});

    QVBoxLayout *laywid = new QVBoxLayout(m_timeSetWid);
    laywid->addSpacing(40);
    laywid->addLayout(hlayBtn);
    laywid->addSpacing(50);
    laywid->addWidget(m_frame);
    laywid->addSpacing(100);
    laywid->addLayout(lyDown);
    laywid->addStretch();
    laywid->addWidget(m_back3Btn,0,Qt::AlignRight);
    laywid->addSpacing(20);
}

/**
  * @brief engineerSetPage::getPassword
  * @param srcCode
  * @return
  */
QString engineerSetPage::getMd5Password(QString srcCode)
{
#ifdef Q_OS_UNIX
    QString md5Str;
    QByteArray byte,result;
    QCryptographicHash md(QCryptographicHash::Md5);
    srcCode += "meyer002690";
    byte = srcCode.toLocal8Bit();
    md.addData(byte);
    result = md.result().toHex();
    md5Str.append(result);

    return QString::fromStdString(getPassword(md5Str.toStdString()));
#else
    return "MY2022";
#endif
}

void engineerSetPage::refreshStaclasswid()
{
    QStringList stacategorylst;
    for(int i=0; i<struGsh.vlabStamap.size(); i++){
        if(struGsh.vlabStamap[i].size() >0){
            QList<QString> classlst;
            slabSta labsta = struGsh.vlabStamap[i].first();
            classlst = labsta.staInfo.keys();
            for(auto val : classlst){
                if(!stacategorylst.contains(val)){
                    stacategorylst<<val;
                }
            }
        }
    }

    auto *pPofile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    for(int i=0; i<MAX_STATISTIC_CLASS; i++){
        if(i < stacategorylst.size()){
            m_staClassEnchk[i]->show();

            QString name = stacategorylst.at(i);
            m_staClassEnchk[i]->setText(name);

            if(pPofile->staclassEnmap.contains(name)){
                m_staClassEnchk[i]->setChecked(pPofile->staclassEnmap[name]);
            }else{
                m_staClassEnchk[i]->setChecked(true);
            }
        }else{
            m_staClassEnchk[i]->setText("");
            m_staClassEnchk[i]->setChecked(false);
            m_staClassEnchk[i]->hide();
        }
    }

}

bool engineerSetPage::execNtp()
{
    QProcess process;
    process.start("ntpd -gq");
    if (process.waitForFinished(15*1000)) {
        QByteArray ba = process.readAll();
        if (ba.contains("ntpd: time set")
            || ba.contains("ntpd: time slew")) {
            gFlow.SystemCmd("hwclock -uw");
            return true;
        }
    } else {
        process.kill();
    }
    return false;
}

/*!
 * \brief 二维码更新
 */
void engineerSetPage::refreshDisplay()
{
    m_stackedWidget->setCurrentIndex(HOME_WIDGET);

    QString code = struCnfg.master_device_code;

    if(!gFlow.inputCodeIsValid(code)){
        return;
    }

    if (code.compare("") == 0) {
        m_codeStr = "";
    } else {
        m_codeStr = code + QDateTime::currentDateTime().toString("yyMMddhhmmss");
    }

    MyQRCode *QRCode = new MyQRCode;
    QPixmap pixmap = QRCode->encode(m_codeStr);
    if (!pixmap.isNull()) {
        m_QRCodeLbl->setPixmap(pixmap.scaled(m_QRCodeLbl->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

/*!
 * \brief 槽函数: 权限卡确认
 */
void engineerSetPage::onCodeBtnClicked()
{
    QString code = struCnfg.master_device_code;
    if(!gFlow.inputCodeIsValid(code)){
        infoWidget->raise();
        infoWidget->setLabelText(lan_deviceCodeErr);
        infoWidget->showSecs();
        return;
    }

    myInputMethod input(lan_passwd, "", g_mainWidget);
    input.setPasswordMode(true);
    int ret = input.exec();
    if(ret == QDialog::Accepted){
        QString str = input.getText();
        QString pwd = getMd5Password(m_codeStr);

        if(str.compare(pwd) == 0) {
            m_stackedWidget->setCurrentIndex(SELECT_WIDGET);
        } else {
            infoWidget->raise();
            infoWidget->setLabelText(lan_pwderr);
            infoWidget->showSecs();
        }
    }
}

/*!
 * \brief 槽函数: 加密
 */
void engineerSetPage::onEncryptBtnClicked()
{
//    MyEncrypt dlg(g_mainWidget);

//    paramDelayCode.m_nEnableDelayCodeInfo = struCnfg.nEnableDelayCodeInfo;
//    dlg.exec();

//    struCnfg.nEnableDelayCodeInfo = paramDelayCode.m_nEnableDelayCodeInfo;

//! 提示确认系统时间
    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_sys_time), myLan.notice, g_mainWidget);
    if(msgBox.exec() == QDialog::Rejected){
        return;
    }

    //! 弹框输入期限码
    myInputMethod inputDlg(myLan.input_code,"" , g_mainWidget);
//    inputDlg.setType(KB_DC);
    if(inputDlg.exec() == QDialog::Accepted){
        QString sMachineId;
        QDate date;
        QString delayCode = inputDlg.getText();

        int nResult = g_delagCode.checkDelayCode(delayCode, sMachineId, date, false);

        if (nResult == DelayCode::DC_SUCCESS) {
            QString sInfo = QString("%1: %2\n%3: %4?")
                                .arg(myLan.serial_num).arg(sMachineId)
                                .arg(myLan.dc_limit).arg(date.toString("yyyy-MM-dd"));
            myMessageBox confirmsg(MSG_QUES, sInfo,myLan.notice, g_mainWidget);
            if (confirmsg.exec() == QDialog::Accepted) {
                    struCnfg.master_device_code = sMachineId;

                    nResult = g_delagCode.encryptAndDecrypt(delayCode, false); //! 进行加密

                    infoWidget->raise();
                    infoWidget->setLabelText(g_delagCode.getText(nResult));
                    infoWidget->showSecs();
            }
        } else {
            infoWidget->raise();
            infoWidget->setLabelText(g_delagCode.getText(nResult));
            infoWidget->showSecs();
        }
    }
}

/*!
 * \brief 槽函数: 语言使能
 */
void engineerSetPage::onLangSetSlt()
{
    m_stackedWidget->setCurrentIndex(LANGUAGE_WIDGET);
    m_langSetWidget->refreshDisplay();
}


void engineerSetPage::onUpdateLangListSlot()
{
    emit updateLangList();
    m_stackedWidget->setCurrentIndex(SELECT_WIDGET);
}

void engineerSetPage::onEditfactorynamebtnSlt()
{
    m_stackedWidget->setCurrentIndex(FACTORYEDIT_WIDGET);
}

void engineerSetPage::onEditnameleSlt()
{
INPUT_NAME1:
    QString str;
    myInputMethod input(myLan.factoryname, m_facNamele->text(), g_mainWidget);
    int ret = input.exec();
    if (ret == QDialog::Accepted) {
        str = input.getText().trimmed();
        if(str.compare("") == 0) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.input_error);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        if(!gFlow.inputStrIsValid(str)) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.warn_contains_speical_character);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        struCnfg.gCompanyName = str;
        m_facNamele->setText(str);
    }
}

/*!
 * \brief 编辑设备名称
 */
void engineerSetPage::onEditDeviceNameSlt()
{
INPUT_NAME1:
    QString str;
    myInputMethod input(myLan.device_name, m_deviceNamele->text(), g_mainWidget);
    int ret = input.exec();
    if (ret == QDialog::Accepted) {
        str = input.getText().trimmed();
        if(str.compare("") == 0) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.input_error);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        if(!gFlow.inputStrIsValid(str)) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.warn_contains_speical_character);
            infoWidget->showSecs();
            goto INPUT_NAME1;
        }
        struCnfg.gDeviceNmae = str;
        m_deviceNamele->setText(str);
    }
}

void engineerSetPage::onStaclasseditSlt()
{
    refreshStaclasswid();
    m_stackedWidget->setCurrentIndex(STATISTICCLASS_WIDGET);

}

void engineerSetPage::onStaclasschkChgSlt(int nIdx)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    QString str = m_staClassEnchk[nIdx]->text();

    pProfile->staclassEnmap[str] = m_staClassEnchk[nIdx]->isChecked();
}

void engineerSetPage::onBackbtnPressedSlt()
{
    m_stackedWidget->setCurrentIndex(SELECT_WIDGET);
}

void engineerSetPage::showEvent(QShowEvent *ev)
{
    Q_UNUSED(ev)
    if (m_nTimerId == -1) {
        m_nTimerId = startTimer(1000);
    }
}

void engineerSetPage::hideEvent(QHideEvent *ev)
{
    Q_UNUSED(ev)
    if (m_nTimerId != -1) {
        killTimer(m_nTimerId);
        m_nTimerId = -1;
    }
}

void engineerSetPage::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev)

    int data[TIME_TYPE];
    QDateTime dateTime = QDateTime::currentDateTime();
    data[0] = dateTime.date().year();
    data[1] = dateTime.date().month();
    data[2] = dateTime.date().day();
    data[3] = dateTime.time().hour();
    data[4] = dateTime.time().minute();
    data[5] = dateTime.time().second();

    for (int i = 0; i < TIME_TYPE; ++i) {
        QString str = QString("%1").arg(data[i], 2, 10, QLatin1Char('0'));
        m_Timele[i]->setText(str);
    }
}

/*!
 * \brief 槽函数: 删除加密文件
 */
void engineerSetPage::onDeleteEncryptFilesBtnClicked()
{
    //! 确认对话框
    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_reset_devicecode), myLan.notice, g_mainWidget);
    if(msgBox.exec() == QDialog::Rejected){
        return;
    }

    struCnfg.master_device_code = QString("6000000000000");


    // //! 加密文件列表
    // QStringList encryFiles;
    // encryFiles << "/userdata/meyer/cnf/encry.dc"
    //            << "/etc/encry.dc1"
    //            << "/opt/encry.dc2";

    // //! 时间文件列表
    // QStringList timeFiles;
    // timeFiles << "/userdata/meyer/cnf/encry.time"
    //           << "/etc/.time1"
    //           << "/opt/.time2";

    // int nDeletedCount = 0;
    // int nFailedCount = 0;

    // //! 删除加密文件
    // for(const QString &filePath : encryFiles){
    //     if(QFile::exists(filePath)){
    //         if(QFile::remove(filePath)){
    //             nDeletedCount++;
    //         }else{
    //             nFailedCount++;
    //         }
    //     }
    // }

    // //! 删除时间文件
    // for(const QString &filePath : timeFiles){
    //     if(QFile::exists(filePath)){
    //         if(QFile::remove(filePath)){
    //             nDeletedCount++;
    //         }else{
    //             nFailedCount++;
    //         }
    //     }
    // }

    // //! 显示结果
    // QString resultMsg;
    // if(nFailedCount == 0){
    //     resultMsg = myLan.delete_success;
    // }else{
    //     resultMsg = QString("%1: %2, %3: %4")
    //                     .arg(myLan.delete_success).arg(nDeletedCount)
    //                     .arg(myLan.delete_failed).arg(nFailedCount);
    // }

    infoWidget->raise();
    infoWidget->setLabelText(myLan.operate_sucess);
    infoWidget->showSecs();
}

/*!
 * \brief 槽函数: 修改系统时区
 */
void engineerSetPage::onSetTimezoneBtnClicked()
{
//#ifdef Q_OS_UNIX
    //! 常用时区列表
    QStringList timezoneList;
    QMap<QString, QString> timezoneMap;
    timezoneMap["中国-上海 (UTC+8)"] = "Asia/Shanghai";
    timezoneMap["中国-香港 (UTC+8)"] = "Asia/Hong_Kong";
    timezoneMap["日本-东京 (UTC+9)"] = "Asia/Tokyo";
    timezoneMap["韩国-首尔 (UTC+9)"] = "Asia/Seoul";
    timezoneMap["新加坡 (UTC+8)"] = "Asia/Singapore";
    timezoneMap["泰国-曼谷 (UTC+7)"] = "Asia/Bangkok";  
    timezoneMap["阿联酋-迪拜 (UTC+4)"] = "Asia/Dubai";
    timezoneMap["印度-加尔各答 (UTC+5:30)"] = "Asia/Kolkata";
    timezoneMap["英国-伦敦 (UTC+0)"] = "Europe/London";
    timezoneMap["法国-巴黎 (UTC+1)"] = "Europe/Paris";
    timezoneMap["德国-柏林 (UTC+1)"] = "Europe/Berlin";
    timezoneMap["俄罗斯-莫斯科 (UTC+3)"] = "Europe/Moscow";
    timezoneMap["美国-纽约 (UTC-5)"] = "America/New_York";
    timezoneMap["美国-洛杉矶 (UTC-8)"] = "America/Los_Angeles";
    timezoneMap["美国-芝加哥 (UTC-6)"] = "America/Chicago"; 
    timezoneMap["澳大利亚-悉尼 (UTC+10)"] = "Australia/Sydney";
    timezoneMap["新西兰-奥克兰 (UTC+12)"] = "Pacific/Auckland"; 
    timezoneMap["巴西-巴西利亚 (UTC-3)"] = "America/Sao_Paulo";
    timezoneMap["格林尼治(GMT+3)"] = "Etc/GMT+3";

    if(myLan.nLang == 0){
        timezoneList    << "中国-上海 (UTC+8)"      
                        << "中国-香港 (UTC+8)"      
                        << " 日本-东京 (UTC+9)"         
                        << "韩国-首尔 (UTC+9)"         
                        << "新加坡 (UTC+8)"      
                        << "泰国-曼谷 (UTC+7)"        
                        << "阿联酋-迪拜 (UTC+4)"          
                        << "印度-加尔各答 (UTC+5:30)"        
                        << "英国-伦敦 (UTC+0)"       
                        << "法国-巴黎 (UTC+1)"       
                        << "德国-柏林 (UTC+1)"       
                        << "俄罗斯-莫斯科 (UTC+3)"      
                        << "美国-纽约 (UTC-5)"   
                        << "美国-洛杉矶 (UTC-8)" 
                        << "美国-芝加哥 (UTC-6)"    
                        << "澳大利亚-悉尼 (UTC+10)"    
                        << "新西兰-奥克兰 (UTC+12)"
                        << "巴西-巴西利亚 (UTC-3)"
                        << "格林尼治(GMT+3)";
    }else{
        timezoneList    << "Asia/Shanghai"      // 中国-上海 (UTC+8)
                        << "Asia/Hong_Kong"     // 中国-香港 (UTC+8)
                        << "Asia/Tokyo"         // 日本-东京 (UTC+9)
                        << "Asia/Seoul"         // 韩国-首尔 (UTC+9)
                        << "Asia/Singapore"     // 新加坡 (UTC+8)
                        << "Asia/Bangkok"       // 泰国-曼谷 (UTC+7)
                        << "Asia/Dubai"         // 阿联酋-迪拜 (UTC+4)
                        << "Asia/Kolkata"       // 印度-加尔各答 (UTC+5:30)
                        << "Europe/London"      // 英国-伦敦 (UTC+0)
                        << "Europe/Paris"       // 法国-巴黎 (UTC+1)
                        << "Europe/Berlin"      // 德国-柏林 (UTC+1)
                        << "Europe/Moscow"      // 俄罗斯-莫斯科 (UTC+3)
                        << "America/New_York"   // 美国-纽约 (UTC-5)
                        << "America/Los_Angeles"// 美国-洛杉矶 (UTC-8)
                        << "America/Chicago"    // 美国-芝加哥 (UTC-6)
                        << "Australia/Sydney"   // 澳大利亚-悉尼 (UTC+10)
                        << "Pacific/Auckland"  // 新西兰-奥克兰 (UTC+12)
                        << "America/Sao_Paulo"   // 巴西-巴西利亚 (UTC-3)
                        << "Etc/GMT+3";
    }
   

    //! 使用通用列表选择对话框
    myListSelectDialog selectDlg(myLan.select_timezone, timezoneList, g_mainWidget);
    if(selectDlg.exec() != QDialog::Accepted){
        return;
    }

    QString selectedTimezone = selectDlg.getSelectedText();
    if(selectedTimezone.isEmpty()){
        return;
    }

    //! 确认对话框
    QString confirmMsg = QString("%1: %2?").arg(myLan.confirm_set_timezone).arg(selectedTimezone);
    myMessageBox msgBox(MSG_QUES, confirmMsg, myLan.notice, g_mainWidget);
    if(msgBox.exec() == QDialog::Rejected){
        return;
    }

    QString resultMsg;

    //! 执行系统命令设置时区 (永久生效)
    //! 使用 timedatectl 命令设置时区
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);

    if(myLan.nLang == 0 && timezoneMap.contains(selectedTimezone)){
        selectedTimezone = timezoneMap[selectedTimezone];
    }
    QString linkCmd = QString("rm -rf /etc/localtime && ln -sf /usr/share/zoneinfo/%1 /etc/localtime").arg(selectedTimezone);
    process.start("sh", QStringList() << "-c" << linkCmd);

    if (!process.waitForFinished(5000)) {
        process.terminate();
        if (!process.waitForFinished(1000)) {
            process.kill();
            process.waitForFinished(1000);
        }
        resultMsg = myLan.set_timezone_failed;
    } else {
        if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
            resultMsg = QString("%1: %2").arg(myLan.set_timezone_success).arg(selectedTimezone);
            //! 如果系统加密，同步加密系统时间
            g_delagCode.updatetime();
    } else {//！输出用于排查
            QString out = process.readAll();
            Q_UNUSED(out);
            resultMsg = myLan.set_timezone_failed;
        }
    }

    infoWidget->raise();
    infoWidget->setLabelText(resultMsg);
    infoWidget->showSecs();
//#endif
}

/*!
 * \brief engineerSetPage::onCalibrateSysTimeBtnClicked
 */
void engineerSetPage::onCalibrateSysTimeBtnClicked()
{
    //! 校验网络
    if(struGsh.nNetwortStat != NETWORK_NORMAL){
        infoWidget->raise();
        infoWidget->setLabelText(myLan.network_abnormal);
        infoWidget->showSecs(3);
    }else{
        m_calSystimeBtn->setDisabled(true);

        infoWidget->raise();
        infoWidget->setLabelText(QString("%1....").arg(myLan.time_calibrate));

        QFuture<bool> future = QtConcurrent::run(this, &engineerSetPage::execNtp);
        m_watcher.setFuture(future);
    }
}

void engineerSetPage::onFutureFinished()
{
    m_calSystimeBtn->setDisabled(false);

    infoWidget->raise();
    infoWidget->setLabelText(m_watcher.result()?myLan.sucess:myLan.failed);
    infoWidget->showSecs(3);

    if(m_watcher.result()){
        //! 系统加密后同步加密中的系统时间
        g_delagCode.updatetime();
    }
}
