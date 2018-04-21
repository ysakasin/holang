#include "holang/node.hpp"

using namespace std;
using namespace holang;

void BoolLiteralNode::print(int offset) {
  print_offset(offset);
  cout << "BoolLiteral " << (value ? "true" : "false") << endl;
}

void BoolLiteralNode::code_gen(vector<Code> *codes) {
  codes->push_back({.op = Instruction::PUT_BOOL});
  codes->push_back({.bval = value});
}
