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
