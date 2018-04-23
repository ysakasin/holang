#include "holang.hpp"
#include "holang/parser.hpp"
#include "holang/vm.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace holang;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "require source code" << endl;
    return -1;
  }

  string src(argv[1]);
  ifstream ifs(src);
  if (ifs.fail()) {
    std::cerr << src << ": Not found." << std::endl;
    return -1;
  }
  istreambuf_iterator<char> it(ifs);
  istreambuf_iterator<char> last;
  string code(it, last);

  init_keywords();

  vector<Token *> token_chain;
  lex(code, token_chain);

  holang::Parser parser(token_chain);
  Node *root = parser.parse();
  vector<Code> codes;
  // codes.push_back({.op = Instruction::PUT_ENV});
  // codes.push_back({.ival = 0});
  root->code_gen(&codes);
  // codes[1].ival = size_local_idents();

  HolangVM vm(parser.toplevel_val_size());
  vm.codes = &codes;
  // cout << "----- vm.eval() begin -----" << endl;
  vm.eval();
  // cout << "----- vm.eval() end -------" << endl;
  // vm.print_stack();
}
