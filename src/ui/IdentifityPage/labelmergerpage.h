#ifndef LABELMERGERPAGE_H
#define LABELMERGERPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include "core/globalflow/mylanguage.h"
#include "params/define.h"
#include "common/setexporttypewidget.h"
#include "common/myinputmethod.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"
#include "common/selectfiledialog.h"

class labelmerger : public QWidget
{
    Q_OBJECT
public:
    explicit labelmerger(QWidget *parent = nullptr);
    void createPage();
    void layOutPage();
    void connectAndSlt();
    void refreshPage(int nCamId);
    void refreshBtnStaus(void);
signals:

public slots:
    void onMegerlabBtnPressed(void);
    void onRemovelabBtnPressed(void);

private:
    QGroupBox   *m_MerlabGb;
    QLabel      *m_orilablb;

    QListWidget *m_orilablstwid;
    QTreeWidget *m_MergrtreWid;
    QPushButton *m_mergerBtn;
    QPushButton *m_removeBtn;

    int         m_ntype=0;                  //! 相机ID
private:
    void refreshlablstwiditem(void);
    void refreMergrlabtreewid(void);
};

#endif // LABELMERGERPAGE_H
