#pragma once

#include "keyword.hpp"
#include <ostream>

namespace holang {
enum class TokenType {
  NUMBER,
  STRING,
  KEYWORD,
  IDENT,
  NEWLINE,
  TEOF, // EOF is defined by stdio.h
};

struct Token {
  TokenType type;
  union {
    std::string *sval;
    Keyword keyword;
  };
  int line;
  int column;

  Token(TokenType type) : type(type) {}
  Token(TokenType type, std::string *sval) : type(type), sval(sval) {}
  Token(TokenType type, Keyword keyword) : type(type), keyword(keyword) {}
};

static std::ostream &operator<<(std::ostream &out, const TokenType type) {
  switch (type) {
  case TokenType::NUMBER:
    return out << "NUMBER";
  case TokenType::STRING:
    return out << "STRING";
  case TokenType::KEYWORD:
    return out << "KEYWORD";
  case TokenType::IDENT:
    return out << "IDENT";
  case TokenType::NEWLINE:
    return out << "NEWLINE";
  case TokenType::TEOF:
    return out << "EOF";
  }
}

static std::ostream &operator<<(std::ostream &out, const Token *token) {
  if (token->type == TokenType::KEYWORD) {
    return out << token->type << " " << token->keyword;
  } else if (token->type == TokenType::IDENT) {
    return out << token->type << " " << *token->sval;
  } else if (token->type == TokenType::NUMBER) {
    return out << token->type << " " << *token->sval;
  } else {
    return out << token->type;
  }
}
} // namespace holang
