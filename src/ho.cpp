#include "holang.hpp"
#include "holang/lexer.hpp"
#include "holang/parser.hpp"
#include "holang/vm.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace holang;

int main(int argc, char *argv[]) {
  bool show_ast = false;
  bool show_token = false;
  if (argc < 2) {
    cerr << "require source code" << endl;
    return -1;
  }

  if (argc >= 3) {
    string opt(argv[2]);
    if (opt == "--ast") {
      show_ast = true;
    } else if (opt == "--token") {
      show_token = true;
    }
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

  vector<Token *> token_chain;
  holang::Lexer lexer(code);
  lexer.lex(token_chain);

  if (show_token) {
    for (auto *token : token_chain) {
      cout << token << endl;
    }
    return 0;
  }

  holang::Parser parser(token_chain);
  Node *root = parser.parse();
  CodeSequence codes(src);
  // codes.push_back({.op = Instruction::PUT_ENV});
  // codes.push_back({.ival = 0});
  if (root == nullptr) {
    return 0;
  }
  if (show_ast) {
    root->print(0);
    return 0;
  }
  root->code_gen(&codes);
  // codes[1].ival = size_local_idents();

  HolangVM vm(parser.toplevel_val_size());
  vm.codes = &codes;
  vm.eval();
}
