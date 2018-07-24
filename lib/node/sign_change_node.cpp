#include "holang/node.hpp"

using namespace std;
using namespace holang;

void SignChangeNode::print(int offset) {
  print_offset(offset);
  cout << "SignChange" << endl;
  body->print(offset + 1);
}

void SignChangeNode::code_gen(vector<Code> *codes) {
  body->code_gen(codes);
  codes->push_back({.op = Instruction::PUT_INT});
  codes->push_back({.ival = -1});
  codes->push_back({.op = Instruction::MUL});
}
