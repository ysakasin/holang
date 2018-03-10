#pragma once

#include "holang/object.h"
#include <string>

enum ValueType {
  VINT,
  VDOUBLE,
  VBOOL,
  VOBJECT,
};

struct Value {
  ValueType type;
  union {
    int ival;
    double dval;
    bool bval;
    Object *objval;
  };

  Value() {}
  Value(int i) : type(VINT), ival(i) {}
  Value(double d) : type(VDOUBLE), dval(d) {}
  Value(bool b) : type(VBOOL), bval(b) {}
  Value(Object *obj) : type(VOBJECT), objval(obj) {}

  Func *find_method(const std::string &name);

  const std::string to_s() {
    switch (type) {
    case VINT:
      return std::to_string(ival);
    case VBOOL:
      return bval ? "true" : "false";
    case VDOUBLE:
      return std::to_string(dval);
    case VOBJECT:
      return objval->to_s();
    }
  }
};
