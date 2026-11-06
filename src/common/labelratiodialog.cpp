#include "labelratiodialog.h"
#include "common/myinputpanel.h"
#include "common/mymessagebox.h"
#include "params/globalparams.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtMath>

LabelRatioDialog::LabelRatioDialog(const QStringList &labels, const QMap<QString, double> &ratios,
                                   QWidget *parent)
    : QDialog(parent), m_ratios(ratios), m_confirmButton(nullptr)
{
    setWindowTitle(QString::fromUtf8("AI标签定标"));
    setMinimumWidth(520);

    auto *layout = new QVBoxLayout(this);
    auto *grid = new QGridLayout;
    grid->addWidget(new QLabel(QString::fromUtf8("AI标签")), 0, 0);
    grid->addWidget(new QLabel(QString::fromUtf8("占比(%)")), 0, 1);

    for (const QString &label : labels) {
        const double value = m_ratios.value(label, 0.0);
        m_ratios[label] = value;

        auto *labelWidget = new QLabel(label);
        auto *ratioButton = new QPushButton(QString::number(value, 'f', 2));
        ratioButton->setProperty("labelName", label);
        ratioButton->setMinimumHeight(48);
        grid->addWidget(labelWidget, m_ratioButtons.size() + 1, 0);
        grid->addWidget(ratioButton, m_ratioButtons.size() + 1, 1);
        m_ratioButtons.append(ratioButton);
        connect(ratioButton, &QPushButton::clicked, this, &LabelRatioDialog::onRatioButtonClicked);
    }

    layout->addLayout(grid);
    auto *buttonLayout = new QHBoxLayout;
    m_confirmButton = new QPushButton(QString::fromUtf8("确认"));
    auto *cancelButton = new QPushButton(QString::fromUtf8("取消"));
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_confirmButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(m_confirmButton, &QPushButton::clicked, this, &LabelRatioDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QMap<QString, double> LabelRatioDialog::ratios() const
{
    return m_ratios;
}

void LabelRatioDialog::onRatioButtonClicked()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    if (!button) {
        return;
    }

    const QString label = button->property("labelName").toString();
    auto *inputPanel = new myInputPanel(floatType, 0, 100, m_ratios.value(label),
                                        QString::fromUtf8("占比(%)"), this);
    if (inputPanel->exec() == QDialog::Accepted) {
        const double value = inputPanel->getValue();
        m_ratios[label] = value;
        button->setText(QString::number(value, 'f', 2));
    }
    delete inputPanel;
}

void LabelRatioDialog::onConfirmClicked()
{
    double total = 0.0;
    for (const QPushButton *button : m_ratioButtons) {
        total += m_ratios.value(button->property("labelName").toString());
    }

    if (qAbs(total - 100.0) > 0.001) {
        myMessageBox messageBox(MSG_WARN,
                                QString::fromUtf8("所有AI标签占比之和必须等于100%"),
                                QString::fromUtf8("提示"), this);
        messageBox.exec();
        return;
    }

    accept();
}
