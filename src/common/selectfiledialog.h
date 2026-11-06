#ifndef SELECTFILEDIALOG_H
#define SELECTFILEDIALOG_H

#include <QDialog>
#include <QLayout>
#include <QDir>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include "core/globalflow/mylanguage.h"

class SelectFileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectFileDialog(QWidget *parent = 0,bool bMultiSelect = false);
    explicit SelectFileDialog(QStringList strList, QWidget *parent = 0, bool bMultiSelect = false);

    void setDialogAttr(void);       /*!< 设置对话框属性 */
    void createPage(void);          /*!< 创建页面 */
    void connectSigAndSlt(void);    /*!< 连接信号与槽 */
    void setFilter(QStringList strList);    /*!< 设置过滤器 */
    QString getFileName(void);      /*!< 获取文件名 */
    QStringList getFileNames(void);  /*!< 获取文件名列表 */
    void setDir(QString strDir,QStringList strList, bool bUseDir = true);
signals:

public slots:
    void onOkBtnPressed(void);      /*!< 槽函数: 确定 */
    void onCancelBtnPressed(void);  /*!< 槽函数: 取消 */
private:
    QPushButton   *m_okBtn;          /*!< 确定 */
    QPushButton   *m_cancelBtn;      /*!< 取消 */
    QListWidget   *m_listWidget;     /*!< 文件列表 */
    QStringList   m_fileStrList;      /*!< 文件名列表 */
    QLabel        *m_titleLabel;
    QWidget       *widget;
    bool          m_bMultiSelect;
};

#endif // SELECTFILEDIALOG_H
