#include "getimagepage.h"
#include "core/io/udp/myudp.h"
#include "src/common/mymessagebox.h"
#include "src/common/selectfiledialog.h"


#define MAT_BYTE_NUM (COLOR_TYPE_NUM+2)

#define IM01_MAT_BYTE_NUM 8
#define IM03_MAT_BYTE_NUM 14

getImagePage::getImagePage(QWidget *parent) : QWidget(parent)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

getImagePage::~getImagePage()
{
    if (gImageData != NULL) {
        delete[] gImageData;
        gImageData = NULL;
    }

    if (gImageData_2048 != NULL) {
        delete[] gImageData_2048;
        gImageData_2048 = NULL;
    }
}

/**
 * @brief getImagePage::刷新显示
 */
void getImagePage::refreshPageDisplay()
{
//    if (n_analyzeNum != struCnfg.analyzeNum) {
        n_analyzeNum = struCnfg.analyzeNum;
//        refreshAnalyzeComBox();
//    }

    refreshViewBoxDis();

    refreshImgDisPlay();

    refreshBtnState();
}

/**
 * @brief getImagePage::初始化参数
 */
void getImagePage::initParams()
{
    start_cam =  myLan.capture;
    export_img = myLan.exporting;
    import_img = myLan.import;
    delete_img = myLan.del;
    delete_all_img = myLan.del_all;
    thread_value = myLan.triger_lowlimit;
    overmaximg =myLan.over_max_picture_num;

    RGB_type = "RGB";
    NGB_type = "NGB";
    NNN_type = "NNN";

    n_CapIndex = 0;
    imgIndex = 0;
    n_ImgType = 0;
    m_bNNNEnable = false;
    n_ImageCount = 0;

    gImageData = new char[BIG_IMAGE_HEIGHT*SENSOR_PIXELS_640*IM01_MAT_BYTE_NUM];

    gImageData_2048 = new char[BIG_IMAGE_HEIGHT*SENSOR_PIXELS_2048*MAT_BYTE_NUM];

    for (int i = 0; i < MAX_IMG_NUM; i++) {
        m_sNameOfImg[i] = "";
        m_picName[i] = "";
    }
    n_sample = 640;
    isCaptureAllView = false;
    isDeleteImg = false;
    viewMap    = new QSignalMapper();
    imgDistMap = new QSignalMapper();
    imgTypeMap = new QSignalMapper();
    n_analyzeNum = struCnfg.analyzeNum;

    setBigPic(false);
}

/**
 * @brief getImagePage::构造界面
 */
void getImagePage::createPage()
{
    delTimer = new QTimer();

    QString BoxStyle = QString("QComboBox {border: 1px solid lightgray; border-radius: 10px; padding: 1px 1px 1px 1px;background-color: #FFFFFF;}");
    QString BoxArrowStyle = QString("QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 50px; "
                                    "border-left-width: 0px; border-left-color:darkgray; border-left-style: solid; "
                                    "border-top-right-radius: 3px; border-bottom-right-radius: 3px;}");
    QString BoxArrowImage = QString("QComboBox::down-arrow {image:url(:/res/png/icon_down_arrow); width:20px; height: 20px;}");
    QStringList  portList,imgTypeList;
    portList << import_img << export_img;
    imgTypeList << RGB_type << NGB_type/* << NNN_type*/;

    imgDisGBox = new QGroupBox();
    imgDisGBox->setFixedWidth(WIN_WID/2+200);

    QGridLayout *imgDisGridLayout = new QGridLayout(imgDisGBox);  //! 图片列表
    imgDisGridLayout->setSizeConstraint(QLayout::SetFixedSize);
    QVBoxLayout *imgDisVBLayout[MAX_IMG_NUM];
    for (int i = 0; i < MAX_IMG_NUM; i++) {
        imgDisVBLayout[i] = new QVBoxLayout();
        imgDisBtn[i] = new QPushButton();
        imgItemBtn[i] = new QPushButton();
        imgDisBtn[i]->setFixedSize(370,370);
        imgItemBtn[i]->setFixedSize(370,50);
        imgDisBtn[i]->setEnabled(false);
        imgItemBtn[i]->setEnabled(false);
        imgDisVBLayout[i]->addWidget(imgDisBtn[i]);
        imgDisVBLayout[i]->addWidget(imgItemBtn[i]);
        imgDisGridLayout->addLayout(imgDisVBLayout[i],i/3,i%3);

        showImg(i, false);

        imgDistMap->setMapping(imgDisBtn[i],i);
        connect(imgDisBtn[i], SIGNAL(pressed()), imgDistMap, SLOT(map()));
    }

    m_pScroll = new QScrollArea();
    m_pScroll->setFixedSize(WIN_WID/2+240, WIN_HEI-180);
    m_pScroll->setContentsMargins(0,0,0,0);
    m_pScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pScroll->setWidget(imgDisGBox);

    getImgGBox = new QGroupBox(myLan.params_set); //! 拍照参数设置GBox
    getImgGBox->setFixedSize(380, WIN_HEI-200);
    QVBoxLayout *getImgLayout = new QVBoxLayout(getImgGBox);
    getImgLayout->setContentsMargins(0,40,0,0);

    QListWidget *listWidget = new QListWidget();
    analyzeCombox = new QComboBox();
    analyzeCombox->setFixedSize(BTN_WID*2+100,BTN_HEI);
    analyzeCombox->setStyleSheet(QString("%1 %2 %3").arg(BoxStyle).arg(BoxArrowStyle).arg(BoxArrowImage));
    analyzeCombox->setModel(listWidget->model());
    analyzeCombox->setView(listWidget);
    analyzeCombox->hide();
//    refreshAnalyzeComBox();

    getBak = new MyCheckBox(QString("%1").arg(myLan.rm_background), struGsh.nGetBak);
    getBak->setFixedSize(BTN_WID,BTN_HEI);

    coCapchk = new MyCheckBox(myLan.synchronize,false);
    coCapchk->setFixedSize(BTN_WID,BTN_HEI);

    QGridLayout *viewVLayout = new QGridLayout();
    viewVLayout->setVerticalSpacing(20);
    for (int i = 0; i < MAX_CAM_NUM; i++) {
        viewBox[i] = new MyCheckBox(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i], false);  //! 相机视角
        viewBox[i]->setFixedSize(BTN_WID+20,BTN_HEI);
        viewVLayout->addWidget(viewBox[i], i/2, i%2);
        viewMap->setMapping(viewBox[i],i);
        connect(viewBox[i], SIGNAL(pressed()), viewMap, SLOT(map()));
    }

    viewVLayout->addWidget(coCapchk,3,0);
    viewVLayout->addWidget(getBak,3,1);


    sampleLineLbl = new QLabel(QString("%1").arg(myLan.capture_line));
    sampleLineLbl->setFixedSize(BTN_WID, BTN_HEI);
    sampleLineBtn = new QPushButton(QString("%1").arg(n_sample));
    sampleLineBtn->setFixedSize(BTN_WID, BTN_HEI);
    QHBoxLayout *sampleLayout = new QHBoxLayout();
    sampleLayout->addWidget(sampleLineLbl);
    sampleLayout->addWidget(sampleLineBtn);

    threadValueLbl = new QLabel(thread_value);  //! 触发下限
    threadValueLbl->setFixedSize(BTN_WID, BTN_HEI);
    threadValueLbl->setText(thread_value);
    threadValueBtn = new QPushButton();
    threadValueBtn->setFixedSize(BTN_WID, BTN_HEI);
    threadValueBtn->setText(QString("%1").arg(struGsh.nThreadValue));
    QHBoxLayout *midLayout = new QHBoxLayout();
    midLayout->addWidget(threadValueLbl);
    midLayout->addWidget(threadValueBtn);

    QVBoxLayout *vlayCapline = new QVBoxLayout();
    vlayCapline->addLayout(sampleLayout);
    vlayCapline->addLayout(midLayout);

    QVBoxLayout *vimgtypelay =new QVBoxLayout();
    QHBoxLayout *imgTypeLayout = new QHBoxLayout();  //!RGB NGB
    for (int i = 0; i < TYPE_TOTAL-1; i++) {
        imgTypeBtn[i] = new QPushButton();
        imgTypeBtn[i]->setFixedSize(BTN_WID-20, BTN_HEI-20);
        imgTypeBtn[i]->setText(QString("%1").arg(imgTypeList.at(i)));
        imgTypeMap->setMapping(imgTypeBtn[i],i);
        connect(imgTypeBtn[i], SIGNAL(pressed()), imgTypeMap, SLOT(map()));
        imgTypeLayout->addWidget(imgTypeBtn[i]);
    }


    nnnBox = new MyCheckBox(""/*NNN_type*/, m_bNNNEnable);
    QStringList imgtypelst;
    imgtypelst<<myLan.all<<"NNN"<<"PPP"<<"QQQ";
    QListWidget *imgtypelstWid = new QListWidget();
    imgTypeComb = new QComboBox();
    imgTypeComb->setFixedSize(BTN_WID-20, BTN_HEI-20);
    imgTypeComb->setStyleSheet(QString("%1 %2 %3").arg(BoxStyle).arg(BoxArrowStyle).arg(BoxArrowImage));
    imgTypeComb->setModel(imgtypelstWid->model());
    imgTypeComb->setView(imgtypelstWid);
    imgTypeComb->addItems(imgtypelst);
    imgTypeComb->setDisabled(true);
    QHBoxLayout *hlaytype = new QHBoxLayout();
    hlaytype->addWidget(nnnBox,0,Qt::AlignLeft);
    hlaytype->setContentsMargins(55,0,10,0);
    hlaytype->addWidget(imgTypeComb,0,Qt::AlignLeft);
    vimgtypelay->addLayout(imgTypeLayout);
    vimgtypelay->addLayout(hlaytype);

    startGetBtn = new QPushButton(); //! 拍照
    startGetBtn->setText(start_cam);
    startGetBtn->setFixedSize(BTN_WID, BTN_HEI-10);

    deleteBtn = new QPushButton(); //! 删除照片
    deleteBtn->setText(delete_img);
