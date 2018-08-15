#include "holang/node.hpp"

using namespace std;
using namespace holang;

void ReturnNode::print(int offset) {
  print_offset(offset);
  cout << "Return" << endl;
  expr->print(offset + 1);
}

void ReturnNode::code_gen(CodeSequence *codes) {
  expr->code_gen(codes);
  codes->append(Instruction::RET);
}
