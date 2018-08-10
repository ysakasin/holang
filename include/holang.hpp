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
void print_token_chain(const std::vector<Token *> &token_chain);
int size_local_idents();
void set_func(const std::string &name, Func *func);
} // namespace holang
