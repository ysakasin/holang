#include "holang/node.hpp"

using namespace std;
using namespace holang;

void AssignNode::print(int offset) {
  print_offset(offset);
  cout << "Assign " << lhs->ident << " : " << lhs->index << endl;
  rhs->print(offset + 1);
}

void AssignNode::code_gen(CodeSequence *codes) {
  rhs->code_gen(codes);
  codes->append(Instruction::STORE_LOCAL);
  codes->append(lhs->index);
}
