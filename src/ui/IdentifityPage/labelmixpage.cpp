#include "labelmixpage.h"

labelmixpage::labelmixpage(QWidget *parent) : QWidget(parent),
    m_nCamId(0),
    m_bUpdating(false)
{
    createPage();
    layOutPage();
    connectAndSlt();
}

void labelmixpage::createPage()
{
    m_mixGroup = new QGroupBox();

    m_tableWidget = new QTableWidget();
    m_tableWidget->setColumnCount(2);
    m_tableWidget->setHorizontalHeaderLabels(QStringList() <<myLan.ori_lab <<myLan.mapping_lab);

    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_tableWidget->verticalHeader()->setHidden(true);

    QString tableStyle = QString("QTableWidget {border:1px solid lightgray; border-radius: 5px; font-size:22px; background-color: white;} "
                                 "QHeaderView::section {background-color:#f0f0f0; font-size:24px; font-weight:bold; height:50px; border:1px solid lightgray;} "
                                 "QTableWidget::item {height:55px;}");
    m_tableWidget->setStyleSheet(tableStyle);

    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_tableWidget);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
}

void labelmixpage::layOutPage()
{
    QVBoxLayout *vlayGroup = new QVBoxLayout(m_mixGroup);
    vlayGroup->setContentsMargins(20, 30, 20, 20);
    vlayGroup->addWidget(m_scrollArea);

    QHBoxLayout *hlaymain = new QHBoxLayout();
    hlaymain->addWidget(m_mixGroup);
    setLayout(hlaymain);
}

void labelmixpage::connectAndSlt()
{

}

void labelmixpage::refreshPage(int nCamId)
{
    m_nCamId = nCamId;
    m_bUpdating = true;

    gFlow.validateLabMixMap(m_nCamId);

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;
    if (pProfile->vlabMixmap.size() <= m_nCamId) {
        pProfile->vlabMixmap.resize(gFlow.getMachineCamNum());
    }

    QList<QString> origLabels;
    if (m_nCamId >= 0 && m_nCamId < struGsh.vlabNamemap.size()) {
        origLabels = struGsh.vlabNamemap[m_nCamId].values();
    }

    m_tableWidget->setRowCount(0);
    m_tableWidget->setRowCount(origLabels.size());

    QString comboStyle = QString("QComboBox {border: 1px solid lightgray; border-radius: 5px; padding: 1px 5px; background-color: #FFFFFF; font-size: 22px;} "
                                 "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 40px;} "
                                 "QComboBox QAbstractItemView::item {height: 45px; font-size: 20px;}");

    for (int i = 0; i < origLabels.size(); ++i) {
        QString origLab = origLabels.at(i);

        QTableWidgetItem *itemOrig = new QTableWidgetItem(origLab);
        itemOrig->setFlags(itemOrig->flags() & ~Qt::ItemIsEditable);
        itemOrig->setTextAlignment(Qt::AlignCenter);
        m_tableWidget->setItem(i, 0, itemOrig);

        QComboBox *combo = new QComboBox();
        combo->setStyleSheet(comboStyle);

        QListWidget *listWid = new QListWidget();
        combo->setModel(listWid->model());
        combo->setView(listWid);

        combo->addItem(myLan.empty, QString(""));

        for (const QString &otherLab : origLabels) {
            if (otherLab != origLab) {
                combo->addItem(otherLab, otherLab);
            }
        }

        QString currentMapped = pProfile->vlabMixmap[m_nCamId].value(origLab, "");
        int curIdx = combo->findData(currentMapped);
        if (curIdx >= 0) {
            combo->setCurrentIndex(curIdx);
        } else {
            combo->setCurrentIndex(0);
        }

        combo->setProperty("origLab", origLab);
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &labelmixpage::onMapComboChanged);

        m_tableWidget->setCellWidget(i, 1, combo);
        m_tableWidget->setRowHeight(i, 55);
    }

    m_bUpdating = false;
}

void labelmixpage::onMapComboChanged(int index)
{
    Q_UNUSED(index);
    if (m_bUpdating) return;

    QComboBox *combo = qobject_cast<QComboBox*>(sender());
    if (!combo) return;

    QString origLab = combo->property("origLab").toString();
    QString mappedLab = combo->currentData().toString();

    auto *pProfile = &struCnfg.analyzeInfo[struGsh.nAnalyzeIndex]->analyzeProfile;

    if(pProfile->nStaticGroup == 1){
        //! 统计组个数为1时，遍历所有相机统一设置映射标签
        int nCamNum = gFlow.getMachineCamNum();
        
        for(int nCam=0; nCam<nCamNum && nCam <pProfile->vlabMixmap.size(); nCam++){
            pProfile->vlabMixmap[nCam][origLab] = mappedLab;
        }
    }else{
        if(pProfile->vlabMixmap.size() <= m_nCamId){
           return;
        }
        pProfile->vlabMixmap[m_nCamId][origLab] = mappedLab;
    }
}
