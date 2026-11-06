#include "labelproritypage.h"

labelprority::labelprority(QWidget *parent) : QWidget(parent),
    m_bSync(false),m_viewIdx(0)
{
    createPage();
    layOutPage();
    connectAndSlt();
}

void labelprority::createPage()
{
    m_labProGb = new QGroupBox();

    m_modeTypeBtn= new QPushButton(myLan.alone);
    m_modeTypeBtn->setFixedSize(BTN_WID+60, BTN_HEI);
    m_modeTypeBtn->setIconSize(QSize(50,50));
    m_modeTypeBtn->setIcon(QIcon(":/res/png/swmode.png"));

    m_labPriGb = new QGroupBox(myLan.label_priority); //! 标签优先级
    m_labMutexgb = new QGroupBox(myLan.lab_mutex);

    for(int i=0; i<MAX_LABELS; i++){
        m_labnamelb[i] = new QLabel();
        m_labPrile[i] = new MyLineEdit();
        m_labPrile[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_labnamelb[i]->hide();
        m_labPrile[i]->hide();

        m_labmutexlb[i] = new QLabel();
        m_labmutexle[i] = new MyLineEdit();
        m_labmutexle[i]->setStyleSheet("background-color:rgba(220,220,220,100)");
        m_labmutexlb[i]->hide();
        m_labmutexle[i]->hide();

    }
}

void labelprority::layOutPage()
{
    QGridLayout *gridlaylabpri = new QGridLayout(m_labPriGb);
    gridlaylabpri->setContentsMargins(0,50,0,0);
    for(int i=0; i<(MAX_LABELS/2); i++){
        gridlaylabpri->addWidget(m_labnamelb[2*i],i,0);
        gridlaylabpri->addWidget(m_labPrile[2*i],i,1);
        gridlaylabpri->addWidget(m_labnamelb[2*i+1],i,2);
        gridlaylabpri->addWidget(m_labPrile[2*i+1],i,3);
    }

    QGridLayout *labmutxgd = new QGridLayout(m_labMutexgb);
    labmutxgd->setContentsMargins(0,50,0,0);
    for(int i=0; i<(MAX_LABELS/2); i++){
        labmutxgd->addWidget(m_labmutexlb[2*i],i,0);
        labmutxgd->addWidget(m_labmutexle[2*i],i,1);
        labmutxgd->addWidget(m_labmutexlb[2*i+1],i,2);
        labmutxgd->addWidget(m_labmutexle[2*i+1],i,3);
    }

    QVBoxLayout *vlaybtn = new QVBoxLayout();
    vlaybtn->addSpacing(15);
    vlaybtn->addWidget(m_modeTypeBtn);
    vlaybtn->addStretch();

    QHBoxLayout *hlaymain = new QHBoxLayout(m_labProGb);
    hlaymain->addWidget(m_labPriGb);
    hlaymain->addSpacing(100);
    hlaymain->addWidget(m_labMutexgb);
    hlaymain->addSpacing(80);
    hlaymain->addLayout(vlaybtn);

    QHBoxLayout *hlaypage = new QHBoxLayout();
    hlaypage->addWidget(m_labProGb);
    setLayout(hlaypage);

}

void labelprority::connectAndSlt()
{
    QSignalMapper *labmutexmap = new QSignalMapper();
    QSignalMapper *labprimap = new QSignalMapper();


    for(int i=0; i<MAX_LABELS; i++){
        labmutexmap->setMapping(m_labmutexle[i],i);
        connect(m_labmutexle[i], &MyLineEdit::pressed, labmutexmap, QOverload<>::of(&QSignalMapper::map));

        labprimap->setMapping(m_labPrile[i],i);
        connect(m_labPrile[i], &MyLineEdit::pressed, labprimap, QOverload<>::of(&QSignalMapper::map));
    }

    connect(labmutexmap, QOverload<int>::of(&QSignalMapper::mapped), this, &labelprority::onlabMutexleChgSlt);
    connect(labprimap, QOverload<int>::of(&QSignalMapper::mapped), this, &labelprority::onlabPriorityChgSlt);

    connect(m_modeTypeBtn,&QPushButton::pressed, this, &labelprority::onbtnSyncPressedSlt);
}

void labelprority::refreshPage(int nCamid)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    if(pProfile->nStaticGroup <= 1){
        m_bSync = true;
        m_modeTypeBtn->setText(myLan.same);
        m_modeTypeBtn->hide();
    }else{
        m_modeTypeBtn->show();
    }

    m_viewIdx = nCamid;
    reFreshlabPri(m_viewIdx);
    reFreshlabMutex(m_viewIdx);
}

