#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <QString>
#include <QByteArray>

struct LogStreamSettings
{
    QString name;
    QString tagScript;
};

class LogStream
{
public:
    LogStream(const LogStreamSettings &settings);

    virtual ~LogStream();

    virtual void start() = 0;
    virtual void end() = 0;
    virtual QByteArray getBytes() = 0;


    LogStreamSettings settings;
};

#endif // LOGSTREAM_H
