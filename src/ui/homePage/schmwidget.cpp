#include "schmwidget.h"
#include "QLayout"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"
#include "common/myinputmethod.h"
#include "params/globalparams.h"
#include "params/define.h"

schmWidget::schmWidget(int curShemIndex, QWidget *parent) : QDialog(parent)
  , m_curSchemIndex(curShemIndex)
{
    initParams();

    createPage();

    connectSigAndSlot();
}

/**
 * @brief schmWidget::初始化参数
 */
void schmWidget::initParams()
{
    if(!(m_curSchemIndex >= 0 && m_curSchemIndex < struCnfg.schemeList.size()))
        m_curSchemIndex = 0;
}

/**
 * @brief schmWidget::搭建界面
 */
void schmWidget::createPage()
{
    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("background-color: rgb(0, 0, 0, 100);");

    m_widget = new QWidget(this);
    m_widget->setWindowFlags(Qt::FramelessWindowHint);

    m_widget->setGeometry((WIN_WID-1000)/2, (WIN_HEI-900)/2, 1000, 900);
    m_widget->setStyleSheet("background-color:white;border-radius: 20px;");
    m_widget->show();

    m_schemCode = struCnfg.schemeList[m_curSchemIndex].code;
    m_schemName = struCnfg.schemeList[m_curSchemIndex].name;

    m_schmLbl = new QLabel(QString("%1：%2").arg(myLan.current_schme).arg(m_schemName));
    m_schmLbl->setFixedWidth(BTN_WID*4);
    QHBoxLayout *titleLay = new QHBoxLayout;
    titleLay->addSpacing(60);
    titleLay->addWidget(m_schmLbl);
    titleLay->addStretch();

    m_homelist = new QListWidget();
    m_homelist->setFixedSize(600, 700);
    m_homelist->setStyleSheet(QString("QListWidget{border:1px solid lightgray; border-radius:10px;outline:0px;"
                                      "background-color:white;font:25px;}")
                              +QString("QListWidget::Item:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen)
                              +QString("QListWidget::Item:first{border-radius:5px}"));
    m_homelist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateHomeList();

    m_copyBtn   = new QPushButton(QString("%1%2").arg(myLan.copy).arg(myLan.schme));
    m_copyBtn->setIcon(QIcon(":/res/png/Edit_Copy.png"));
    m_copyBtn->setIconSize(QSize(50,50));
    m_copyBtn->setFixedSize(BTN_WID+80,BTN_HEI);
    m_copyBtn->setStyleSheet(g_enableBtnStyle);

    m_deleteBtn = new QPushButton(QString("%1%2").arg(myLan.del).arg(myLan.schme));
    m_deleteBtn->setIcon(QIcon(":/res/png/del.png"));
    m_deleteBtn->setIconSize(QSize(50,50));
    m_deleteBtn->setStyleSheet(g_enableBtnStyle);
    m_deleteBtn->setFixedSize(BTN_WID+80,BTN_HEI);

    m_editBtn   = new QPushButton(myLan.rename);
    m_editBtn->setIcon(QIcon(":/res/png/Edit_Rename.png"));
    m_editBtn->setStyleSheet(g_enableBtnStyle);
    m_editBtn->setFixedSize(BTN_WID+80,BTN_HEI);
    m_editBtn->setIconSize(QSize(50,50));

    m_backupBtn = new QPushButton(QString("%1%2").arg(myLan.backup).arg(myLan.schme));
    m_backupBtn->setStyleSheet(g_enableBtnStyle);
    m_backupBtn->setFixedSize(BTN_WID+80,BTN_HEI);
    m_backupBtn->setIcon(QIcon(":/res/png/Action_Backup.png"));
    m_backupBtn->setIconSize(QSize(50,50));

    m_restoreBtn = new QPushButton(QString("%1%2").arg(myLan.restore).arg(myLan.schme));
    m_restoreBtn->setStyleSheet(g_enableBtnStyle);
    m_restoreBtn->setFixedSize(BTN_WID+80,BTN_HEI);
    m_restoreBtn->setIcon(QIcon(":/res/png/Action_Restore.png"));
    m_restoreBtn->setIconSize(QSize(50,50));

    m_closeBtn = new QPushButton(myLan.back);
    m_closeBtn->setFixedSize(BTN_WID+80,BTN_HEI);
    m_closeBtn->setStyleSheet(g_enableBtnStyle);
    m_closeBtn->setIcon(QIcon(":/res/png/Action_Back.png"));
    m_closeBtn->setIconSize(QSize(50,50));

    QVBoxLayout *downLay = new QVBoxLayout;
    downLay->setSpacing(40);
    downLay->addWidget(m_copyBtn);
    downLay->addWidget(m_deleteBtn);
    downLay->addWidget(m_backupBtn);
    downLay->addWidget(m_restoreBtn);
    downLay->addWidget(m_editBtn);
    downLay->addWidget(m_closeBtn);

    QHBoxLayout *hLay = new QHBoxLayout;
    hLay->addWidget(m_homelist);
    hLay->addLayout(downLay);

    QVBoxLayout *gbLay = new QVBoxLayout(m_widget);
    gbLay->addSpacing(30);
    gbLay->addLayout(titleLay);
    gbLay->addSpacing(20);
    gbLay->addLayout(hLay);
    gbLay->addStretch();
}

/**
 * @brief schmWidget::信号槽绑定
 */
void schmWidget::connectSigAndSlot()
{
    connect(m_copyBtn,       SIGNAL(clicked(bool)),          this,  SLOT(onCopyBtnClickedSlt()));
    connect(m_deleteBtn,     SIGNAL(clicked(bool)),          this,  SLOT(onDeleteBtnClickedSlt()));
    connect(m_editBtn,       SIGNAL(clicked(bool)),          this,  SLOT(onEditBtnClickedSlt()));
    connect(m_homelist,      SIGNAL(currentRowChanged(int)), this,  SLOT(onHomeListChangedSlt(int)));
    connect(m_closeBtn,      SIGNAL(pressed()),              this,  SLOT(onCloseSchemeSlt()));
    connect(m_backupBtn,     SIGNAL(pressed()),              this,  SLOT(onBackupSchemeSlt()));
    connect(m_restoreBtn,    SIGNAL(pressed()),              this,  SLOT(onRestoreSchemeSlt()));
}

/**
 * @brief schmWidget::更新首页列表
 */
void schmWidget::updateHomeList()
{
    m_homelist->clear();

    auto &schmLst = struCnfg.schemeList;
    for(int i = 0; i < schmLst.size(); i++) {
        QListWidgetItem *listItem = new QListWidgetItem(schmLst[i].name);
        listItem->setSizeHint(QSize(m_homelist->width()-20,75));
        m_homelist->addItem(listItem);
    }

    if(!(m_curSchemIndex >= 0 && m_curSchemIndex < m_homelist->count()))
        m_curSchemIndex = 0;

    m_homelist->setCurrentRow(m_curSchemIndex);
}

/**
 * @brief schmWidget::复制方案
 */
void schmWidget::onCopyBtnClickedSlt()
{
    if (m_homelist->count() == MAX_TARGET_SCHM) {
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1%2!").arg(myLan.over_max_schme_num).arg(MAX_TARGET_SCHM));
        infoWidget->showSecs();
        return;
    }

    myMessageBox msgBox(MSG_QUES,QString("%1?").arg(myLan.confirm_copy_schme), m_copyBtn->text(), g_mainWidget);
    int ret = msgBox.exec();

    if(ret == QDialog::Accepted) {
        bool bCopyOK = false;
        QString curDate;
        for(int i = 2; i <= MAX_TARGET_SCHM; ++i) {
            QString schmName = QString("%1_%2").arg(m_homelist->currentItem()->text()).arg(i);

            if (!gFlow.findSchmFromList(schmName)) {
                //! 边缘服务器方案+1
                int nSchm = m_homelist->currentRow();

                curDate = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
                schemeInfo scheme;
                scheme.name = schmName;
                scheme.code = curDate;
                struCnfg.schemeList.push_back(scheme);

                struCnfg.nSchemTotal++;

                for (int k = 0; k < struCnfg.analyzeNum; k++) {
                    QString analyzeFile = QString("%1_%2").arg(CNF_ANALYZE).arg(k);
                    QString analyze_proname = struCnfg.analyzeInfo[k]->anlyze_schemeList[nSchm].code;
                    QString new_analyze_proname =  QString("%1_%2").arg(analyzeFile).arg(curDate);

                    QFile file_analyze;
                    file_analyze.copy(analyze_proname, new_analyze_proname);

                    struCnfg.analyzeInfo[k]->anlyze_schemeList[struCnfg.nSchemTotal-1].code = new_analyze_proname;
                }

                gFlow.saveGlobalParams(CNF_GLOBAL);

                updateHomeList();
                bCopyOK = true;
                break;
            }
        }
        if (!bCopyOK){
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.copy_schme_failed));
            infoWidget->showSecs();
        }else{
            //! 创建方案数据统计表
            g_sqliteInter->createTable(curDate);

            //! 上传物联网
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
                QJsonObject staicsjson;
                staicsjson["Scheme"] = true;
                g_Lotsocket->uploadInfo(staicsjson);
            }
        }
    }
}

