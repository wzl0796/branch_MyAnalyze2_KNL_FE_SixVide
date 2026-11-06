#ifndef MYPIEGRAPH_H
#define MYPIEGRAPH_H

#include <QMap>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include "params/define.h"
//#include "mylanguage.h"

/*!
 * \brief 饼状图控件类
 */
class myPieGraph : public QWidget
{
    Q_OBJECT
public:
    explicit myPieGraph(QWidget *parent = 0);

    void setValue(double, double, double);
    void resetTypeName();
    void setTitle(QString title);
    void setMatType(int nMatType);

signals:

protected:
    void paintEvent(QPaintEvent *ev);

public slots:

private:
    double            m_fValue1;       // 杂质
    double            m_fValue2;       // 碎米
    double            m_fValue3;       // 互混
    QString           m_sTitle;        // 标题
    int               m_nMatType;      // 物料种类
    QStringList       strType;         // 分类名称
};

//针对单个饼状图通用化
class mySinglePieGraph : public QWidget
{
    Q_OBJECT
public:
    explicit mySinglePieGraph(QWidget *parent = 0);

    bool setName(QStringList cStringList, QList<QColor> colorList);
    bool setValue(QList<double> fValueList);
    void setTitle(QString title);
    void setMatType(int nMatType);

signals:

protected:
    void paintEvent(QPaintEvent *ev);

public slots:

private:
    enum{
        COLOR1 = 0,
        COLOR2 = 170,
        COLOR3 = 245
    };
    QList<double>     m_fValue;
    QStringList       m_cStringList;
    int               m_dataSize;
    QList<QColor>     m_colorList;
    int               m_nMatType;      // 物料种类
    QString           m_sTitle;        // 标题
};
#endif // MYPIEGRAPH_H
