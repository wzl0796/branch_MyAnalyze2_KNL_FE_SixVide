#include "identifity.h"

Identifity::Identifity(QWidget *parent) : QWidget(parent)
{
    cratePage();
    layoutPage();
    connectAndSlt();
}

void Identifity::cratePage()
{
    QStringList titlelst, iconlst;
    titlelst << myLan.label_params << myLan.AI << myLan.other << myLan.tradition;

    iconlst << ":/res/png/tab_adjust_normal.png"
             << ":/res/png/Sorter_AI.png"
             << ":/res/png/tab_valve_normal.png"
             << ":/res/png/tab_valve_normal.png";


    m_listWid = new QListWidget();
    m_listWid->setMovement(QListView::Static);
    m_listWid->setViewMode(QListView::IconMode);
    m_listWid->setFixedSize(BTN_WID+15, WIN_HEI-180);
    m_listWid->setIconSize(QSize(BTN_HEI, BTN_HEI-15));
    m_listWid->setStyleSheet(QString("QListWidget{border:1px solid lightgray; "
                                      "border-radius:10px;outline:0px;"
                                      "padding:30px,0px,0px,0px;"
                                      "background-color:white;"
                                      "font:25px;}"
                                      "QListWidget::Item:selected{"
                                      "border-right:5px solid #43CD80}"));


    for (int i = 0; i < PAGE_ALL; i++) {
        QListWidgetItem *listItem = new QListWidgetItem(titlelst.at(i));
        listItem->setSizeHint(QSize(BTN_WID+13,90));
        listItem->setTextAlignment(Qt::AlignCenter);
        listItem->setIcon(QIcon(iconlst.at(i)));
        m_listWid->addItem(listItem);
    }
//    if(struCnfg.nMachineType == nMachine_Bottle){
//        m_listWid->item(PAGE_SHAPENESS)->setHidden(true);
//    }


    m_labSetPage = new labelsetPage();
    m_aiPage = new AIWidget();
    m_shapenessPage = new shapenesspage();
    m_traditionarithPage = new traditionArith();

    m_stackWid = new QStackedWidget();
    m_stackWid->addWidget(m_labSetPage);
    m_stackWid->addWidget(m_aiPage);
    m_stackWid->addWidget(m_shapenessPage);
    m_stackWid->addWidget(m_traditionarithPage);

    m_stackWid->setFixedSize(WIN_WID-315, WIN_HEI-180);
    m_stackWid->setStyleSheet("background-color:white; border-radius:10px;");

    m_listWid->setCurrentRow(0);

}

void Identifity::layoutPage()
{
    QHBoxLayout *hlaymain = new QHBoxLayout();
    hlaymain->addWidget(m_listWid);
    hlaymain->addWidget(m_stackWid);

    setLayout(hlaymain);
}

void Identifity::connectAndSlt()
{
    connect(m_listWid, &QListWidget::currentRowChanged, this, &Identifity::onListitemChgSlt);
}

void Identifity::refreshPage()
{
    int nIdx = m_listWid->currentRow();
    onListitemChgSlt(nIdx);
}

void Identifity::onListitemChgSlt(int nIdx)
{
    m_stackWid->setCurrentIndex(nIdx);

    switch (nIdx) {
    case PAGE_LAB_SET:
        m_labSetPage->refreshPage();
        break;
    case PAGE_AI:
        m_aiPage->refreshPage();
        break;
    case PAGE_SHAPENESS:
        m_shapenessPage->refreshPage();
        break;
    case PAGE_TRADITON:
        m_traditionarithPage->refreshPage();
        break;
    default:
        break;
    }
}

void Identifity::onMachineTypeChgSlt(bool bBottle)
{
//    if(bBottle){
//        m_listWid->item(PAGE_SHAPENESS)->setSizeHint(QSize(BTN_WID+13,1));
//    }else{
//        m_listWid->item(PAGE_SHAPENESS)->setSizeHint(QSize(BTN_WID+13,90));
//    }
}