/**
 * @brief schmWidget::删除方案
 */
void schmWidget::onDeleteBtnClickedSlt()
{
    QString file_name = "";
    bool isChange = false;
    bool bCurschme = false;
    int curRow = m_homelist->currentRow();

    if(curRow == struCnfg.nCntScheme){
        bCurschme = true;
    }

    if (struCnfg.nSchemTotal <= 1 || bCurschme) {
        infoWidget->raise();
        infoWidget->setLabelText(myLan.forbid_del_schme);
        infoWidget->showSecs();
        return;
    }
    myMessageBox msgBox(MSG_QUES, QString("%1").arg(myLan.delschme_And_cleandatadb)
                        , QString("%1").arg(myLan.confirm_del_schme), g_mainWidget);
    int ret = msgBox.exec();
    if(ret == QDialog::Accepted){
        QString schmcode =struCnfg.schemeList[curRow].code;

        //! 先删除边缘服务器器下挂的方案
        gFlow.delSchmFromList(curRow);

        //! 再删除所有物料下分析仪和设备对应的方案
        for (int j = 0; j < struCnfg.analyzeNum; j++) {
            QString analyze_proname = struCnfg.analyzeInfo[j]->anlyze_schemeList[curRow].code;
            QFile file_analyze;
            file_analyze.remove(analyze_proname);

            for (int k = curRow; k < struCnfg.nSchemTotal-1; k++) {
                struCnfg.analyzeInfo[j]->anlyze_schemeList[k].code = struCnfg.analyzeInfo[j]->anlyze_schemeList[k+1].code;
            }

            for(int k=struCnfg.nSchemTotal; k<MAX_TARGET_SCHM; k++){
                struCnfg.analyzeInfo[j]->anlyze_schemeList[k].code = QString("NULL");
            }
        }

        if (curRow == struCnfg.nCntScheme) {
            struCnfg.nCntScheme = 0;
            isChange = true;
        } else if (curRow < struCnfg.nCntScheme){
            struCnfg.nCntScheme--;
            isChange = true;
        }

        struCnfg.nSchemTotal--;

        if (isChange) {
            for(int i=0; i< struCnfg.analyzeNum; i++){
                //! 方案参数加载
                file_name = struCnfg.analyzeInfo[i]->anlyze_schemeList[struCnfg.nCntScheme].code;
                gFlow.getAnalyzeProfileParams(&struCnfg.analyzeInfo[i]->analyzeProfile, file_name);
            }
        }
        updateHomeList();

        g_sqliteInter->dropTable(schmcode);

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
            QJsonObject staicsjson;
            staicsjson["Scheme"] = true;
            g_Lotsocket->uploadInfo(staicsjson);
        }
    }
}

