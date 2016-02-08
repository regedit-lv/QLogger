#include "logstorage.h"

#include "settings.h"
#include "texthighlighter.h"
#include "debug.h"
#include "texthighlighter.h"

#include <QTableWidgetItem>
#include <QScrollBar>

LogStorage *LogStorage::instance = NULL;

LogStorage::LogStorage() {
    tableTags = NULL;
    tableTags = NULL;
    textEdit = NULL;
    lastLogNumber = 0;
}

LogStorage *LogStorage::getInstance() {
    if (NULL == instance) {
        instance = new LogStorage();
    }

    return instance;
}

int LogStorage::add(FilterItem &filter) {
    QTableWidgetItem *ti = new QTableWidgetItem(filter.name);

    filter.widget = ti;
    filter.widget->setCheckState(filter.isVisible ? Qt::Checked : Qt::Unchecked);

    int i = filters.add(filter);

    tableFilters->insertRow(i);
    tableFilters->setItem(i, 0, ti);

    return i;
}

void LogStorage::remove(const FilterItem &filter)
{
    int index = filters.indexOf(filter.widget);
    tableFilters->removeRow(filter.widget->row());
    filters.removeAt(index);
    TextHighlighter::instance()->removeRule(filter.ruleId);
}

void LogStorage::add(LogItem item) {

    item.number = lastLogNumber++;

    DBG_OUT << item.number << " item: " << item.type << " " << item.tag << " " << item.text;

    // remove end of line symbols
    while (item.text.length() > 0 &&
           ('\n' == item.text[item.text.length() - 1] ||
           '\r' == item.text[item.text.length() - 1]) ) {
        item.text.remove(item.text.length() - 1, 1);
    }

    logs.append(item);

    if (logs.size() > Settings::getInstance()->maxLinesOfLogs) {
        logs.removeFirst();
    }

    if (-1 == tags.indexOf(item.tag)) {
        addTag(item.tag);
    }

    if (-1 == types.indexOf(item.type)) {
        addType(item.type);
    }

    if (isVisible(item)) {
        textEdit->appendPlainText(getLogString(item));
    }
}

void LogStorage::setUI(QPlainTextEdit *textEdit, QTableWidget *tableTags, QTableWidget *tableTypes,
                       QTableWidget *tableFilters) {
    this->textEdit = textEdit;
    this->tableTypes = tableTypes;
    this->tableTags = tableTags;
    this->tableFilters = tableFilters;
}

void LogStorage::clear() {
    tableTags->setRowCount(0);
    tableTypes->setRowCount(0);
    tableFilters->setRowCount(0);
    textEdit->clear();
    logs.clear();
    tags.clear();
    types.clear();
    filters.clear();
}

void LogStorage::clearTags() {
    // remove references to tag objects
    for (int i = 0; i < logs.size(); i++) {
        logs[i].pTag = NULL;
    }
    tags.clear();
    tableTags->setRowCount(0);
}

void LogStorage::clearLogs() {
    textEdit->clear();
    logs.clear();
}

void LogStorage::setTagVisibility(QString tag, bool isVisible) {
    int i = tags.indexOf(tag);
    bool visibilityChanged = false;

    if (-1 != i) {
        visibilityChanged = tags[i].isVisible != isVisible;
    }

    if (visibilityChanged) {
        tags[i].isVisible = isVisible;
        refresh();
    }
}

void LogStorage::setTypeVisibility(QString type, bool isVisible) {
    int i = types.indexOf(type);
    bool visibilityChanged = false;

    if (-1 != i) {
        visibilityChanged = types[i].isVisible != isVisible;
    }

    if (visibilityChanged) {
        types[i].isVisible = isVisible;
        refresh();
    }
}

