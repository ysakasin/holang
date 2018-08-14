#include "holang/node.hpp"

using namespace std;
using namespace holang;

void ImportNode::print(int offset) {
  print_offset(offset);
  cout << "Import" << endl;
  module->print(offset + 1);
}

void ImportNode::code_gen(CodeSequence *codes) {
  module->code_gen(codes);
  codes->append(Instruction::IMPORT);
}
