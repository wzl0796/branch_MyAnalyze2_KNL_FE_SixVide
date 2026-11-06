#include "filecopythread.h"

FileCopyThread::FileCopyThread()
{
}

FileCopyThread::~FileCopyThread()
{
}

void FileCopyThread::setAddr(QString addr,QString file)
{
    srcAddr = addr;
    fileName = file;
}

void FileCopyThread::setDestAddr(QString addr, QString file)
{
    destAddr = addr;
    destFileName = file;
}

void FileCopyThread::run(void)
{
    QString strInfo;
    QProcess cmdCopy;
    cmdCopy.setReadChannelMode(QProcess::MergedChannels);
    cmdCopy.start(QString("cp -f %1%2 %3%4").arg(srcAddr).arg(fileName).arg(destAddr).arg(destFileName));
    if(cmdCopy.waitForStarted()){
        while(!cmdCopy.waitForFinished()){
            strInfo = cmdCopy.readAll();
        }
    } else {
        cmdCopy.kill();
        return;
    }
    cmdCopy.kill();
}
