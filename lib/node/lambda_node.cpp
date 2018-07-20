#include "holang/node.hpp"

using namespace std;
using namespace holang;

void LambdaNode::print(int offset) {
  print_offset(offset);
  cout << "Lambda" << endl;
  body->print(offset + 1);
}

void LambdaNode::code_gen(vector<Code> *codes) {
  vector<Code> body_code;

  body->code_gen(&body_code);
  body_code.push_back({Instruction::RET});
  codes->push_back({Instruction::PUT_LAMBDA});
  codes->push_back({.funcval = new Func(body_code)});
}
