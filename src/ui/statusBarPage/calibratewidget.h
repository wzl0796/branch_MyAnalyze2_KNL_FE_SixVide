#ifndef CALIBRATEWIDGET_H
#define CALIBRATEWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QTabBar>
#include <QButtonGroup>
#include "src/core/globalflow/globalflow.h"
#include "src/common/mylineedit.h"
#include "core/globalflow/mylanguage.h"
#include "common/myswitchbutton.h"

/*!
 * \brief 标准设置页面
 */
class calibrateWidget : public QDialog
{
    Q_OBJECT
public:
    calibrateWidget(QWidget *parent = 0);

    ~calibrateWidget();

public slots:
    void onArithSensChanged(int nIdx);
    void onCamidBtnSlt(int nIdx);
    void onSyncOrAloneBtnPressed(void);
    void onSamplechnelBtnPressed(void);
    void onSimEnChgSlt(bool bSta);
    void onSimSaveMatChgSlt(bool bSta);
protected:
    virtual void mousePressEvent(QMouseEvent *event);

private:
    void createMainPage(void);
    void createSchMainPage(void);

    void refreshPage();

    QString setPointPos(double value, uint smallNum);

    void refreshArithSens(int nCamId);
private:
    QWidget *widget;
    QWidget *pageSchMain;

    QButtonGroup*m_camIdbtnGroup;
    QPushButton *m_camIdbtns[MAX_CAM_NUM];
    QPushButton *m_syncBtn;
    QPushButton *m_samplechanelBtn;

    QLabel   *m_Arithsenslb[MAX_LABELS];
    MyLineEdit *m_Arithsensle[MAX_LABELS];
    QLabel *m_SimEnlb;
    mySwitchButton *m_SimEnsw;
    QLabel *m_SimMatEnlb;
    mySwitchButton *m_SimMatEnsw;

    int         m_nCamidx = 0;

    bool        m_bSync = false;
    int         m_nsamplechanel = 0;

};

#endif // CALIBRATEWIDGET_H
