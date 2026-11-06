#include "barcodescannerthread.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QSet>
#include <QDebug>

#ifdef Q_OS_UNIX
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
BarcodeScannerThread *g_barcodeScannerThread;

BarcodeScannerThread::BarcodeScannerThread(QObject *parent)
    : QThread(parent)
{
}

BarcodeScannerThread::~BarcodeScannerThread()
{
    stop();
    wait(3000);
#ifdef Q_OS_UNIX
    closeAllDevices();
#endif
}

void BarcodeScannerThread::stop()
{
#ifdef Q_OS_UNIX
    m_running = false;
#endif
    requestInterruption();
}

#ifndef Q_OS_UNIX

void BarcodeScannerThread::run()
{
    // Windows / 非 Linux 平台不启用扫码枪后台扫描
}

#else

bool BarcodeScannerThread::isExcludedName(const QString &name)
{
    const QString n = name.toLower();
    static const char *kExclude[] = {
        "touch", "goodix", "gt9", "ft5", "ili", "rockchip",
        "adc-keys", "gpio-keys", "power", "headset", "rk805",
        "rk817", "pwm-ir", "mouse", "consumer control", "system control"
    };
    for (const char *key : kExclude) {
        if (n.contains(QLatin1String(key)))
            return true;
    }
    return false;
}

bool BarcodeScannerThread::isScannerName(const QString &name)
{
    if (name.isEmpty() || isExcludedName(name))
        return false;

    const QString n = name.toLower();
    static const char *kInclude[] = {
        "barcode", "scanner", "scan gun", "scangun",
        "honeywell", "symbol", "zebra", "datalogic", "newland",
        "mindeo", "cipherlab", "youjie", "sunlux", "uratek",
        "usb barcode","scan keyboard"
    };
    for (const char *key : kInclude) {
        if (n.contains(QLatin1String(key)))
            return true;
    }
    return false;
}

