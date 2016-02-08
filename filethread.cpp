#include "filethread.h"

#include "adblogcat.h"
#include "debug.h"

FileThread::FileThread(const QString &fileName)
{
    file = NULL;
    this->fileName = fileName;
}

FileThread::~FileThread()
{
    delete file;
}


void FileThread::startLogging() {
    file = new QFile(fileName);
    file->open(QIODevice::ReadOnly);
}

void FileThread::endLogging() {
    if (NULL != file) {
        file->close();
        delete file;
        file = NULL;
    }
}

QByteArray FileThread::getLogBytes() {
    QByteArray newBytes;

    if (NULL != file) {
        newBytes = file->readAll();
    }

    return newBytes;
}

void FileThread::parseLog(const QString &line, QString &type, QString &tag, QString &text) {
    AdbLogcat::parseLog(line, type, tag, text);
    type = "file";
}
