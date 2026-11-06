#include "calibratewidget.h"
#include "params/define.h"
#include "src/common/myinputpanel.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QListWidget>

calibrateWidget::calibrateWidget(QWidget *parent)
    :QDialog(parent),m_nCamidx(0)
{
    createMainPage();
}

calibrateWidget::~calibrateWidget()
{
}

void calibrateWidget::createMainPage()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID, WIN_HEI);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    createSchMainPage();

    widget = new QWidget(this);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setGeometry(0, 630, WIN_WID, WIN_HEI-630);
    widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");
    widget->show();

    QHBoxLayout *mainLay = new QHBoxLayout(widget);
    mainLay->addWidget(pageSchMain);
}

void calibrateWidget::createSchMainPage()
{
    QHBoxLayout *hUpLay = new QHBoxLayout;
    m_camIdbtnGroup = new QButtonGroup();
    m_camIdbtnGroup->setExclusive(true);
    for(int i=0 ; i<MAX_CAM_NUM; i++){
        m_camIdbtns[i] = new QPushButton(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->CamName[i]);
        m_camIdbtns[i]->setFixedSize(BTN_WID+30,BTN_HEI-10);
        m_camIdbtnGroup->addButton(m_camIdbtns[i],i);
        hUpLay->addWidget(m_camIdbtns[i]);
    }

    m_syncBtn = new QPushButton(myLan.alone);
    m_syncBtn->setFixedSize(BTN_WID,BTN_HEI-10);

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    m_samplechanelBtn = new QPushButton(QString(pProfile->strcSortclass.sortclassname[m_nsamplechanel]));
    m_samplechanelBtn->setFixedSize(BTN_WID+50,BTN_HEI-10);

    for(int i=0; i < MAX_LABELS; i++){//! SOC标签灵敏度
        m_Arithsenslb[i] = new QLabel(QString(""));
        m_Arithsensle[i] = new MyLineEdit();
        m_Arithsensle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_Arithsensle[i]->setText(QString("90"));
        m_Arithsenslb[i]->hide();
        m_Arithsensle[i]->hide();
    }

    //! 仿真
    m_SimEnlb = new QLabel(myLan.simulate_en);
    m_SimEnlb->setFixedHeight(BTN_HEI);
    m_SimEnlb->hide();
    m_SimEnsw = new mySwitchButton();
    m_SimEnsw->setButtonStyle(mySwitchButton::ButtonStyle_Rect);
    m_SimEnsw->setFixedSize(BTN_WID+20,BTN_HEI-10);
    m_SimEnsw->hide();
    m_SimMatEnlb = new QLabel(myLan.clear_simulate_pic);
    m_SimMatEnlb->setFixedHeight(BTN_HEI);
    m_SimMatEnlb->hide();
    m_SimMatEnsw = new mySwitchButton();
    m_SimMatEnsw->setButtonStyle(mySwitchButton::ButtonStyle_Rect);
    m_SimMatEnsw->setFixedSize(BTN_WID+20,BTN_HEI-10);
    m_SimMatEnsw->hide();


    //! 页面布局
    QGridLayout *glaysens = new QGridLayout();
    glaysens->setContentsMargins(20,10,20,5);
    for(int i=0; i < MAX_LABELS; i++){
        glaysens->addWidget(m_Arithsenslb[i],i/6, 2*(i%6),1,1,Qt::AlignRight);
        glaysens->addWidget(m_Arithsensle[i],i/6, 2*(i%6)+1,1,1,Qt::AlignLeft);
    }


    hUpLay->addStretch();
    hUpLay->addWidget(m_syncBtn);
    hUpLay->addSpacing(20);
    hUpLay->addWidget(m_samplechanelBtn);
    hUpLay->addSpacing(10);
    hUpLay->addWidget(m_SimEnlb);
    hUpLay->addWidget(m_SimEnsw);
    hUpLay->addWidget(m_SimMatEnlb);
    hUpLay->addWidget(m_SimMatEnsw);


    QVBoxLayout *Lay = new QVBoxLayout;
    Lay->addLayout(hUpLay);
    Lay->addSpacing(10);
    Lay->addLayout(glaysens);
    Lay->addStretch();

    pageSchMain = new QWidget;
    pageSchMain->setLayout(Lay);


    connect(m_syncBtn,SIGNAL(pressed()),this,SLOT(onSyncOrAloneBtnPressed()));
    connect(m_samplechanelBtn, SIGNAL(pressed()), this, SLOT(onSamplechnelBtnPressed()));


    QSignalMapper* ArithsensSigmap = new QSignalMapper();
    for(int i=0; i<MAX_LABELS; i++){
        ArithsensSigmap->setMapping(m_Arithsensle[i], i);
        connect(m_Arithsensle[i], SIGNAL(pressed()), ArithsensSigmap, SLOT(map()));
    }
    connect(ArithsensSigmap,SIGNAL(mapped(int)),this,SLOT(onArithSensChanged(int)));

    connect(m_camIdbtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onCamidBtnSlt(int)));

    connect(m_SimEnsw, &mySwitchButton::checkedChanged,this,&calibrateWidget::onSimEnChgSlt);
    connect(m_SimMatEnsw, &mySwitchButton::checkedChanged, this, &calibrateWidget::onSimSaveMatChgSlt);

    refreshPage();
}