//    deleteBtn->setStyleSheet("font-size:22px;");
    deleteBtn->setFixedSize(BTN_WID, BTN_HEI-10);

    QHBoxLayout *hlaycapBtn = new QHBoxLayout();
    hlaycapBtn->addWidget(startGetBtn);
    hlaycapBtn->addWidget(deleteBtn);

    QHBoxLayout *downLayout = new QHBoxLayout();
    for (int i = 0; i < 2; i++) {
        exportBtn[i] = new QPushButton();
        exportBtn[i]->setFixedSize(BTN_WID, BTN_HEI-10);
        exportBtn[i]->setText(portList.at(i));
        downLayout->addWidget(exportBtn[i]);
    }

    m_backBtn = new QPushButton();
    m_backBtn->setFixedSize(BTN_WID, BTN_HEI-10);
    m_backBtn->setText(myLan.back);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_backBtn);
    btnLayout->addSpacing(4);

    QVBoxLayout *vlaydown = new QVBoxLayout();
    vlaydown->addLayout(hlaycapBtn);
    vlaydown->addSpacing(5);
    vlaydown->addLayout(downLayout);
    vlaydown->addSpacing(20);
    vlaydown->addLayout(btnLayout);
    vlaydown->addSpacing(5);


    feederBtn = new QToolButton();
    feederBtn->setText("开始下料");
    feederBtn->hide();
    feederBtn->setIcon(QIcon(":/res/png/Media_Start.png"));
    feederBtn->setIconSize(QSize(50, 50));
    feederBtn->setFixedSize(BTN_WID+160, BTN_HEI);
    feederBtn->setDisabled(true);
    feederBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    feederBtn->setStyleSheet(QString("QToolButton{background-color: %1;font: 24px;}").arg(g_ColorLightGray3));
    QHBoxLayout *feederLayout = new QHBoxLayout();
    feederLayout->addWidget(feederBtn, 0, Qt::AlignCenter);

    getImgLayout->addWidget(analyzeCombox);
    getImgLayout->addStretch(2);
    getImgLayout->addLayout(viewVLayout);
    getImgLayout->addStretch(2);
    getImgLayout->addLayout(vlayCapline);
    getImgLayout->addStretch(2);
    getImgLayout->addLayout(vimgtypelay);
    getImgLayout->addStretch(2);
    getImgLayout->addLayout(feederLayout);
    getImgLayout->addStretch(3);
    getImgLayout->addLayout(vlaydown);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_pScroll);
    mainLayout->addWidget(getImgGBox);
}

/**
 * @brief getImagePage::信号槽绑定
 */
void getImagePage::connectSigAndSlot()
{
    connect(&gFlow,         SIGNAL(sigGetImageData(QByteArray,int,int)), this, SLOT(onImageDataReceived(QByteArray,int,int)));
    connect(viewMap,        SIGNAL(mapped(int)),       this,    SLOT(onViewBoxPressedSlt(int)));
    connect(imgDistMap,     SIGNAL(mapped(int)),       this,    SLOT(onImgDisPlayBtnPressedSlt(int)));
    connect(imgTypeMap,     SIGNAL(mapped(int)),       this,    SLOT(onImgTypeBtnPressedSlt(int)));
    connect(nnnBox,         SIGNAL(pressed()),         this,    SLOT(onNNNBoxPressedSlt()));
    connect(startGetBtn,    SIGNAL(clicked(bool)),     this,    SLOT(onCaptureBtnPressedSlt()));
    connect(threadValueBtn, SIGNAL(pressed()),         this,    SLOT(onThreadValueBtnPressedSlt()));
    connect(sampleLineBtn,  SIGNAL(pressed()),         this,    SLOT(onSampleBtnPressedSlt()));
    connect(deleteBtn,      SIGNAL(pressed()),         this,    SLOT(onImgDeleteBtnPressedSlt()));
    connect(deleteBtn,      SIGNAL(released()),        this,    SLOT(onImgDeleteBtnReleasedSlt()));
    connect(delTimer,       SIGNAL(timeout()),         this,    SLOT(onDeleteTimerOutSlt()));
    connect(feederBtn,      SIGNAL(released()),        this,    SLOT(onFeederBtnPressedSlt()));
    connect(getBak,         SIGNAL(pressed()),         this,    SLOT(onGetBakBtnPressedSlt()));
    connect(coCapchk,       SIGNAL(pressed()),         this,    SLOT(onCoCapChkPressedSlt()));
    connect(exportBtn[0],   SIGNAL(pressed()),         this,    SLOT(onImgImportBtnPressedSlt()));
    connect(exportBtn[1],   SIGNAL(pressed()),         this,    SLOT(onImgExportBtnPressedSlt()));
    connect(m_backBtn,      &QPushButton::pressed, this,     &getImagePage::sigbackBtnpressed);
}

/**
 * @brief getImagePage::刷新分析仪列表
 */
void getImagePage::refreshAnalyzeComBox()
{
    disconnect(analyzeCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComBoxChangedSlt(int)));
    analyzeCombox->clear();

    for (int i = 0; i < n_analyzeNum; i++) {
        analyzeCombox->addItem(QIcon(""), QString(gFlow.getSpecifyAnalyzeGlobal(i)->sAnalyzeName));
    }
    analyzeCombox->setCurrentIndex(struGsh.nAnalyzeIndex);
    connect(analyzeCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComBoxChangedSlt(int)));
}

