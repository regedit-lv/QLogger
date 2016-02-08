#include "adblogcat.h"

#include "debug.h"

AdbLogcat::AdbLogcat()
{
}

bool AdbLogcat::parseLog(const QString &line, QString &type, QString &tag, QString &text) {
    type = "android";
    text = "";
    bool parseOk = true;

    // search for tag start
    int tagStart = line.indexOf('/');
    int tagEnd = -1;

    if (-1 == tagStart) {
        parseOk = false;
    }

    if (parseOk) {
        tagStart++;
        // search for tag end
        // search for tag '('
        int tagEnd1 = line.indexOf('(', tagStart);
        // search for tag ':'
        int tagEnd2 = line.indexOf(':', tagStart);

        if (-1 != tagEnd1) {
            tagEnd = tagEnd1;
        }

        if (-1 != tagEnd2 && tagEnd2 < tagEnd) {
            tagEnd = tagEnd2;
        }

        if (-1 == tagEnd) {
            parseOk = false;
        }
    }

    if (parseOk) {
        tag = line.mid(tagStart, tagEnd - tagStart);
    } else {
        tag = "";
    }

    text = line;

    return parseOk;
}

