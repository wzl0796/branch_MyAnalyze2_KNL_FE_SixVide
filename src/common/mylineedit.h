#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QFont>


/*!
 * \enum 枚举编辑框尺寸类型
 */
enum {
    LE_NORMAL = 0,  /*! 正常尺寸 */
    LE_SMALL,       /*! 小尺寸 */
    LE_BIG,         /*! 大尺寸 */
    LE_TYPE
};

enum {
    FONT_SMALL = 0, /*! 小字体：20 */
    FONT_NORMAL,    /*! 标准字体：25 */
    FONT_TITLE,     /*! 大字体：30 */
    FONT_TYPE

};

class MyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit MyLineEdit(QWidget *parent = NULL);

    void initparams(void);
    QFont getFont(int nType = FONT_NORMAL);
    QSize getLeSize(int nType = LE_SMALL);

protected:
    void mousePressEvent(QMouseEvent *ev);

signals:
    void pressed(void);

public slots:

private:
    void _setLeStyle(void);     /*! 设置样式 */
    QFont m_font[FONT_TYPE];
    QSize m_sizeLe[LE_TYPE];
};

#endif // MYLINEEDIT_H
