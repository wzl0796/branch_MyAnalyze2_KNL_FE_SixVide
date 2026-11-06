#ifndef ROUNDPLOT_H
#define ROUNDPLOT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include <QMouseEvent>
#include "qpainter.h"
#include "qtimer.h"
#include "qdebug.h"
class RoundPlot : public QWidget
{
	Q_OBJECT
public:
	RoundPlot(QWidget *parent = 0);
	~RoundPlot();

protected:
	void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *ev);

    void initLocalParams();
	void drawDial(QPainter *painter);
	void drawBgOut(QPainter *painter);
	void drawBgRound(QPainter *painter);
	void drawBgCenter(QPainter *painter);
	void drawText(QPainter *painter);

private:
	double minValue;            //最小值
	double maxValue;            //最大值
	double value;               //目标值
	double angle;               //起始旋转角度
    int precision;              //精确度,小数点几位
	QString unit;               //单位

    double valuePercent;        //目标值百分比（加减号中间显示）
    QString centerStr;         //中间显示的值

    int changeDirection;        //0 减少   1  增加

	QColor usedColor;           //已使用百分比颜色
	QColor freeColor;           //未使用百分比颜色

	QColor rangeTextColor;      //范围值文字颜色
	QColor valueTextColor;      //目标值文字颜色

	QColor valueBgColor;        //目标值背景色
	QColor outBgColor;          //外边框背景色
	QColor centerBgColorStart;  //中间圆环渐变背景起始颜色
	QColor centerBgColorEnd;    //中间圆环渐变背景结束颜色

    QTimer *timer;              //快速调节绘制定时器
    QTimer *pressTimer;         //按钮按下定时器
    qint64 startTime;           //按钮点击开始时间

    QPushButton *plusBtn;      //加号按钮
    QPushButton *minusBtn;     //减号按钮
    QLabel *percentLbl;        //按钮中间比例值

private slots:
	void updateValue();

    void onMinusBtnPressedSlt();
    void onMinusBtnReleasedSlt();
    void onPlusBtnPressedSlt();
    void onPlusBtnReleasedSlt();

    void onPressBtnTimerOutSlt();

    void updateFastAdjustValue();

public:
	//获取最小值
	double getMinValue()const
	{
		return minValue;
	}

	//获取最大值
    double getMaxValue()const
	{
		return maxValue;
	}

	//获取目标值
	double getValue()const
	{
		return value;
	}

	//获取旋转角度
	double getAngle()const
	{
		return angle;
	}

	//获取小数点后几位
    int getPrecision()const
	{
        return precision;
	}

	//获取单位
	QString getUnit()const
	{
		return unit;
	}

    //获取当前值
    int getCurrentValue()const
    {
        return valuePercent;
    }

public slots:
	//设置最大最小值-范围值
	void setRange(double minValue, double maxValue);
	void setRange(int minValue, int maxValue);

	//设置目标值
	void setValue(double value);
	void setValue(int value);

    //设置中间显示的值
    void setCenterText(QString str);

	//设置旋转角度
	void setAngle(double angle);

	//设置小数点后几位
    void setPrecision(int precision);

	//设置文字后面的单位
	void setUnit(QString unit);

	//设置已使用百分比颜色
	void setUsedColor(QColor usedColor);
	//设置未使用百分比颜色
	void setFreeColor(QColor freeColor);

	//设置范围值文字颜色
	void setRangeTextColor(QColor rangeTextColor);
	//设置目标值文字颜色
	void setValueTextColor(QColor valueTextColor);

	//设置目标值背景色
	void setValueBgColor(QColor valueBgColor);
	//设置外边框背景色
	void setOutBgColor(QColor outBgColor);
	//设置中央圆环背景色
	void setCenterBgColor(QColor centerBgColorStart, QColor centerBgColorEnd);
    //设置快速变化方向，增加还是减少
    void setChangeFlag(int flag);

signals:
    void valueChanged(int valuePercent);
    void pressed(void);
};


class RoundProcess : public QWidget
{
    Q_OBJECT
public:
    RoundProcess(QWidget *parent = 0);
    ~RoundProcess();

    void updateProcess(int nPercent);

protected:
    void paintEvent(QPaintEvent *);

    void initLocalParams();
    void drawDial(QPainter *painter);
    void drawBgCenter(QPainter *painter);
    void drawText(QPainter *painter);

signals:

public slots:

private:
    QString   centerStr;           //! 中间显示的值
    double    angle;               //! 起始旋转角度
    double    valuePercent;        //! 目标值百分比（加减号中间显示）
    QColor    usedColor;           //! 已使用百分比颜色
    QColor    freeColor;           //! 未使用百分比颜色
    QColor    valueBgColor;        //! 目标值背景色
    QColor    valueTextColor;      //! 目标值文字颜色
};

#endif // ROUNDPLOT_H
