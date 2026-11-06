#include "mycheckbox.h"

MyCheckBox::MyCheckBox(const QString text, bool isChecked, QBoxLayout::Direction dir, QWidget *parent) : QWidget(parent)
{
    /* buttons init */
    checkLabel = new QLabel(text);

    checkBtn = new QPushButton();
    checkBtn->setFixedSize(SMALL_BTN_WID-10,SMALL_BTN_WID-10);
    checkBtn->setIconSize(QSize(SMALL_BTN_WID-12, SMALL_BTN_WID-12));
    QString btnStyle = QString("QPushButton {"
                               "text-align: center;"
                               "outline: none;"
                               "background-color: #ffffff;"
                               "border-color: #afafaf;"
                               "border-width: 1px;"
                               "border-style: solid;"
                               "border-radius: 5;"
                               "padding: 3px;}"

                               "QPushButton:pressed {"
                               "padding-left: 5px;"
                               "padding-top: 5px;"
                               "background-color: #036eb8;}");

    checkBtn->setStyleSheet(btnStyle);

    /* buttons update */
    checked=false;
    setChecked(isChecked);

    /* signals to slots */
    connect(checkBtn, SIGNAL(pressed()), this, SLOT(click()));

    /* layout */
    QBoxLayout *mainLayout = new QBoxLayout(dir);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(checkBtn);
    mainLayout->addWidget(checkLabel);
    setLayout(mainLayout);
}

void MyCheckBox::click()
{
    setChecked(!checked);
    emit pressed();
}

void MyCheckBox::setChecked(bool isChecked)
{
    if (checked != isChecked) {
        /* update params */
        checked = isChecked;

        /* update display */
        if (checked) {
            checkBtn->setIcon(QIcon(":/res/png/action_select.png"));
        } else {
            checkBtn->setIcon(QIcon(""));
        }

        /* trigger */
        emit toggled(checked);
    }
}

bool MyCheckBox::isChecked()
{
    return checked;
}

void MyCheckBox::setText(QString text)
{
    checkLabel->setText(text);
}

void MyCheckBox::setCheckBtnSize(int wid, int hei)
{
    checkBtn->setFixedSize(QSize(wid, hei));
    checkBtn->setIconSize(QSize(wid-5, hei-5));
}

void MyCheckBox::setTextFont(QFont font)
{
    checkLabel->setFont(font);
}
