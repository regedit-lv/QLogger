#ifndef LOGSTORAGE_H
#define LOGSTORAGE_H

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QList>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QSettings>

#include <taglist.h>
#include <typelist.h>
#include "filterlist.h"

typedef struct LogItem {
    int number;
    QString type;
    QString tag;
    QString text;
    TagItem *pTag; // reference to tag item
    TypeItem *pType; // reference to type item

    LogItem() {
        pTag = NULL;
        pType = NULL;
    }

} LogItem;

class LogStorage
{
public:
    static LogStorage *getInstance();

    int add(FilterItem &filter);
    void remove(const FilterItem &filter);
    void add(LogItem item);
    void setUI(QPlainTextEdit *textEdit, QTableWidget *tableTags, QTableWidget *tableTypes,
               QTableWidget *tableFilters);
    void clear();
    void clearTags();
    void clearLogs();

    void setTagVisibility(QString tag, bool isVisible);
    void setTypeVisibility(QString type, bool isVisible);
    void setFilterVisibility(QString name, bool isVisible);
    void refresh();
    LogItem *getLogByNumber(int number);

    TagItem *getTagItem(LogItem &item, bool createIfdoes = false);
    TypeItem *getTypeItem(LogItem &item);

    void saveInfo(QSettings &settings);
    void loadInfo(QSettings &settings);

    const TagList *getTags();
    const TypeList *getTypes();
    const FilterList *getFilters();

    int addTag(const QString &tag);
    int addType(const QString &type);

protected:
    LogStorage();
    static LogStorage *instance;

    QString getLogString(const LogItem &item);
    bool isVisible(LogItem &item);

    QList<LogItem> logs;
    TagList tags;
    TypeList types;
    FilterList filters;
    QTableWidget *tableTags;
    QTableWidget *tableTypes;
    QTableWidget *tableFilters;
    QPlainTextEdit *textEdit;
    int lastLogNumber;
};

#endif // LOGSTORAGE_H
