#include "ejectorselfcheckwidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QIntValidator>

ejectorSelfCheckWidget::ejectorSelfCheckWidget(QWidget *parent)
    : QWidget(parent), m_currentEjector(1),m_outputOn(false)
{
    m_curEjcLb = new QLabel(myLan.cur_ejec);
    m_ejectorEdit = new MyLineEdit();
    m_ejectorEdit->setFixedWidth(260);
    m_ejectorEdit->setStyleSheet("background-color:rgba(220,220,220,100)");

    m_ejectorSlider = new QSlider(Qt::Horizontal);

    QString sty = QString("QRadioButton::indicator{width: 40px; height: 40px;}");
    m_highSpeed = new QRadioButton(myLan.hig_speed);
    m_highSpeed->setStyleSheet(sty);
    m_lowSpeed = new QRadioButton(myLan.low_speed);
    m_lowSpeed->setStyleSheet(sty);

    m_testEnabled = new MyCheckBox(myLan.sigle_ejc);
    m_testEnabled->setCheckBtnSize(50,50);

    m_startButton = new QPushButton(myLan.start);
    m_startButton->setFixedSize(BTN_WID,BTN_HEI);
    m_stopButton = new QPushButton(myLan.stop);
    m_stopButton->setFixedSize(BTN_WID,BTN_HEI);

    m_backButton = new QPushButton(myLan.back);
    m_backButton->setFixedSize(BTN_WID,BTN_HEI);


    m_timer = new QTimer(this);

    m_ejectorEdit->setValidator(new QIntValidator(1, EJ_NUM, m_ejectorEdit));
    m_ejectorEdit->setText(QStringLiteral("1"));
    m_ejectorEdit->setFixedSize(110, BTN_HEI);
    m_ejectorSlider->setRange(1, EJ_NUM);
    m_ejectorSlider->setValue(1);
    m_ejectorSlider->setFixedHeight(BTN_HEI);

    m_highSpeed->setChecked(true);
    m_stopButton->setEnabled(false);

    QHBoxLayout *numberLayout = new QHBoxLayout();
    numberLayout->addSpacing(10);
    numberLayout->addWidget(m_curEjcLb);
    numberLayout->addSpacing(6);
    numberLayout->addWidget(m_ejectorEdit);
    numberLayout->addStretch();

    QVBoxLayout *vlaydis = new QVBoxLayout();
    vlaydis->addLayout(numberLayout);
    vlaydis->addSpacing(40);
    vlaydis->addWidget(m_ejectorSlider);

    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLayout->addSpacing(10);
    speedLayout->addWidget(m_highSpeed);
    speedLayout->addSpacing(60);
    speedLayout->addWidget(m_lowSpeed);
    speedLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_stopButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->addLayout(vlaydis);
    layout->addSpacing(35);
    layout->addWidget(m_testEnabled, 0, Qt::AlignCenter);
    layout->addSpacing(25);
    layout->addLayout(speedLayout);
    layout->addStretch();
    layout->addLayout(buttonLayout);
    layout->addWidget(m_backButton);

    connect(m_startButton, &QPushButton::clicked, this, &ejectorSelfCheckWidget::onStartClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &ejectorSelfCheckWidget::onStopClicked);
    connect(m_backButton, &QPushButton::clicked, this, &ejectorSelfCheckWidget::onBackClicked);
    connect(m_ejectorEdit, &MyLineEdit::pressed, this, &ejectorSelfCheckWidget::onEjectorEditChanged);
    connect(m_ejectorSlider, &QSlider::valueChanged, this, &ejectorSelfCheckWidget::onEjectorSliderChanged);
    connect(m_timer, &QTimer::timeout, this, &ejectorSelfCheckWidget::onTimerTimeout);
}

void ejectorSelfCheckWidget::onStartClicked()
{
    m_currentEjector = m_ejectorSlider->value();
    m_outputOn = false;
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    onTimerTimeout();
    m_timer->start(interval());
}

void ejectorSelfCheckWidget::onStopClicked()
{
    m_timer->stop();
    if (m_outputOn) {
        sendSelfCheck(0);
        m_outputOn = false;
    }
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
}

void ejectorSelfCheckWidget::onBackClicked()
{
    emit backRequested();
}

void ejectorSelfCheckWidget::sendSelfCheck(int onOff)
{
    gFlow.setEjectorSelfCheck(m_currentEjector-1, onOff);
}

void ejectorSelfCheckWidget::onEjectorEditChanged(void)
{
    myInputPanel *inputPanel = new myInputPanel(intType, 1, EJ_NUM, 1, "", g_mainWidget);
    int nRet = inputPanel->exec();
    if(nRet == QDialog::Accepted) {
        int n_sample = inputPanel->getValue();
        setEjectorNumber(n_sample);
    }
}

void ejectorSelfCheckWidget::onEjectorSliderChanged(int value)
{
    setEjectorNumber(value);
}

void ejectorSelfCheckWidget::setEjectorNumber(int value)
{
    m_currentEjector = value;
    if (m_ejectorSlider->value() != value) {
        m_ejectorSlider->setValue(value);
    }
    if (m_ejectorEdit->text().toInt() != value) {
        m_ejectorEdit->setText(QString::number(value));
    }
}

void ejectorSelfCheckWidget::onTimerTimeout()
{
    sendSelfCheck(m_outputOn ? 0 : 1);
    m_outputOn = !m_outputOn;
    if (!m_testEnabled->isChecked() && !m_outputOn) {
        m_currentEjector = m_currentEjector >= EJ_NUM ? 1 : m_currentEjector + 1;
        setEjectorNumber(m_currentEjector);
    }
}

int ejectorSelfCheckWidget::interval() const
{
    return m_highSpeed->isChecked() ? 100 : 300;
}
