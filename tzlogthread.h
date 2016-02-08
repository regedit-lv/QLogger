#ifndef TZLOGTHREAD_H
#define TZLOGTHREAD_H

#include <QProcess>

#include "loggerthread.h"

class TZLogThread : public LoggerThread
{
public:
    TZLogThread();
    virtual ~TZLogThread();
protected:
    QProcess *process;
    virtual void startLogging();
    virtual void endLogging();
    virtual QByteArray getLogBytes();
    virtual void parseLog(const QString &line, QString &type, QString &tag, QString &text);
};

#endif // TZLOGTHREAD_H