void LogStorage::setFilterVisibility(QString name, bool isVisible) {
    int i = filters.indexOf(name);
    bool visibilityChanged = false;

    if (-1 != i) {
        visibilityChanged = filters[i].isVisible != isVisible;
    }

    if (visibilityChanged) {
        filters[i].isVisible = isVisible;
        int index = TextHighlighter::instance()->indexOfRule(filters[i].ruleId);
        if (-1 != index) {
            TextHighlighter::instance()->rules[index].isActive = isVisible;
        }

        refresh();
    }
}

void LogStorage::refresh() {
    QString text = QString("");

    QScrollBar *vb = textEdit->verticalScrollBar();
    QScrollBar *hb = textEdit->verticalScrollBar();

    int verticalValue = vb->value();
    int horizontalValue = hb->value();

    textEdit->clear();
    for (int i = 0; i < logs.size(); i++) {

        /*
        DBG_OUT << "log tag: " << logs[i].tag
                 << " index: " << ti
                 << " visible: " << tags[ti].isVisible;
        */

        if (isVisible(logs[i])) {
            text.append(getLogString(logs[i]));
            text.append("\n");
        }
    }
    textEdit->appendPlainText(text);

    vb->setValue(verticalValue);
    hb->setValue(horizontalValue);
}

LogItem *LogStorage::getLogByNumber(int number) {
    if (logs.empty()) {
        return NULL;
    }

    int fn = logs[0].number;
    int i = number - fn;

    if (i >= 0 && i < logs.size()) {
        return &logs[i];
    } else {
        return NULL;
    }
}

int LogStorage::addTag(const QString &tag) {
    int index = tags.indexOf(tag);

    if (-1 == index) {
        QTableWidgetItem *tiTag = new QTableWidgetItem(tag);
        tiTag->setCheckState(Qt::Checked);

        TagItem ti;
        ti.widget = tiTag;
        ti.tag = tag;
        ti.isVisible = true;

        index = tags.add(ti);

        tableTags->insertRow(index);
        tableTags->setItem(index, 0, tiTag);
    }

    return index;
}

int LogStorage::addType(const QString &type) {
    QTableWidgetItem *tiType = new QTableWidgetItem(type);
    tiType->setCheckState(Qt::Checked);

    TypeItem ti;
    ti.widget = tiType;
    ti.type = type;
    ti.isVisible = true;

    int i = types.add(ti);

    tableTypes->insertRow(i);
    tableTypes->setItem(i, 0, tiType);

    return i;
}

QString LogStorage::getLogString(const LogItem &item) {
    return QString("%1: %2")
            .arg(item.number, Settings::getInstance()->lineNumberWidth)
            .arg(item.text);
}

TagItem *LogStorage::getTagItem(LogItem &item, bool create) {
    if (NULL == item.pTag) {
        int ti = tags.indexOf(item.tag);
        if (-1 != ti) {
            item.pTag = &tags[ti];
        }
    }

    if (create) {
        int i = addTag(item.tag);
        item.pTag = &tags[i];
    }

    return item.pTag;
}

TypeItem *LogStorage::getTypeItem(LogItem &item) {
    if (NULL == item.pType) {
        int ti = types.indexOf(item.type);
        if (-1 != ti) {
            item.pType = &types[ti];
        }
    }

    return item.pType;
}

void LogStorage::saveInfo(QSettings &settings)
{
    settings.beginGroup("Log");
    types.saveInfo(settings);
    tags.saveInfo(settings);
    filters.saveInfo(settings);
    settings.endGroup();
}

