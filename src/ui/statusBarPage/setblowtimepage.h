#ifndef SETBLOWTIMEPAGE_H
#define SETBLOWTIMEPAGE_H

#include <QDialog>
#include <QObject>
#include "params/define.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include "params/globalparams.h"
#include "core/globalflow/globalflow.h"
#include "common/mymessagebox.h"
#include "common/myinputpanel.h"
#include "common/mylineedit.h"
#include "core/globalflow/mylanguage.h"

class setblowtimepage : public QDialog
{
    Q_OBJECT
public:
    setblowtimepage(QWidget *parent = 0);
    ~setblowtimepage();

    void createPage(void);
    void refreshpage();

public slots:
    void onBackBtnpressedSlt(void);
    void onDelayBtnpressedSlt();
    void onBlowBtnpressedSlt();
    void onRectResizeChgSlt();
private:
    QWidget *m_wid;
    QGroupBox *m_gb;
    QLabel *m_delaylb;
    QPushButton *m_delaybtn;
    QLabel *m_blowlb;
    QPushButton *m_blowbtn;
    QLabel *m_resizeRectLb;
    MyLineEdit *m_resizeRectLe;

    QPushButton *m_bckbtn;
};

#endif // SETBLOWTIMEPAGE_H
