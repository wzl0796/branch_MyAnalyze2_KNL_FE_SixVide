#ifndef MYROUNDEDPROGRESS_H
#define MYROUNDEDPROGRESS_H

#include <QWidget>

class MyRoundedProgress : public QWidget
{
    Q_OBJECT
public:
    explicit MyRoundedProgress(QWidget *parent = 0);

    void setSum(const int & sum);
    void setCur(const int & cur);

    void setLegalMin(const int &min);
    void setLegalMax(const int &max);

signals:

protected:
    void paintEvent(QPaintEvent *event);

public slots:

private:
    int m_sum, m_cur;
    int m_legalMin, m_legalMax;
};

#endif // MYROUNDEDPROGRESS_H
