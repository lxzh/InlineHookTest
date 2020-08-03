//
// Created by ljf12 on 2020/8/3.
//

#include "test_class.h"
#include "util/common.h"

void TestClass::setField(int field) {
    this->field = field;
}

void TestClass::printValue(int value) {
    LOGD("printValue value=%u", value);
}