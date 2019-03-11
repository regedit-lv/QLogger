#pragma once

#include <logstream.h>
#include <QFile>

struct FileSettings
{
    LogStreamSettings streamSettings;
    QString name;
};

class FileStream : public LogStream
{
public:
    FileStream(const FileSettings &comPortSettings);
    virtual ~FileStream();

    virtual void start();
    virtual void end();
    virtual QByteArray getBytes();

protected:
    FileSettings _fileSettings;
    QFile _file;
};


