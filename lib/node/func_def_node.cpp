#include "holang/node.hpp"

using namespace std;
using namespace holang;

void FuncDefNode::print(int offset) {
  print_offset(offset);
  cout << "FuncDef " << name << endl;
  body->print(offset + 1);
}

void FuncDefNode::code_gen(vector<Code> *codes) {
  vector<Code> body_code;

  body->code_gen(&body_code);
  body_code.push_back({Instruction::RET});
  codes->push_back({Instruction::DEF_FUNC});
  codes->push_back({.sval = &name});
  codes->push_back({.objval = (Object *)new Func(body_code)});
}
