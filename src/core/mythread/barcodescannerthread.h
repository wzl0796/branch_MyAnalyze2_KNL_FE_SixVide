#ifndef BARCODESCANNERTHREAD_H
#define BARCODESCANNERTHREAD_H

#include <QThread>
#include <QString>
#include <QHash>
#include <QMutex>

/*!
 * \brief RK356X/Linux 扫码枪后台线程
 *
 * 周期性扫描 /dev/input 下的 HID 键盘型扫码枪，独占打开后读取键值，
 * 以回车结束一帧条码，通过信号与 EVENT_BARCODE_SCANNED 向外通知。
 * Windows 下为空实现，保证交叉编译通过。
 */
class BarcodeScannerThread : public QThread
{
    Q_OBJECT
public:
    explicit BarcodeScannerThread(QObject *parent = nullptr);
    ~BarcodeScannerThread() override;

    void stop();

signals:
    void sigBarcodeScanned(const QString &code);
    void sigDeviceChanged(const QString &devicePath, bool plugged);

protected:
    void run() override;

private:
#ifdef Q_OS_UNIX
    struct DeviceContext {
        int fd = -1;
        QString path;
        QString name;
        QString buffer;
        bool shift = false;
    };

    void refreshDevices();
    void closeDevice(const QString &path);
    void closeAllDevices();
    bool openDevice(const QString &path, const QString &name);
    void processInput(DeviceContext *ctx);
    static bool isScannerName(const QString &name);
    static bool isExcludedName(const QString &name);
    static QChar keyCodeToChar(int code, bool shift);

    bool m_running = true;
    qint64 m_lastScanMs = 0;
    QHash<QString, DeviceContext*> m_devices; // path -> ctx
    QMutex m_mutex;
#endif
};
extern BarcodeScannerThread *g_barcodeScannerThread;
#endif // BARCODESCANNERTHREAD_H
