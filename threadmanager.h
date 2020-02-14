#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QList>

#include "comportstream.h"
#include "streamthread.h"

class ThreadManager
{
public:

    static ThreadManager *getInstance();
    LoggerThread *add(LogStream *stream);
    LoggerThread *get(const QString &name);
    void remove(LoggerThread *thread);
protected:
    static ThreadManager *instance;
    ThreadManager();
    QList<LoggerThread*> _threads;
};

#endif // THREADMANAGER_H
