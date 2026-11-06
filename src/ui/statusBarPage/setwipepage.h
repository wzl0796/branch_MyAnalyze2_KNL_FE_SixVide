#ifndef SETWIPEPAGE_H
#define SETWIPEPAGE_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>
#include "params/define.h"
#include "params/globalparams.h"
#include "core/globalflow/mylanguage.h"

class setWipePage : public QDialog
{
    Q_OBJECT
public:
    explicit setWipePage(QWidget *parent = 0);

    void refreshDisplay(void);

signals:

public slots:
    void onWipeValueBtnPressedSlt(void);

    void onWipeDelayValuePressedSlt(void);

    void onManulWipeBtnPreesedSlt(void);

    void onAirknifeValuechgSlt(void);

    void onAirknifeChannelSlt(void);

    void onBackBtnPressedSlt(void);

    void onResetjarSlt();

private:
    void initParams(void);

    void createPage(void);

    void connectSigAndSlot(void);

private:
    QWidget      *m_widget;
    QGroupBox    *m_groupBox;
    QLabel       *m_WipeLbl, *m_WipeDelayLbl;
    QPushButton  *m_btnWipe,*m_btnWipeValue,*m_btnWipeDelayValue;
    QPushButton  *m_backBtn;
    QLabel       *m_Airknifetimelb, *m_Airknifechanellb;
    QPushButton  *m_AirknifeTimebtn,*m_AiarknifChanelbtn;
    QPushButton  *m_resetjarBtn;

private:
    int nAnalyzeNum;

    bool bChanged = false;
};

#endif // SETWIPEPAGE_H
