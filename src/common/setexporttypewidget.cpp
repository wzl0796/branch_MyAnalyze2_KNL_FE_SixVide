#include "setexporttypewidget.h"

setExportTypeWidget::setExportTypeWidget(QWidget *parent) :
    QDialog(parent)
{
    init();

    createPage();

    connectSigAndSlt();
}

/*!
 * \brief 初始化
 */
void setExportTypeWidget::init()
{
    m_index = 0;
    m_arrayType = 0;
}

/*!
 * \brief 创建页面
 */
void setExportTypeWidget::createPage()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0, 0, WIN_WID+30, WIN_HEI+40);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    m_widget = new QWidget(this);
    m_widget->setWindowFlags(Qt::FramelessWindowHint);
    m_widget->setGeometry((WIN_WID - 600)/2, (WIN_HEI - 360)/2, 600, 410);
    m_widget->setStyleSheet("QWidget{background-color:white; border-radius:10px;}");
    m_widget->show();

    m_titleLbl = new QLabel(myLan.select_export_config);
    m_typeBtnGroup = new QButtonGroup(this);
    m_typeBtnGroup->setExclusive(true);
    m_arrayBtnGroup = new QButtonGroup(this);
    m_arrayBtnGroup->setExclusive(true);
    

    QHBoxLayout *btnLay[3],*hlayarrayBtn[2];
    QStringList strList,strArray;
    strList <<myLan.export_select_item << myLan.export_date_item << myLan.cloud;
    strArray <<myLan.array_row << myLan.array_colum;


    for(int i = 0 ; i < 3; ++i) {
        m_typeLbl[i] = new QLabel(strList[i]);
        m_typeLbl[i]->setFixedWidth(BTN_WID*2);
        m_typeLbl[i]->setWordWrap(true);
        m_typeBtn[i] = new QRadioButton();
        m_typeBtn[i]->setFixedSize(60,60);
        m_typeBtnGroup->addButton(m_typeBtn[i], i);
        btnLay[i] = new QHBoxLayout();
        btnLay[i]->setSpacing(10);
        btnLay[i]->addStretch();
        btnLay[i]->addWidget(m_typeBtn[i]);
        btnLay[i]->addWidget(m_typeLbl[i]);
        btnLay[i]->addStretch();

        if(i < 2){
            m_arrayLbl[i] = new QLabel(strArray[i]);
            m_arrayLbl[i]->setFixedWidth(BTN_WID*2);
            m_arrayLbl[i]->setWordWrap(true);
            m_arrayLbl[i]->hide();
            m_arrayBtn[i] = new QRadioButton();
            m_arrayBtn[i]->setFixedSize(60,60);
            m_arrayBtn[i]->hide();
            m_arrayBtnGroup->addButton(m_arrayBtn[i], i);
            hlayarrayBtn[i] = new QHBoxLayout();
            hlayarrayBtn[i]->setSpacing(10);
            hlayarrayBtn[i]->addStretch();
            hlayarrayBtn[i]->addWidget(m_arrayBtn[i]);
            hlayarrayBtn[i]->addWidget(m_arrayLbl[i]);
            hlayarrayBtn[i]->addStretch();
        }
    }

    m_typeLbl[2]->hide();
    m_typeBtn[2]->hide();
    m_typeBtn[0]->setChecked(true);
    m_arrayBtn[0]->setChecked(true);

    m_okBtn = new QPushButton(myLan.ok);
    m_okBtn->setFixedSize(BTN_WID,BTN_HEI);
    m_okBtn->setStyleSheet(g_enableBtnStyle);

    m_cancelBtn = new QPushButton(myLan.cancel);
    m_cancelBtn->setFixedSize(BTN_WID,BTN_HEI);
    m_cancelBtn->setStyleSheet(g_enableBtnStyle);

    QHBoxLayout *downLay = new QHBoxLayout;
    downLay->addWidget(m_okBtn);
    downLay->addWidget(m_cancelBtn);

    QVBoxLayout *Lay = new QVBoxLayout(m_widget);
    Lay->addWidget(m_titleLbl, 0, Qt::AlignLeft);
    Lay->addStretch();
    Lay->addLayout(btnLay[0]);
    Lay->addStretch();
    Lay->addLayout(btnLay[1]);
    Lay->addStretch();
    Lay->addLayout(btnLay[2]);
    Lay->addStretch();
    Lay->addLayout(hlayarrayBtn[0]);
    Lay->addStretch();
    Lay->addLayout(hlayarrayBtn[1]);
    Lay->addStretch();
    Lay->addLayout(downLay);
    Lay->addSpacing(20);
}

/*!
 * \brief 连接信号与槽
 */
void setExportTypeWidget::connectSigAndSlt()
{
    connect(m_typeBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onTypeBtnClicked(int)));
    connect(m_arrayBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(onArrayBtnClicked(int)));
    connect(m_okBtn,     SIGNAL(clicked(bool)), this, SLOT(onOKBtnClicked()));
    connect(m_cancelBtn, SIGNAL(clicked(bool)), this, SLOT(onCancelBtnClicked()));
}

/*!
 * \brief 槽函数: 关闭
 */
void setExportTypeWidget::onOKBtnClicked()
{
    m_widget->hide();
    accept();
}

/*!
 * \brief 槽函数: 关闭
 */
void setExportTypeWidget::onCancelBtnClicked()
{
    m_widget->hide();
    reject();
}

/*!
 * \brief 获取选择索引
 */
int setExportTypeWidget::getSelIndex()
{
    return m_index;
}

/*!
 * \brief 设置text
 * \param text
 */
void setExportTypeWidget::settitleText(QString text)
{
    m_titleLbl->setText(text);
}

/*!
 * \brief 设置text
 * \param lst
 */
void setExportTypeWidget::setcontentText(QStringList lst)
{
    for(int i=0; i<lst.size()&&i<3; i++){
        m_typeLbl[i]->setText(lst.at(i));
    }
}

int setExportTypeWidget::getArraytype()
{
    return m_arrayType;
}

void setExportTypeWidget::setarrayBtnshow()
{
    for(int i=0; i<2; i++){
        m_arrayBtn[i]->show();
        m_arrayLbl[i]->show();
    }
}

void setExportTypeWidget::setarrayBtntext(QStringList lst)
{
    for(int i=0; i<lst.size() && i<2; i++){
        m_arrayLbl[i]->setText(lst.at(i));
    }
}

/*!
 * \brief 槽函数: 关闭
 */
void setExportTypeWidget::onTypeBtnClicked(int index)
{
    m_index = index;
}

void setExportTypeWidget::onArrayBtnClicked(int index)
{
    m_arrayType = index;
}
