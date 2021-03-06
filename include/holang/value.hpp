#pragma once

#include "holang/object.hpp"
#include <string>

namespace holang {
enum class Type {
  INT,
  DOUBLE,
  BOOL,
  FUNCTION,
  OBJECT,
};

struct Func;

struct Value {
  Type type;
  union {
    int ival;
    double dval;
    bool bval;
    Func *funcval;
    Object *objval;
  };

  Value() {}
  Value(int i) : type(Type::INT), ival(i) {}
  Value(double d) : type(Type::DOUBLE), dval(d) {}
  Value(bool b) : type(Type::BOOL), bval(b) {}
  Value(Func *func) : type(Type::FUNCTION), funcval(func) {}
  Value(Object *obj) : type(Type::OBJECT), objval(obj) {}

  Func *find_method(const std::string &name);
  Object *find_field(const std::string &name);

  const std::string to_s() {
    switch (type) {
    case Type::INT:
      return std::to_string(ival);
    case Type::BOOL:
      return bval ? "true" : "false";
    case Type::DOUBLE:
      return std::to_string(dval);
    case Type::FUNCTION:
      return "Func";
    case Type::OBJECT:
      return objval->to_s();
    default:
      return "known";
    }
  }
};
} // namespace holang
