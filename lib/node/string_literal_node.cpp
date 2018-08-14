#include "holang/node.hpp"

using namespace std;
using namespace holang;

void StringLiteralNode::print(int offset) {
  print_offset(offset);
  cout << "StringLiteral \"" << str << "\"" << endl;
}

void StringLiteralNode::code_gen(CodeSequence *codes) {
  codes->append(Instruction::PUT_STRING);
  codes->append(&str);
}
