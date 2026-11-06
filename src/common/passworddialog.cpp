#include "passworddialog.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDebug>

PasswordDialog::PasswordDialog(QWidget *parent)
    : QDialog(parent)
    , password("")
    , qrCodeGenerator(new MyQRCode(this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    // setStyleSheet("background-color:#F1F0F5;");

    setStyleSheet(QString("background-color:%1;").arg(g_ColorLightGray3));
    setGeometry(WIN_WID/3, WIN_HEI/4, 800, 500);

    setWindowTitle(myLan.please_input_password);
    setModal(true);
    
    setupUI();
    createNumberButtons();
}

PasswordDialog::~PasswordDialog()
{
}

QString PasswordDialog::getPassword() const
{
    return password;
}

void PasswordDialog::setQRCodeData(const QString &data)
{
    QJsonObject object;
    object["module"] = 1; //! 1: 高级权限  2： 厂家权限
    object["device"] = data;  //! 机编
    object["time"] = QDateTime::currentDateTime().toString("yyyyMMdd") + QString("000000");
    object["version"] = QString("v1.00");


    QJsonDocument jd;
    jd.setObject(object);

    //! 删去换行符和空格
    QByteArray tidyJson = jd.toJson().replace(0x0a,"").replace(0x20, "");

    //! Qt qCompress 比 zlib compress 压缩的数据前面多4个字节(未压缩前的数据长度)
    QByteArray qtCompressInfo = qCompress(tidyJson, 9);
    QByteArray compressInfo = qtCompressInfo.mid(4);
    QByteArray result = compressInfo.toBase64();
    result.insert(21, 't');
    result.insert(10, 'o');
    result.insert(4, 'i');

    if(!result.isEmpty()){
        QPixmap qrPixmap = qrCodeGenerator->encode(result);
        if (!qrPixmap.isNull()) {
            qrCodeLabel->setPixmap(qrPixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::FastTransformation));
        }
    }

}

void PasswordDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        int digit = event->key() - Qt::Key_0;
        onDigitPressed(digit);
    } else if (event->key() == Qt::Key_Backspace) {
        onBackspaceButtonClicked();
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        onOkButtonClicked();
    } else if (event->key() == Qt::Key_Escape) {
        onCancelButtonClicked();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void PasswordDialog::onNumberButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int digit = button->text().toInt();
        onDigitPressed(digit);
    }
}

void PasswordDialog::onDigitPressed(int digit)
{
    if (password.length() < MAX_PASSWORD_LENGTH) {
        password += QString::number(digit);
        updatePasswordDisplay();
    }
}

void PasswordDialog::onClearButtonClicked()
{
    password.clear();
    updatePasswordDisplay();
}

void PasswordDialog::onBackspaceButtonClicked()
{
    if (!password.isEmpty()) {
        password.chop(1);
        updatePasswordDisplay();
    }
}

void PasswordDialog::onOkButtonClicked()
{
    if (!password.isEmpty()) {
        accept();
    }
}

void PasswordDialog::onCancelButtonClicked()
{
    reject();
}

