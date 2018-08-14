#include "holang/node.hpp"

using namespace std;
using namespace holang;

void SignChangeNode::print(int offset) {
  print_offset(offset);
  cout << "SignChange" << endl;
  body->print(offset + 1);
}

void SignChangeNode::code_gen(CodeSequence *codes) {
  body->code_gen(codes);
  codes->append(Instruction::PUT_INT);
  codes->append(-1);
  codes->append(Instruction::MUL);
}
