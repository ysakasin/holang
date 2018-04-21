#include "holang/node.hpp"

using namespace holang;

void IntLiteralNode::print(int offset) {
  print_offset(offset);
  cout << "IntLiteral " << value << "" << endl;
}

void IntLiteralNode::code_gen(vector<Code> *codes) {
  codes->push_back({.op = Instruction::PUT_INT});
  codes->push_back({.ival = value});
}
