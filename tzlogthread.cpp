#include "tzlogthread.h"

TZLogThread::TZLogThread()
{
    process = NULL;
}

TZLogThread::~TZLogThread()
{
    if (NULL != process) {
        process->terminate();
        delete process;
    }
}

void TZLogThread::startLogging() {
    if (NULL == process) {
        process = new QProcess();
    }

    process->setProcessChannelMode(QProcess::MergedChannels);

    // wait for devices
    process->start("adb wait-for-device", QIODevice::ReadOnly);
    process->waitForFinished();

    process->start("adb shell cat d/tzdbg/qsee_log", QIODevice::ReadOnly);
    process->waitForStarted();
}

void TZLogThread::endLogging() {
    process->terminate();
    process->waitForFinished();
}

QByteArray TZLogThread::getLogBytes() {
    QByteArray newBytes;

    do {
        process->waitForReadyRead();
        newBytes = process->readAll();

        if (newBytes.size() == 0 && process->state() == QProcess::NotRunning) {
            // unable to read. Maybe phone is plugged off
            // restart adb
            endLogging();
            startLogging();
        }
    } while (false);

    return newBytes;
}

void TZLogThread::parseLog(const QString &line, QString &type, QString &tag, QString &text) {
    type = "trustzone";
    text = "";

    // search for tag ':'
    int tagEnd = line.indexOf(':');

    if (-1 != tagEnd) {
        tag = line.mid(0, tagEnd);
    } else {
        tag = "";
    }
    text = line;
}
