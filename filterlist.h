#ifndef FILTERLIST_H
#define FILTERLIST_H

#include <QString>
#include <QTableWidgetItem>
#include <QList>
#include <QSettings>
#include <QRegExp>

#include "abstractitem.h"

struct FilterItem : public AbstractItem
{
public:
    enum FilterAction {
        SHOW = 0,
        HIDE,
    };

    QString name;
    FilterAction action;

    FilterItem(const QString &name, FilterAction action, const QString &expression);
    bool isFound(const QString &text) const;
    const QString &expression() const;
    void setExpression(const QString &expression);
private:
    QString _expression;
    QRegExp _regExp;
};


class FilterList : public QList<FilterItem>
{
public:
    int add(const FilterItem &item);
    FilterItem *get(const QTableWidgetItem *tableItem) const;
    int indexOf(const QString &name) const;
    int indexOf(const QTableWidgetItem *tableItem) const;

    void saveInfo(QSettings &settings);
    void loadInfo(QSettings &settings);
};

#endif // FILTERITEM_H