void calibrateWidget::refreshPage()
{
    m_nCamidx =0;
    if(struCnfg.nMachineType == nMachine_Bottle){
        m_syncBtn->hide();
        if(struGsh.nAuthority == AUTHORITY_ENGINEER){
            m_SimEnlb->show();
            m_SimEnsw->show();
            m_SimMatEnlb->show();
            m_SimMatEnsw->show();
            m_SimEnsw->setChecked(struGsh.bAISimulate);
            m_SimMatEnsw->setChecked(struGsh.bAISaveMat);

            // Ensure mutual exclusivity at init: if one is checked, disable the other
            if (m_SimEnsw->getChecked()) {
                QSignalBlocker block(m_SimMatEnsw);
                m_SimMatEnsw->setChecked(false);
                m_SimMatEnsw->setEnabled(false);
            } else if (m_SimMatEnsw->getChecked()) {
                QSignalBlocker block(m_SimEnsw);
                m_SimEnsw->setChecked(false);
                m_SimEnsw->setEnabled(false);
            } else {
                m_SimEnsw->setEnabled(true);
                m_SimMatEnsw->setEnabled(true);
            }
        }
    }else{
        m_SimEnlb->hide();
        m_SimEnsw->hide();
        m_SimMatEnlb->hide();
        m_SimMatEnsw->hide();
        m_syncBtn->show();
    }

    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_camIdbtns[i]->show();
        }else{
            m_camIdbtns[i]->hide();
        }
        if(i ==m_nCamidx){
            m_camIdbtns[i]->setStyleSheet("background-color:#007454;");
        }else{
            m_camIdbtns[i]->setStyleSheet("background-color:white;");
        }
    }

    m_samplechanelBtn->setHidden(!struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.bEnautoSample);

    //! 刷新算法灵敏度
    refreshArithSens(m_nCamidx);

}

/*!
 * \brief 槽: 灵敏度修改
 * \param nIdx
 */
void calibrateWidget::onArithSensChanged(int nIdx)
{
    int nmax = 100;
    if(struCnfg.nMachineType == nMachine_Bottle){
        if(gFlow.bIsIMSerialCam(m_nCamidx)){
            QList<QString> namelist = struGsh.vlabNamemap[m_nCamidx].values();
            if(nIdx == namelist.size()){
                nmax = 5000;
            }else{
                nmax= 100;
            }
        }
    }

    myInputPanel *inputPanel = new myInputPanel(intType, 0, nmax, m_Arithsensle[nIdx]->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        m_Arithsensle[nIdx]->setText(QString("%1").arg(n_sample));
        if(m_bSync){
            for(int i=0; i<gFlow.getMachineCamNum(); i++){
                struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.sens[i][nIdx+m_nsamplechanel*MAX_LABELS] = n_sample;
            }
        }else{
            struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.sens[m_nCamidx][nIdx+m_nsamplechanel*MAX_LABELS] = n_sample;
        }
        if(struCnfg.nMachineType == nMachine_Crop){
            gFlow.SocParamssend(struGsh.nAnalyzeIndex,0);
        }else{
            int camType = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[m_nCamidx];
            if(camType == 1) {
                gFlow.sendSocTraditionSens(m_nCamidx);
            } else if (gFlow.bIsIMSerialCam(m_nCamidx)|| gFlow.bIsUltraCam(m_nCamidx)){
                gFlow.SocParamssend(struGsh.nAnalyzeIndex,0);
            }
        }
    }
}

/*!
 * \brief 槽：相机ID更改
 */
void calibrateWidget::onCamidBtnSlt(int nIdx)
{
    m_nCamidx = nIdx;

    for(int i=0; i<MAX_CAM_NUM; i++){
        if(i<gFlow.getMachineCamNum()){
            m_camIdbtns[i]->show();
        }else{
            m_camIdbtns[i]->hide();
        }
        if(i ==m_nCamidx){
            m_camIdbtns[i]->setStyleSheet("background-color:#007454;");
        }else{
            m_camIdbtns[i]->setStyleSheet("background-color:white;");
        }
    }

    refreshArithSens(m_nCamidx);
}

void calibrateWidget::onSyncOrAloneBtnPressed()
{
    m_bSync = !m_bSync;
    if(m_bSync){
        m_syncBtn->setText(myLan.same);
    }else{
        m_syncBtn->setText(myLan.alone);
    }
}

/*!
 * \brief 自动取样通道下标修改
 */
void calibrateWidget::onSamplechnelBtnPressed()
{
    auto *pSorter = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strcSortclass;
    m_nsamplechanel +=1;
    if(m_nsamplechanel >= pSorter->nmaxNum){
        m_nsamplechanel = 0;
    }
    m_samplechanelBtn->setText(QString(pSorter->sortclassname[m_nsamplechanel]));

    //! 刷新灵敏度
    refreshArithSens(m_nCamidx);
}

