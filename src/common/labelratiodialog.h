#ifndef LABELRATIODIALOG_H
#define LABELRATIODIALOG_H

#include <QDialog>
#include <QMap>
#include <QPushButton>
#include <QVector>

class LabelRatioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabelRatioDialog(const QStringList &labels, const QMap<QString, double> &ratios,
                              QWidget *parent = nullptr);
    QMap<QString, double> ratios() const;

private slots:
    void onRatioButtonClicked();
    void onConfirmClicked();

private:
    QMap<QString, double> m_ratios;
    QVector<QPushButton *> m_ratioButtons;
    QPushButton *m_confirmButton;
};

#endif // LABELRATIODIALOG_H
