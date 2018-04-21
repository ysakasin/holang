#include "holang/node.hpp"

using namespace std;
using namespace holang;

void IfNode::print(int offset) {
  print_offset(offset);
  cout << "if" << endl;
  cond->print(offset + 1);

  print_offset(offset);
  cout << "then " << endl;
  then->print(offset + 1);

  if (els != nullptr) {
    print_offset(offset);
    cout << "else" << endl;
    els->print(offset + 1);
  }
}

void IfNode::code_gen(vector<Code> *codes) {
  cond->code_gen(codes);

  int from_if = codes->size() + 1;
  codes->push_back({Instruction::JUMP_IFNOT});
  codes->push_back({.ival = 0}); // dummy
  then->code_gen(codes);

  int from_then = codes->size() + 1;
  codes->push_back({Instruction::JUMP});
  codes->push_back({.ival = 0}); // dummy

  int to_else = codes->size();
  if (els == nullptr) {
    // nilの概念ができたらnilにする
    codes->push_back({Instruction::PUT_INT});
    codes->push_back({.ival = 0});
  } else {
    els->code_gen(codes);
  }
  int to_end = codes->size();

  codes->at(from_if).ival = to_else;
  codes->at(from_then).ival = to_end;
}
