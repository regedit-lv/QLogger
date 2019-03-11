#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H

#include <QTableWidget>

struct AbstractItem {
    QTableWidgetItem *widget;
    int ruleId;
    bool isVisible;

    AbstractItem() {
        widget = nullptr;
        ruleId = 0;
        isVisible = false;
    }
};

#endif // ABSTRACTITEM_H
