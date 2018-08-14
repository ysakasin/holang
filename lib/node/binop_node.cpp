#include "holang/node.hpp"

using namespace std;
using namespace holang;

void BinopNode::print(int offset) {
  print_offset(offset);
  cout << "Binop " << op << endl;
  lhs->print(offset + 1);
  rhs->print(offset + 1);
}

Instruction to_opcode(TokenType c) {
  switch (c) {
  case TokenType::Plus:
    return Instruction::ADD;
  case TokenType::Minus:
    return Instruction::SUB;
  case TokenType::Mul:
    return Instruction::MUL;
  case TokenType::Div:
    return Instruction::DIV;
  case TokenType::Mod:
    return Instruction::MOD;
  case TokenType::LessThan:
    return Instruction::LESS;
  case TokenType::GreaterThan:
    return Instruction::GREATER;
  case TokenType::Equal:
    return Instruction::EQUAL;
  default:
    cerr << "err: to_opecode" << endl;
    exit(1);
  }
}

void BinopNode::code_gen(CodeSequence *codes) {
  lhs->code_gen(codes);
  rhs->code_gen(codes);
  codes->append(to_opcode(op));
}
