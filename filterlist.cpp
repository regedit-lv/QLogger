#include "filterlist.h"

FilterItem::FilterItem(const QString &name, FilterAction action, const QString &expression) {
    this->name = name;
    this->action = action;
    setExpression(expression);
}

bool FilterItem::isFound(const QString &text) const
{
    bool result = false;

    result =_regExp.indexIn(text) != -1;

    return result;
}

const QString &FilterItem::expression() const
{
    return _expression;
}

void FilterItem::setExpression(const QString &expression)
{
    _expression = expression;
    _regExp = QRegExp(expression, Qt::CaseInsensitive);
    _regExp.setMinimal(true);
}

int FilterList::add(const FilterItem &item)
{
    int i = 0;

    while (i < size()) {
        if (item.name < at(i).name) {
            break;
        }
        i++;
    }

    insert(i, item);

    return i;
}

FilterItem *FilterList::get(const QTableWidgetItem *tableItem) const
{
    int i = indexOf(tableItem);
    return i == -1 ? NULL : &(FilterItem&)at(i);
}

int FilterList::indexOf(const QString &name) const
{
    for (int i = 0; i < size(); i++) {
        if (name == at(i).name) {
            return i;
        }
    }
    return -1;
}

int FilterList::indexOf(const QTableWidgetItem *tableItem) const
{
    for (int i = 0; i < size(); i++) {
        if (tableItem == at(i).widget) {
            return i;
        }
    }
    return -1;
}

void FilterList::saveInfo(QSettings &settings)
{
    settings.beginWriteArray("filters", length());
    for (int i = 0; i < length(); i++)
    {
        const FilterItem &item = at(i);

        settings.setArrayIndex(i);
        settings.setValue("name", item.name);
        settings.setValue("action", item.action);
        settings.setValue("expression", item.expression());
        settings.setValue("isVisible", item.isVisible);
        settings.setValue("ruleId", item.ruleId);
    }
    settings.endArray();
}

void FilterList::loadInfo(QSettings &settings)
{

}
