#include "settings.h"
#include "debug.h"
#include "texthighlighter.h"
#include "logstorage.h"

Settings *Settings::instance = NULL;
const char *Settings::FileName = "settings.ini";

Settings::Settings() {

}

void Settings::init() {
    loadSettings();
    saveSettings();
}

Settings *Settings::getInstance() {
    if (NULL == instance) {
        instance = new Settings();
        instance->init();
    }

    return instance;
}

void Settings::loadSettings() {
    QSettings settings(FileName, QSettings::IniFormat);

    settings.beginGroup("Logging");
    maxLinesOfLogs = settings.value("maxLinesOfLogs", 100000).toInt();
    lineNumberWidth = QString::number(maxLinesOfLogs).length();
    highlightingFileName = settings.value("highlightingFileName", "default_hl.ini").toString();

    logThreadStatus[LogThreadAdbLogcat] = settings.value("threadAdbLogcatStatus", true).toBool();
    logThreadStatus[LogThreadTrustzone] = settings.value("threadTrustzoneStatus", true).toBool();
    logThreadStatus[LogThreadKernel] = settings.value("threadKernelLogStatus", true).toBool();

    // adb logcat
    flagVTime = settings.value("flagVTime", false).toBool();
    flagBMain = settings.value("flagBMain", true).toBool();
    flagBSystem = settings.value("flagBSystem", true).toBool();
    flagBRadio = settings.value("flagBRadio", false).toBool();
    flagBEvents = settings.value("flagBEvents", false).toBool();
    flagBCrash = settings.value("flagBCrash", true).toBool();

    settings.endGroup();

    TextHighlighter::instance()->loadInfo(settings);
    LogStorage::getInstance()->loadInfo(settings);
}

void Settings::saveSettings() {
    QSettings settings(FileName, QSettings::IniFormat);

    settings.beginGroup("Logging");
    settings.setValue("maxLinesOfLogs", maxLinesOfLogs);
    settings.setValue("highlightingFileName", highlightingFileName);

    settings.setValue("threadAdbLogcatStatus", logThreadStatus[LogThreadAdbLogcat]);
    settings.setValue("threadTrustzoneStatus", logThreadStatus[LogThreadTrustzone]);
    settings.setValue("threadKernelLogStatus", logThreadStatus[LogThreadKernel]);

    // adb logcat
    settings.setValue("flagVTime", flagVTime);
    settings.setValue("flagBMain", flagBMain);
    settings.setValue("flagBSystem", flagBSystem);
    settings.setValue("flagBRadio", flagBRadio);
    settings.setValue("flagBEvents", flagBEvents);
    settings.setValue("flagBCrash", flagBCrash);

    settings.endGroup();

    TextHighlighter::instance()->saveInfo(settings);
    LogStorage::getInstance()->saveInfo(settings);
}


QString Settings::getAdbLogcatFlags() {
    QString flags = "";

    if (flagVTime) {
        flags += " -v time";
    }

    if (flagBMain) {
        flags += " -b main";
    }

    if (flagBSystem) {
        flags += " -b system";
    }

    if (flagBRadio) {
        flags += " -b radio";
    }

    if (flagBEvents) {
        flags += " -b events";
    }

    if (flagBCrash) {
        flags += " -b crash";
    }

    return flags;
}
