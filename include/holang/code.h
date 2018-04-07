#pragma once

#include "holang/instruction.h"
#include <string>

class Object;

union Code {
  Instruction op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Object *objval;
};