void PasswordDialog::setupUI()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0); // 左右无间距

    QHBoxLayout *hLayouttitle = new QHBoxLayout();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    
    iconLabel = new QLabel(myLan.please_input_password);
    iconLabel->setFixedSize(800, 60);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(QString("color:white; background-color:%1;").arg(g_ColorMYGreen));
    iconLabel->setScaledContents(true);


    //! 左侧二维码区域
    QWidget *qrCodePanel = new QWidget();
    QVBoxLayout *qrCodeLayout = new QVBoxLayout(qrCodePanel);
    
    qrCodeLabel = new QLabel();
    qrCodeLabel->setAlignment(Qt::AlignCenter);
    qrCodeLabel->setMinimumSize(300, 300);
    qrCodeLabel->setStyleSheet("border: 1px solid #ccc; background-color: white;");
    
    qrCodeLayout->addStretch();
    qrCodeLayout->addWidget(qrCodeLabel);
    qrCodeLayout->addStretch();
    
    // 右侧密码输入区域
    QWidget *passwordPanel = new QWidget();
    QVBoxLayout *passwordLayout = new QVBoxLayout(passwordPanel);
    
    // 密码显示框
    passwordDisplay = new QLineEdit();
    passwordDisplay->setAlignment(Qt::AlignCenter);
    passwordDisplay->setReadOnly(true);
    passwordDisplay->setEchoMode(QLineEdit::Password);
    passwordDisplay->setMinimumHeight(40);
    passwordDisplay->setStyleSheet("font-size: 18px; border: 1px solid #ccc; padding: 5px;background-color: white;");
    
    // 数字按钮面板
    numberPanel = new QWidget();
    numberButtonLayout = new QGridLayout(numberPanel);
    
    // 控制按钮
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    clearButton = new QPushButton(myLan.clear);
    clearButton->setMinimumHeight(40);
    clearButton->setStyleSheet("background-color: white;");
    connect(clearButton, &QPushButton::clicked, this, &PasswordDialog::onClearButtonClicked);
    
    backspaceButton = new QPushButton(myLan.back);
    backspaceButton->setMinimumHeight(40);
    backspaceButton->setStyleSheet("background-color: white;");
    backspaceButton->hide();
    connect(backspaceButton, &QPushButton::clicked, this, &PasswordDialog::onBackspaceButtonClicked);
    
    okButton = new QPushButton(myLan.ok);
    okButton->setMinimumHeight(40);
    okButton->setStyleSheet("background-color: white;");
    connect(okButton, &QPushButton::clicked, this, &PasswordDialog::onOkButtonClicked);
    
    cancelButton = new QPushButton(myLan.cancel);
    cancelButton->setMinimumHeight(40);
    cancelButton->setStyleSheet("background-color: white;");
    connect(cancelButton, &QPushButton::clicked, this, &PasswordDialog::onCancelButtonClicked);
    
    controlLayout->addWidget(clearButton);
    controlLayout->addWidget(backspaceButton);
    controlLayout->addWidget(okButton);
    controlLayout->addWidget(cancelButton);
    
    passwordLayout->addWidget(passwordDisplay);
    passwordLayout->addWidget(numberPanel);
    passwordLayout->addLayout(controlLayout); 
    
    mainLayout->addWidget(qrCodePanel, 1);
    mainLayout->addWidget(passwordPanel, 2);

    hLayouttitle->addWidget(iconLabel);
    hLayouttitle->setContentsMargins(0, 0, 0, 0); // 左右无间距
    hLayouttitle->setSpacing(0);

    vLayout->addLayout(hLayouttitle);
    vLayout->addLayout(mainLayout);
    
    // 设置默认按钮
    okButton->setDefault(true);
}

void PasswordDialog::createNumberButtons()
{
    // 创建数字按钮0-9
    for (int i = 1; i <= 9; i++) {
        QPushButton *button = new QPushButton(QString::number(i));
        button->setMinimumSize(60, 60);
        button->setStyleSheet("font-size: 20px; font-weight: bold; background-color: white;");
        connect(button, &QPushButton::clicked, this, &PasswordDialog::onNumberButtonClicked);
        
        int row = (i - 1) / 3;
        int col = (i - 1) % 3;
        numberButtonLayout->addWidget(button, row, col);
    }
    
    // 按钮0
    QPushButton *button0 = new QPushButton("0");
    button0->setMinimumSize(60, 60);
    button0->setStyleSheet("font-size: 20px; font-weight: bold;background-color: white;");
    connect(button0, &QPushButton::clicked, this, &PasswordDialog::onNumberButtonClicked);
    numberButtonLayout->addWidget(button0, 3, 1);
}

void PasswordDialog::updatePasswordDisplay()
{
    passwordDisplay->setText(QString(password.length(), QChar('*')));
}
