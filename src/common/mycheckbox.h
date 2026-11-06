#ifndef MYCHECKBOX_H
#define MYCHECKBOX_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include "params/define.h"

class MyCheckBox : public QWidget
{
    Q_OBJECT
public:
    explicit MyCheckBox(const QString text, bool isChecked = false, QBoxLayout::Direction dir = QBoxLayout::LeftToRight, QWidget *parent = 0);
    bool isChecked();
    void setText(QString text);
    void setChecked(bool checked);
    void setCheckBtnSize(int wid, int hei);
    void setTextFont(QFont font);
    QString text(){return checkLabel->text();}
signals:
    void pressed();
    void toggled(bool checked);

public slots:
    void click();

private:
    QPushButton *checkBtn;
    QLabel      *checkLabel;
    bool        checked;
};

#endif // MYCHECKBOX_H
