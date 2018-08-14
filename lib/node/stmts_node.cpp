#include "holang/node.hpp"

using namespace std;
using namespace holang;

void StmtsNode::print(int offset) {
  current->print(offset);
  next->print(offset);
}

void StmtsNode::code_gen(CodeSequence *codes) {
  current->code_gen(codes);
  codes->append(Instruction::POP);
  next->code_gen(codes);
}
