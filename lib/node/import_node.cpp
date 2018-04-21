#include "holang/node.hpp"

using namespace std;
using namespace holang;

void ImportNode::print(int offset) {
  print_offset(offset);
  cout << "Import" << endl;
  module->print(offset + 1);
}

void ImportNode::code_gen(vector<Code> *codes) {
  module->code_gen(codes);
  codes->push_back({Instruction::IMPORT});
}
