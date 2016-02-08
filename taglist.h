#ifndef TAGLIST_H
#define TAGLIST_H

#include <QList>
#include <QString>
#include <QSettings>

#include "abstractitem.h"

typedef struct TagItem : public AbstractItem {
    QString tag;

    TagItem() : AbstractItem() {
    }
} TagItem;

class TagList : public QList<TagItem>
{
public:
    int add(const TagItem &item);
    TagItem *get(const QString &tag) const;
    TagItem *get(const QTableWidgetItem *tableItem) const;
    int indexOf(const QString &tag) const;
    int indexOf(const QTableWidgetItem *tableItem) const;

    void saveInfo(QSettings &settings);
    void loadInfo(QSettings &settings);

};

#endif // TAGLIST_H
