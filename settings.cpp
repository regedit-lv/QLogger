#include "settings.h"
#include "debug.h"
#include "texthighlighter.h"
#include "logstorage.h"

Settings *Settings::instance = nullptr;
const char *Settings::FileName = "settings.ini";

Settings::Settings() {

}

void Settings::init() {
    loadSettings();
    saveSettings();
}

Settings *Settings::getInstance() {
    if (nullptr == instance) {
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

    settings.beginGroup("Streams");
    int count = settings.beginReadArray("ComPorts");

    for (int i = 0; i < count; i++)
    {
        ComPortSettings comPort;

        settings.setArrayIndex(i);
        comPort.streamSettings.name = settings.value("name", "").toString();
        comPort.streamSettings.tagScript = settings.value("tagScript", "").toString();
        comPort.port = settings.value("port", "").toString();
        comPort.baudRate = settings.value("baudRate", -1).toInt();
        comPort.dataBits = static_cast<QSerialPort::DataBits>(settings.value("dataBits", -1).toInt());
        comPort.parity = static_cast<QSerialPort::Parity>(settings.value("parity", -1).toInt());
        comPort.stopBits = static_cast<QSerialPort::StopBits>(settings.value("stopBits", -1).toInt());
        comPort.flowControl = static_cast<QSerialPort::FlowControl>(settings.value("flowControl", -1).toInt());

        comPorts.append(comPort);
    }
    settings.endArray();

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

    settings.beginGroup("Streams");
    settings.beginWriteArray("ComPorts", comPorts.size());
    for (int i = 0; i < comPorts.size(); i++)
    {
        const ComPortSettings &comPort = comPorts.at(i);

        settings.setArrayIndex(i);
        settings.setValue("name", comPort.streamSettings.name);
        settings.setValue("tagScript", comPort.streamSettings.tagScript);
        settings.setValue("port", comPort.port);
        settings.setValue("baudRate", comPort.baudRate);
        settings.setValue("dataBits", comPort.dataBits);
        settings.setValue("parity", comPort.parity);
        settings.setValue("stopBits", comPort.stopBits);
        settings.setValue("flowControl", comPort.flowControl);
    }
    settings.endArray();

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

ComPortSettings *Settings::getComPortSettings(const QString name)
{
    for (ComPortSettings &comPort : comPorts)
    {
        DBG_OUT << name << " " << comPort.streamSettings.name << "\n";
        if (comPort.streamSettings.name == name)
        {
            DBG_OUT << "true\n";
            return &comPort;
        }
        DBG_OUT << "false\n";
    }

    return nullptr;
}

bool Settings::deleteComPortSettings(const QString name)
{
    for (int i = 0; i < comPorts.size(); i++)
    {
        ComPortSettings &comPort = comPorts[i];
        if (comPort.streamSettings.name == name)
        {
            comPorts.removeAt(i);
            return true;
        }
    }

    return false;
}
