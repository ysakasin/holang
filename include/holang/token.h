#pragma once

#include <string>

#define keyword(name, _) name,
enum Keyword {
#include "holang/keyword.inc"
};
#undef keyword

#define keyword(_, word) #word,
const std::string KEYWORD_S[] = {
#include "holang/keyword.inc"
};
#undef keyword

enum TokenType {
  TNUMBER,
  TSTRING,
  TKEYWORD,
  TIDENT,
  TNEWLINE,
  TEOF,
};

struct Token {
  TokenType type;
  union {
    std::string *sval;
    Keyword keyword;
  };
  int line;
  int column;
};

static std::ostream &operator<<(std::ostream &out, const TokenType type) {
  switch (type) {
  case TNUMBER:
    return out << "TNUMBER";
  case TSTRING:
    return out << "TSTRING";
  case TKEYWORD:
    return out << "TKEYWORD";
  case TIDENT:
    return out << "TIDENT";
  case TNEWLINE:
    return out << "TNEWLINE";
  case TEOF:
    return out << "TEOF";
  }
}

static std::ostream &operator<<(std::ostream &out, const Keyword keyword) {
  return out << KEYWORD_S[keyword];
}

static std::ostream &operator<<(std::ostream &out, const Token *token) {
  if (token->type == TKEYWORD) {
    return out << token->type << " " << token->keyword;
  } else if (token->type == TIDENT) {
    return out << token->type << " " << *token->sval;
  } else {
    return out << token->type;
  }
}
