#include "holang/node.hpp"

using namespace std;
using namespace holang;

void SignChangeNode::print(int offset) {
  print_offset(offset);
  cout << "SignChange" << endl;
  body->print(offset + 1);
}

void SignChangeNode::code_gen(vector<Code> *codes) {
  exit_by_unsupported("SignChangeNode#code_gen()");
}
