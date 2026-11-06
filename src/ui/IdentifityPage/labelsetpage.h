#ifndef LABELSETPAGE_H
#define LABELSETPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QSignalMapper>
#include <QTabBar>
#include <QStackedWidget>
#include "common/mylineedit.h"
#include "common/mycheckbox.h"
#include "core/globalflow/mylanguage.h"
#include "params/globalparams.h"
#include "common/myinputpanel.h"
#include "labelmergerpage.h"
#include "labelproritypage.h"
#include "labelmixpage.h"
#include "core/globalflow/globalflow.h"

#define MAX_MIXLAB_NUM 60
class labelsetPage : public QWidget
{
    Q_OBJECT
public:
    enum{
        LAB_HIDE,
        LAB_COEFF,
        LAB_THD,
        LAB_INDEX,
        LAB_SECONDARY,
        LAB_PAOPERTY,
        LAB_ALL
    };
public:
    explicit labelsetPage(QWidget *parent = nullptr);
    void createPage();
    void layOutPage();
    void connectAndSlt();
    void refreshPage();

signals:

public slots:
    void onTabchgedSlt(int nIdx);

    void onTypeBtnPressedSlt();
    void onLabhidPressedSlt(int nIdx);
    void onLabcoeffChgSlt(int nIdx);
    void onLabthdChgSlt(int nIdx);
    void onLabpropertyChgSlt(int nIdx);
    void onLabidxChgSlt(int nIdx);
    void onMetalnumChgSlt(void);
    void onLabsecondaryChgSlt(int nIdx);
private:
    void reRreshlabWid();

private:
    QPushButton *m_typeswBtn;
    int         m_nIdx=0;   //! 相机ID

    QTabBar        *m_tabBar;
    QStackedWidget *m_stackwid;

    labelmerger  *m_labMerPage; //! 标签合并

    labelprority *m_labPriPage; //! 标签优先级

    labelmixpage *m_labMixPage; //! 标签互混

    QWidget     *m_labParamsPage; //! 标签参数
    QGridLayout *glaymaterial;
    QGroupBox   *m_materialParamsSetGb;
    QScrollArea *m_materialScroarea;
    QLabel      *m_titlelb[LAB_ALL];
    MyCheckBox  *m_labchk[MAX_MIXLAB_NUM];
    MyLineEdit  *m_labconvertcofle[MAX_MIXLAB_NUM];     //! 转换系数
    MyLineEdit  *m_labthdsetle[MAX_MIXLAB_NUM];        //! 阈值设置
    MyLineEdit  *m_labIdxle[MAX_MIXLAB_NUM];           //! 标签序号
    QPushButton *m_propertysetBtn[MAX_MIXLAB_NUM];     //! 好坏属性设置
    MyCheckBox  *m_secondarychk[MAX_MIXLAB_NUM];      //! 次一级标签
    QLabel      *m_Metallb;
    MyLineEdit  *m_Metalle;


};

#endif // LABELSETPAGE_H