/**
 * @brief getImagePage::分析仪切换
 */
void getImagePage::onComBoxChangedSlt(int index)
{
    struGsh.nAnalyzeIndex = index;
}

/**
 * @brief getImagePage::拍照相机选择
 * @param index
 */
void getImagePage::onViewBoxPressedSlt(int index)
{
    if (n_CapIndex == index)
        return;

    n_CapIndex = index;

    refreshViewBoxDis();
}

/**
 * @brief getImagePage::onImgDisPlayBtnPressedSlt
 * @param index
 */
void getImagePage::onImgDisPlayBtnPressedSlt(int index)
{
    if (isDeleteImg) {
        myMessageBox msgBox(MSG_QUES, delete_img, delete_warn, g_mainWidget);

        int ret = msgBox.exec();
        if (ret != QDialog::Accepted) {
            return;
        }
        infoWidget->raise();
        infoWidget->setLabelText(myLan.apply_and_wait);
        infoWidget->delayShow();
        deteleCurImg(index);
        infoWidget->hide();
    } else {
        myView *viewPage = new myView(g_mainWidget, false);
        QString imageStr = m_picName[index];
        viewPage->setViewData(imageStr);
        viewPage->show();
    }
}

/**
 * @brief getImagePage::onImgDeleteBtnPressedSlt
 */
void getImagePage::onImgDeleteBtnPressedSlt()
{
    m_nPressTimeStart = clock();
    delTimer->start(2000);
}

void getImagePage::onImgDeleteBtnReleasedSlt()
{
    delTimer->stop();

    if((clock() - m_nPressTimeStart)/CLOCKS_PER_SEC < 2) { //按压时间少于3s，单个删除图片
        isDeleteImg = !isDeleteImg;
        refreshBtnState();
    }
}

void getImagePage::onDeleteTimerOutSlt()
{
    delTimer->stop();
    myMessageBox msgBox(MSG_QUES, delete_all_img, delete_warn,g_mainWidget);

    int ret = msgBox.exec();
    if (ret == QDialog::Accepted) {
        infoWidget->raise();
        infoWidget->setLabelText(myLan.apply_and_wait);
        infoWidget->show();

        //! 删除原图片
//        QProcess process;
//        QString cmd = QString("rm -rf /tmp/*.png");
//        process.start(cmd);
//        process.waitForFinished(100);

        QDir dir("/tmp");
        QStringList filters;
        filters << "*.png";
        QFileInfoList fileLst = dir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::Files);
        foreach (auto tmp, fileLst) {
            if(!tmp.fileName().contains("simulate_")){
                QFile pngFile(tmp.absoluteFilePath());
                pngFile.remove();
            }
        }

        infoWidget->hide();

        for (int i = 0; i < MAX_IMG_NUM; i++) {
            m_sNameOfImg[i] = "";
            m_picName[i] = "";
        }

        isDeleteImg = false;
        imgIndex = 0;
        refreshImgDisPlay();
        refreshBtnState();
    }
}

//! 导入图片
void getImagePage::onImgImportBtnPressedSlt()
{
    bool bIsImg = true;
    QString strImg;
    QImage  image;
    QString shell;
    QStringList strList,strListDir;
    strList  << "*.png";
    int imgCount = 0;

    if (!gFlow.mountUDisk()) {
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_found_udisk));
        infoWidget->delayShow();
        gFlow.sleep(1);
        infoWidget->hide();
        return;
    }

    QString pngDirStr = QString("%1Meyer_mFV").arg(MOUNT_UDISK_PATH);
    QDir pngDir(pngDirStr);
    pngDir.setFilter(QDir::NoDotAndDotDot|QDir::Dirs);
    if(pngDir.entryList().isEmpty() == true){
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_found_picture));
        infoWidget->showSecs();
        gFlow.umountUDisk(MOUNT_UDISK_PATH);
        return ;
    }

    QString selectDir;
    SelectFileDialog *dialog = new SelectFileDialog(g_mainWidget);
    dialog->setDir(pngDirStr,strListDir);
    if (dialog->exec() == QDialog::Accepted) {
        selectDir = pngDirStr+QString("/")+dialog->getFileName();
        QDir selDir(selectDir);
        /*if(selDir.entryList(strList).isEmpty()){
            infoWidget->setLabelText("路径下无图片文件");
            infoWidget->showSecs();
            gFlow.umountUDisk();
            return;
        }else*/{
            infoWidget->raise();;
            infoWidget->setLabelText(myLan.apply_and_wait);
            infoWidget->show();

            //拷贝图片前先删除图片
            QDir dir("/tmp");
            QStringList filters;
            filters << "*.png";
            QFileInfoList fileLst = dir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::Files);
            foreach (auto tmp, fileLst) {
                if(!tmp.fileName().contains("simulate_")){
                    QFile pngFile(tmp.absoluteFilePath());
                    pngFile.remove();
                }
            }

            for (int i = 0; i < MAX_IMG_NUM; i++) {
                m_sNameOfImg[i] = "";
            }

            for (int i = imgCount; i < MAX_IMG_NUM; i++) {
                m_picName[i] = "";
            }

            QMap<QString, QVector<QString>> mapName2Index;
            QFileInfoList dirLst = selDir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::AllDirs);
            foreach (auto tmp, dirLst){
                QString tmpName = tmp.fileName();
                QVector<QString> &indexVct = mapName2Index[tmpName];

                QDir subDir(selectDir + "/" + tmpName);
                QFileInfoList subFileLst = subDir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::Files);
                foreach (auto subTmp, subFileLst){
                    indexVct.push_back(subTmp.fileName());
                }
            }

            fileLst = selDir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::Files);
            foreach (auto tmp, fileLst){
                mapName2Index[""].push_back(tmp.fileName());
            }

            for (int i = 0; i < MAX_IMG_NUM; i++) {
                for(int j = 0; j < 2; j++){
                    QString picName;
                    if(0 == j){
                        picName = QString("%1.png").arg(i);
                    }else{
                        picName = QString("%1_N.png").arg(i);
                    }

                    QMap<QString, QVector<QString>>::const_iterator m = mapName2Index.constBegin();
                    while (m != mapName2Index.constEnd()) {
                        QString subDirStr = m.key();
                        auto picVct = m.value();
                        for(int n = 0; n < picVct.size(); n++){
                            if(picVct[n] == picName){
                                QString tmpName = (subDirStr != "" ? (QString("/"+subDirStr+"/" )) : QString("/"))+picVct[n];
                                strImg =  selectDir+tmpName;
                                bIsImg = image.load(strImg);
                                if (bIsImg) {
                                    QFile file(strImg);
                                    file.copy(QString("/tmp/%1").arg(picName));
//                                    shell = QString("cp -f %1 /tmp/%2").arg(strImg).arg(picName);
//                                    qDebug("%s", qPrintable(shell));
//                                    system(qPrintable(shell));

//                                    gFlow.sleep(1);

                                    m_picName[imgCount] = "/tmp/" +picName;
                                    m_sNameOfImg[imgCount] = subDirStr;
                                    imgCount++;

                                    infoWidget->setLabelText(QString("%1(%2%)").arg(myLan.apply_and_wait).arg(100*imgCount/MAX_IMG_NUM));
                                } else {
                                    break;
                                }
                            }
                        }

                        ++m;
                    }
                }
            }

            if(imgCount == 0){
                infoWidget->setLabelText(QString("%1!").arg(myLan.import_pic_failed));
            }else{
                infoWidget->setLabelText(QString("%1(%2)!").arg(myLan.import_pic_sucess).arg(imgCount));
                imgIndex = imgCount;
            }

            system("sync");
            gFlow.sleep(2);

            gFlow.umountUDisk(MOUNT_UDISK_PATH);

            infoWidget->setLabelText(QString("%1....").arg(myLan.refresh));

            refreshImgDisPlay();

            infoWidget->delayHide();
        }
    }else{
        return;
    }
    return;
}

