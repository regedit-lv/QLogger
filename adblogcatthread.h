#ifndef ADBLOGCATTHREAD_H
#define ADBLOGCATTHREAD_H

#include "loggerthread.h"

#include <QProcess>

class AdbLogcatThread : public LoggerThread
{
public:
    AdbLogcatThread();
    virtual ~AdbLogcatThread();

protected:
    QProcess *process;
    virtual void startLogging();
    virtual void endLogging();
    virtual QByteArray getLogBytes();
    virtual void parseLog(const QString &line, QString &type, QString &tag, QString &text);
};

#endif // ADBLOGCATTHREAD_H
