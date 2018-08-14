#include "holang/node.hpp"

using namespace std;
using namespace holang;

void LambdaNode::print(int offset) {
  print_offset(offset);
  cout << "Lambda" << endl;
  body->print(offset + 1);
}

void LambdaNode::code_gen(CodeSequence *codes) {
  CodeSequence body_code(codes->source_path);

  body->code_gen(&body_code);
  body_code.append(Instruction::RET);

  codes->append(Instruction::PUT_LAMBDA);
  codes->append(new Func(body_code));
}
