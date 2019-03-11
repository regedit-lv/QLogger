#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QList>
#include <QString>
#include "loggerthread.h"
#include "comportstream.h"

class Settings
{
public:
    enum LogThread {
        LogThreadAdbLogcat,
        LogThreadTrustzone,
        LogThreadKernel,
        LogThreadSize
    };

    static Settings *getInstance();

    void loadSettings();
    void saveSettings();


    int maxLinesOfLogs;
    int lineNumberWidth;
    bool logThreadStatus[LogThreadSize];
    QString highlightingFileName;

    // adb logcat
    bool flagVTime;
    bool flagBMain;
    bool flagBSystem;
    bool flagBRadio;
    bool flagBEvents;
    bool flagBCrash;

    QString getAdbLogcatFlags();

    // Com port streams
    QList<ComPortSettings> comPorts;

    ComPortSettings *getComPortSettings(const QString name);
    bool deleteComPortSettings(const QString name);
private:
    Settings();
    void init();
    static const char *FileName;
    static Settings *instance;
};

#endif // SETTINGS_H