QChar BarcodeScannerThread::keyCodeToChar(int code, bool shift)
{
    // 注意：Linux KEY_A~KEY_Z 键码不连续（KEY_B=48 > KEY_Z=44），不能用 code-KEY_A 换算
    auto letter = [shift](char lower) {
        return QLatin1Char(shift ? char(lower - 'a' + 'A') : lower);
    };

    switch (code) {
    case KEY_1: return shift ? QLatin1Char('!') : QLatin1Char('1');
    case KEY_2: return shift ? QLatin1Char('@') : QLatin1Char('2');
    case KEY_3: return shift ? QLatin1Char('#') : QLatin1Char('3');
    case KEY_4: return shift ? QLatin1Char('$') : QLatin1Char('4');
    case KEY_5: return shift ? QLatin1Char('%') : QLatin1Char('5');
    case KEY_6: return shift ? QLatin1Char('^') : QLatin1Char('6');
    case KEY_7: return shift ? QLatin1Char('&') : QLatin1Char('7');
    case KEY_8: return shift ? QLatin1Char('*') : QLatin1Char('8');
    case KEY_9: return shift ? QLatin1Char('(') : QLatin1Char('9');
    case KEY_0: return shift ? QLatin1Char(')') : QLatin1Char('0');

    case KEY_A: return letter('a');
    case KEY_B: return letter('b');
    case KEY_C: return letter('c');
    case KEY_D: return letter('d');
    case KEY_E: return letter('e');
    case KEY_F: return letter('f');
    case KEY_G: return letter('g');
    case KEY_H: return letter('h');
    case KEY_I: return letter('i');
    case KEY_J: return letter('j');
    case KEY_K: return letter('k');
    case KEY_L: return letter('l');
    case KEY_M: return letter('m');
    case KEY_N: return letter('n');
    case KEY_O: return letter('o');
    case KEY_P: return letter('p');
    case KEY_Q: return letter('q');
    case KEY_R: return letter('r');
    case KEY_S: return letter('s');
    case KEY_T: return letter('t');
    case KEY_U: return letter('u');
    case KEY_V: return letter('v');
    case KEY_W: return letter('w');
    case KEY_X: return letter('x');
    case KEY_Y: return letter('y');
    case KEY_Z: return letter('z');

    case KEY_MINUS:        return shift ? QLatin1Char('_') : QLatin1Char('-');
    case KEY_EQUAL:        return shift ? QLatin1Char('+') : QLatin1Char('=');
    case KEY_LEFTBRACE:    return shift ? QLatin1Char('{') : QLatin1Char('[');
    case KEY_RIGHTBRACE:   return shift ? QLatin1Char('}') : QLatin1Char(']');
    case KEY_SEMICOLON:    return shift ? QLatin1Char(':') : QLatin1Char(';');
    case KEY_APOSTROPHE:   return shift ? QLatin1Char('"') : QLatin1Char('\'');
    case KEY_GRAVE:        return shift ? QLatin1Char('~') : QLatin1Char('`');
    case KEY_BACKSLASH:    return shift ? QLatin1Char('|') : QLatin1Char('\\');
    case KEY_COMMA:        return shift ? QLatin1Char('<') : QLatin1Char(',');
    case KEY_DOT:          return shift ? QLatin1Char('>') : QLatin1Char('.');
    case KEY_SLASH:        return shift ? QLatin1Char('?') : QLatin1Char('/');
    case KEY_SPACE:        return QLatin1Char(' ');
    case KEY_KP0:          return QLatin1Char('0');
    case KEY_KP1:          return QLatin1Char('1');
    case KEY_KP2:          return QLatin1Char('2');
    case KEY_KP3:          return QLatin1Char('3');
    case KEY_KP4:          return QLatin1Char('4');
    case KEY_KP5:          return QLatin1Char('5');
    case KEY_KP6:          return QLatin1Char('6');
    case KEY_KP7:          return QLatin1Char('7');
    case KEY_KP8:          return QLatin1Char('8');
    case KEY_KP9:          return QLatin1Char('9');
    case KEY_KPDOT:        return QLatin1Char('.');
    case KEY_KPMINUS:      return QLatin1Char('-');
    case KEY_KPPLUS:       return QLatin1Char('+');
    case KEY_KPASTERISK:   return QLatin1Char('*');
    case KEY_KPSLASH:      return QLatin1Char('/');
    default:
        break;
    }
    return QChar();
}

