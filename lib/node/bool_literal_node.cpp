#include "holang/node.hpp"

using namespace std;
using namespace holang;

void BoolLiteralNode::print(int offset) {
  print_offset(offset);
  cout << "BoolLiteral " << (value ? "true" : "false") << endl;
}

void BoolLiteralNode::code_gen(CodeSequence *codes) {
  codes->append(Instruction::PUT_BOOL);
  codes->append(value);
}
