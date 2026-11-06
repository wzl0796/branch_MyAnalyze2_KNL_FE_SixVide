#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>
#include "params/define.h"

class myListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit myListWidget(QWidget *parent = 0);

signals:

public slots:
};

#endif // MYLISTWIDGET_H