bool BarcodeScannerThread::openDevice(const QString &path, const QString &name)
{
    if (m_devices.contains(path))
        return true;

    const QByteArray pathBytes = path.toLocal8Bit();
    int fd = ::open(pathBytes.constData(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        qWarning("BarcodeScanner: open %s failed: %s",
                 qPrintable(path), strerror(errno));
        return false;
    }

    // 独占抓取，避免条码键值灌进当前焦点控件
    if (ioctl(fd, EVIOCGRAB, 1) < 0) {
        qWarning("BarcodeScanner: grab %s failed: %s",
                 qPrintable(path), strerror(errno));
        ::close(fd);
        return false;
    }

    auto *ctx = new DeviceContext;
    ctx->fd = fd;
    ctx->path = path;
    ctx->name = name;
    m_devices.insert(path, ctx);

    qInfo("BarcodeScanner: opened %s (%s)", qPrintable(path), qPrintable(name));
    emit sigDeviceChanged(path, true);
    return true;
}

void BarcodeScannerThread::closeDevice(const QString &path)
{
    DeviceContext *ctx = m_devices.take(path);
    if (!ctx)
        return;
    if (ctx->fd >= 0) {
        ioctl(ctx->fd, EVIOCGRAB, 0);
        ::close(ctx->fd);
    }
    qInfo("BarcodeScanner: closed %s", qPrintable(path));
    emit sigDeviceChanged(path, false);
    delete ctx;
}

void BarcodeScannerThread::closeAllDevices()
{
    const QStringList keys = m_devices.keys();
    for (const QString &path : keys)
        closeDevice(path);
}

void BarcodeScannerThread::refreshDevices()
{
    QDir inputDir(QStringLiteral("/dev/input"));
    if (!inputDir.exists())
        return;

    QSet<QString> alive;
    const QFileInfoList events = inputDir.entryInfoList(
                QStringList() << QStringLiteral("event*"),
                QDir::System | QDir::Files | QDir::Readable,
                QDir::Name);

    for (const QFileInfo &fi : events) {
        const QString path = fi.absoluteFilePath();
        const QByteArray pathBytes = path.toLocal8Bit();
        int fd = ::open(pathBytes.constData(), O_RDONLY | O_NONBLOCK);
        if (fd < 0)
            continue;

        char nameBuf[256] = {0};
        if (ioctl(fd, EVIOCGNAME(sizeof(nameBuf)), nameBuf) < 0) {
            ::close(fd);
            continue;
        }
        ::close(fd);

        const QString name = QString::fromLocal8Bit(nameBuf);
        if (!isScannerName(name))
            continue;

        alive.insert(path);
        openDevice(path, name);
    }

    const QStringList opened = m_devices.keys();
    for (const QString &path : opened) {
        if (!alive.contains(path))
            closeDevice(path);
    }
}

void BarcodeScannerThread::processInput(DeviceContext *ctx)
{
    if (!ctx || ctx->fd < 0)
        return;

    input_event ev;
    while (true) {
        const ssize_t n = ::read(ctx->fd, &ev, sizeof(ev));
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            qWarning("BarcodeScanner: read %s error: %s",
                     qPrintable(ctx->path), strerror(errno));
            closeDevice(ctx->path);
            break;
        }
        if (n != sizeof(ev))
            break;

        if (ev.type != EV_KEY)
            continue;

        if (ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) {
            ctx->shift = (ev.value != 0);
            continue;
        }

        if (ev.value != 1)
            continue;

        if (ev.code == KEY_ENTER || ev.code == KEY_KPENTER) {
            const QString code = ctx->buffer.trimmed();
            ctx->buffer.clear();
            if (code.isEmpty())
                continue;

            emit sigBarcodeScanned(code);

            continue;
        }

        if (ev.code == KEY_BACKSPACE) {
            if (!ctx->buffer.isEmpty())
                ctx->buffer.chop(1);
            continue;
        }

        const QChar ch = keyCodeToChar(int(ev.code), ctx->shift);
        if (!ch.isNull()) {
            // 防止异常设备持续灌入导致内存膨胀
            if (ctx->buffer.size() < 512)
                ctx->buffer.append(ch);
        }
    }
}

void BarcodeScannerThread::run()
{
    m_running = true;
    m_lastScanMs = 0;
    while (m_running && !isInterruptionRequested()) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - m_lastScanMs >= 2000) {
            refreshDevices();
            m_lastScanMs = now;
        }

        if (m_devices.isEmpty()) {
            msleep(200);
            continue;
        }

        const QList<DeviceContext*> ctxList = m_devices.values();
        QVector<pollfd> fds;
        fds.reserve(ctxList.size());
        for (DeviceContext *ctx : ctxList) {
            pollfd pfd;
            pfd.fd = ctx->fd;
            pfd.events = POLLIN;
            pfd.revents = 0;
            fds.push_back(pfd);
        }

        const int ret = ::poll(fds.data(), nfds_t(fds.size()), 200);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            qWarning("BarcodeScanner: poll failed: %s", strerror(errno));
            msleep(200);
            continue;
        }
        if (ret == 0)
            continue;

        for (int i = 0; i < fds.size(); ++i) {
            if (!(fds[i].revents & (POLLIN | POLLERR | POLLHUP)))
                continue;
            const QString path = ctxList[i]->path;
            if (fds[i].revents & (POLLERR | POLLHUP)) {
                closeDevice(path);
                continue;
            }
            DeviceContext *ctx = m_devices.value(path, nullptr);
            if (ctx)
                processInput(ctx);
        }
    }

    closeAllDevices();
}

#endif // Q_OS_UNIX
