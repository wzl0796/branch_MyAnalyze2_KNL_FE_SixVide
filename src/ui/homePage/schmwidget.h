#ifndef SCHMWIDGET_H
#define SCHMWIDGET_H

#include <QDialog>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include "core/globalflow/sqliteinter.h"
#include "core/io/iot/localsocketthread.h"

class schmWidget : public QDialog
{
    Q_OBJECT
public:
    explicit schmWidget(int curShemIndex, QWidget *parent = 0);

    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

    void updateHomeList();                   /*! 更新首页列表 */

signals:

public slots:
    void onCopyBtnClickedSlt();
    void onDeleteBtnClickedSlt();
    void onEditBtnClickedSlt();
    void onHomeListChangedSlt(int index);
    void onCloseSchemeSlt();
    void onBackupSchemeSlt();
    void onRestoreSchemeSlt();

private:
    int        m_materialType;
    int        m_curSchemIndex;

    QString    m_schemCode;
    QString    m_schemName;

private:
    QWidget     *m_widget;

    QListWidget *m_homelist;
    QLabel      *m_schmLbl;
    QPushButton *m_copyBtn, *m_deleteBtn, *m_editBtn, *m_closeBtn;
    QPushButton *m_backupBtn, *m_restoreBtn;
};

#endif // SCHMWIDGET_H
