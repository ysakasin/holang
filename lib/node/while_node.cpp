#include "holang/node.hpp"

using namespace std;
using namespace holang;

void WhileNode::print(int offset) {
  print_offset(offset);
  cout << "while" << endl;
  cond->print(offset + 1);

  print_offset(offset);
  cout << "do " << endl;
  body->print(offset + 1);
}

void WhileNode::code_gen(vector<Code> *codes) {
  int to_cond = codes->size();
  cond->code_gen(codes);
  codes->push_back({Instruction::JUMP_IFNOT});
  codes->push_back({.ival = 0}); // dummy
  int from_cond = codes->size() - 1;

  body->code_gen(codes);
  codes->push_back({Instruction::JUMP});
  codes->push_back({.ival = to_cond});

  codes->at(from_cond).ival = codes->size();
}
