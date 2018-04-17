#pragma once

#include <string>
#include <vector>

#include "holang/code.hpp"
#include "holang/node.hpp"
#include "holang/object.hpp"
#include "holang/token.hpp"
#include "holang/value.hpp"

namespace holang {
void print_token(const Token *token);
Node *parse(const std::vector<Token *> &chain);
void eval(std::vector<Code> *codes);
int lex(const std::string &code, std::vector<Token *> &token_chain);
void init_keywords();
void print_token_chain(const std::vector<Token *> &token_chain);
int size_local_idents();
void set_func(const std::string &name, Func *func);
}
