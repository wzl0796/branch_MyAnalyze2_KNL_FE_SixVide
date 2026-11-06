#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QTime>
#include <QTimer>

class myPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit myPushButton();

protected:
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

signals:
    void long_clicked();

public slots:
    void timeOutSlt();

private:
    QTimer  *m_timer;         /*! 耗时计时器 */
    QTime   *m_time;          /*! 耗时计时器 */
    int     nTimePress;
    int     nTimeRelease;
};

#endif // MYPUSHBUTTON_H