void LogStorage::loadInfo(QSettings &settings)
{
    settings.beginGroup("Log");
    int count = settings.beginReadArray("types");
    for (int i = 0; i < count; i++) {
        bool ok;

        settings.setArrayIndex(i);
        QString type = settings.value("type", "").toString();

        int j = types.indexOf(type);

        if (-1 == j) {
            j = addType(type);
        }

        TypeItem &ti = types[j];

        ti.isVisible = settings.value("isVisible", true).toBool();

        ti.ruleId = settings.value("ruleId", -1).toInt(&ok);
        if (!ok) {
            DBG_OUT << "Failed type " << i << " : ruleId";
            continue;
        }

        int ri = TextHighlighter::instance()->indexOfRule(ti.ruleId);
        if (-1 != ri) {
            ti.widget->setBackgroundColor(TextHighlighter::instance()->rules[ri].color);
        }
        ti.widget->setCheckState(ti.isVisible ? Qt::Checked : Qt::Unchecked);
    }
    settings.endArray();

    count = settings.beginReadArray("tags");
    for (int i = 0; i < count; i++) {
        bool ok;

        settings.setArrayIndex(i);
        QString tag = settings.value("tag", "").toString();

        int j = tags.indexOf(tag);

        if (-1 == j) {
            j = addTag(tag);
        }

        TagItem &ti = tags[j];

        ti.isVisible = settings.value("isVisible", true).toBool();

        ti.ruleId = settings.value("ruleId", -1).toInt(&ok);
        if (!ok) {
            DBG_OUT << "Failed tag " << i << " : ruleId";
            continue;
        }

        int ri = TextHighlighter::instance()->indexOfRule(ti.ruleId);
        if (-1 != ri) {
            ti.widget->setTextColor(TextHighlighter::instance()->rules[ri].color);
        }
        ti.widget->setCheckState(ti.isVisible ? Qt::Checked : Qt::Unchecked);
    }
    settings.endArray();

    count = settings.beginReadArray("filters");
    for (int i = 0; i < count; i++) {
        bool ok;
        FilterItem filter("", FilterItem::SHOW, "");

        settings.setArrayIndex(i);
        filter.name = settings.value("name", "").toString();

        filter.action = (FilterItem::FilterAction)settings.value("action", -1).toInt(&ok);
        if (!ok) {
            DBG_OUT << "Failed filter " << i << " : action";
            continue;
        }

        filter.setExpression(settings.value("expression", "").toString());

        filter.isVisible = settings.value("isVisible", true).toBool();

        filter.ruleId = settings.value("ruleId", -1).toInt(&ok);
        if (!ok) {
            DBG_OUT << "Failed filter " << i << " : ruleId";
            continue;
        }

        int j = filters.indexOf(filter.name);

        if (-1 == j) {
            add(filter);
        } else {
            filters[j] = filter;
        }

        int ri = TextHighlighter::instance()->indexOfRule(filter.ruleId);
        if (-1 != ri) {
            filter.widget->setTextColor(TextHighlighter::instance()->rules[ri].color);
            TextHighlighter::instance()->rules[ri].isActive = filter.isVisible;
        }
        filter.widget->setCheckState(filter.isVisible ? Qt::Checked : Qt::Unchecked);
    }
    settings.endArray();

    settings.endGroup();
}

const TagList *LogStorage::getTags()
{
    return &tags;
}

const TypeList *LogStorage::getTypes()
{
    return &types;
}

const FilterList *LogStorage::getFilters()
{
    return &filters;
}


bool LogStorage::isVisible(LogItem &item) {
    TagItem *tagItem = getTagItem(item);
    TypeItem *typeItem = getTypeItem(item);
    bool visible = true;


    if (NULL != tagItem && !tagItem->isVisible) {
        visible = false;
    }

    if (NULL != typeItem && !typeItem->isVisible) {
        visible = false;
    }

    // check off filters
    if (visible) {
        for (int i = 0; i < filters.size(); i++) {
            const FilterItem &filter = filters[i];
            if (FilterItem::HIDE == filter.action && filter.isVisible) {
                if (filter.isFound(item.text)) {
                    visible = false;
                }
            }
        }
    }

    // check on filters
    if (!visible) {
        for (int i = 0; i < filters.size(); i++) {
            const FilterItem &filter = filters[i];
            if (FilterItem::SHOW == filter.action && filter.isVisible) {
                if (filter.isFound(item.text)) {
                    visible = true;
                }
            }
        }
    }
    return visible;
}
