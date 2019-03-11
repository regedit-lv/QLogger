#ifndef STREAMTHREAD_H
#define STREAMTHREAD_H

#include "logstream.h"
#include "loggerthread.h"

#include <QtScript/QScriptEngine>

class StreamThread : public LoggerThread
{
public:
    StreamThread(LogStream *stream);
    virtual ~StreamThread();
    LogStream *getStream();
protected:
    QScriptEngine _scriptEngine;
    LogStream *_stream;
    virtual void startLogging();
    virtual void endLogging();
    virtual QByteArray getLogBytes();
    virtual void parseLog(const QString &line, QString &type, QString &tag, QString &text);

};

#endif // STREAMTHREAD_H
