#include "holang/lexer.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace holang;

map<string, TokenType> Lexer::keywords;

void Lexer::init_keywords() {
  if (!Lexer::keywords.empty()) {
    return;
  }

  keywords["true"] = TokenType::True;
  keywords["false"] = TokenType::False;
  keywords["if"] = TokenType::If;
  keywords["else"] = TokenType::Else;
  keywords["func"] = TokenType::Func;
  keywords["class"] = TokenType::Class;
  keywords["import"] = TokenType::Import;
}

bool Lexer::is_next(char c) {
  if (code_str[head] == c) {
    head++;
    return true;
  } else {
    return false;
  }
}

char Lexer::nextc() const { return code_str[head]; }

char Lexer::readc() { return code_str[head++]; }

void Lexer::unreadc() { head--; }

Token *make_integer(const string &sval) {
  uint64_t i = stoi(sval);
  return new Token(i);
}

Token *make_double(const string &sval) {
  double d = stod(sval);
  return new Token(d);
}

Token *make_token(TokenType type) { return new Token(type); }

Token *Lexer::read_number(char c) {
  string sval(1, c);
  bool has_dot = false;
  while (true) {
    char c = readc();
    if (c == '.') {
      if (has_dot) {
        unreadc();
        break;
      }
      char nc = readc();
      if (isdigit(nc)) {
        sval.push_back('.');
        sval.push_back(nc);
        has_dot = true;
      } else {
        unreadc();
        unreadc();
        break;
      }
    } else if (isdigit(c)) {
      sval.push_back(c);
    } else {
      unreadc();
      break;
    }
  }

  if (has_dot) {
    return make_double(sval);
  } else {
    return make_integer(sval);
  }
}

Token *make_ident(const string &ident) {
  return new Token(TokenType::Ident, ident);
}

Token *Lexer::read_ident(char c) {
  string sval(1, c);
  while (true) {
    char c = readc();
    if (!(isalnum(c) || c == '_')) {
      unreadc();
      auto k = keywords.find(sval);
      if (k != keywords.end())
        return make_token(k->second);
      else
        return make_ident(sval);
    }
    sval.push_back(c);
  }
}

Token *make_str(const string &str) { return new Token(TokenType::String, str); }

Token *Lexer::read_str() {
  string str = "";
  while (true) {
    char c = readc();
    if (c == '"') {
      break;
    }
    str.push_back(c);
  }
  return make_str(str);
}

Token *make_eof() { return new Token(TokenType::TEOF); }

Token *make_newline() { return new Token(TokenType::NewLine); }

void Lexer::invalid(char c) const {
  cerr << "unexpected character: " << c;
  cerr << " at line " << line << ", col " << head - line_begin_at << endl;
  exit(1);
}

bool isblank(char c) { return c == ' ' || c == '\t'; }

void Lexer::skip_blank() {
  char c = readc();
  while (isblank(c)) {
    c = readc();
  }
  unreadc();
}

void Lexer::skip_to_newline() {
  while (readc() != '\n') {
  }
  line++;
  line_begin_at = head;
}

void Lexer::skip_blank_lines() {
  while (true) {
    char c = readc();
    if (isblank(c)) {
    } else if (c == '\n') {
      line++;
      line_begin_at = head;
    } else if (c == '#') {
      skip_to_newline();
    } else {
      break;
    }
  }
  unreadc();
}

Token *Lexer::take_token() {
  skip_blank();

  token_begin_at = head;
  char c = readc();
  switch (c) {
  case '0' ... '9':
    return read_number(c);
  case '"':
    return read_str();
  case '+':
    if (is_next('+')) {
      return make_token(TokenType::PlusPlus);
    } else if (is_next('=')) {
      return make_token(TokenType::PlusAssign);
    } else {
      return make_token(TokenType::Plus);
    }
  case '-':
    if (is_next('-')) {
      return make_token(TokenType::MinusMinus);
    } else if (is_next('=')) {
      return make_token(TokenType::MinusAssign);
    } else {
      return make_token(TokenType::Minus);
    }
  case '*':
    if (is_next('*')) {
      invalid(c);
    } else if (is_next('=')) {
      return make_token(TokenType::MulAssign);
    } else {
      return make_token(TokenType::Mul);
    }
  case '/':
    if (is_next('/')) {
      invalid(c);
    } else if (is_next('=')) {
      return make_token(TokenType::DivAssign);
    } else {
      return make_token(TokenType::Div);
    }
  case '%':
    return make_token(TokenType::Mod);
  case '<':
    if (is_next('=')) {
      return make_token(TokenType::LessEqualThan);
    } else {
      return make_token(TokenType::LessThan);
    }
  case '>':
    if (is_next('=')) {
      return make_token(TokenType::GreaterEqualThan);
    } else {
      return make_token(TokenType::GreaterThan);
    }
  case '=':
    if (is_next('=')) {
      return make_token(TokenType::Equal);
    } else {
      return make_token(TokenType::Assign);
    }
  case '!':
    if (is_next('=')) {
      return make_token(TokenType::NotEqual);
    } else {
      return make_token(TokenType::Not);
    }
  case '(':
    return make_token(TokenType::ParenL);
  case ')':
    return make_token(TokenType::ParenR);
  case '{':
    return make_token(TokenType::BraseL);
  case '}':
    return make_token(TokenType::BraseR);
  case '[':
    return make_token(TokenType::BracketL);
  case ']':
    return make_token(TokenType::BracketR);
  case ',':
    return make_token(TokenType::Comma);
  case '.':
    return make_token(TokenType::Dot);
  case '|':
    if (is_next('|')) {
      return make_token(TokenType::OR);
    } else {
      return make_token(TokenType::VertialBar);
    }
  case '&':
    if (is_next('&')) {
      return make_token(TokenType::AND);
    } else {
      return make_token(TokenType::Anpersand);
    }
  case '\n':
    line++;
    line_begin_at = head;
    skip_blank_lines();
    return make_newline();
  case '#':
    skip_to_newline();
    skip_blank_lines();
    return make_newline();
  case '\0':
    return make_eof();
  case 'a' ... 'z':
  case 'A' ... 'Z':
  case '_':
    return read_ident(c);
  default:
    invalid(c);
    return nullptr;
  }
}

void Lexer::lex(vector<Token *> &token_chain) {
  line = 1;
  head = 0;

  while (head <= code_str.size()) {
    Token *token = take_token();
    token->line = line;
    token->column = token_begin_at - line_begin_at + 1;
    token_chain.push_back(token);
  }
}

void print_token(const Token *token) {
  cout << token << "\t ";
  cout << "(" << token->line << "," << token->column << ")" << endl;
}

void print_token_chain(const vector<Token *> &token_chain) {
  for (const Token *token : token_chain) {
    ::print_token(token);
  }
}
