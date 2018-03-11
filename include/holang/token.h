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
    return out << std::string("TNUMBER");
  case TSTRING:
    return out << std::string("TSTRING");
  case TKEYWORD:
    return out << std::string("TKEYWORD");
  case TIDENT:
    return out << std::string("TIDENT");
  case TNEWLINE:
    return out << std::string("TNEWLINE");
  case TEOF:
    return out << std::string("TEOF");
  }
}

static std::ostream &operator<<(std::ostream &out, const Keyword keyword) {
  return out << KEYWORD_S[keyword];
}

static std::ostream &operator<<(std::ostream &out, const Token *token) {
  if (token->type == TKEYWORD) {
    return out << token->type << std::string(" ") << token->keyword;
  } else if (token->type == TIDENT) {
    return out << token->type << std::string(" ") << *token->sval;
  } else {
    return out << token->type;
  }
}
