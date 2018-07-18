#include "holang.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace holang;

static map<string, Keyword> keywords;

void holang::init_keywords() {
  keywords["+"] = Keyword::ADD;
  keywords["-"] = Keyword::SUB;
  keywords["*"] = Keyword::MUL;
  keywords["/"] = Keyword::DIV;
  keywords["%"] = Keyword::MOD;
  keywords["<"] = Keyword::LT;
  keywords[">"] = Keyword::GT;
  keywords["="] = Keyword::ASSIGN;
  keywords["true"] = Keyword::TRUE;
  keywords["false"] = Keyword::FALSE;
  keywords["if"] = Keyword::IF;
  keywords["else"] = Keyword::ELSE;
  keywords["{"] = Keyword::BRACEL;
  keywords["}"] = Keyword::BRACER;
  keywords["("] = Keyword::PARENL;
  keywords[")"] = Keyword::PARENR;
  keywords[","] = Keyword::COMMA;
  keywords["."] = Keyword::DOT;
  keywords["func"] = Keyword::FUNC;
  keywords["class"] = Keyword::CLASS;
  keywords["import"] = Keyword::IMPORT;
}

int code_head;
string code_str;
int line;
int line_head;
int t_head;

char nextc() { return code_str[code_head]; }

char readc() { return code_str[code_head++]; }

void unreadc() { code_head--; }

Token *make_number(const string &sval) {
  string *str = new string(sval);
  return new Token({TokenType::NUMBER, .sval = str});
}

Token *make_keyword(Keyword keyword) {
  return new Token({TokenType::KEYWORD, .keyword = keyword});
}

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
  string *sval = new string(ident);
  return new Token({TokenType::IDENT, .sval = sval});
}

Token *read_ident(char c) {
  string sval(1, c);
  while (true) {
    char c = readc();
    if (!(isalnum(c) || c == '_')) {
      unreadc();
      auto k = keywords.find(sval);
      if (k != keywords.end())
        return make_keyword(k->second);
      else
        return make_ident(sval);
    }
    sval.push_back(c);
  }
}

Token *make_str(const string &str) {
  string *sval = new string(str);
  return new Token({TokenType::STRING, .sval = sval});
}

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

Token *make_eof() { return new Token({TokenType::TEOF}); }

Token *make_newline() { return new Token({TokenType::NEWLINE}); }

void invalid(char c) {
  cerr << "unexpected character:" << c << endl;
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

Token *take_token() {
  skip_blank();
  char c = readc();
  switch (c) {
  case '0' ... '9':
    return read_number(c);
  case '"':
    return read_str();
  case '+':
    return make_keyword(Keyword::ADD);
  case '-':
    return make_keyword(Keyword::SUB);
  case '*':
    return make_keyword(Keyword::MUL);
  case '/':
    return make_keyword(Keyword::DIV);
  case '%':
    return make_keyword(Keyword::MOD);
  case '<':
    return make_keyword(Keyword::LT);
  case '>':
    return make_keyword(Keyword::GT);
  case '=':
    return make_keyword(Keyword::ASSIGN);
  case '(':
    return make_keyword(Keyword::PARENL);
  case ')':
    return make_keyword(Keyword::PARENR);
  case '{':
    return make_keyword(Keyword::BRACEL);
  case '}':
    return make_keyword(Keyword::BRACER);
  case ',':
    return make_keyword(Keyword::COMMA);
  case '.':
    return make_keyword(Keyword::DOT);
  case '\n':
    line++;
    line_head = code_head;
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
