#ifndef FILECOPYTHREAD_H
#define FILECOPYTHREAD_H

#include <QThread>
#include <QProcess>

class FileCopyThread : public QThread
{
    Q_OBJECT

public:
   FileCopyThread();
   ~FileCopyThread();

   void setAddr(QString addr, QString file);
   void setDestAddr(QString addr, QString file);

signals:
    void fileCopyFinishSignal(int);

public:
    void run(void);

private:

public:
    QString fileName, destFileName;
    QString srcAddr, destAddr;
};

#endif // FILECOPYTHREAD_H
