#include "kernellogthread.h"

KernelLogThread::KernelLogThread()
{
    process = NULL;
}

KernelLogThread::~KernelLogThread()
{
    if (NULL != process) {
        process->terminate();
        delete process;
    }
}

void KernelLogThread::startLogging() {
    if (NULL == process) {
        process = new QProcess();
    }

    process->setProcessChannelMode(QProcess::MergedChannels);

    // wait for devices
    process->start("adb wait-for-device", QIODevice::ReadOnly);
    process->waitForFinished();

    process->start("adb shell cat /proc/kmsg", QIODevice::ReadOnly);
    process->waitForStarted();
}

void KernelLogThread::endLogging() {
    process->terminate();
    process->waitForFinished();
}

QByteArray KernelLogThread::getLogBytes() {
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

void KernelLogThread::parseLog(const QString &line, QString &type, QString &tag, QString &text) {
    type = "kernel";
    text = "";

    // search for tag ':'
    int tagEnd = line.indexOf(':');
    int tagStart = line.indexOf(']');

    if (-1 != tagEnd && -1 != tagStart) {
        tagStart += 2;
        tag = line.mid(tagStart, tagEnd - tagStart);
    } else {
        tag = "";
    }
    text = line;
}

