#include "streamthread.h"

StreamThread::StreamThread(LogStream *stream) : LoggerThread(LoggerThreadType::Stream)
{
    _stream = stream;
}

StreamThread::~StreamThread()
{
    delete _stream;
}

LogStream *StreamThread::getStream()
{
    return _stream;
}

void StreamThread::startLogging()
{
    _stream->start();
}

void StreamThread::endLogging()
{
    _stream->end();
}

QByteArray StreamThread::getLogBytes()
{
    return _stream->getBytes();
}

void StreamThread::parseLog(const QString &line, QString &type, QString &tag, QString &text)
{
    type = _stream->settings.name;

    QScriptValue fun = _scriptEngine.evaluate(_stream->settings.tagScript);
    QScriptValueList args;
    args << line;
    QScriptValue result = fun.call(QScriptValue(), args);

    tag = result.toString();
    text = line;
}
