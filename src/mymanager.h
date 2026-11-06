#ifndef MYMANAGER_H
#define MYMANAGER_H

#include <QObject>
#include <QThread>
#include "core/io/udp/myudp.h"
#include "core/globalflow/globalflow.h"
#include "core/mythread/myprocess.h"
#include "core/mythread/netcheckthread.h"
#include "common/mycurve.h"
#include "core/io/myserial/myserialcom.h"
#include "core/io/localsocket/mylocalsocket.h"
#include "core/io/iot/localsocketthread.h"
#include "core/mythread/barcodescannerthread.h"

class myManager : public QObject
{
    Q_OBJECT
public:
    explicit myManager(QObject *parent = 0);
    ~myManager();

    void initGlobalObject(void);

    void initSerialCom(void);

signals:


public slots:

};

#endif // MYMANAGER_H
