#include "./holang.hh"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

const string TOKEN_TYPE_S[] = {
    "NUMBER", "KEYWORD", "IDENT", "NEWLINE", "EOF",
};

static map<string, Keyword> keywords;

#define keyword(name, word) keywords[#word] = name;
void init_keywords() {
#include "./keyword.inc"
}
#undef keyword

int code_head;
string code_str;
int line;
int line_head;

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
  case '\n':
    line++;
    line_head = code_head - 1;
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
    token->column = code_head - line_head;
    token_chain.push_back(token);
  }
  return 0;
}

void print_token(const Token *token) {
  cout << TOKEN_TYPE_S[token->type] << "\t";
  switch (token->type) {
  case TNUMBER:
  case TIDENT:
    cout << *token->sval;
    break;
  case TKEYWORD:
    cout << KEYWORD_S[token->keyword];
    break;
  default:;
  }
  cout << "(" << token->line << "," << token->column << ")" << endl;
}

void print_token_chain(const vector<Token *> &token_chain) {
  for (const Token *token : token_chain) {
    print_token(token);
  }
}