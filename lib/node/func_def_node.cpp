#include "holang/node.hpp"

using namespace std;
using namespace holang;

void FuncDefNode::print(int offset) {
  print_offset(offset);
  cout << "FuncDef " << name << endl;
  body->print(offset + 1);
}

void FuncDefNode::code_gen(CodeSequence *codes) {
  CodeSequence body_code(codes->source_path);

  body->code_gen(&body_code);
  body_code.append(Instruction::RET);

  codes->append(Instruction::DEF_FUNC);
  codes->append(&name);
  codes->append((Object *)new Func(body_code));
}
