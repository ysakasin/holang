#pragma once

#include <string>
#include "holang/instruction.h"

class Object;

union Code {
  Instruction op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Object *objval;
};
