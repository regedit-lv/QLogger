#include "idhelper.h"

int IdHelper::lastId = 0;

IdClass::IdClass() {
    IdHelper::lastId = id;
}

IdClass::IdClass(int id) {
    if (0 == id) {
        this->id = IdHelper::getNextId();
    } else {
        this->id = id;
        if (id > IdHelper::lastId) {
            IdHelper::lastId = id;
        }
    }
}

IdStruct::IdStruct() {
    IdHelper::lastId = id;
}

IdStruct::IdStruct(int id) {
    if (0 == id) {
        this->id = IdHelper::getNextId();
    } else {
        this->id = id;
        if (id > IdHelper::lastId) {
            IdHelper::lastId = id;
        }
    }
}

int IdHelper::getNextId() {
    return ++lastId;
}

int IdHelper::setMinId(int minId) {
    if (minId > lastId) {
        lastId = minId;
    }
}
