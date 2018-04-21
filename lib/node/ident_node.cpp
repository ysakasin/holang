#include "holang/node.hpp"

using namespace std;
using namespace holang;

void IdentNode::print(int offset) {
  print_offset(offset);
  cout << "Ident " << ident << " : " << index << endl;
}

void IdentNode::code_gen(vector<Code> *codes) {
  codes->push_back({.op = Instruction::LOAD_LOCAL});
  codes->push_back({.ival = index});
}
