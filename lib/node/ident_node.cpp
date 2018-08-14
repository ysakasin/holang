#include "holang/node.hpp"

using namespace std;
using namespace holang;

void IdentNode::print(int offset) {
  print_offset(offset);
  cout << "Ident " << ident << " : " << index << endl;
}

void IdentNode::code_gen(CodeSequence *codes) {
  codes->append(Instruction::LOAD_LOCAL);
  codes->append(index);
}
