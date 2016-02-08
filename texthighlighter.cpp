#include "texthighlighter.h"

#include "debug.h"

#include <QTextCharFormat>

#include "settings.h"
#include "logstorage.h"

TextHighlighter *TextHighlighter::_instance = NULL;
QTextDocument *TextHighlighter::_document = NULL;

TextHighlighter *TextHighlighter::instance() {
    if (_instance == NULL) {
        _instance = new TextHighlighter(_document);
    }
    return _instance;
}

void TextHighlighter::setDocument(QTextDocument *document) {
    TextHighlighter::_document = document;
    delete _instance;
    _instance = NULL;
}

TextHighlighter::TextHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document)
{
}

void TextHighlighter::addRule(const TextHighlighterRule &rule) {
    int i = indexOfRule(rule.id);

    if (-1 == i) {
        int index = insertIndex(rule);
        rules.insert(index, rule);
    } else {
        rules[i] = rule;
    }
}

void TextHighlighter::removeRule(int ruleId) {
    int i = indexOfRule(ruleId);
    if (-1 != i) {
        rules.removeAt(i);
    }
}

void TextHighlighter::clearRules() {
    rules.clear();
}

int TextHighlighter::indexOfRule(int ruleId) {
    int index = -1;

    for (int i = 0; i < rules.length(); i++) {
        if (rules[i].id == ruleId) {
            index = i;
            break;
        }
    }

    return index;
}

void TextHighlighter::saveInfo(QSettings &settings) {
    settings.beginGroup("Highlighting");
    settings.beginWriteArray("highlighterRules", rules.size());
    for (int i = 0; i < rules.size(); i++) {
        TextHighlighterRule &r = rules[i];

        settings.setArrayIndex(i);
        settings.setValue("id", r.id);
        settings.setValue("pattern", r.pattern);
        settings.setValue("patternType", r.patternType);
        settings.setValue("color", r.color.name());
        settings.setValue("colorType", r.colorType);
        settings.setValue("highlightType", r.highlightType);
        settings.setValue("isActive", r.isActive);
    }
    settings.endArray();
    settings.endGroup();
}

void TextHighlighter::loadInfo(QSettings &settings) {
    settings.beginGroup("Highlighting");
    int count = settings.beginReadArray("highlighterRules");

    for (int i = 0; i < count; i++) {
        TextHighlighterRule r;
        bool ok;

        settings.setArrayIndex(i);

        r.id = settings.value("id", -1).toInt(&ok);
        if (!ok) {
            ERR_OUT << "Failed rule " << i << " : id";
            continue;
        }

        r.pattern = settings.value("pattern", "").toString();
        if ("" == r.pattern) {
            ERR_OUT << "Failed rule " << i << " : pattern";
            continue;
        }

        r.patternType = (TextHighlighterRule::PatternType)settings.value("patternType", -1).toInt(&ok);
        if (!ok) {
            ERR_OUT << "Failed rule " << i << " : patternType";
            continue;
        }

        QString color = settings.value("color", "").toString();
        if ("" == color) {
            ERR_OUT << "Failed rule " << i << " : color";
            continue;
        }
        r.color = QColor(color);

        r.colorType = (TextHighlighterRule::ColorType)settings.value("colorType", -1).toInt(&ok);
        if (!ok) {
            ERR_OUT << "Failed rule " << i << " : colorType";
            continue;
        }

        r.highlightType = (TextHighlighterRule::HighlightType)settings.value("highlightType", -1).toInt(&ok);
        if (!ok) {
            ERR_OUT << "Failed rule " << i << " : highlightType";
            continue;
        }

        r.isActive = settings.value("isActive", true).toBool();

        IdHelper::setMinId(r.id);
        addRule(r);
    }
    settings.endArray();
    settings.endGroup();
}

void TextHighlighter::highlightBlock(const QString &text) {
    bool ok = false;
    int number = text.mid(0, Settings::getInstance()->lineNumberWidth).toInt(&ok);

    if (!ok) {
        return;
    }

    // clear block background
    QTextCursor cursor(this->document());
    QTextBlockFormat bf = currentBlock().blockFormat();
    bf.clearBackground();

    cursor.setPosition(currentBlock().position());
    cursor.setPosition(currentBlock().position() + currentBlock().length() - 1, QTextCursor::KeepAnchor);
    cursor.setBlockFormat(bf);

    DBG_OUT << "log number: " << number << " text: " << text;
    LogItem *logItem = LogStorage::getInstance()->getLogByNumber(number);
    QString &logText = logItem->text;

    if (NULL == logItem) {
        return;
    }

    foreach (const TextHighlighterRule &r, rules) {
        if (!r.isActive) {
            continue;
        }
        QTextCharFormat ruleFormat;

        switch (r.colorType) {
            case TextHighlighterRule::COLOR_TEXT:
                ruleFormat.setForeground(r.color);
                break;

            case TextHighlighterRule::COLOR_BACKGROUND:
                ruleFormat.setBackground(r.color);
                break;
        }

        int startIndex = 0;
        while (startIndex < logText.length()) {
            int length = 0;

            switch (r.patternType) {
                case TextHighlighterRule::LOG_STARTS_WITH:
                    if (0 == startIndex) {
                        if (logText.startsWith(r.pattern, Qt::CaseInsensitive)) {
                            length = r.pattern.length();
                        }
                    }
                    break;

                case TextHighlighterRule::TAG:
                    if (0 == startIndex) {
                        if (logItem->tag == r.pattern) {
                            length = r.pattern.length();
                        }
                    }
                    break;

                case TextHighlighterRule::LOG_CONTAIN:
                    startIndex = logText.indexOf(r.pattern, startIndex, Qt::CaseInsensitive);
                    if (-1 != startIndex) {
                        length = r.pattern.length();
                    }
                    break;

                case TextHighlighterRule::LOG_REGEXP:
                {
                    QRegExp regExp(r.pattern, Qt::CaseInsensitive);
                    regExp.setMinimal(true);
                    startIndex = regExp.indexIn(logText, startIndex);
                    if (-1 != startIndex) {
                        length = regExp.matchedLength();
                    }
                    break;
                }

                case TextHighlighterRule::TYPE:
                    if (logItem->type == r.pattern) {
                        QTextCursor cursor(this->document());
                        QTextBlockFormat bf = currentBlock().blockFormat();
                        bf.setBackground(r.color);

                        cursor.setPosition(currentBlock().position());
                        cursor.setPosition(currentBlock().position() + currentBlock().length(), QTextCursor::KeepAnchor);
                        cursor.setBlockFormat(bf);
                    }
                    break;
            }

            if (0 != length) {
                int highlightingStartPosition = 0;
                if (TextHighlighterRule::HIGHLIGHT_LINE == r.highlightType) {
                    highlightingStartPosition = 0;
                    length = text.length();
                } else {
                    highlightingStartPosition = startIndex + Settings::getInstance()->lineNumberWidth + 2;
                }

                setFormat(highlightingStartPosition, length, ruleFormat);
                startIndex += length;
            } else {
                // pattern not found
                break;
            }
        }
    }

    // highlight line numbers
    QTextCharFormat ruleFormat;
    ruleFormat.setForeground(QColor(80, 80, 80));
    ruleFormat.setBackground(QColor(0, 0, 0));
    setFormat(0, Settings::getInstance()->lineNumberWidth + 2, ruleFormat);
}

int TextHighlighter::insertIndex(const TextHighlighterRule &rule) {
    int i = 0;

    while (i < rules.size()) {
        if (rule.highlightType < rules[i].highlightType) {
            break;
        }
        i++;
    }

    return i;
}