/**
 * @brief getImagePage::确认拍照
 */
void getImagePage::onCaptureBtnPressedSlt()
{
    bool bover = false;
    if((isCaptureAllView && (imgIndex >= MAX_IMG_NUM-4))
        || imgIndex >=MAX_IMG_NUM){
        bover = true;
    }

    QString tipstr = bover? overmaximg:start_cam;

    myMessageBox msgBox(MSG_QUES, tipstr, start_cam, g_mainWidget);
    int ret = msgBox.exec();
    if ((ret != QDialog::Accepted) || bover) {
        return;
    }

    setBigPic(false);//! 设置采集行数
    getImage();

    QScrollBar *pVerScrollBar = m_pScroll->verticalScrollBar();
    pVerScrollBar->setValue(pVerScrollBar->maximum());
}

/**
 * @brief getImagePage::导出图片
 */
void getImagePage::onImgExportBtnPressedSlt()
{
    QDir pngDir("/tmp");
    QStringList filter;
    filter<<"*.png";
    if(pngDir.entryInfoList(filter).count()==0) {//无图片供导出
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.no_found_picture));
        infoWidget->show();
        gFlow.sleep(2);
        infoWidget->hide();
        return;
    }

    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_export_picture), export_img, g_mainWidget);
    int ret = msgBox.exec();
    if (ret == QDialog::Accepted) {
        if (!gFlow.mountUDisk()) {
            infoWidget->raise();
            infoWidget->setLabelText(myLan.no_found_udisk);
            infoWidget->show();
            gFlow.sleep(1);
            infoWidget->hide();
            return;
        }
        infoWidget->raise();
        infoWidget->setLabelText(myLan.apply_and_wait);
        infoWidget->show();
        QDateTime time = QDateTime::currentDateTime();
        QString pngDirStr = QString("%1%2/%3").arg(MOUNT_UDISK_PATH)
                                .arg("Meyer_mFV").arg(time.toString("yyyyMMddhhmmss"));
        QDir pngDir(pngDirStr);
        if (!pngDir.exists()) {
            pngDir.mkpath(pngDirStr);
        }

        bool bRet = true;
        //分目录保存
        for(int i = 0; i < MAX_IMG_NUM && (m_picName[i] != ""); i++){
            if(m_sNameOfImg[i] != ""){
                QString subDirStr = pngDirStr + "/" + m_sNameOfImg[i];
                QDir subDir(subDirStr);
                if (!subDir.exists()) {
                    subDir.mkpath(subDirStr);
                }

                QFileInfo fileInfo(m_picName[i]);
                QFile picFile(m_picName[i]);
                QString newFile = subDirStr+"/"+fileInfo.fileName();
                bool bRetTmp = picFile.copy(newFile);

                if(!bRetTmp)
                    bRet = false;
            }else{
                QFileInfo fileInfo(m_picName[i]);
                QFile picFile(m_picName[i]);
                bool bRetTmp = picFile.copy(pngDirStr+"/"+fileInfo.fileName());

                if(!bRetTmp)
                    bRet = false;
            }
        }


        gFlow.sleep(1);

        system("sync");
        gFlow.sleep(3);

        if(!bRet) {
            infoWidget->setLabelText(QString("%1!").arg(myLan.export_failed));
        }else {
            infoWidget->setLabelText(QString("%1!").arg(myLan.export_sucess));
        }

        gFlow.umountUDisk(MOUNT_UDISK_PATH);

        infoWidget->delayHide();
    }
}

/**
 * @brief getImagePage::触发阈值设置
 */
void getImagePage::onThreadValueBtnPressedSlt()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 255, threadValueBtn->text().toDouble(), threadValueLbl->text(), g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        struGsh.nThreadValue = inputPanel->getValue();
        threadValueBtn->setText(QString("%1").arg(struGsh.nThreadValue));
    }
}

/**
 * @brief getImagePage::采集行数下发
 */
void getImagePage::onSampleBtnPressedSlt()
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, BIG_IMAGE_HEIGHT, sampleLineBtn->text().toDouble(), sampleLineLbl->text(), g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        n_sample = inputPanel->getValue();
        m_picHeight = n_sample;
        sampleLineBtn->setText(QString("%1").arg(n_sample));
        gFlow.sendSampleLinePara(struGsh.nAnalyzeIndex, n_sample);
    }
}

/**
 * @brief getImagePage::刷新拍照相机显示
 */
void getImagePage::refreshViewBoxDis()
{
    for (int i = 0; i < MAX_CAM_NUM; i++) {
        viewBox[i]->setChecked(false);
        viewBox[i]->setText(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        if(i < gFlow.getMachineCamNum()){
            viewBox[i]->show();
        }else{
            viewBox[i]->hide();
        }
    }

    viewBox[n_CapIndex]->setChecked(true);

}

/**
 * @brief getImagePage::刷新图片显示
 */
void getImagePage::refreshImgDisPlay()
{
    QString imgName;
    QImage image;
    bool bIsImg;

    for (int i = 0; i < MAX_IMG_NUM; i++) {
    //        imgName.sprintf("E:/tmp/%d.png",i);
            bIsImg = image.load(m_picName[i]);
            if (bIsImg) {
                QSize tmp = image.size();
                int width = tmp.width();
                int height = tmp.height();
                if( width> 370 ||  height> 370){
                    if(width > height){
                        tmp.setWidth(370);
                        tmp.setHeight(370*height/width);
                    }else{
                        tmp.setHeight(370);
                        tmp.setWidth(370*width/height);
                    }
                }

                imgDisBtn[i]->setIconSize(tmp);
                imgDisBtn[i]->setIcon(QPixmap::fromImage(image));
                imgDisBtn[i]->setEnabled(true);
                imgItemBtn[i]->setText(QString("%1-%2").arg(i+1).arg(m_sNameOfImg[i]));
                showImg(i, true);
            } else {
                imgDisBtn[i]->setIcon(QPixmap());
                imgDisBtn[i]->setEnabled(false);
                imgItemBtn[i]->setText("");
                showImg(i, false);
            }
      }
}

/**
 * @brief getImagePage::刷新按钮
 */
void getImagePage::refreshBtnState()
{
    QString strDelPressed = QString("color:white;background-color:#00f434;font-size:22px;");
    QString strDelUnPressed = QString("background-color:white;font-size:22px;");

    QString strPressed = QString("color:white;background-color:#007454;");
    QString strUnPressed = QString("background-color:white;");

    if (isDeleteImg) {
        deleteBtn->setStyleSheet(strDelPressed);
    } else {
        deleteBtn->setStyleSheet(strDelUnPressed);
    }

    if (TYPE_RGB == n_ImgType) {
        imgTypeBtn[TYPE_RGB]->setStyleSheet(strPressed);
        imgTypeBtn[TYPE_NGB]->setStyleSheet(strUnPressed);
//        imgTypeBtn[TYPE_NNN]->setStyleSheet(strUnPressed);
    } else if(TYPE_NGB == n_ImgType){
        imgTypeBtn[TYPE_RGB]->setStyleSheet(strUnPressed);
        imgTypeBtn[TYPE_NGB]->setStyleSheet(strPressed);
//        imgTypeBtn[TYPE_NNN]->setStyleSheet(strUnPressed);
    }/* else {
        imgTypeBtn[TYPE_RGB]->setStyleSheet(strUnPressed);
        imgTypeBtn[TYPE_NGB]->setStyleSheet(strUnPressed);
        imgTypeBtn[TYPE_NNN]->setStyleSheet(strPressed);
    }*/

    nnnBox->setChecked(m_bNNNEnable);
}

/**
 * @brief getImagePage::开关下料按钮
 */
void getImagePage::onFeederBtnPressedSlt()
{
    struGsh.nFeederState = !struGsh.nFeederState;

    refreshFeederBtnState();

    gFlow.sendFeederPara(struGsh.nAnalyzeIndex);

    gFlow.sendOnOff(struGsh.nAnalyzeIndex);

    g_serial->SwMotorOnOff(struGsh.nFeederState);
}

/**
 * @brief getImagePage::去背景按钮
 */
void getImagePage::onGetBakBtnPressedSlt()
{
    struGsh.nGetBak = ! struGsh.nGetBak;
    getBak->setChecked(struGsh.nGetBak);
}

/*!
 * \brief 槽: 同步拍照
 */
void getImagePage::onCoCapChkPressedSlt()
{
    isCaptureAllView = coCapchk->isChecked();
}

/**
 * @brief getImagePage::拍照类型选择
 * @param index
 */
void getImagePage::onImgTypeBtnPressedSlt(int index)
{
    n_ImgType = index;
    refreshBtnState();
}

void getImagePage::onNNNBoxPressedSlt()
{
    m_bNNNEnable = nnnBox->isChecked();
    if(m_bNNNEnable){
        imgTypeComb->setDisabled(false);
    }else{
        imgTypeComb->setDisabled(true);
    }
}

/**
 * @brief getImagePage::onImageDataReceived
 * @param ba
 * @param dataLength
 */
void getImagePage::onImageDataReceived(QByteArray ba, int dataLength,int nUnit)
{
    if(!gFlow.bHighResolution(struGsh.nAnalyzeIndex, nUnit)){
        if(SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnit]){
            memcpy(gImageData+n_ImageCount*SENSOR_PIXELS_320*IM01_MAT_BYTE_NUM, ba, dataLength);
        }else{
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[nUnit] == 0){//! 6波段
                memcpy(gImageData+n_ImageCount*SENSOR_PIXELS_640*IM01_MAT_BYTE_NUM, ba, dataLength);
            }else{
                memcpy(gImageData+n_ImageCount*SENSOR_PIXELS_640*IM03_MAT_BYTE_NUM, ba, dataLength);
            }
        }
    }else{
        memcpy(gImageData_2048+n_ImageCount*SENSOR_PIXELS_2048*MAT_BYTE_NUM, ba, dataLength);
    }
    n_ImageCount++;
    infoWidget->setLabelText(str_cap + QString("(%1%)").arg(n_ImageCount*100/m_picHeight));
}

