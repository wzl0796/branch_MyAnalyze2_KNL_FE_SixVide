#include "roundplot.h"

RoundPlot::RoundPlot(QWidget *parent) : QWidget(parent)
{
    initLocalParams();

    updateValue();

    plusBtn = new QPushButton(this);
    plusBtn->setIcon(QIcon(":/res/png/add.png"));
    plusBtn->setIconSize(QSize(48,48));
    plusBtn->setFixedSize(50,50);
    minusBtn = new QPushButton(this);
    minusBtn->setIcon(QIcon(":/res/png/deduce.png"));
    minusBtn->setIconSize(QSize(48,48));
    minusBtn->setFixedSize(50,50);
    percentLbl = new QLabel(QString("%1%").arg(valuePercent),this);
    percentLbl->setFixedSize(80,50);
    percentLbl->setAlignment(Qt::AlignHCenter);
    percentLbl->setStyleSheet(QString("font:20px;"));

    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer,      SIGNAL(timeout()),  this,   SLOT(updateFastAdjustValue()));

    pressTimer = new QTimer();
    connect(pressTimer, SIGNAL(timeout()), this, SLOT(onPressBtnTimerOutSlt()));

    connect(minusBtn,   SIGNAL(pressed()),  this,   SLOT(onMinusBtnPressedSlt()));
    connect(minusBtn,   SIGNAL(released()), this,   SLOT(onMinusBtnReleasedSlt()));
    connect(plusBtn,    SIGNAL(pressed()),  this,   SLOT(onPlusBtnPressedSlt()));
    connect(plusBtn,    SIGNAL(released()), this,   SLOT(onPlusBtnReleasedSlt()));
}

RoundPlot::~RoundPlot()
{
}

void RoundPlot::paintEvent(QPaintEvent *)
{
	int width = this->width();
	int height = this->height();

    minusBtn->setGeometry(width/2 - 90,height-50,40,40);
    plusBtn->setGeometry(width/2 + 55,height-50,40,40);
    percentLbl->setGeometry(width/2 - 40,height-40,80,50);

	//绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width / 2, height / 2);

	int side = qMin(width, height);
	painter.scale(side / 200.0, side / 200.0);

	//绘制已使用百分比及未使用百分比进度
	drawDial(&painter);

    //绘制最里框文字圆形背景
	drawBgCenter(&painter);

    //绘制中间值文字
    drawText(&painter);

    //刷新加减号中间值比例值
    percentLbl->setText(QString("%1%").arg(valuePercent));
}

void RoundPlot::mouseReleaseEvent(QMouseEvent *ev)
{
    int x = ev->x();
    int y = ev->y();

//    int width = this->width();
//    int height = this->height();
    if (x >= 128 && x <= 232 && y >= 140 && y <= 240) {
        emit pressed();
    }
    qDebug() << "x: " << x << " y: " << y;
}

/* 初始化参数*/
void RoundPlot::initLocalParams()
{
    minValue = 0;
    maxValue = 100;
    value = 0;
    centerStr = "";
    precision = 0;
    angle = 40;
    unit = "";
    changeDirection = 0;

    valuePercent = 0;

    usedColor = QColor(0, 116, 87);
    freeColor = QColor(169, 169, 169);

    rangeTextColor = QColor(103, 103, 103);
    valueTextColor = QColor(250, 250, 250);

    valueBgColor = QColor(0, 116, 87);
    outBgColor = QColor(202, 225, 255);
    centerBgColorStart = QColor(67, 205, 128);
    centerBgColorEnd = QColor(60, 179, 113);
}

void RoundPlot::drawDial(QPainter *painter)
{
    int radius = 80;
	double lineWidth = 2.5;
	painter->save();
	painter->rotate(angle);

	//根据起始旋转的角度计算每次坐标需要旋转的角度,按照100等分计算
	//每次旋转的角度=360-(起始角度*2--分左右)/100
	double rotate = (double)(360 - (angle * 2)) / 100;

	//绘制已使用百分比
	painter->setPen(QPen(usedColor, lineWidth));

    for (double i = 0; i < valuePercent; i++) {
		painter->drawLine(0, radius, 0, radius / 1.2);
		painter->rotate(rotate);
	}

	//绘制未使用百分比
	painter->setPen(QPen(freeColor, lineWidth));

    for (double i = valuePercent; i < 100; i++) {
		painter->drawLine(0, radius, 0, radius / 1.2);
		painter->rotate(rotate);
	}

    painter->restore();
}

