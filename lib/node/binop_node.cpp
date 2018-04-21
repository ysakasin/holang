#include "holang/node.hpp"

using namespace std;
using namespace holang;

void BinopNode::print(int offset) {
  print_offset(offset);
  cout << "Binop " << op << endl;
  lhs->print(offset + 1);
  rhs->print(offset + 1);
}

Instruction to_opcode(Keyword c) {
  switch (c) {
  case Keyword::ADD:
    return Instruction::ADD;
  case Keyword::SUB:
    return Instruction::SUB;
  case Keyword::MUL:
    return Instruction::MUL;
  case Keyword::DIV:
    return Instruction::DIV;
  case Keyword::LT:
    return Instruction::LESS;
  case Keyword::GT:
    return Instruction::GREATER;
  default:
    cerr << "err: to_opecode" << endl;
    exit(1);
  }
}

void BinopNode::code_gen(vector<Code> *codes) {
  lhs->code_gen(codes);
  rhs->code_gen(codes);
  codes->push_back({.op = to_opcode(op)});
}
