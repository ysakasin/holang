#include "holang/node.hpp"

using namespace std;
using namespace holang;

void PrimeExprNode::print(int offset) {
  prime->print(offset);
  traier->print(offset + 1);
}

void PrimeExprNode::code_gen(CodeSequence *codes) {
  prime->code_gen(codes);
  traier->code_gen(codes);
}
