#include "myqrcode.h"
#include "src/3rdparty/libqrencode/qrencode.h"

MyQRCode::MyQRCode(QObject *parent)
    : QObject(parent)
{

}

MyQRCode::~MyQRCode()
{

}

QPixmap MyQRCode::encode(const QString &data)
{
    if (data.isEmpty())
        return QPixmap();

    QRcode *qrcode = QRcode_encodeString(data.toStdString().c_str(), 1, QR_ECLEVEL_Q, QR_MODE_8, 0);
    if (qrcode != NULL) {
        unsigned char *p = qrcode->data;
        int width = qrcode->width > 0 ? qrcode->width : 1;
        QImage qrImage(QSize(width, width), QImage::Format_RGB32);

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                qrImage.setPixel(j, i, (*p&0x01) ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
                p++;
            }
        }

        QRcode_free(qrcode);

        return QPixmap::fromImage(qrImage);
    }

    return QPixmap();
}

QString MyQRCode::decode(const QImage &data)
{
    return QString();
}

QPixmap MyQRCode::encode(QByteArray input)
{
    if (input.isEmpty())
        return QPixmap();

    QRcode *qrcode = QRcode_encodeData(input.size(), reinterpret_cast<const uchar *>(input.constData()), 1, QR_ECLEVEL_L);
    if (qrcode != NULL) {
        unsigned char *p = qrcode->data;
        int width = qrcode->width > 0 ? qrcode->width : 1;
        QImage qrImage(QSize(width, width), QImage::Format_RGB32);

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                qrImage.setPixel(j, i, (*p&0x01) ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
                p++;
            }
        }

        QRcode_free(qrcode);

        return QPixmap::fromImage(qrImage);
    }

    return QPixmap();
}
