#ifndef MYLABEL_H
#define MYLABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class mylabel : public QLabel
{
    Q_OBJECT
public:
    explicit mylabel(QWidget *parent = 0);

protected:
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void released(void);

public slots:
};

#endif // MYLABEL_H
