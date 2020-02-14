#include "threadmanager.h"

ThreadManager *ThreadManager::instance = nullptr;

ThreadManager::ThreadManager() {

}

ThreadManager *ThreadManager::getInstance() {
    if (nullptr == instance) {
        instance = new ThreadManager();
    }

    return instance;
}

LoggerThread *ThreadManager::add(LogStream *stream)
{
    LoggerThread *thread = new StreamThread(stream);

    _threads.append(thread);

    return thread;
}

LoggerThread *ThreadManager::get(const QString &name)
{
    for (LoggerThread *loggerThread : _threads)
    {
        if (loggerThread->getName() == name)
        {
            return loggerThread;
        }
    }

    return nullptr;
}

void ThreadManager::remove(LoggerThread *thread)
{
    _threads.removeOne(thread);
}
