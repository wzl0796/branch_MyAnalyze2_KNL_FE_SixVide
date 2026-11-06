#ifndef MYDEVICELIST_H
#define MYDEVICELIST_H

#include <QWidget>
#include <QPushButton>
#include <QFrame>
#include <QSignalMapper>
#include "params/define.h"


//分析仪列表控件
class myDeviceList : public QWidget
{
    Q_OBJECT
public:
    explicit myDeviceList(QWidget *parent = 0);
    void refreshPage();

    void setCurrentIndex(int index);

signals:
    void changeSelected(int index);

public slots:
    void onDeviceChanged(int index);

private:
    QPushButton     *m_deviceBtn[MAX_ANALYZE_NUM];
    QFrame          *m_Vline[MAX_ANALYZE_NUM-1];
    QSignalMapper   *m_deviceBtnMap;

    int             m_Num;              //总设备数
    int             m_currentIndex;     //当前设备索引

    QString         m_styleSelect;
    QString         m_styleUnSelect;
};

#endif // MYDEVICELIST_H
