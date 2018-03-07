#pragma once

#include <functional>
#include <iostream>
#include <map>
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

class Object;

union Code {
  OpCode op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Object *objval;
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
  VOBJECT,
};

class Object;
struct Func;

struct Value {
  ValueType type;
  union {
    int ival;
    double dval;
    bool bval;
    Object *objval;
  };

  Value() {}
  Value(int i) : type(VINT), ival(i) {}
  Value(double d) : type(VDOUBLE), dval(d) {}
  Value(bool b) : type(VBOOL), bval(b) {}
  Value(Object *obj) : type(VOBJECT), objval(obj) {}

  Func *find_method(const std::string &name);

  const std::string to_s() {
    switch (type) {
    case VINT:
      return std::to_string(ival);
    case VBOOL:
      return bval ? "true" : "false";
    case VDOUBLE:
      return std::to_string(dval);
    case VOBJECT:
      return "<Object>";
    }
  }
};

enum FuncType {
  FBUILTIN,
  FUSERDEF,
};

using NativeFunc = std::function<Value(Value *, Value *, int)>;

struct Func {
  FuncType type;
  NativeFunc native;
  std::vector<Code> body;

  // Func() {}
  Func(const Func &func)
      : type(func.type), native(func.native), body(func.body) {}
  Func(NativeFunc native) : type(FBUILTIN), native(native) {}
  Func(const std::vector<Code> &body) : type(FUSERDEF), body(body) {}
};

#include "./object.hh"

void print_token(const Token *token);
Node *parse(const std::vector<Token *> &chain);
void eval(std::vector<Code> *codes);
int lex(const std::string &code, std::vector<Token *> &token_chain);
void init_keywords();
void print_token_chain(const std::vector<Token *> &token_chain);
int size_local_idents();
void set_func(const std::string &name, Func *func);
