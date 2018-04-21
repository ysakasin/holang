#include "holang/node.hpp"

using namespace std;
using namespace holang;

void KlassDefNode::print(int offset) {
  print_offset(offset);
  cout << "KlassDef " << name << endl;
  body->print(offset + 1);
}

void KlassDefNode::code_gen(vector<Code> *codes) {
  codes->push_back({Instruction::LOAD_CLASS});
  codes->push_back({.sval = &name});

  body->code_gen(codes);

  codes->push_back({Instruction::PREV_ENV});
}
