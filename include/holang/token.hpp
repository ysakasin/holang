#pragma once

#include <ostream>

namespace holang {
enum class TokenType {
  // literal
  Integer,
  Double,
  String,
  Ident,
  True,
  False,

  // operator
  Plus,             // +
  Minus,            // -
  Mul,              // *
  Div,              // '/'
  Mod,              // %
  LessThan,         // <
  GreaterThan,      // >
  Equal,            // ==
  Assign,           // =
  NotEqual,         // !=
  Not,              // !
  PlusPlus,         // ++
  MinusMinus,       // --
  LessEqualThan,    // <=
  GreaterEqualThan, // >=
  PlusAssign,       // +=
  MinusAssign,      // -=
  MulAssign,        // *=
  DivAssign,        // /=
  AND,              // &&
  OR,               // ||

  // keyword
  If,
  Else,
  Func,
  Class,
  Import,

  // delimiter
  ParenL,     // (
  ParenR,     // )
  BraseL,     // {
  BraseR,     // }
  BracketL,   // [
  BracketR,   // ]
  Comma,      // ,
  Dot,        // .
  VertialBar, // |
  Anpersand,  // &
  NewLine,    // \n
  TEOF,       // EOF is defined by stdio.h
};

struct Token {
  TokenType type;
  union {
    uint64_t i;
    double d;
    std::string str;
  };
  int line;
  int column;

  Token(TokenType type) : type(type) {}
  Token(uint64_t i) : type(TokenType::Integer), i(i) {}
  Token(double d) : type(TokenType::Double), d(d) {}
  Token(TokenType type, const std::string &str) : type(type), str(str) {}

  ~Token() {
    if (type == TokenType::String || type == TokenType::Ident) {
      str.~basic_string();
    }
  }
};

static std::ostream &operator<<(std::ostream &out, const TokenType type) {
  switch (type) {
  // literal
  case TokenType::Integer:
    return out << "Integer";
  case TokenType::Double:
    return out << "Double";
  case TokenType::String:
    return out << "String";
  case TokenType::Ident:
    return out << "Ident";
  case TokenType::True:
    return out << "True";
  case TokenType::False:
    return out << "False";

  // operator
  case TokenType::Plus:
    return out << "Plus";
  case TokenType::Minus:
    return out << "Minus";
  case TokenType::Mul:
    return out << "Mul";
  case TokenType::Div:
    return out << "Div";
  case TokenType::Mod:
    return out << "Mod";
  case TokenType::LessThan:
    return out << "LessThan";
  case TokenType::GreaterThan:
    return out << "GreaterThan";
  case TokenType::Equal:
    return out << "Equal";
  case TokenType::Assign:
    return out << "Assign";
  case TokenType::NotEqual:
    return out << "NotEqual";
  case TokenType::Not:
    return out << "Not";
  case TokenType::PlusPlus:
    return out << "PlusPlus";
  case TokenType::MinusMinus:
    return out << "MinusMinus";
  case TokenType::LessEqualThan:
    return out << "LessEqualThan";
  case TokenType::GreaterEqualThan:
    return out << "GreaterEqualThan";
  case TokenType::PlusAssign:
    return out << "PlusAssign";
  case TokenType::MinusAssign:
    return out << "MinusAssign";
  case TokenType::MulAssign:
    return out << "MulAssign";
  case TokenType::DivAssign:
    return out << "DivAssign";
  case TokenType::AND:
    return out << "AND";
  case TokenType::OR:
    return out << "OR";

  // keyword
  case TokenType::If:
    return out << "If";
  case TokenType::Else:
    return out << "Else";
  case TokenType::Func:
    return out << "Func";
  case TokenType::Class:
    return out << "Class";
  case TokenType::Import:
    return out << "Import";

  // delimitor
  case TokenType::ParenL:
    return out << "ParenL";
  case TokenType::ParenR:
    return out << "ParenR";
  case TokenType::BraseL:
    return out << "BraseL";
  case TokenType::BraseR:
    return out << "BraseR";
  case TokenType::BracketL:
    return out << "BracketL";
  case TokenType::BracketR:
    return out << "BracketR";
  case TokenType::Comma:
    return out << "Comma";
  case TokenType::Dot:
    return out << "Dot";
  case TokenType::VertialBar:
    return out << "VertialBar";
  case TokenType::Anpersand:
    return out << "Anpersand";
  case TokenType::NewLine:
    return out << "NewLine";
  case TokenType::TEOF:
    return out << "EOF";
  }
}

static std::ostream &operator<<(std::ostream &out, const Token *token) {
  char pos[15];
  snprintf(pos, 15, "(%3d,%3d)", token->line, token->column);
  out << pos << ' ';

  if (token->type == TokenType::Integer) {
    return out << token->type << " " << token->i;
  } else if (token->type == TokenType::Double) {
    return out << token->type << " " << token->d;
  } else if (token->type == TokenType::Ident ||
             token->type == TokenType::String) {
    return out << token->type << " " << token->str;
  } else {
    return out << token->type;
  }
}
} // namespace holang
