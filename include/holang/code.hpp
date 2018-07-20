#pragma once

#include "holang/instruction.hpp"
#include <string>

namespace holang {
class Object;
struct Func;

union Code {
  Instruction op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Func *funcval;
  Object *objval;
};
} // namespace holang