/**
 * @brief getImagePage::getImage
 */
void getImagePage::getImage()
{
    QList <CaptureConfig> qlCaptureConfig;

//    emit disconnectSig(struGsh.nAnalyzeIndex);

    setCaptureList(qlCaptureConfig);

    sendCapturePara(qlCaptureConfig);

    int index = 0;
    for(CaptureConfig &tmp : qlCaptureConfig) {
        index++;
        infoWidget->setLabelText(QString("%1%2/%3").arg(myLan.capture).arg(index).arg(qlCaptureConfig.count()));
        infoWidget->raise();
        infoWidget->show();
        processImgDataFromCamera(tmp);
    }

    processCaptureResult(qlCaptureConfig);

    //    emit connectSig(struGsh.nAnalyzeIndex);
}

void getImagePage::setCaptureList(QList<CaptureConfig> &qlCaptureConfig)
{
    QList<int> capList;
    CaptureConfig config;

    if (isCaptureAllView) {
        for(int i=0; i<gFlow.getMachineCamNum();i++){
            capList << i;
        }
    } else {
        capList << n_CapIndex;
    }

    foreach (int nUnit, capList) {
        config.nUnitAddr = nUnit;
        config.dataDepth = m_picHeight;

        bool bIM01 = SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnit];
        bool bIM03 = SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnit];
        config.nProtoType = (bIM01||bIM03) ? PROTOCAL_2: PROTOCAL_1;
        config.result = ERR_SUCCESS;

        if(bIM01){
            config.dateLen = config.dataDepth*SENSOR_PIXELS_320*IM01_MAT_BYTE_NUM;
        }else if(bIM03){
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[nUnit] == 0){//! 6波段
                config.dateLen = config.dataDepth*SENSOR_PIXELS_640*IM01_MAT_BYTE_NUM;
            }else{
                config.dateLen = config.dataDepth*SENSOR_PIXELS_640*IM03_MAT_BYTE_NUM;
            }
        }else{
            config.dateLen = config.dataDepth*SENSOR_PIXELS_1024*COLOR_TYPE_NUM;
        }

        qlCaptureConfig << config;
    }
}

void getImagePage::sendCapturePara(QList<CaptureConfig> &qlCaptureConfig)
{
//    gFlow.sendCtrlSignalMode(struGsh.nAnalyzeIndex, 1);
//    gFlow.msleep(100);
    foreach (CaptureConfig tmp, qlCaptureConfig) {
        gFlow.sendImgWaveReq(struGsh.nAnalyzeIndex, tmp.nUnitAddr, 0, 0, tmp.dataDepth-1, false, struGsh.nGetBak, true);
    }
}

/**
 * @brief getImagePage::processImgDataFromCamera
 * @param index
 */
void getImagePage::processImgDataFromCamera(CaptureConfig &captureConfig)
{
//    int dataLenPerFrame = SENSOR_PIXELS_2048*COLOR_TYPE_NUM;
//    int dataLen = captureConfig.dateLen;
    int count = 0;
    int stripResult = 0;
    char cdata = 1;
    int nCmd = CMD_IMG_UPLOAD;
    QByteArray ba;
//    QString str;

    struGsh.nIsCapaturing = true;
    n_ImageCount = 0;
    ba = gFlow.packet_TypeA_NetData(CAMERA_CK, MODULE_CAM, captureConfig.nUnitAddr, nCmd, &cdata, 1);
    g_udpSocket[struGsh.nAnalyzeIndex]->writeDatagram(ba);
    str_cap = infoWidget->getLabelText();

    bool bTimeout = false;
    int lastNum = n_ImageCount;
    //! 开始异步等待
#ifndef Q_OS_WIN
    while(n_ImageCount < m_picHeight) {
        gFlow.sleep(1);
        count++;

        if(n_ImageCount == lastNum && count > 10){
            bTimeout = true;
            break;
        }
        lastNum = n_ImageCount;
    }
#endif
    infoWidget->hide();
    struGsh.nIsCapaturing = false;
    if (bTimeout) {
        captureConfig.result =  ERR_TIMEOUT;
        printf("timeout for frame[0].\n");
        fflush(0);
        goto EXIT;
    } else if (n_ImageCount != m_picHeight){
        captureConfig.result =  ERR_DATA;
        printf("getImg err!\n");
        fflush(0);
        goto EXIT;
    }

    if(!gFlow.bHighResolution(struGsh.nAnalyzeIndex, captureConfig.nUnitAddr))
        stripResult = stripFrameData(gImageData, captureConfig.nUnitAddr);
    else{
        stripResult = stripFrameData_2048(gImageData_2048, captureConfig.nUnitAddr);
    }

EXIT:
    if(ERR_SUCCESS != captureConfig.result){
        gFlow.sendBoardRevise(struGsh.nAnalyzeIndex, captureConfig.nUnitAddr);
    }

    n_ImageCount = 0;
    memset(gImageData, 0, BIG_IMAGE_HEIGHT*SENSOR_PIXELS_640*IM01_MAT_BYTE_NUM);
    memset(gImageData_2048, 0, BIG_IMAGE_HEIGHT*SENSOR_PIXELS_2048*MAT_BYTE_NUM);
}

