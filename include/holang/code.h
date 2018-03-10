#pragma once

#include <string>

#define opcode(name) name,
enum OpCode {
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
  OpCode op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Object *objval;
};
