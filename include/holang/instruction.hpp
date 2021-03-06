#pragma once

#include <iostream>
#include <string>

namespace holang {
enum class Instruction {
  PUT_ENV,
  PUT_INT,
  PUT_BOOL,
  PUT_STRING,
  PUT_LAMBDA,
  POP,
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  LESS,
  GREATER,
  EQUAL,
  STORE_LOCAL,
  LOAD_LOCAL,
  JUMP,
  JUMP_IF,
  JUMP_IFNOT,
  CALL_FUNC,
  RET,
  PUT_SELF,
  DEF_FUNC,
  LOAD_CLASS,
  PREV_ENV,
  LOAD_OBJ_FIELD,
  IMPORT,
};

static std::ostream &operator<<(std::ostream &out,
                                const Instruction instruction) {
  switch (instruction) {
  case Instruction::PUT_ENV:
    return out << "PUT_ENV";
  case Instruction::PUT_INT:
    return out << "PUT_INT";
  case Instruction::PUT_BOOL:
    return out << "PUT_BOOL";
  case Instruction::PUT_STRING:
    return out << "PUT_STRING";
  case Instruction::PUT_LAMBDA:
    return out << "PUT_LAMBDA";
  case Instruction::POP:
    return out << "POP";
  case Instruction::ADD:
    return out << "ADD";
  case Instruction::SUB:
    return out << "SUB";
  case Instruction::MUL:
    return out << "MUL";
  case Instruction::DIV:
    return out << "DIV";
  case Instruction::MOD:
    return out << "MOD";
  case Instruction::LESS:
    return out << "LESS";
  case Instruction::GREATER:
    return out << "GREATER";
  case Instruction::EQUAL:
    return out << "EQUAL";
  case Instruction::STORE_LOCAL:
    return out << "STORE_LOCAL";
  case Instruction::LOAD_LOCAL:
    return out << "LOAD_LOCAL";
  case Instruction::JUMP:
    return out << "JUMP";
  case Instruction::JUMP_IF:
    return out << "JUMP_IF";
  case Instruction::JUMP_IFNOT:
    return out << "JUMP_IFNOT";
  case Instruction::CALL_FUNC:
    return out << "CALL_FUNC";
  case Instruction::RET:
    return out << "RET";
  case Instruction::PUT_SELF:
    return out << "PUT_SELF";
  case Instruction::DEF_FUNC:
    return out << "DEF_FUNC";
  case Instruction::LOAD_CLASS:
    return out << "LOAD_CLASS";
  case Instruction::PREV_ENV:
    return out << "PREV_ENV";
  case Instruction::LOAD_OBJ_FIELD:
    return out << "LOAD_OBJ_FIELD";
  case Instruction::IMPORT:
    return out << "IMPORT";
  }
}
} // namespace holang
