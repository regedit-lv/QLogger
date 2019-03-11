#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QList>

#include "comportstream.h"
#include "streamthread.h"

class ThreadManager
{
public:

    static ThreadManager *getInstance();
    LoggerThread *Add(LogStream *stream);

protected:
    static ThreadManager *instance;
    ThreadManager();
    QList<LoggerThread*> threads;
};

#endif // THREADMANAGER_H