/**
 * @brief getImagePage::processCaptureResult
 */
bool getImagePage::processCaptureResult(QList<CaptureConfig> &qlCaptureConfig)
{

    QString text;
    QString enterStr;
    bool hasFailed = false;

    #ifdef Q_OS_UNIX
        enterStr = "\n";
    #else
        enterStr = "\r\n";
    #endif

    int index = 0;
    foreach(CaptureConfig tmp, qlCaptureConfig) {
        index++;
        if (tmp.result != ERR_SUCCESS) {
            hasFailed = true;
        }
        switch (tmp.result) {
        case ERR_SUCCESS:
            text += QString("%1/%2: %3").arg(index).arg(qlCaptureConfig.count()).arg(myLan.sucess);
            break;
        case ERR_TIMEOUT:
            text += QString("%1/%2: %3").arg(index).arg(qlCaptureConfig.count()).arg(myLan.timeout);
            break;
        case ERR_DATA:
            text += QString("%1/%2: %3(%4)").arg(index).arg(qlCaptureConfig.count()).arg(myLan.failed).arg(tmp.result);
            break;
        case ERR_MALLOC_FAILED:
            text += QString("%1/%2: %3(%4)").arg(index).arg(qlCaptureConfig.count()).arg(myLan.failed).arg(tmp.result);
            break;
        case ERR_CONFIG:
            text += QString("%1/%2: %3(%4)").arg(index).arg(qlCaptureConfig.count()).arg(myLan.failed).arg(tmp.result);
            break;
        }
        text += enterStr;
    }
    infoWidget->hide();

//    gFlow.sendCtrlSignalMode(struGsh.nAnalyzeIndex, 0);

    if (hasFailed) {
        myMessageBox msgBox(MSG_TIPS, text, "", g_mainWidget);
        msgBox.exec();
    }

    return !hasFailed;
}

/**
 * @brief getImagePage::stripFrameData
 * @param data
 * @return
 */
int getImagePage::stripFrameData(char *data, int nUnitAddr)
{
    int retCode = 0;
    if (data == NULL) {
        retCode = -1;
        return retCode;
    }

    if(SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
        char *tmpData = new char[SENSOR_PIXELS_320*m_picHeight*3];
        if(nullptr == tmpData){
            return -3;
        }

        QVector<int> imgTypeVct;
        imgTypeVct.push_back(n_ImgType);
        if(m_bNNNEnable)
            imgTypeVct.push_back(TYPE_NNN);

        QPair<QPair<QString, QString>, QString> name = findImageName();

        int tmpImgType = 0;
        for(int index = 0; index < imgTypeVct.size(); index++){
            tmpImgType = imgTypeVct[index];

            if(TYPE_RGB == tmpImgType || TYPE_NGB == tmpImgType){
                int bias = tmpImgType == TYPE_RGB?2:5;

                for(int i = 0; i < m_picHeight; i++) {
                    for(int j = 0; j < SENSOR_PIXELS_320; j++) {
                        tmpData[i*SENSOR_PIXELS_320*3+j*3]    = data[i*SENSOR_PIXELS_320*8 + j*8+bias];  //r-1/n-0
                        tmpData[i*SENSOR_PIXELS_320*3+j*3+1]  = data[i*SENSOR_PIXELS_320*8 + j*8+3];  //g
                        tmpData[i*SENSOR_PIXELS_320*3+j*3+2]  = data[i*SENSOR_PIXELS_320*8 + j*8+4];  //b
                    }
                }
            } else { //NNN
                for(int i = 0; i < m_picHeight; i++) {
                    for(int j = 0; j < SENSOR_PIXELS_320; j++) {
                        tmpData[i*SENSOR_PIXELS_320*3+j*3]    = data[i*SENSOR_PIXELS_320*8 + j*8+5];  //n-0
                        tmpData[i*SENSOR_PIXELS_320*3+j*3+1]  = data[i*SENSOR_PIXELS_320*8 + j*8+6];  //n-0
                        tmpData[i*SENSOR_PIXELS_320*3+j*3+2]  = data[i*SENSOR_PIXELS_320*8 + j*8+7];  //n-0
                    }
                }
            }

            if (imgIndex >= MAX_IMG_NUM) {
                break;
//                imgIndex = MAX_IMG_NUM-1;
            }

            QString fileName = (0==index) ? name.first.first : name.first.second;
            bool ret = saveBMPFile(fileName, SENSOR_PIXELS_320, m_picHeight, (uchar *)tmpData);

            if (!ret) {
                retCode = -2;
            }

            if (retCode == 0) {
                displayImgInfo(nUnitAddr, fileName, tmpImgType);
            }
        }

        if(nullptr != tmpData){
            delete[] tmpData;
            tmpData = nullptr;
        }
    }else if(SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
        char *tmpData = new char[SENSOR_PIXELS_640*m_picHeight*3];
        if(nullptr == tmpData){
            return -3;
        }

        QVector<int> imgTypeVct;
        imgTypeVct.push_back(n_ImgType);
        if(m_bNNNEnable){
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[nUnitAddr]==0){//! 6波段
                imgTypeVct.push_back(TYPE_NNN);
            }else{//! 12波段
                if(imgTypeComb->currentIndex() ==0){
                    for(int i=2; i<5; i++){
                        imgTypeVct.push_back(i);
                    }
                }else{
                    imgTypeVct.push_back(TYPE_NNN);
                }
            }
        }

        QPair<QPair<QString, QString>, QString> name = findImageName();

        int tmpImgType = 0;
        for(int index = 0; index < imgTypeVct.size(); index++){
            tmpImgType = imgTypeVct[index];

            if(TYPE_RGB == tmpImgType || TYPE_NGB == tmpImgType){
                int bias = tmpImgType == TYPE_RGB?2:5;
                for(int i = 0; i < m_picHeight; i++) {//! RGB
                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[nUnitAddr] == 0){//! 6波段
                        for(int j = 0; j < SENSOR_PIXELS_640; j++) {
                            tmpData[i*SENSOR_PIXELS_640*3+j*3]    = data[i*SENSOR_PIXELS_640*8 + j*8+bias];  //! R
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+1]  = data[i*SENSOR_PIXELS_640*8 + j*8+3];     //! G
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+2]  = data[i*SENSOR_PIXELS_640*8 + j*8+4];    //!  B
                        }
                    }else{
                        for(int j = 0; j < SENSOR_PIXELS_640; j++) {
                            tmpData[i*SENSOR_PIXELS_640*3+j*3]    = data[i*SENSOR_PIXELS_640*14 + j*14+bias];  //! R
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+1]  = data[i*SENSOR_PIXELS_640*14 + j*14+3];     //! G
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+2]  = data[i*SENSOR_PIXELS_640*14 + j*14+4];    //!  B
                        }
                    }
                }
            } else { //! NNN
                for(int i = 0; i < m_picHeight; i++) {
                    if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWaveSeg[nUnitAddr] == 0){//! 6波段
                        for(int j = 0; j < SENSOR_PIXELS_640; j++) {//! 6波段
                            tmpData[i*SENSOR_PIXELS_640*3+j*3]    = data[i*SENSOR_PIXELS_640*8 + j*8+5];  //n-0
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+1]  = data[i*SENSOR_PIXELS_640*8 + j*8+6];  //n-0
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+2]  = data[i*SENSOR_PIXELS_640*8 + j*8+7];  //n-0
                        }
                    }else{//! 12波段
                        int nbias = 5;
                        if(imgTypeComb->currentIndex() ==1){
                            nbias = 5;
                        }else if(imgTypeComb->currentIndex() ==2){
                            nbias = 8;
                        }else if(imgTypeComb->currentIndex() ==3){
                            nbias = 11;
                        }
                        for(int j = 0; j < SENSOR_PIXELS_640; j++) {
                            tmpData[i*SENSOR_PIXELS_640*3+j*3]    = data[i*SENSOR_PIXELS_640*14 + j*14+nbias];  //n-0
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+1]  = data[i*SENSOR_PIXELS_640*14 + j*14+nbias+1];  //n-0
                            tmpData[i*SENSOR_PIXELS_640*3+j*3+2]  = data[i*SENSOR_PIXELS_640*14 + j*14+nbias+1];  //n-0
                        }
                    }
                }
            }

            if (imgIndex >= MAX_IMG_NUM) {
                break;
            }

            QString fileName,strname;
            switch (index) {
            case 0:
                fileName = name.first.first;
                break;
            case 1:
                fileName = name.first.second;
                break;
            case 2:
                strname = name.first.second;
                fileName = strname.replace(7,1,"M");
                break;
            case 3:
                strname = name.first.second;
                fileName = strname.replace(7,1,"P");
                break;
            default:
                break;
            }
            bool ret = saveBMPFile(fileName, SENSOR_PIXELS_640, m_picHeight, (uchar *)tmpData);

            if (!ret) {
                retCode = -2;
            }

            if (retCode == 0) {
                displayImgInfo(nUnitAddr, fileName, tmpImgType);
            }

        }

        if(nullptr != tmpData){
            delete[] tmpData;
            tmpData = nullptr;
        }
    }


    return retCode;
}

