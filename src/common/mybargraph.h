#ifndef MYBARGRAPH_H
#define MYBARGRAPH_H

#include <QPainter>
#include <QPaintEvent>
#include <QMap>
#include "params/define.h"
//#include "mylanguage.h"

/*!
 * \brief 柱状图控件类
 */
class myBarGraph : public QWidget
{
    Q_OBJECT
public:
    explicit myBarGraph(QWidget *parent = 0);

    void setValue(QList<double>, QList<double>, QList<double>, QStringList);
    void setMaxValue(double maxValue);
    void setTitle(QString title);

signals:

protected:
    void paintEvent(QPaintEvent *ev);

public slots:

private:
    QList<double>     m_fTargetMinValue;  // 分类目标数据
    QList<double>     m_fTargetMaxValue;  // 分类目标数据
    QList<double>     m_fRealValue;    // 分类实际数据
    QStringList       m_sType;         // 分类名称
    double            m_maxValue;      // 最大柱状高度
    QString           m_sTitle;        // 标题
};

#endif // MYBARGRAPH_H
