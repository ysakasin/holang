#include <functional>
#include <string>
#include <vector>

enum TokenType {
  TNUMBER,
  TKEYWORD,
  TIDENT,
  TNEWLINE,
  TEOF,
};

#define keyword(name, _) name,
enum Keyword {
#include "./keyword.inc"
};
#undef keyword

#define keyword(_, word) #word,
const std::string KEYWORD_S[] = {
#include "./keyword.inc"
};
#undef keyword

struct Token {
  TokenType type;
  union {
    std::string *sval;
    Keyword keyword;
  };
  int line;
  int column;
};

#define opcode(name) name,
enum OpCode {
#include "./opcode.inc"
};
#undef opcode

#define opcode(name) #name,
const std::string OPCODE_S[] = {
#include "./opcode.inc"
};
#undef opcode

union Code {
  OpCode op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
};

enum NodeType {
  AST_INT_LITERAL,
  AST_BOOL_LITERAL,
  AST_BINOP,
  AST_ASSIGN,
  AST_STMTS,
  AST_IDENT,
  AST_IF,
};

struct Node {
  NodeType type;
  virtual void print(){};
  virtual void code_gen(std::vector<Code> *codes){};
  Node(NodeType type) : type(type){};
};

enum ValueType {
  VINT,
  VDOUBLE,
  VBOOL,
};

struct Value {
  ValueType type;
  union {
    int ival;
    double dval;
    bool bval;
  };

  const std::string to_s() {
    switch (type) {
    case VINT:
      return std::to_string(ival);
    case VBOOL:
      return bval ? "true" : "false";
    case VDOUBLE:
      return std::to_string(dval);
    }
  }
};

enum FuncType {
  FBUILTIN,
  FUSERDEF,
};

struct Func {
  FuncType type;
  std::function<Value(Value *, int)> native;
  std::vector<Code> body;

  // Func() {}
  Func(const Func &func)
      : type(func.type), native(func.native), body(func.body) {}
  Func(std::function<Value(Value *, int)> native)
      : type(FBUILTIN), native(native) {}
  Func(const std::vector<Code> &body) : type(FUSERDEF), body(body) {}
};

void print_token(const Token *token);
Node *parse(const std::vector<Token *> &chain);
void eval(std::vector<Code> *codes);
int lex(const std::string &code, std::vector<Token *> &token_chain);
void init_keywords();
void print_token_chain(const std::vector<Token *> &token_chain);
int size_local_idents();
void set_func(const std::string &name, Func *func);
