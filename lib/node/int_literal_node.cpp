#include "holang/node.hpp"

using namespace holang;

void IntLiteralNode::print(int offset) {
  print_offset(offset);
  cout << "IntLiteral " << value << "" << endl;
}

void IntLiteralNode::code_gen(CodeSequence *codes) {
  codes->append(Instruction::PUT_INT);
  codes->append(value);
}
