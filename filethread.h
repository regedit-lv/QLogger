#ifndef FILETHREAD_H
#define FILETHREAD_H

#include <QFile>

#include "loggerthread.h"

class FileThread : public LoggerThread
{
public:
    FileThread(const QString &fileName);
    virtual ~FileThread();

protected:
    QFile *file;
    QString fileName;
    virtual void startLogging();
    virtual void endLogging();
    virtual QByteArray getLogBytes();
    virtual void parseLog(const QString &line, QString &type, QString &tag, QString &text);
};

#endif // FILETHREAD_H
