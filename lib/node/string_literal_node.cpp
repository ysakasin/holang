#include "holang/node.hpp"

using namespace std;
using namespace holang;

void StringLiteralNode::print(int offset) {
  print_offset(offset);
  cout << "StringLiteral \"" << str << "\"" << endl;
}

void StringLiteralNode::code_gen(vector<Code> *codes) {
  codes->push_back({.op = Instruction::PUT_STRING});
  codes->push_back({.sval = &str});
}
