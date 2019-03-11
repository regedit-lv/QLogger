#include "filestream.h"

#include <QMessageBox>

FileStream::FileStream(const FileSettings &fileSettings)
    : LogStream(fileSettings.streamSettings)
{
    _fileSettings = fileSettings;
}

FileStream::~FileStream()
{
    _file.close();
}

void FileStream::start()
{
    _file.setFileName(_fileSettings.name);
    if(!_file.open(QIODevice::ReadWrite) )
    {
    }
}

void FileStream::end()
{
    if(_file.isOpen())
    {
        _file.close();
    }
}

QByteArray FileStream::getBytes()
{
    if (_file.isOpen())
    {
        QByteArray bytes = _file.readAll();
        _file.close();
        return bytes;
    }
    else
    {
        return QByteArray();
    }
}

