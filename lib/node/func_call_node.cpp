#include "holang/node.hpp"

using namespace std;
using namespace holang;

void FuncCallNode::print(int offset) {
  print_offset(offset);
  cout << "Call " << name << endl;
  for (const auto &arg : args) {
    arg->print(offset + 1);
  }
}

void FuncCallNode::code_gen(vector<Code> *codes) {
  if (!is_trailer) {
    codes->push_back({Instruction::PUT_SELF});
  }

  for (Node *arg : args) {
    arg->code_gen(codes);
  }
  codes->push_back({Instruction::CALL_FUNC});
  codes->push_back({.sval = &name});
  codes->push_back({.ival = (int)args.size()});
}
