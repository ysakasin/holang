#pragma once

#include "holang/node.hpp"
#include "holang/token.hpp"
#include "holang/variable_table.hpp"
#include <iostream>
#include <vector>

namespace holang {
class Parser {
public:
  Parser(const std::vector<Token *> &token_chain) : token_chain(token_chain) {}
  Node *parse();
  int toplevel_val_size() { return variable_table.size(); }

private:
  Token *get() { return token_chain[head++]; }
  Token *get_ident() {
    Token *token = get();
    if (token->type != TokenType::Ident) {
      exit_by_unexpected(TokenType::Ident, token);
    }
    return token;
  }
  void unget() { head--; }

private:
  void take(TokenType type);
  void consume_newlines();

private:
  bool is_next(TokenType type, int offset = 0) {
    return token_chain[head - offset]->type == type;
  }
  bool is_eof() { return is_next(TokenType::TEOF); }
  bool next_token(TokenType type) {
    Token *token = get();
    if (token->type == type) {
      return true;
    }
    unget();
    return false;
  }

private:
  Node *read_toplevel();

  Node *read_stmt();
  Node *read_if();
  Node *read_funcdef();
  Node *read_klassdef();
  Node *read_import();
  Node *read_while();
  Node *read_return();
  Node *read_suite();

  Node *read_expr();
  Node *read_assignment_expr();
  Node *read_equal_expr();
  Node *read_comp_expr();
  Node *read_multiplicative_expr();
  Node *read_additive_expr();
  Node *read_factor();
  Node *read_prime_expr();
  Node *read_traier();

  Node *read_prime();
  Node *read_number();
  Node *read_string();
  Node *read_name_or_funccall(bool is_trailer);
  Node *read_block();
  void read_exprs(std::vector<Node *> &args);
  void read_arglist(std::vector<Node *> *args);
  void read_params(std::vector<std::string *> *params);

private:
  void exit_by_unexpected(TokenType expect, Token *actual) {
    std::cerr << "unexpected token: ";
    std::cerr << "line " << actual->line << ", column " << actual->column
              << std::endl;
    std::cerr << "  expect: " << expect << std::endl;
    std::cerr << "  actual: " << actual << std::endl;
    exit(1);
  }
  void exit_by_unexpected(const std::string &expect, Token *actual) {
    std::cerr << "unexpected token: ";
    std::cerr << "line " << actual->line << ", column " << actual->column
              << std::endl;
    std::cerr << "  expect: " << expect << std::endl;
    std::cerr << "  actual: " << actual << std::endl;
    exit(1);
  }

private:
  const std::vector<Token *> token_chain;
  VariableTable variable_table;
  int head = 0;
};
} // namespace holang
