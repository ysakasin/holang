#include "holang.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

static map<string, Keyword> keywords;

#define keyword(name, word) keywords[#word] = name;
void init_keywords() {
#include "holang/keyword.inc"
}
#undef keyword

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
  return new Token({TNUMBER, .sval = str});
}

Token *make_keyword(Keyword keyword) {
  return new Token({TKEYWORD, .keyword = keyword});
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
  return new Token({TIDENT, .sval = sval});
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
  return new Token({TSTRING, .sval = sval});
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

Token *make_eof() { return new Token({TEOF}); }

Token *make_newline() { return new Token({TNEWLINE}); }

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
    return make_keyword(KADD);
  case '-':
    return make_keyword(KSUB);
  case '*':
    return make_keyword(KMUL);
  case '/':
    return make_keyword(KDIV);
  case '<':
    return make_keyword(KLT);
  case '>':
    return make_keyword(KGT);
  case '=':
    return make_keyword(KASSIGN);
  case '(':
    return make_keyword(KPARENL);
  case ')':
    return make_keyword(KPARENR);
  case '{':
    return make_keyword(KBRACEL);
  case '}':
    return make_keyword(KBRACER);
  case ',':
    return make_keyword(KCOMMA);
  case '.':
    return make_keyword(KDOT);
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

int lex(const string &code, vector<Token *> &token_chain) {
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
    print_token(token);
  }
}
