#include "adblogcatthread.h"

#include <QList>

#include "debug.h"
#include "adblogcat.h"
#include "settings.h"

AdbLogcatThread::AdbLogcatThread()
{
    process = NULL;
}

AdbLogcatThread::~AdbLogcatThread()
{
    if (NULL != process) {
        process->terminate();
        delete process;
    }
}

void AdbLogcatThread::startLogging() {
    QString cmd = "";
    if (NULL == process) {
        process = new QProcess();
    }

    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start("adb wait-for-device", QIODevice::ReadOnly);
    process->waitForFinished();

    cmd = "adb logcat -c";
    cmd += Settings::getInstance()->getAdbLogcatFlags();
    process->start(cmd, QIODevice::ReadOnly);
    process->waitForFinished();

    cmd = "adb logcat";
    cmd += Settings::getInstance()->getAdbLogcatFlags();
    process->start(cmd, QIODevice::ReadOnly);
    process->waitForStarted();
}

void AdbLogcatThread::endLogging() {
    process->terminate();
    process->waitForFinished();
}

QByteArray AdbLogcatThread::getLogBytes() {
    QByteArray newBytes;
    static int zeroCount = 0;

    do {
        process->waitForReadyRead();
        newBytes = process->readAll();

        if (newBytes.size() == 0 && process->state() == QProcess::NotRunning) {
            zeroCount++;
            if (zeroCount > 10) {
                zeroCount = 0;
            }
            // unable to read. Maybe phone is plugged off
            if (process->state() == QProcess::NotRunning) {
                // restart adb
                endLogging();
                startLogging();
            }
        }
    } while (false);

    return newBytes;
}

void AdbLogcatThread::parseLog(const QString &line, QString &type, QString &tag, QString &text) {
    AdbLogcat::parseLog(line, type, tag, text);
}
