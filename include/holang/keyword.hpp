#pragma once

#include <ostream>

namespace holang {
enum class Keyword {
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  LT,
  GT,
  EQUAL,
  ASSIGN,
  TRUE,
  FALSE,
  IF,
  ELSE,
  BRACEL,
  BRACER,
  PARENL,
  PARENR,
  COMMA,
  DOT,
  FUNC,
  CLASS,
  IMPORT,
};

static std::ostream &operator<<(std::ostream &out, const Keyword keyword) {
  switch (keyword) {
  case Keyword::ADD:
    return out << "+";
  case Keyword::SUB:
    return out << "-";
  case Keyword::MUL:
    return out << "*";
  case Keyword::DIV:
    return out << "/";
  case Keyword::MOD:
    return out << "%";
  case Keyword::LT:
    return out << "<";
  case Keyword::GT:
    return out << ">";
  case Keyword::EQUAL:
    return out << "==";
  case Keyword::ASSIGN:
    return out << "=";
  case Keyword::TRUE:
    return out << "true";
  case Keyword::FALSE:
    return out << "false";
  case Keyword::IF:
    return out << "if";
  case Keyword::ELSE:
    return out << "else";
  case Keyword::BRACEL:
    return out << "{";
  case Keyword::BRACER:
    return out << "}";
  case Keyword::PARENL:
    return out << "(";
  case Keyword::PARENR:
    return out << ")";
  case Keyword::COMMA:
    return out << ",";
  case Keyword::DOT:
    return out << ".";
  case Keyword::FUNC:
    return out << "func";
  case Keyword::CLASS:
    return out << "class";
  case Keyword::IMPORT:
    return out << "import";
  }
}
} // namespace holang
