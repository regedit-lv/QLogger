#ifndef TYPELIST_H
#define TYPELIST_H

#include <QList>
#include <QString>
#include <QSettings>

#include "abstractitem.h"

typedef struct TypeItem : public AbstractItem {
    QString type;

    TypeItem() : AbstractItem() {
    }
} TypeItem;


class TypeList : public QList<TypeItem>
{
public:
    int add(const TypeItem &item);
    TypeItem *get(const QString &type) const;
    TypeItem *get(const QTableWidgetItem *tableItem) const;
    int indexOf(const QString &type) const;
    int indexOf(const QTableWidgetItem *tableItem) const;

    void saveInfo(QSettings &settings);
    void loadInfo(QSettings &settings);
};

#endif // TYPELIST_H
