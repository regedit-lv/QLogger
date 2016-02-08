#include "typelist.h"

int TypeList::add(const TypeItem &item)
{
    int i = 0;

    while (i < size()) {
        if (item.type < at(i).type) {
            break;
        }
        i++;
    }

    insert(i, item);

    return i;
}

TypeItem *TypeList::get(const QString &type) const
{
    int i = indexOf(type);
    return i == -1 ? NULL : &(TypeItem&)at(i);
}

TypeItem *TypeList::get(const QTableWidgetItem *tableItem) const
{
    int i = indexOf(tableItem);
    return i == -1 ? NULL : &(TypeItem&)at(i);
}

int TypeList::indexOf(const QString &type) const
{
    for (int i = 0; i < size(); i++) {
        if (type == at(i).type) {
            return i;
        }
    }
    return -1;
}

int TypeList::indexOf(const QTableWidgetItem *tableItem) const
{
    for (int i = 0; i < size(); i++) {
        if (tableItem == at(i).widget) {
            return i;
        }
    }
    return -1;
}

void TypeList::saveInfo(QSettings &settings)
{
    settings.beginWriteArray("types", length());
    for (int i = 0; i < length(); i++)
    {
        const TypeItem &ti = at(i);

        settings.setArrayIndex(i);
        settings.setValue("isVisible", ti.isVisible);
        settings.setValue("ruleId", ti.ruleId);
        settings.setValue("type", ti.type);

    }
    settings.endArray();
}

void TypeList::loadInfo(QSettings &settings)
{

}
