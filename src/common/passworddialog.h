#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include "src/common/myqrcode.h"
#include "core/globalflow/mylanguage.h"
#include "src/params/define.h"

/*!
 * \brief 密码输入对话框类
 */
class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent = nullptr);
    ~PasswordDialog();

    QString getPassword() const;
    void    setQRCodeData(const QString &data);
    void    clearPassWord(){password.clear();passwordDisplay->clear();}

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onNumberButtonClicked();
    void onClearButtonClicked();
    void onBackspaceButtonClicked();
    void onOkButtonClicked();
    void onCancelButtonClicked();

private:
    void setupUI();
    void createNumberButtons();
    void updatePasswordDisplay();
    void onDigitPressed(int digit);
    
    QLabel *iconLabel;
    QLabel *qrCodeLabel;
    QLineEdit *passwordDisplay;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *clearButton;
    QPushButton *backspaceButton;
    
    QGridLayout *numberButtonLayout;
    QWidget *numberPanel;
    
    QString password;
    MyQRCode *qrCodeGenerator;
    
    static const int MAX_PASSWORD_LENGTH = 6;
};

#endif // PASSWORDDIALOG_H
