#ifndef IDENTIFITY_H
#define IDENTIFITY_H

#include <QWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include "core/globalflow/mylanguage.h"
#include "params/globalparams.h"
#include "labelsetpage.h"
#include "aiwidget.h"
#include "shapenesspage.h"
#include "traditionarith.h"

class Identifity : public QWidget
{
    Q_OBJECT

public:
    enum {
        PAGE_LAB_SET,
        PAGE_AI,
        PAGE_SHAPENESS,
        PAGE_TRADITON,
        PAGE_ALL
    };
public:
    explicit Identifity(QWidget *parent = nullptr);
    void cratePage();
    void layoutPage();
    void connectAndSlt();
    void refreshPage();
signals:

public slots:
    void onListitemChgSlt(int nIdx);
    void onMachineTypeChgSlt(bool bBottle);

private:
    QStackedWidget *m_stackWid;
    QListWidget    *m_listWid;

    labelsetPage   *m_labSetPage;
    AIWidget       *m_aiPage;
    shapenesspage  *m_shapenessPage;
    traditionArith *m_traditionarithPage;
};

#endif // IDENTIFITY_H
