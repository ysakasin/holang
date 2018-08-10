#pragma once

#include "holang/token.hpp"
#include <cstddef>
#include <map>
#include <vector>

namespace holang {
class Lexer {
public:
  Lexer() { init_keywords(); }
  Lexer(const std::string &str) : code_str(str) { init_keywords(); }
  void lex(std::vector<Token *> &token_chain);

private:
  Token *take_token();

  char readc();
  void unreadc();
  char nextc() const;

  bool is_next(char c);

  Token *read_number(char c);
  Token *read_ident(char c);
  Token *read_str();

  void skip_blank();
  void skip_to_newline();
  void skip_blank_lines();

  void invalid(char c) const;

private:
  static std::map<std::string, TokenType> keywords;
  static void init_keywords();

  std::string code_str;
  size_t head = 0;
  size_t line = 0;
  size_t line_begin_at = 0;
  size_t token_begin_at = 0;
};
} // namespace holang
