#ifndef SHAPENESSPAGE_H
#define SHAPENESSPAGE_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include "common/mycheckbox.h"
#include "common/mylineedit.h"
#include "common/myswitchbutton.h"
#include "core/globalflow/mylanguage.h"
#include "params/globalparams.h"
#include "common/myinputpanel.h"

class shapenesspage:public QWidget
{
    Q_OBJECT
public:
    explicit shapenesspage(QWidget *parent = nullptr);

    void cratePage();
    void layoutPage();
    void connectAndSlt();
    void refreshPage();

public slots:
    void onshapeEnStaChgSlt(bool bsta);
    void onshapeEnablechgSlt(int nIdx);
    void onShapeParamsChgSlt(int nIdx);
    void onShapeLabelEnChgSlt(int nIdx);
    void onClassfitythdChgSlt(int nIdx);
private:
    //! 形选
    QGroupBox   *m_staticParamsetGb; //! 形选参数
    MyCheckBox  *m_shapeParamschk[3];
    QLabel      *m_framelb[3];
    MyLineEdit  *m_shapeParamsle[6];
    QLabel      *m_bEnshapelb;
    mySwitchButton *m_bEnshapesw;

    QGroupBox   *m_shplabEnGb;   //! 形选标签
    MyCheckBox  *m_shplabchk[MAX_LABELS];

    //! 其他
    QGroupBox   *m_classficationGb;
    QLabel      *m_classifylb[MAX_CLASSIFY_NUM -1];
    MyLineEdit  *m_classifyle[MAX_CLASSIFY_NUM -1];

    QStringList m_shapelst;

};

#endif // SHAPENESSPAGE_H
