#ifndef MYQRCODE_H
#define MYQRCODE_H

#include <QObject>
#include <QImage>
#include <QPixmap>


class MyQRCode : public QObject
{
    Q_OBJECT

public:
    MyQRCode(QObject *parent = 0);
    ~MyQRCode();
    QPixmap encode(const QString &data);
    QString decode(const QImage &data);
    QPixmap encode(QByteArray input);
private:
};
#endif // MYQRCODE_H
