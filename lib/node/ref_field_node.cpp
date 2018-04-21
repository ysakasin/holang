#include "holang/node.hpp"

using namespace std;
using namespace holang;

void RefFieldNode::print(int offset) {
  print_offset(offset);
  cout << "." << *field << endl;
}

void RefFieldNode::code_gen(vector<Code> *codes) {
  codes->push_back({Instruction::LOAD_OBJ_FIELD});
  codes->push_back({.sval = field});
}
