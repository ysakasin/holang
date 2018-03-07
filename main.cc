#include "./holang.hh"
#include "./vm.hh"
#include <fstream>
#include <iostream>

using namespace std;

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
  print_token_chain(token_chain);
  Node *root = parse(token_chain);
  vector<Code> codes;
  // codes.push_back({.op = OP_PUT_ENV});
  // codes.push_back({.ival = 0});
  root->code_gen(&codes);
  // codes[1].ival = size_local_idents();

  HolangVM vm(size_local_idents());
  vm.codes = &codes;
  cout << "----- vm.eval() begin -----" << endl;
  vm.eval();
  cout << "----- vm.eval() end -------" << endl;
  vm.print_stack();
}
