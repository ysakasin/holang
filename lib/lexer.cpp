#include "holang.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace holang;

static map<string, TokenType> keywords;

void holang::init_keywords() {
  keywords["true"] = TokenType::True;
  keywords["false"] = TokenType::False;
  keywords["if"] = TokenType::If;
  keywords["else"] = TokenType::Else;
  keywords["func"] = TokenType::Func;
  keywords["class"] = TokenType::Class;
  keywords["import"] = TokenType::Import;
}

int code_head;
string code_str;
int line;
int line_head;
int t_head;

bool is_next(char c) {
  if (code_str[code_head] == c) {
    code_head++;
    return true;
  } else {
    return false;
  }
}

char nextc() { return code_str[code_head]; }

char readc() { return code_str[code_head++]; }

void unreadc() { code_head--; }

Token *make_number(const string &sval) {
  uint64_t i = stoi(sval);
  return new Token(i);
}

Token *make_token(TokenType type) { return new Token(type); }

Token *read_number(char c) {
  string sval(1, c);
  while (true) {
    char c = readc();
    if (!isdigit(c)) {
      unreadc();
      return make_number(sval);
    }
    sval.push_back(c);
  }
}

Token *make_ident(const string &ident) {
  return new Token(TokenType::Ident, ident);
}

Token *read_ident(char c) {
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

Token *read_str() {
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

void invalid(char c) {
  cerr << "unexpected character: " << c;
  cerr << " at line " << line << ", col " << code_head - line_head << endl;
  exit(1);
}

bool isblank(char c) { return c == ' ' || c == '\t'; }

void skip_blank() {
  char c = readc();
  while (isblank(c)) {
    t_head++;
    c = readc();
  }
  unreadc();
}

void skip_comment() {
  while (readc() != '\n') {
  }
  line++;
  line_head = code_head;
}

void skip_blank_lines() {
  while (true) {
    char c = readc();
    if (isblank(c)) {
    } else if (c == '\n') {
      line++;
      line_head = code_head;
    } else if (c == '#') {
      skip_comment();
    } else {
      break;
    }
  }
  unreadc();
}

Token *take_token() {
  skip_blank();
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
    line_head = code_head;
    skip_blank_lines();
    return make_newline();
  case '#':
    skip_comment();
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

int holang::lex(const string &code, vector<Token *> &token_chain) {
  line = 1;
  code_str = code;
  code_head = 0;
  while (code_head < code.length() + 1) {
    Token *token = take_token();
    token->line = line;
    token->column = t_head - line_head + 1;
    token_chain.push_back(token);
    t_head = code_head;
  }
  return 0;
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