void calibrateWidget::onSimEnChgSlt(bool bSta)
{
    struGsh.bAISimulate = bSta;

    if (bSta) {
        QSignalBlocker block(m_SimMatEnsw);
        m_SimMatEnsw->setChecked(false);
        m_SimMatEnsw->setEnabled(false);
        struGsh.bAISaveMat = false;
    } else {
        m_SimMatEnsw->setEnabled(true);
    }

    gFlow.SocAdParamssend(struGsh.nAnalyzeIndex, 0);
}

void calibrateWidget::onSimSaveMatChgSlt(bool bSta)
{
    struGsh.bAISaveMat = bSta;

    if (bSta) {
        QSignalBlocker block(m_SimEnsw);
        m_SimEnsw->setChecked(false);
        m_SimEnsw->setEnabled(false);
        struGsh.bAISimulate = false;
    } else {
        m_SimEnsw->setEnabled(true);
    }

    gFlow.SocAdParamssend(struGsh.nAnalyzeIndex, 0);
}

/*!
 * \brief 触摸事件
 */
void calibrateWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->pos().y() < 630)
        this->hide();
}

QString calibrateWidget::setPointPos(double value, uint smallNum)
{
    QString str = QString::number(value);
    int index = str.indexOf('.');
    if(index != -1 && (str.length()-index-1) > smallNum)
        str = QString::number(value, 'f', smallNum);

    return str;
}

/*!
 * \brief 刷新Soc的算法灵敏度
 * \param nCamId
 */
void calibrateWidget::refreshArithSens(int nCamId)
{
    auto *pProAI = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
    if(struCnfg.nMachineType == nMachine_Crop){//! 坚果
        QList<QString> namelist = struGsh.vlabNamemap[nCamId].values();
        int num =namelist.size();
        for(int i=0; i<MAX_LABELS; i++){
            if(i < num){
                m_Arithsenslb[i]->setText(namelist.at(i));
                m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                m_Arithsenslb[i]->show();
                m_Arithsensle[i]->show();
            }else{
                m_Arithsenslb[i]->hide();
                m_Arithsensle[i]->hide();
            }
        }
    }else{//! 瓶片
        if(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->struCaParams->nWorkMode[nCamId] == 1){//! 传统
            for(int i=0; i<MAX_LABELS; i++){
                if(i < 1){
                    m_Arithsenslb[i]->setText(QString("%1").arg(myLan.opaque));
                    m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                    m_Arithsenslb[i]->show();
                    m_Arithsensle[i]->show();
                }else{
                    m_Arithsenslb[i]->hide();
                    m_Arithsensle[i]->hide();
                }
            }
        } else if (gFlow.bIsIMSerialCam(nCamId)){//! 高光谱
            QList<QString> namelist = struGsh.vlabNamemap[nCamId].values();
            int num =namelist.size();
            for(int i=0; i<MAX_LABELS; i++){
                if(i < num){
                    m_Arithsenslb[i]->setText(namelist.at(i));
                    m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                    m_Arithsenslb[i]->show();
                    m_Arithsensle[i]->show();
                }else if(i ==num){
                    m_Arithsenslb[i]->setText(myLan.domain);
                    m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                    m_Arithsenslb[i]->show();
                    m_Arithsensle[i]->show();
                }else{
                    m_Arithsenslb[i]->hide();
                    m_Arithsensle[i]->hide();
                }
            }
        } else if(gFlow.bIsUltraCam(nCamId)){//! 紫外
            QStringList namelst;
            namelst<<myLan.aging<<myLan.fluore;
            for(int i=0; i<MAX_LABELS; i++){
                if(i < 2){
                    m_Arithsenslb[i]->setText(namelst.at(i));
                    m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                    m_Arithsenslb[i]->show();
                    m_Arithsensle[i]->show();
                }else{
                    m_Arithsenslb[i]->hide();
                    m_Arithsensle[i]->hide();
                }
            }
        }else {
            if(0){//! （粒子料需求添加，目前用不到）
                QList<QString> namelist = struGsh.vlabNamemap[nCamId].values();
                for(int i=0; i<MAX_LABELS; i++){
                    if(i<namelist.size()){
                        m_Arithsenslb[i]->setText(namelist.at(i));
                        m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                        m_Arithsenslb[i]->show();
                        m_Arithsensle[i]->show();
                    }else{
                        m_Arithsenslb[i]->hide();
                        m_Arithsensle[i]->hide();
                    }
                }
            }else{
                QStringList namelst;
                namelst<<myLan.Light_color<<myLan.trans_white;
                for(int i=0; i<MAX_LABELS; i++){
                    if(i<2){
                        m_Arithsenslb[i]->show();
                        m_Arithsensle[i]->show();
                        m_Arithsenslb[i]->setText(namelst.at(i));
                        m_Arithsensle[i]->setText(QString("%1").arg(pProAI->sens[nCamId][i+m_nsamplechanel*MAX_LABELS]));
                    }else{
                        m_Arithsenslb[i]->hide();
                        m_Arithsensle[i]->hide();
                    }
                }
            }
        }
    }
}
