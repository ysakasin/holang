#pragma once

#include <iostream>
#include <string>

enum class Instruction {
  PUT_ENV,
  LOAD_INT,
  LOAD_BOOL,
  LOAD_STRING,
  POP,
  ADD,
  SUB,
  MUL,
  DIV,
  LESS,
  GREATER,
  LOCAL_STORE,
  LOCAL_LOAD,
  JUMP,
  JUMP_IF,
  JUMP_IFNOT,
  CALL_FUNC,
  RET,
  PUT_SELF,
  DEF_FUNC,
  LOAD_CLASS,
  PREV_ENV,
};

static std::ostream &operator<<(std::ostream &out,
                                const Instruction instruction) {
  switch (instruction) {
  case Instruction::PUT_ENV:
    return out << "PUT_ENV";
  case Instruction::LOAD_INT:
    return out << "LOAD_INT";
  case Instruction::LOAD_BOOL:
    return out << "LOAD_BOOL";
  case Instruction::LOAD_STRING:
    return out << "LOAD_STRING";
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
  case Instruction::LESS:
    return out << "LESS";
  case Instruction::GREATER:
    return out << "GREATER";
  case Instruction::LOCAL_STORE:
    return out << "LOCAL_STORE";
  case Instruction::LOCAL_LOAD:
    return out << "LOCAL_LOAD";
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
  }
}
