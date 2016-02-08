#ifndef IDHELPER_H
#define IDHELPER_H

#include <QList>

class IdClass {
public:
    int id;

    IdClass();
    IdClass(int id);
};

struct IdStruct {
    int id;

    IdStruct();
    IdStruct(int id);
};


class IdHelper
{
public:
    static int getNextId();
    static int setMinId(int minId);
protected:
    static int lastId;

    // template <typename T>
    static int indexById(int id, QList<IdStruct> list) {
        for (int i = 0; i < list.size(); i++) {
            if (list[i].id == id) {
                return i;
            }
        }
        return -1;
    }

    friend class IdClass;
    friend struct IdStruct;
};

#endif // IDHELPER_H
