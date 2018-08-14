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

void FuncCallNode::code_gen(CodeSequence *codes) {
  if (!is_trailer) {
    codes->append(Instruction::PUT_SELF);
  }

  for (Node *arg : args) {
    arg->code_gen(codes);
  }
  codes->append(Instruction::CALL_FUNC);
  codes->append(&name);
  codes->append((int)args.size());
}