void labelprority::onlabMutexleChgSlt(int nIdx)
{
    myInputPanel *inputPanel = new myInputPanel(intType, 0, 100, m_labmutexle[nIdx]->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        auto *pPrams =  &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
        int n_sample = inputPanel->getValue();
        m_labmutexle[nIdx]->setText(QString("%1").arg(n_sample));
        QString labname = m_labmutexlb[nIdx]->text();
        if(!m_bSync){
            pPrams->vlabMutexmap[m_viewIdx][labname] = n_sample;
        }else{
            for(int i=0; i<gFlow.getMachineCamNum() && i<pPrams->vlabMutexmap.size(); i++){
                pPrams->vlabMutexmap[i][labname] = n_sample;
            }
        }
    }
}

void labelprority::onlabPriorityChgSlt(int nIdx)
{
    int max = struGsh.vlabNamemap[m_viewIdx].keys().size();

    myInputPanel *inputPanel = new myInputPanel(intType, 0, max?max:1, m_labPrile[nIdx]->text().toInt(), "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        auto *pPrams =  &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
        int n_sample = inputPanel->getValue();
        m_labPrile[nIdx]->setText(QString("%1").arg(n_sample));
        QString labname = m_labnamelb[nIdx]->text();
        if(!m_bSync && pPrams->vlabelidxmap[m_viewIdx].contains(labname)){
            pPrams->vlabelidxmap[m_viewIdx][labname] = n_sample;
        }else if(m_bSync){
            for(int i=0; i<gFlow.getMachineCamNum() && i<pPrams->vlabelidxmap.size(); i++){
                if(pPrams->vlabelidxmap[i].contains(labname)){
                    pPrams->vlabelidxmap[i][labname] = n_sample;
                }
            }
        }
    }
}

void labelprority::onbtnSyncPressedSlt()
{
    m_bSync = !m_bSync;
    if(m_bSync){
        m_modeTypeBtn->setText(myLan.same);
    }else{
        m_modeTypeBtn->setText(myLan.alone);
    }
}

void labelprority::reFreshlabPri(int nCamId)
{
    QList<QString> keylst1 = struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabelidxmap[nCamId].keys();
    for(int i=0; i<MAX_LABELS; i++){
        if(i<keylst1.size()){
            m_labnamelb[i]->show();
            m_labPrile[i]->show();
            m_labnamelb[i]->setText(keylst1.at(i));
            m_labPrile[i]->setText(QString("%1").arg(struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI.vlabelidxmap[nCamId][keylst1.at(i)]));
        }else{
            m_labnamelb[i]->hide();
            m_labPrile[i]->hide();
        }
    }
}

void labelprority::reFreshlabMutex(int nCamId)
{
    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile.strctAI;
    QList<QString> keylst1 = pProfile->vlabMutexmap[nCamId].keys();
    for(int i=0; i<MAX_LABELS; i++){
        if(i<keylst1.size()){
            m_labmutexlb[i]->show();
            m_labmutexle[i]->show();
            m_labmutexlb[i]->setText(keylst1.at(i));
            m_labmutexle[i]->setText(QString("%1").arg(pProfile->vlabMutexmap[nCamId][keylst1.at(i)]));
        }else{
            m_labmutexlb[i]->hide();
            m_labmutexle[i]->hide();
        }
    }
}
