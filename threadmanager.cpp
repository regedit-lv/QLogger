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

LoggerThread *ThreadManager::Add(LogStream *stream)
{
    LoggerThread *thread = new StreamThread(stream);

    threads.append(thread);

    return thread;
}
