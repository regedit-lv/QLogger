#include "loggerthread.h"

#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

LoggerThread::LoggerThread(LoggerThreadType type)
{
    status = RUNNING;
    _type = type;
}

LoggerThreadType LoggerThread::getType()
{
    return _type;
}

void LoggerThread::run() {
    QList<QString> types;
    QList<QString> tags;
    QList<QString> texts;
    QByteArray data;


    startLogging();

    while (true) {
        types.clear();
        tags.clear();
        texts.clear();
        QByteArray newBytes = getLogBytes();

        if (newBytes.size() == 0) {
            break;
        }

        data.append(newBytes);
        //DBG_OUT << "data: " << QString(data);

        // split by lines
        while (true) {
            int endIndex = data.indexOf('\n');
            if (-1 == endIndex) {
//                DBG_OUT << "-- Data left " << data.size() << " --";
//                DBG_OUT << QString(data);
                break;
            }

            data[endIndex] = 0;
            if (endIndex > 0 && '\r' == data[endIndex - 1]) {
                data[endIndex - 1] = 0;
            }

            QString line = QString(data.data());
            data.remove(0, endIndex + 1);

            QString type, tag, text;

            DBG_OUT << line;

            parseLog(line, type, tag, text);
            types.append(type);
            tags.append(tag);
            texts.append(text);
        }

        if (STOPPING == status) {
            break;
        }
        if (RUNNING == status) {
            emit newLogItems(types, tags, texts);
        }
    }

    endLogging();
    status = STOPPED;
    emit finished(this);
    deleteLater();
}

void LoggerThread::resume() {
    if (isRunning()) {
        status = RUNNING;
    } else {
        start();
    }
}

void LoggerThread::pause() {
    status = PAUSED;
}

void LoggerThread::stop() {
    status = STOPPING;
}

LoggerThread::Status LoggerThread::getStatus() {
    return status;
}
