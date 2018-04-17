#pragma once

#include "holang/code.hpp"
#include "holang/instruction.hpp"
#include "holang/keyword.hpp"
#include "holang/object.hpp"
#include <vector>

namespace holang {
struct Node {
  virtual void print(int offset){};
  virtual void code_gen(std::vector<Code> *codes) = 0;
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
  int value;
  IntLiteralNode(int value) : value(value){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "IntLiteral " << value << "" << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = Instruction::PUT_INT});
    codes->push_back({.ival = value});
  }
};

static constexpr const char *bool2s(const bool val) {
  return val ? "true" : "false";
}

struct BoolLiteralNode : public Node {
  bool value;
  BoolLiteralNode(bool value) : value(value){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "BoolLiteral " << bool2s(value) << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = Instruction::PUT_BOOL});
    codes->push_back({.bval = value});
  }
};

struct StringLiteralNode : public Node {
  string str;
  StringLiteralNode(const string &str) : str(str){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "StringLiteral \"" << str << "\"" << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = Instruction::PUT_STRING});
    codes->push_back({.sval = &str});
  }
};

struct IdentNode : public Node {
  string ident;
  int index;
  IdentNode(const string &ident, int index) : ident(ident), index(index) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Ident " << ident << " : " << index << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = Instruction::LOAD_LOCAL});
    codes->push_back({.ival = index});
  }
};

static Instruction to_opcode(Keyword c) {
  switch (c) {
  case Keyword::ADD:
    return Instruction::ADD;
  case Keyword::SUB:
    return Instruction::SUB;
  case Keyword::MUL:
    return Instruction::MUL;
  case Keyword::DIV:
    return Instruction::DIV;
  case Keyword::LT:
    return Instruction::LESS;
  case Keyword::GT:
    return Instruction::GREATER;
  default:
    cerr << "err: to_opecode" << endl;
    exit(1);
  }
}

struct BinopNode : public Node {
  Keyword op;
  Node *lhs, *rhs;
  BinopNode(Keyword op, Node *lhs, Node *rhs) : op(op), lhs(lhs), rhs(rhs) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Binop " << op << endl;
    lhs->print(offset + 1);
    rhs->print(offset + 1);
  }
  virtual void code_gen(vector<Code> *codes) {
    lhs->code_gen(codes);
    rhs->code_gen(codes);
    codes->push_back({.op = to_opcode(op)});
  }
};

struct AssignNode : public Node {
  IdentNode *lhs;
  Node *rhs;
  AssignNode(IdentNode *ident, Node *rhs) : lhs(ident), rhs(rhs) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Assign " << lhs->ident << " : " << lhs->index << endl;
    rhs->print(offset + 1);
  }
  virtual void code_gen(vector<Code> *codes) {
    rhs->code_gen(codes);
    codes->push_back({.op = Instruction::STORE_LOCAL});
    codes->push_back({.ival = lhs->index});
  }
};

struct ExprsNode : public Node {
  Node *current;
  Node *next;
  int num;
  ExprsNode(Node *current, Node *next, int num)
      : current(current), next(next), num(num) {}
  virtual void print(int offset) {
    current->print(offset);
    next->print(offset);
  }
  virtual void code_gen(vector<Code> *codes) {
    current->code_gen(codes);
    next->code_gen(codes);
  }
};

struct StmtsNode : public Node {
  Node *current;
  Node *next;
  StmtsNode(Node *current, Node *next) : current(current), next(next) {}
  virtual void print(int offset) {
    current->print(offset);
    next->print(offset);
  }
  virtual void code_gen(vector<Code> *codes) {
    current->code_gen(codes);
    codes->push_back({Instruction::POP});
    next->code_gen(codes);
  }
};

struct IfNode : public Node {
  Node *cond, *then, *els;
  IfNode(Node *cond, Node *then, Node *els)
      : cond(cond), then(then), els(els) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "if" << endl;
    cond->print(offset + 1);

    print_offset(offset);
    cout << "then " << endl;
    then->print(offset + 1);

    if (els != nullptr) {
      print_offset(offset);
      cout << "else" << endl;
      els->print(offset + 1);
    }
  }
  virtual void code_gen(vector<Code> *codes) {
    cond->code_gen(codes);

    int from_if = codes->size() + 1;
    codes->push_back({Instruction::JUMP_IFNOT});
    codes->push_back({.ival = 0}); // dummy
    then->code_gen(codes);

    int from_then = codes->size() + 1;
    codes->push_back({Instruction::JUMP});
    codes->push_back({.ival = 0}); // dummy

    int to_else = codes->size();
    if (els == nullptr) {
      // nilの概念ができたらnilにする
      codes->push_back({Instruction::PUT_INT});
      codes->push_back({.ival = 0});
    } else {
      els->code_gen(codes);
    }
    int to_end = codes->size();

    codes->at(from_if).ival = to_else;
    codes->at(from_then).ival = to_end;
  }
};

struct FuncCallNode : public Node {
  string name;
  vector<Node *> args;
  bool is_trailer;
  FuncCallNode(const string &name, const vector<Node *> &args, bool is_trailer)
      : name(name), args(args), is_trailer(is_trailer) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Call " << name << endl;
    for (const auto &arg : args) {
      arg->print(offset + 1);
    }
  }
  virtual void code_gen(vector<Code> *codes) {
    if (!is_trailer) {
      codes->push_back({Instruction::PUT_SELF});
    }

    for (Node *arg : args) {
      arg->code_gen(codes);
    }
    codes->push_back({Instruction::CALL_FUNC});
    codes->push_back({.sval = &name});
    codes->push_back({.ival = (int)args.size()});
  }
};

struct FuncDefNode : public Node {
  string name;
  vector<string *> params;
  Node *body;
  FuncDefNode(const string &name, const vector<string *> &params, Node *body)
      : name(name), params(params), body(body) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "FuncDef " << name << endl;
    body->print(offset + 1);
  }
  virtual void code_gen(vector<Code> *codes) {
    vector<Code> body_code;

    body->code_gen(&body_code);
    body_code.push_back({Instruction::RET});
    codes->push_back({Instruction::DEF_FUNC});
    codes->push_back({.sval = &name});
    codes->push_back({.objval = (Object *)new Func(body_code)});
  }
};

struct KlassDefNode : public Node {
  string name;
  Node *body;
  KlassDefNode(const string &name, Node *body) : name(name), body(body) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "KlassDef " << name << endl;
    body->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({Instruction::LOAD_CLASS});
    codes->push_back({.sval = &name});

    body->code_gen(codes);

    codes->push_back({Instruction::PREV_ENV});
  }
};

struct SignChangeNode : public Node {
  Node *body;
  SignChangeNode(Node *body) : body(body) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "SignChange" << endl;
    body->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    exit_by_unsupported("SignChangeNode#code_gen()");
  }
};

struct PrimeExprNode : public Node {
  Node *prime;
  Node *traier;
  PrimeExprNode(Node *prime, Node *traier) : prime(prime), traier(traier) {}
  virtual void print(int offset) {
    prime->print(offset);
    traier->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    prime->code_gen(codes);
    traier->code_gen(codes);
  }
};

struct RefFieldNode : public Node {
  string *field;
  RefFieldNode(string *field) : field(field) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "." << *field << endl;
  }

  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({Instruction::LOAD_OBJ_FIELD});
    codes->push_back({.sval = field});
  }
};

struct ImportNode : public Node {
  Node *module;
  ImportNode(Node *module) : module(module) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Import" << endl;
    module->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    module->code_gen(codes);
    codes->push_back({Instruction::IMPORT});
  }
};
} // namespace holang
