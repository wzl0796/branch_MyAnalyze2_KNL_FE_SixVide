#include "selectfiledialog.h"
#include "params/define.h"

SelectFileDialog::SelectFileDialog(QWidget *parent,bool bMultiSelect) : QDialog(parent),m_bMultiSelect(bMultiSelect)
{
    setDialogAttr();

    createPage();

    connectSigAndSlt();
}

SelectFileDialog::SelectFileDialog(QStringList strList, QWidget *parent, bool bMultiSelect) :QDialog(parent),
                m_bMultiSelect(false)
    
{
    m_bMultiSelect = bMultiSelect;

    setDialogAttr();

    createPage();

    connectSigAndSlt();

    setFilter(strList);
}

/*!
 * \brief 设置对话框属性
 */
void SelectFileDialog::setDialogAttr()
{
    setWindowFlags(Qt::FramelessWindowHint);

    setGeometry(0, 0, WIN_WID+10, WIN_HEI+10);
}

/*!
 * \brief 创建页面
 */
void SelectFileDialog::createPage()
{
    QFont font;
    font.setPixelSize(20);
    QFont titleFont;
    titleFont.setPixelSize(25);
    setStyleSheet("background-color: rgb(0,0,0,100);");

    widget = new QWidget(this);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setGeometry(WIN_WID/3, WIN_HEI/4, 750, 480);
    widget->setStyleSheet(QString("QWidget{background-color:%1;}").arg(g_ColorLightGray3));
    widget->show();

    //! 布局管理器
    QVBoxLayout *mainLay = new QVBoxLayout(widget);
    QHBoxLayout *upLay = new QHBoxLayout;
    QHBoxLayout *downLay = new QHBoxLayout;

    m_titleLabel = new QLabel;
    m_titleLabel->setFixedSize(BTN_WID*4, BTN_HEI);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->hide();

    //! 页面控件
    m_listWidget = new QListWidget;
    m_listWidget->setStyleSheet(QString("QListWidget{border:1px solid lightgray; border-radius:10px;outline:0px;"
                                      "background-color:white;}")
                              +QString("QListWidget::Item:selected{background-color:%1;color:white;}").arg(g_ColorMYGreen)
                              +QString("QListWidget::Item:first{border-radius:5px}"));
    m_listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listWidget->setFont(font);
    if(m_bMultiSelect){
        m_listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    }
   
    m_okBtn = new QPushButton(myLan.ok);
    m_okBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_okBtn->setStyleSheet("background-color:white;");
    m_cancelBtn = new QPushButton(myLan.cancel);
    m_cancelBtn->setFixedSize(BTN_WID, BTN_HEI);
    m_cancelBtn->setStyleSheet("background-color:white;");

    upLay->addWidget(m_titleLabel);

    downLay->addWidget(m_okBtn);
    downLay->addStretch();
    downLay->addWidget(m_cancelBtn);
    mainLay->addLayout(upLay);
    mainLay->addWidget(m_listWidget);
    mainLay->addLayout(downLay);
}

/*!
 * \brief 连接信号与槽
 */
void SelectFileDialog::connectSigAndSlt()
{
    connect(m_okBtn,     SIGNAL(pressed()), this, SLOT(onOkBtnPressed()));
    connect(m_cancelBtn, SIGNAL(pressed()), this, SLOT(onCancelBtnPressed()));
}

/*!
 * \brief 设置过滤器
 * \param strList 过滤字符串
 */
void SelectFileDialog::setFilter(QStringList strList)
{
    QDir dir(g_sUpgradeDir);

    m_fileStrList.clear();
    m_titleLabel->hide();
    m_fileStrList = dir.entryList(strList);

    for (int i = 0; i < m_fileStrList.size(); i++) {
        QString str = QString("\n%1").arg(m_fileStrList.at(i));
        m_listWidget->addItem(str);
    }

    if (m_fileStrList.size() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

/*!
 * \brief 获取文件名
 * \return 文件名
 */
QString SelectFileDialog::getFileName()
{
    QString str = QString("");
    int nRow = m_listWidget->currentRow();
    if (nRow >= 0 && m_fileStrList.size() > nRow) {
        str = m_fileStrList.at(nRow);
    }

    return str;
}

/*!
 * \brief 获取文件名列表
 * \return 文件名列表
*/
QStringList SelectFileDialog::getFileNames()
{
    if(m_bMultiSelect){
        QStringList strList;
        for(int i = 0; i < m_listWidget->selectedItems().size(); i++){
            strList<<m_listWidget->selectedItems().at(i)->text().remove("\n");
        }
        return strList;
    }
    return QStringList();
}

/*!
 * \brief 槽函数: 确定
 */
void SelectFileDialog::onOkBtnPressed()
{
    accept();
}

/*!
 * \brief 槽函数: 取消
 */
void SelectFileDialog::onCancelBtnPressed()
{
    reject();
}

void SelectFileDialog::setDir(QString strDir, QStringList strList, bool bUseDir)
{
    if(bUseDir){
        QDir dir(strDir);
        m_fileStrList.clear();
        m_fileStrList = dir.entryList(strList);

        for (int i = 0; i < m_fileStrList.size(); i++) {
            QString str = QString("\n%1").arg(m_fileStrList.at(i));
            m_listWidget->addItem(str);
        }

        if (m_fileStrList.size() > 0) {
            m_listWidget->setCurrentRow(0);
        }
    }else{
        m_fileStrList.clear();
        m_fileStrList = strList;
        if (m_fileStrList.size() > 0) {
            for(QString var : m_fileStrList){
                m_listWidget->addItem(var);
            }
            m_listWidget->setCurrentRow(0);
        }
    }
}