/**
 * @brief schmWidget::重命名方案
 */
void schmWidget::onEditBtnClickedSlt()
{
    QString str = QString("%1?").arg(myLan.confirm_rename_schme);
    myMessageBox msgBox(MSG_QUES, str, m_editBtn->text(), g_mainWidget);
    int ret = msgBox.exec();
    if(ret == QDialog::Accepted) {
        QString sCurSchmName = m_homelist->currentItem()->text();
        myInputMethod input(myLan.input_schme_name, sCurSchmName, g_mainWidget);

        int ret = input.exec();
        if (ret == QDialog::Accepted) {
            QString str = input.getText().trimmed();
            if(str.compare("") == 0) {
                infoWidget->raise();
                infoWidget->setLabelText(myLan.invalid_schme_name);
                infoWidget->showSecs();
                return;
            }
            if(!gFlow.inputStrIsValid(str)) {
                infoWidget->raise();
                infoWidget->setLabelText(myLan.invalid_schme_name);
                infoWidget->showSecs();
                return;
            }
            if(gFlow.findSchmFromList(input.getText())) {
                infoWidget->raise();
                infoWidget->setLabelText(myLan.invalid_schme_name);
                infoWidget->showSecs();
                return;
            }
            m_homelist->currentItem()->setText(str);
            gFlow.renameSchmName(m_homelist->currentRow(), str);

            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
                QJsonObject staicsjson;
                staicsjson["Scheme"] = true;
                g_Lotsocket->uploadInfo(staicsjson);
            }
        }
    }
}

