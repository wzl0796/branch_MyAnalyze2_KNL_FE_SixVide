#ifndef SELECTLANGWIDGET_H
#define SELECTLANGWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class selectLangWidget : public QWidget
{
    Q_OBJECT
public:
    explicit selectLangWidget(QWidget *parent = 0);

    void refreshDisplay();

signals:
    void updateList();

public slots:
    void onAddBtnClicked();
    void onDelBtnClicked();
    void onCancelBtnClicked();
    void onSureBtnClicked();

private:
    void updateSelectWidget();

private:
    QWidget    *m_langEnaWidget;

    /* 语言设置界面的控件 */
    QListWidget *m_allListWidget;
    QListWidget *m_selListWidget;

    QPushButton *m_addBtn, *m_delBtn;
    QPushButton *m_sureBtn, *m_cancelBtn;

    QList<int>  m_allList;
    QList<int>  m_selList;
    int         m_currentIndex;
};

#endif // SELECTLANGWIDGET_H