void RoundPlot::drawBgOut(QPainter *painter)
{
    int radius = 65;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(outBgColor);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void RoundPlot::drawBgRound(QPainter *painter)
{
    int radius = 50;
	painter->save();
	QConicalGradient conicalGradient(radius, radius, 90);
	conicalGradient.setColorAt(0, centerBgColorStart);
	conicalGradient.setColorAt(1.0, centerBgColorEnd);
	painter->setPen(Qt::NoPen);
	painter->setBrush(conicalGradient);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void RoundPlot::drawBgCenter(QPainter *painter)
{
    int radius = 40;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(valueBgColor);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void RoundPlot::drawText(QPainter *painter)
{
	painter->save();

//    QString strCenterValue = QString("%1").arg(QString::number(centerValue, 'f', precision));
    QString strUnit = QString("%1").arg(unit);

    QFont ft;
    ft.setPixelSize(20);
    // 绘制中心数值
    painter->setFont(ft);
    painter->setPen(QPen(valueTextColor));
    QFontMetricsF fm1 = QFontMetricsF(painter->font());
    QSizeF size1 = fm1.size(Qt::TextSingleLine, centerStr);
    painter->drawText(-size1.width() / 2, size1.height() / 3, centerStr);

    //绘制中心数值单位
    painter->setFont(QFont("Arial",12));
    painter->setPen(QPen(valueTextColor));
    QFontMetricsF fm2 = QFontMetricsF(painter->font());
    QSizeF size2 = fm2.size(Qt::TextSingleLine, strUnit);
    painter->drawText(-size2.width() / 2, size2.height() / 3 + 25, strUnit);

	painter->restore();
}

void RoundPlot::updateValue()
{
    update();
}

/* 响应减号按钮按下事件*/
void RoundPlot::onMinusBtnPressedSlt()
{
    startTime = QDateTime::currentMSecsSinceEpoch();
    pressTimer->start(1000);
    setChangeFlag(0);
}

/* 响应减号按钮释放事件*/
void RoundPlot::onMinusBtnReleasedSlt()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    pressTimer->stop();
    if((currentTime - startTime) < 1000) { //按压时间少于1s，单步调节
        valuePercent--;
        if(valuePercent < minValue) {
            valuePercent = minValue;
        }
    } else {
        if(timer->isActive()) {
            timer->stop();
        }
    }

    update();

    emit valueChanged(valuePercent);
}

/* 响应加号按钮按下事件*/
void RoundPlot::onPlusBtnPressedSlt()
{
    startTime = QDateTime::currentMSecsSinceEpoch();
    pressTimer->start(1000);
    setChangeFlag(1);
}

/* 响应加号按钮释放事件*/
void RoundPlot::onPlusBtnReleasedSlt()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    pressTimer->stop();
    if((currentTime - startTime) < 1000) { //按压时间少于1s，单步调节
        valuePercent++;
        if(valuePercent > maxValue) {
            valuePercent = maxValue;
        }
    } else {
        if(timer->isActive()) {
            timer->stop();
        }
    }

    update();

    emit valueChanged(valuePercent);
}

/* 响应按钮按压定时器超时事件*/
void RoundPlot::onPressBtnTimerOutSlt()
{
    timer->start();
}

/* 响应快速调节定时器超时事件*/
void RoundPlot::updateFastAdjustValue()
{
    if(changeDirection) {
        if (valuePercent >= maxValue) {
            timer->stop();
            return;
        }

        valuePercent ++;
    } else {
        if (valuePercent <= minValue) {
            timer->stop();
            return;
        }

        valuePercent --;
    }

    update();
}

void RoundPlot::setRange(double minValue, double maxValue)
{
	//如果最小值大于或者等于最大值则不设置
	if (minValue >= maxValue) {
		return;
	}

	valuePercent = 100 * (this->value - this->minValue) / (this->maxValue - this->minValue);
	this->minValue = minValue;
	this->maxValue = maxValue;

	//如果目标值不在范围值内,则重新设置目标值
	if (value < minValue || value > maxValue) {
		setValue(value);
	}
}

void RoundPlot::setRange(int minValue, int maxValue)
{
	setRange((double)minValue, (double)maxValue);
}

void RoundPlot::setValue(double value)
{
	if (value < minValue) {
		this->value = minValue;
	} else if (value > maxValue) {
		this->value = maxValue;
	} else {
		this->value = value;
	}

	valuePercent = 100 * (this->value - this->minValue) / (this->maxValue - this->minValue);
    updateValue();
}

void RoundPlot::setValue(int value)
{
    setValue((double)value);
}

/* 设置中心数值*/
void RoundPlot::setCenterText(QString str)
{
    centerStr = str;
	update();
}

void RoundPlot::setAngle(double angle)
{
	this->angle = angle;
	update();
}

void RoundPlot::setPrecision(int precision)
{
	//精确度最多为 3
	if (precision <= 3) {
		this->precision = precision;
		update();
	}
}

void RoundPlot::setUnit(QString unit)
{
	this->unit = unit;
	update();
}

void RoundPlot::setUsedColor(QColor usedColor)
{
	this->usedColor = usedColor;
	update();
}

void RoundPlot::setFreeColor(QColor freeColor)
{
	this->freeColor = freeColor;
	update();
}

void RoundPlot::setRangeTextColor(QColor rangeTextColor)
{
	this->rangeTextColor = rangeTextColor;
	update();
}

void RoundPlot::setValueTextColor(QColor valueTextColor)
{
	this->valueTextColor = valueTextColor;
	update();
}

void RoundPlot::setValueBgColor(QColor valueBgColor)
{
	this->valueBgColor = valueBgColor;
	update();
}

void RoundPlot::setOutBgColor(QColor outBgColor)
{
	this->outBgColor = outBgColor;
	update();
}

void RoundPlot::setCenterBgColor(QColor centerBgColorStart, QColor centerBgColorEnd)
{
	this->centerBgColorStart = centerBgColorStart;
	this->centerBgColorEnd = centerBgColorEnd;
    update();
}

/* 设置标志位  快速加、快速减*/
void RoundPlot::setChangeFlag(int flag)
{
    changeDirection = flag;
}



/**************************************************
 *******************体检加载组件*********************
 *************************************************/

RoundProcess::RoundProcess(QWidget *parent) : QWidget(parent)
{

    initLocalParams();

    updateProcess(0);
}

RoundProcess::~RoundProcess()
{
}

void RoundProcess::paintEvent(QPaintEvent *)
{
    int width = this->width();
    int height = this->height();

    //绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width / 2, height / 2);

    int side = qMin(width, height);
    painter.scale(side / 200.0, side / 200.0);

    //绘制已使用百分比及未使用百分比进度
    drawDial(&painter);

    //绘制最里框文字圆形背景
    drawBgCenter(&painter);

    //绘制中间值文字
    drawText(&painter);
}

void RoundProcess::drawDial(QPainter *painter)
{
    int radius = 80;
    double lineWidth = 2.5;
    painter->save();
    painter->rotate(angle);

    //根据起始旋转的角度计算每次坐标需要旋转的角度,按照100等分计算
    //每次旋转的角度=360-(起始角度*2--分左右)/100
    double rotate = (double)(360 - (angle * 2)) / 100;


    //绘制已使用百分比
    if (valuePercent > 0) {
        painter->setPen(QPen(usedColor, lineWidth));
    }

    for (double i = 0; i < valuePercent; i++) {
        painter->drawLine(0, radius, 0, radius / 1.2);
        painter->rotate(rotate);
    }
    //绘制未使用百分比
    painter->setPen(QPen(freeColor, lineWidth));

    for (double i = valuePercent; i < 101; i++) {
        painter->drawLine(0, radius, 0, radius / 1.2);
        painter->rotate(rotate);
    }

    painter->restore();
}

void RoundProcess::drawBgCenter(QPainter *painter)
{
    int radius = 40;
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(valueBgColor);
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter->restore();
}

void RoundProcess::drawText(QPainter *painter)
{
    painter->save();
    // 绘制中心数值
    QFont ft;
    ft.setPixelSize(20);
    painter->setFont(ft);
    painter->setPen(QPen(valueTextColor));
    QFontMetricsF fm1 = QFontMetricsF(painter->font());
    QSizeF size1 = fm1.size(Qt::TextSingleLine, centerStr);
    painter->drawText(-size1.width() / 2, size1.height() / 3, centerStr);

    painter->restore();
}

/**
 * @brief RoundProcess::initLocalParams
 */
void RoundProcess::initLocalParams()
{
    angle = 0;
    centerStr = "0%";
    valuePercent = 0;

    usedColor = QColor(0, 116, 87);
    freeColor = QColor(169, 169, 169);
    valueBgColor = QColor(0, 116, 87);
    valueTextColor = QColor(250, 250, 250);
}

void RoundProcess::updateProcess(int nPercent)
{
    valuePercent = nPercent+1;
    centerStr = QString("%1").arg(nPercent);
    update();
}
