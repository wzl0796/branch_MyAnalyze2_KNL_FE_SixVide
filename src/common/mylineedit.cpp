#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    initparams();
    _setLeStyle();
}


void MyLineEdit::initparams()
{
    m_sizeLe[LE_NORMAL] = QSize(150, 60);
    m_sizeLe[LE_SMALL] = QSize(100, 58);
    m_sizeLe[LE_BIG] = QSize(256, 60);

    int pFont[FONT_TYPE] = {21, 25, 30};
    for (int i = 0; i < FONT_TYPE; i++) {
        m_font[i].setFamily("Microsoft YaHei");
        m_font[i].setPixelSize(pFont[i]);
    }
}

QFont MyLineEdit::getFont(int nType)
{
    if (nType >= 0 && nType < FONT_TYPE) {
        return m_font[nType];
    }
    return m_font[FONT_NORMAL];
}

QSize MyLineEdit::getLeSize(int nType)
{
    if (nType < LE_TYPE) {
        return m_sizeLe[nType];
    }
    return m_sizeLe[LE_NORMAL];
}


/*!
 * \brief MyLineEdit::_setLeStyle
 */
void MyLineEdit::_setLeStyle()
{
    setFont(getFont());
    setFixedSize(getLeSize());
    setStyleSheet("background-color:rgba(220,220,220,100)");
}

/*!
 * \brief MyLineEdit::mousePressEvent
 */
void MyLineEdit::mousePressEvent(QMouseEvent *ev)
{
    emit pressed();
    QLineEdit::mousePressEvent(ev);
}




