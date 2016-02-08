#include "taglist.h"

int TagList::add(const TagItem &item)
{
    int i = 0;

    while (i < size()) {
        if (item.tag < at(i).tag) {
            break;
        }
        i++;
    }

    insert(i, item);

    return i;
}

TagItem *TagList::get(const QString &tag) const
{
    int i = indexOf(tag);
    return i == -1 ? NULL : &(TagItem&)at(i);
}

TagItem *TagList::get(const QTableWidgetItem *tableItem) const
{
    int i = indexOf(tableItem);
    return i == -1 ? NULL : &(TagItem&)at(i);
}

int TagList::indexOf(const QString &tag) const
{
    for (int i = 0; i < size(); i++) {
        if (tag == at(i).tag) {
            return i;
        }
    }
    return -1;
}

int TagList::indexOf(const QTableWidgetItem *tableItem) const
{
    for (int i = 0; i < size(); i++) {
        if (tableItem == at(i).widget) {
            return i;
        }
    }
    return -1;
}

void TagList::saveInfo(QSettings &settings)
{
    settings.beginWriteArray("tags", length());
    for (int i = 0; i < length(); i++)
    {
        const TagItem &ti = at(i);

        settings.setArrayIndex(i);
        settings.setValue("isVisible", ti.isVisible);
        settings.setValue("ruleId", ti.ruleId);
        settings.setValue("tag", ti.tag);
    }
    settings.endArray();
}

void TagList::loadInfo(QSettings &settings)
{

}
