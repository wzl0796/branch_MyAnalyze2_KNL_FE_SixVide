#include "mydevicelist.h"
#include <QHBoxLayout>
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"


myDeviceList::myDeviceList(QWidget *parent)
    : QWidget(parent)
{
    m_styleSelect = QString("color:%1;background-color:%2;border-width:0px;font: 25px;padding-left:15px;padding-right:15px;").arg(g_ColorMYGreen).arg(g_ColorLightGreen);
    m_styleUnSelect = QString("color:black;background:transparent;border-width:0px;font: 25px;padding-left:15px;padding-right:15px;");

    m_deviceBtnMap = new QSignalMapper();

    QHBoxLayout *hLay = new QHBoxLayout(this);
    hLay->setSizeConstraint(QLayout::SetFixedSize);
    hLay->setSpacing(10);
    for (int i = 0; i < MAX_ANALYZE_NUM; i++) {
        m_deviceBtn[i] = new QPushButton();
        m_deviceBtn[i]->setFixedHeight(60);
        hLay->addWidget(m_deviceBtn[i]);

        if(i < MAX_ANALYZE_NUM-1){
            m_Vline[i] = new QFrame();
            m_Vline[i]->setFixedSize(2, 40);
            m_Vline[i]->setFrameShape(QFrame::VLine);
            m_Vline[i]->setFrameShadow(QFrame::Sunken);
            m_Vline[i]->setStyleSheet(QString("background:%1;").arg(g_ColorLightGray));
            hLay->addWidget(m_Vline[i], 0, Qt::AlignVCenter);
        }

        m_deviceBtnMap->setMapping(m_deviceBtn[i],i);
        connect(m_deviceBtn[i], SIGNAL(released()), m_deviceBtnMap, SLOT(map()));
    }

    hLay->addStretch(0);

    connect(m_deviceBtnMap, SIGNAL(mapped(int)), this, SLOT(onDeviceChanged(int)));

    refreshPage();
}

void myDeviceList::refreshPage()
{
    m_Num = struCnfg.analyzeNum;
    if(struGsh.nAnalyzeIndex >= 0 && struGsh.nAnalyzeIndex < m_Num)
        m_currentIndex = struGsh.nAnalyzeIndex;
    else
        m_currentIndex = 0;

    QStringList strList;
    for(int i = 0; i < struCnfg.analyzeNum; ++i) {
        strList << QString(gFlow.getSpecifyAnalyzeGlobal(i)->sAnalyzeName);
    }

    for (int i = 0; i < MAX_ANALYZE_NUM; i++) {
        if(i < m_Num){
            m_deviceBtn[i]->show();
            m_deviceBtn[i]->setStyleSheet(m_styleUnSelect);
            m_deviceBtn[i]->setText(i < strList.size() ? strList[i] : "");

            m_deviceBtn[m_currentIndex]->setStyleSheet(m_styleSelect);
        } else {
            m_deviceBtn[i]->hide();
        }

        if(i < m_Num - 1)
            m_Vline[i]->show();
        else if(i < MAX_ANALYZE_NUM - 1)
            m_Vline[i]->hide();
    }
}

void myDeviceList::setCurrentIndex(int index)
{
    m_currentIndex = index;
    refreshPage();
}

void myDeviceList::onDeviceChanged(int index)
{
    m_currentIndex = index;

    for (int i = 0; i < m_Num; i++) {
        m_deviceBtn[i]->setStyleSheet(m_styleUnSelect);
    }
    m_deviceBtn[index]->setStyleSheet(m_styleSelect);

    emit changeSelected(index);
}
