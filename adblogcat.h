#ifndef ADBLOGCAT_H
#define ADBLOGCAT_H

#include <QString>

class AdbLogcat
{
public:
    static bool parseLog(const QString &line, QString &type, QString &tag, QString &text);
private:
    AdbLogcat();
};

#endif // ADBLOGCAT_H