/**
 * @brief schmWidget::列表切换
 */
void schmWidget::onHomeListChangedSlt(int index)
{
    if(index < 0)
        return;

    m_curSchemIndex = index;
}

/**
 * @brief schmWidget::关闭当前页面
 */
void schmWidget::onCloseSchemeSlt()
{
    accept();
}

/*!
 * \brief 备份方案
 */
void schmWidget::onBackupSchemeSlt()
{
    if (m_homelist->count() == MAX_TARGET_SCHM) {
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1%2!").arg(myLan.over_max_schme_num).arg(MAX_TARGET_SCHM));
        infoWidget->showSecs();
        return;
    }

    QString strScheme = m_homelist->currentItem()->text();
    if(strScheme.contains("@back")){
        infoWidget->raise();
        infoWidget->setLabelText(QString("%1!").arg(myLan.scheme_invalid));
        infoWidget->showSecs();
        return;
    }


    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_backup_scheme), myLan.notice, g_mainWidget);
    int ret = msgBox.exec();

    if(ret == QDialog::Accepted) {
        bool bCopyOK = false;
        QString strcurName;
        QString schmName = QString("%1@back").arg(strScheme);

        if (!gFlow.findSchmFromList(schmName)) {
            int nSchm = m_homelist->currentRow();
            for (int k = 0; k < struCnfg.analyzeNum; k++) {
                QString analyze_proname = struCnfg.analyzeInfo[k]->anlyze_schemeList[nSchm].code;
                QString new_analyze_proname = QString("%1@back").arg(analyze_proname);

                QFile file_analyze;
                if(file_analyze.copy(analyze_proname, new_analyze_proname)){//! 边缘服务器方案+1
                    strcurName = struCnfg.schemeList[nSchm].code;
                    schemeInfo scheme;
                    scheme.name = schmName;
                    scheme.code = QString("%1@back").arg(strcurName);
                    struCnfg.schemeList.push_back(scheme);

                    struCnfg.nSchemTotal++;

                    struCnfg.analyzeInfo[k]->anlyze_schemeList[struCnfg.nSchemTotal-1].code = new_analyze_proname;
                    gFlow.saveGlobalParams(CNF_GLOBAL);

                    updateHomeList();
                    bCopyOK = true;
                }else{
                    bCopyOK = false;
                    qDebug()<<"backup profile scheme failed";
                }
            }
        }else{
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.current_scheme_alreadybackup));
            infoWidget->showSecs();
            return;
        }

        if (!bCopyOK){
            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.backup_scheme_failed));
            infoWidget->showSecs();
        }else{
            //! 创建方案数据统计表
            g_sqliteInter->createTable(QString("%1@back").arg(strcurName));

            //! 上传物联网
            if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
                QJsonObject staicsjson;
                staicsjson["Scheme"] = true;
                g_Lotsocket->uploadInfo(staicsjson);
            }

            infoWidget->raise();
            infoWidget->setLabelText(QString("%1!").arg(myLan.backup_scheme_sucess));
            infoWidget->showSecs();
        }
    }
}
/*!
 * \brief  恢复方案
 */
