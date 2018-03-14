#pragma once

#include <string>

#define opcode(name) name,
enum class Instruction {
#include "holang/opcode.inc"
};
#undef opcode

#define opcode(name) #name,
const std::string OPCODE_S[] = {
#include "holang/opcode.inc"
};
#undef opcode

class Object;

union Code {
  Instruction op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Object *objval;
};
