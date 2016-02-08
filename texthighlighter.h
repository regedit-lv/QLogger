#ifndef TEXTHIGHLIGHTER_H
#define TEXTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>
#include <QColor>
#include <QString>
#include <QList>
#include <QSettings>

#include "idhelper.h"

typedef struct TextHighlighterRule : public IdStruct
{
    enum PatternType {
        LOG_STARTS_WITH,
        LOG_CONTAIN,
        LOG_REGEXP,
        TYPE,
        TAG,
    };

    enum ColorType {
        COLOR_TEXT,
        COLOR_BACKGROUND
    };

    enum HighlightType {
        HIGHLIGHT_LINE,
        HIGHLIGHT_TEXT
    };

    QString pattern;
    PatternType patternType;
    QColor color;
    ColorType colorType;
    HighlightType highlightType;
    bool isActive;

    TextHighlighterRule (int id = 0) : IdStruct(id) {
        isActive = true;
    }

} TextHighlighterRule;

class TextHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    QList<TextHighlighterRule> rules;

    static TextHighlighter *instance();
    static void setDocument(QTextDocument *document);

    explicit TextHighlighter(QTextDocument *document = NULL);
    
    void addRule(const TextHighlighterRule &rule);
    void removeRule(int ruleId);
    void clearRules();

    int indexOfRule(int ruleId);
    void saveInfo(QSettings &settings);
    void loadInfo(QSettings &settings);
protected:
    void highlightBlock(const QString &text);

private:
    static TextHighlighter *_instance;
    static QTextDocument *_document;

    TextHighlighter();

    int insertIndex(const TextHighlighterRule &rule);

signals:
    
public slots:
    
};

#endif // TEXTHIGHLIGHTER_H