/**
 * @brief getImagePage::图片显示
 * @param captureConfig
 */
void getImagePage::displayImgInfo(int nUnitAddr, QString fileName, int nImgType)
{
    bool bIsImg = true;
    QString viewImg;
    QImage  image;

    bool bIM01 = false;
    if(SENSOR_IM01 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]
        || SENSOR_IM04 == struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->sensorType[nUnitAddr]){
        bIM01 = true;
    }

    QString strImgType;
    if(TYPE_RGB == nImgType)
        strImgType = "RGB";
    else if(TYPE_NGB == nImgType)
        strImgType = "NGB";
    else if(TYPE_NNN == nImgType)
        strImgType = bIM01 ? "NMP": "NNN";
    else if(nImgType ==3)
        strImgType = bIM01 ? "QLJ": "NNN";
    else if(nImgType ==3)
        strImgType = bIM01 ? "KHI": "NNN";

    bIsImg = image.load(fileName);


    viewImg = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[nUnitAddr] + QString("(%1)").arg(strImgType);

    m_sNameOfImg[imgIndex] = viewImg;
    m_picName[imgIndex] = fileName;

    if(bIsImg) {
        QSize tmp = image.size();
        int width = tmp.width();
        int height = tmp.height();
        if( width> 370 ||  height> 370){
            if(width > height){
                tmp.setWidth(370);
                tmp.setHeight(370*height/width);
            }else{
                tmp.setHeight(370);
                tmp.setWidth(370*width/height);
            }
        }

        imgDisBtn[imgIndex]->setIconSize(tmp);
        imgDisBtn[imgIndex]->setIcon(QPixmap::fromImage(image));
        imgDisBtn[imgIndex]->setEnabled(true);
        imgItemBtn[imgIndex]->setText(QString("%1-%2").arg(imgIndex+1).arg(m_sNameOfImg[imgIndex]));

        showImg(imgIndex, true);
    } else {
        imgDisBtn[imgIndex]->setIcon(QPixmap());
        imgDisBtn[imgIndex]->setEnabled(false);
        imgItemBtn[imgIndex]->setText("");

        showImg(imgIndex, false);
    }

    imgIndex++;
}

/**
 * @brief getImagePage::saveBMPFile
 * @param index
 * @param width
 * @param height
 * @param rgbData
 */
bool getImagePage::saveBMPFile(QString fileName, int width, int height, uchar *rgbData)
{
    QImage img(rgbData, width, height, QImage::Format_RGB888);

    return img.save(fileName, "PNG", 50);
}

bool getImagePage::saveWideBMPFile(int index, int width, int height, uchar *rgbData)
{
    QString name = "";

    QImage img(rgbData, width, height, QImage::Format_RGB888);

    name = QString("/tmp/%1_wide").arg(index);

    return img.save(QString("%1.png").arg(name), "PNG", 50);
}

/**
 * @brief getImagePage::删除图片
 * @param index-
 */
void getImagePage::deteleCurImg(int index)
{
    QVector<int> numVct;
    QVector<QString> numStr;

    QString tmp;
    for(int i = 0; i < MAX_IMG_NUM; i++){
        if(m_picName[i] != ""){
            tmp = m_picName[i].mid(5);
            int prefixNum = tmp.indexOf(".png");
            tmp = tmp.left(prefixNum);

            int indexNo = tmp.indexOf("_N");
            int indexMo = tmp.indexOf("_M");
            int indexPo = tmp.indexOf("_P");

            if(indexNo != -1){
                numVct.push_back(tmp.left(indexNo).toInt());
                numStr.push_back("_N");
            }else if(indexMo != -1){
                numVct.push_back(tmp.left(indexMo).toInt());
                numStr.push_back("_M");
            }else if(indexPo != -1){
                numVct.push_back(tmp.left(indexPo).toInt());
                numStr.push_back("_P");
            }else{
                numVct.push_back(tmp.toInt());
                numStr.push_back("");
            }
        }
    }

    bool bNumChange = true;
    int curNum = numVct[index];
    for(int i = 0; i< numVct.size() ; i++){
        if(i != index){
            if(numVct[i] == curNum){
                bNumChange = false;
                break;
            }
        }
    }

    QFile pngFile1(m_picName[index]);
    if (pngFile1.exists()) {
        pngFile1.close();
        bool retFile = pngFile1.remove();

        if (!retFile) {
            infoWidget->setLabelText("Failed!");
            gFlow.sleep(1);
            infoWidget->hide();
        }
    }

    if(!bNumChange){
        for(int i = index; i< numVct.size()-1 ; i++){
            m_sNameOfImg[i] = m_sNameOfImg[i+1];
            m_picName[i] = m_picName[i+1];
        }
        m_sNameOfImg[numVct.size()-1] = "";
        m_picName[numVct.size()-1] = "";
    }else{
        for(int i = index+1; i< numVct.size() ; i++){
            QFile pngFile1(m_picName[i]);
            m_picName[i-1] = QString("/tmp/%1%2.png").arg(numVct[i]-1).arg(numStr[i]);
            bool bRet = pngFile1.rename(m_picName[i-1]);

            m_sNameOfImg[i-1] = m_sNameOfImg[i];
        }
        m_sNameOfImg[numVct.size()-1] = "";
        m_picName[numVct.size()-1] = "";
    }

    imgIndex--;

    refreshImgDisPlay();
}

/**
 * @brief getImagePage::刷新开关下料按钮状态
 */
