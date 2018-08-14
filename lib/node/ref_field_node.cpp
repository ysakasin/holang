#include "holang/node.hpp"

using namespace std;
using namespace holang;

void RefFieldNode::print(int offset) {
  print_offset(offset);
  cout << "." << field << endl;
}

void RefFieldNode::code_gen(CodeSequence *codes) {
  codes->append(Instruction::LOAD_OBJ_FIELD);
  codes->append(&field);
}