void schmWidget::onRestoreSchemeSlt()
{
    QString file_name = "";
    bool isChange = false;

    QString strschme = m_homelist->currentItem()->text();

    if(!strschme.contains("@back")){
        infoWidget->raise();
        infoWidget->setLabelText(myLan.scheme_invalid);
        infoWidget->showSecs();
        return;
    }


    myMessageBox msgBox(MSG_QUES, QString("%1?").arg(myLan.confirm_restore_scheme)
                        , myLan.notice, g_mainWidget);
    int ret = msgBox.exec();
    if(ret == QDialog::Accepted){
        int curRow = m_homelist->currentRow();
        QString schmcode =struCnfg.schemeList[curRow].code;

        //! 确定原方案是否存在
        if(gFlow.findSchmFromList(QString("%1").arg(strschme).remove("@back"))){//! 存在
                //! 先删除边缘服务器器下挂的方案
                gFlow.delSchmFromList(curRow);

                //! 删除分析仪对应的方案
                for (int j = 0; j < struCnfg.analyzeNum; j++) {
                    QString analyze_curname = struCnfg.analyzeInfo[j]->anlyze_schemeList[curRow].code;
                    QString analyze_proname = QString("%1").arg(analyze_curname).remove("@back");
                    QFile file_analyze(analyze_proname);
                    qDebug()<<"analyze_curname" <<analyze_curname
                             <<"analyze_proname" <<analyze_proname;
                    if(file_analyze.exists()){
                        if(file_analyze.remove(analyze_proname)){
                            file_analyze.setFileName(analyze_curname);
                            file_analyze.rename(analyze_proname);
                        }
                    }else{
                        file_analyze.setFileName(analyze_curname);
                        file_analyze.rename(analyze_proname);
                    }

                    for (int k = curRow; k < struCnfg.nSchemTotal-1; k++) {
                        struCnfg.analyzeInfo[j]->anlyze_schemeList[k].code = struCnfg.analyzeInfo[j]->anlyze_schemeList[k+1].code;
                        qDebug()<<struCnfg.analyzeInfo[j]->anlyze_schemeList[k].code;
                    }
                }


                if (curRow == struCnfg.nCntScheme) {
                    struCnfg.nCntScheme = 0;
                    isChange = true;
                } else if (curRow < struCnfg.nCntScheme){
                    struCnfg.nCntScheme--;
                    isChange = true;
                }else{
                    isChange = true;
                    qDebug()<<"over scheme";
                }

                struCnfg.nSchemTotal--;

                if (isChange) {
                    for(int i=0; i< struCnfg.analyzeNum; i++){//! 方案参数加载
                        file_name = struCnfg.analyzeInfo[i]->anlyze_schemeList[struCnfg.nCntScheme].code;
                        gFlow.getAnalyzeProfileParams(&struCnfg.analyzeInfo[i]->analyzeProfile, file_name);
                    }
                }
                updateHomeList();

                g_sqliteInter->dropTable(schmcode);
        }else{//! 不存在
            g_sqliteInter->renametable(struCnfg.schemeList[curRow].code);

            struCnfg.schemeList[curRow].code.remove("@back");
            struCnfg.schemeList[curRow].name.remove("@back");


            for (int j = 0; j < struCnfg.analyzeNum; j++) {
                QString analyze_curname = struCnfg.analyzeInfo[j]->anlyze_schemeList[curRow].code;

                QFile file_analyze(analyze_curname);

                file_analyze.rename(analyze_curname.remove("@back"));

                qDebug()<<analyze_curname;
                struCnfg.analyzeInfo[j]->anlyze_schemeList[curRow].code = analyze_curname;
            }

            updateHomeList();
        }

        if (curRow == struCnfg.nCntScheme) {//!  重新加载方案参数
            for(int i=0; i< struCnfg.analyzeNum; i++){
                file_name = struCnfg.analyzeInfo[i]->anlyze_schemeList[struCnfg.nCntScheme].code;
                gFlow.getAnalyzeProfileParams(&struCnfg.analyzeInfo[i]->analyzeProfile, file_name);
            }
        }

        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->bEnweb){
            QJsonObject staicsjson;
            staicsjson["Scheme"] = true;
            g_Lotsocket->uploadInfo(staicsjson);
        }
    }
}
