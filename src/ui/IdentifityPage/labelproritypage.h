#ifndef LABELPRORITYPAGE_H
#define LABELPRORITYPAGE_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QSignalMapper>
#include "common/mylineedit.h"
#include "params/globalparams.h"
#include "common/myinputpanel.h"
#include "core/globalflow/globalflow.h"
class labelprority : public QWidget
{
    Q_OBJECT
public:
    explicit labelprority(QWidget *parent = nullptr);
    void createPage();
    void layOutPage();
    void connectAndSlt();
    void refreshPage(int nCamid);

signals:

public slots:
    void onlabMutexleChgSlt(int nIdx);
    void onlabPriorityChgSlt(int nIdx);
    void onbtnSyncPressedSlt();

private:
    void reFreshlabPri(int nCamId);
    void reFreshlabMutex(int nCamId);

private:
    QGroupBox   *m_labProGb;
    QPushButton *m_modeTypeBtn;
    bool        m_bSync;
    int         m_viewIdx;

    QGroupBox   *m_labPriGb; //! 标签优先级
    QLabel      *m_labnamelb[MAX_LABELS];
    MyLineEdit  *m_labPrile[MAX_LABELS];

    QGroupBox   *m_labMutexgb; //! 标签互斥
    QLabel      *m_labmutexlb[MAX_LABELS];
    MyLineEdit  *m_labmutexle[MAX_LABELS];
};

#endif // LABELPRORITYPAGE_H
