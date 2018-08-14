#pragma once

#include "holang/code.hpp"
#include "holang/instruction.hpp"
#include "holang/object.hpp"
#include "holang/token.hpp"
#include <vector>

namespace holang {
struct Node {
  virtual void print(int offset){};
  virtual void code_gen(CodeSequence *codes) = 0;
};

using namespace std;

static void exit_by_unsupported(const string &func) {
  cerr << func << " are not supported yet." << endl;
  exit(1);
}

static void print_offset(int offset) {
  for (int i = 0; i < offset; i++) {
    cout << ".  ";
  }
}

struct IntLiteralNode : public Node {
public:
  IntLiteralNode(int value) : value(value){};
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  const int value;
};

struct BoolLiteralNode : public Node {
public:
  BoolLiteralNode(bool value) : value(value){};
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  const bool value;
};

struct StringLiteralNode : public Node {
public:
  StringLiteralNode(const string &str) : str(str){};
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  std::string str; // want to be const
};

struct IdentNode : public Node {
public:
  IdentNode(const string &ident, int index) : ident(ident), index(index) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

  // TODO
  // private:
  const std::string ident;
  const int index;
};

struct LambdaNode : public Node {
public:
  LambdaNode(const vector<string *> &params, Node *body)
      : params(params), body(body) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  vector<string *> params;
  Node *body;
};

struct BinopNode : public Node {
public:
  BinopNode(TokenType op, Node *lhs, Node *rhs) : op(op), lhs(lhs), rhs(rhs) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  TokenType op;
  Node *lhs, *rhs;
};

struct AssignNode : public Node {
public:
  AssignNode(IdentNode *ident, Node *rhs) : lhs(ident), rhs(rhs) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  IdentNode *lhs;
  Node *rhs;
};

struct ExprsNode : public Node {
public:
  ExprsNode(Node *current, Node *next, int) : current(current), next(next) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *current;
  Node *next;
};

struct StmtsNode : public Node {
public:
  StmtsNode(Node *current, Node *next) : current(current), next(next) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *current;
  Node *next;
};

struct IfNode : public Node {
public:
  IfNode(Node *cond, Node *then, Node *els)
      : cond(cond), then(then), els(els) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *cond, *then, *els;
};

struct WhileNode : public Node {
public:
  WhileNode(Node *cond, Node *body) : cond(cond), body(body) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *cond, *body;
};

struct FuncCallNode : public Node {
public:
  FuncCallNode(const string &name, const vector<Node *> &args, bool is_trailer)
      : name(name), args(args), is_trailer(is_trailer) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  string name;
  const vector<Node *> args;
  Node *block;
  bool is_trailer;
};

struct FuncDefNode : public Node {
public:
  FuncDefNode(const string &name, const vector<string *> &params, Node *body)
      : name(name), params(params), body(body) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  string name;
  vector<string *> params;
  Node *body;
};

struct KlassDefNode : public Node {
public:
  KlassDefNode(const string &name, Node *body) : name(name), body(body) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  string name;
  Node *body;
};

struct SignChangeNode : public Node {
public:
  SignChangeNode(Node *body) : body(body) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *body;
};

struct PrimeExprNode : public Node {
public:
  PrimeExprNode(Node *prime, Node *traier) : prime(prime), traier(traier) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *prime;
  Node *traier;
};

struct RefFieldNode : public Node {
public:
  RefFieldNode(const string &field) : field(field) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  string field;
};

struct ImportNode : public Node {
public:
  ImportNode(Node *module) : module(module) {}
  void print(int offset) override;
  void code_gen(CodeSequence *codes) override;

private:
  Node *module;
};
} // namespace holang
