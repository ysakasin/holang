#include "holang/node.hpp"

using namespace std;
using namespace holang;

void ExprsNode::print(int offset) {
  current->print(offset);
  next->print(offset);
}

void ExprsNode::code_gen(CodeSequence *codes) {
  current->code_gen(codes);
  next->code_gen(codes);
}
