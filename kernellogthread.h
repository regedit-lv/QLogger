#ifndef KERNELLOGTHREAD_H
#define KERNELLOGTHREAD_H

#include "loggerthread.h"

#include <QProcess>

class KernelLogThread : public LoggerThread
{
public:
    KernelLogThread();
    virtual ~KernelLogThread();
protected:
    QProcess *process;
    virtual void startLogging();
    virtual void endLogging();
    virtual QByteArray getLogBytes();
    virtual void parseLog(const QString &line, QString &type, QString &tag, QString &text);
};

#endif // KERNELLOGTHREAD_H
