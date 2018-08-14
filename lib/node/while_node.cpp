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

void WhileNode::code_gen(CodeSequence *codes) {
  int to_cond = codes->size();
  cond->code_gen(codes);
  codes->append(Instruction::JUMP_IFNOT);
  codes->append(0); // dummy
  int from_cond = codes->size() - 1;

  body->code_gen(codes);
  codes->append(Instruction::JUMP);
  codes->append(to_cond);

  codes->at(from_cond).ival = codes->size();
}