void getImagePage::refreshFeederBtnState()
{
    QString textOnOff;
    QString backgroundColor;
    QIcon iconStr;
    if(struGsh.nFeederState) {
        struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->bFeeder = true;
        analyzeCombox->setEnabled(false);
        textOnOff = "停止下料";
        iconStr = QIcon(":res/png/stop.png");
        backgroundColor = QString("QToolButton {"
                                  "text-align:center; "
                                  "padding:10px;"
                                  "outline: none;"
                                  "background-color: rgb(0,141,75);"
                                  "border-color: #dfdfdf;"
                                  "font: 20px \"Microsoft YaHei\";"
                                  "border-radius: 10;}"
                                  "QToolButton:pressed {"
                                  "background-color: rgb(225, 225, 225);}");
    } else {
        struGsh.analyzeInfo[struGsh.nAnalyzeIndex]->bFeeder = false;
        analyzeCombox->setEnabled(true);
        textOnOff = "开始下料";
        iconStr = QIcon(":res/png/Media_Start.png");
        backgroundColor = "QToolButton {"
                          "text-align: center; "
                          "padding:10px;"
                          "outline: none;"
                          "border-color:#dfdfdf;"
                          "font: 20px \"Microsoft YaHei\";"
                          "background-color: white;"
                          "border-radius: 10;}"
                          "QToolButton:pressed {"
                          "background-color: rgb(225, 225, 225);}";
    }

    feederBtn->setText(textOnOff);
    feederBtn->setIcon(iconStr);
//    feederBtn->setStyleSheet(backgroundColor);
}

void getImagePage::setBigPic(bool bBig)
{
    if(bBig){
        m_picHeight = BIG_IMAGE_HEIGHT;
    }else {
        m_picHeight = n_sample;
    }
}

int getImagePage::stripFrameData_2048(char *data, int nUnitAddr)
{
    int retCode = 0;
    if (data == NULL) {
        retCode = -1;
        return retCode;
    }

//    int pBias = SENSOR_PIXELS_1024;
//    int beginCh = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nPixelBegin[0 == nUnitAddr%2 ? 0 : 1];
//    for (int k = beginCh; k < SENSOR_PIXELS_1024; k++) {
//        if ((data[k*6+1] & 0x01) != 0) {
//            pBias = 0;//第一个物料点在2048的前1024中
//            break;
//        }
//    }

    int pBias = 0;
    int endCh = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nPixelEnd[0 == nUnitAddr%2 ? 0 : 1];
    for (int k = SENSOR_PIXELS_1024; k < endCh; k++) {
        if ((data[k*6+1] & 0x01) != 0) {
            pBias = SENSOR_PIXELS_1024;//第一个物料点在2048的前1024中
            break;
        }
    }

    char *tmpData = new char[SENSOR_PIXELS_1024*m_picHeight*3];
    if(nullptr == tmpData){
        return -3;
    }

//    char *tmpWData = new char[SENSOR_PIXELS_2048*m_picHeight*3];
//    if(nullptr == tmpWData){
//        return -3;
//    }

    QVector<int> imgTypeVct;
    imgTypeVct.push_back(n_ImgType);
    if(m_bNNNEnable)
        imgTypeVct.push_back(TYPE_NNN);

    QPair<QPair<QString, QString>, QString> name = findImageName();

    int tmpImgType = 0;
    for(int index = 0; index < imgTypeVct.size(); index++){
        tmpImgType = imgTypeVct[index];

        if(TYPE_RGB == tmpImgType || TYPE_NGB == tmpImgType) {
            int bias = tmpImgType == TYPE_RGB ? 1 : 0;

            for(int i = 0; i < m_picHeight; i++) {
                for(int j = 0; j < SENSOR_PIXELS_1024; j++) {
                    tmpData[i*SENSOR_PIXELS_1024*3+j*3]    = data[i*SENSOR_PIXELS_2048*6 + (j+pBias)*6+2+bias];  //r-1/n-0
                    tmpData[i*SENSOR_PIXELS_1024*3+j*3+1]  = data[i*SENSOR_PIXELS_2048*6 + (j+pBias)*6+2+2];  //g
                    tmpData[i*SENSOR_PIXELS_1024*3+j*3+2]  = data[i*SENSOR_PIXELS_2048*6 + (j+pBias)*6+2+3];  //b
                }

//                for(int j = 0; j < SENSOR_PIXELS_2048; j++) {
//                    tmpWData[i*SENSOR_PIXELS_2048*3+j*3]    = data[i*SENSOR_PIXELS_2048*6 + (j)*6+2+bias];  //r-1/n-0
//                    tmpWData[i*SENSOR_PIXELS_2048*3+j*3+1]  = data[i*SENSOR_PIXELS_2048*6 + (j)*6+2+2];  //g
//                    tmpWData[i*SENSOR_PIXELS_2048*3+j*3+2]  = data[i*SENSOR_PIXELS_2048*6 + (j)*6+2+3];  //b
//                }
            }
        } else {
            for(int i = 0; i < m_picHeight; i++) {
                for(int j = 0; j < SENSOR_PIXELS_1024; j++) {
                    tmpData[i*SENSOR_PIXELS_1024*3+j*3]    = data[i*SENSOR_PIXELS_2048*6 + (j+pBias)*6+2];  //n
                    tmpData[i*SENSOR_PIXELS_1024*3+j*3+1]  = data[i*SENSOR_PIXELS_2048*6 + (j+pBias)*6+2];  //n
                    tmpData[i*SENSOR_PIXELS_1024*3+j*3+2]  = data[i*SENSOR_PIXELS_2048*6 + (j+pBias)*6+2];  //n
                }

//                for(int j = 0; j < SENSOR_PIXELS_2048; j++) {
//                    tmpWData[i*SENSOR_PIXELS_2048*3+j*3]    = data[i*SENSOR_PIXELS_2048*6 + (j)*6+2];  //n
//                    tmpWData[i*SENSOR_PIXELS_2048*3+j*3+1]  = data[i*SENSOR_PIXELS_2048*6 + (j)*6+2];  //n
//                    tmpWData[i*SENSOR_PIXELS_2048*3+j*3+2]  = data[i*SENSOR_PIXELS_2048*6 + (j)*6+2];  //n
//                }
            }
        }

        if (imgIndex >= MAX_IMG_NUM) {
            break;
//            imgIndex = MAX_IMG_NUM-1;
        }

        QString fileName = (0==index) ? name.first.first : name.first.second;
        bool ret = saveBMPFile(fileName, SENSOR_PIXELS_1024, m_picHeight, (uchar *)tmpData);

//        bool ret2 = saveWideBMPFile(imgIndex, SENSOR_PIXELS_2048, m_picHeight, (uchar *)tmpWData);

        if (!ret /*|| !ret2*/) {
            retCode = -2;
        }

        if (retCode == 0) {
            displayImgInfo(nUnitAddr, fileName, tmpImgType);
        }
    }

    if(nullptr != tmpData){
        delete[] tmpData;
        tmpData = nullptr;
    }

//    if(nullptr != tmpWData){
//        delete[] tmpWData;
//        tmpWData = nullptr;
//    }

    return retCode;
}

void getImagePage::showImg(int imgIndex, bool bShow)
{
    if(imgIndex >= 6){
        if(bShow){
            imgDisBtn[imgIndex]->show();
            imgItemBtn[imgIndex]->show();
        }else{
            imgDisBtn[imgIndex]->hide();
            imgItemBtn[imgIndex]->hide();
        }
    }
}


/*! 获取保存图片的文件名
 * \brief CameraBackPacket::findImageName
 * \return
 */
QPair<QPair<QString,QString>, QString> getImagePage::findImageName()
{
    QPair<QPair<QString, QString>, QString> imageName;
    imageName.first.first = "0.png";
    imageName.first.second = "0_N.png";

    for(int i=0; i<MAX_IMG_NUM; i++){
        if(!QFile::exists(QString("/tmp/%1.png").arg(i))){
            imageName.first.first = QString("/tmp/%1.png").arg(i);
            imageName.first.second = QString("/tmp/%1_N.png").arg(i);
            imageName.second = QString("/tmp/%1.txt").arg(i);
            break;
        }
    }

    return imageName;
}
