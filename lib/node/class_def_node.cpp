#include "holang/node.hpp"

using namespace std;
using namespace holang;

void KlassDefNode::print(int offset) {
  print_offset(offset);
  cout << "KlassDef " << name << endl;
  body->print(offset + 1);
}

void KlassDefNode::code_gen(CodeSequence *codes) {
  codes->append(Instruction::LOAD_CLASS);
  codes->append(&name);

  body->code_gen(codes);

  codes->append(Instruction::PREV_ENV);
}
