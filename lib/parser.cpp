#include "holang/parser.h"
#include "holang.h"
#include <iostream>
#include <map>
#include <vector>

using namespace std;
using namespace holang;

static vector<Token *> token_chain;
// static vector<string> local_idents;
int token_head;

#define INVALID(pos)                                                           \
  cerr << "unexpected token at " #pos << endl;                                 \
  exit(1)

class LocalIdentTable {
  using IdentTable = vector<string>;
  vector<IdentTable> tables;

public:
  LocalIdentTable() : tables(1) { tables.back().push_back("self"); }
  int get(const string &ident) {
    auto &local_idents = tables.back();
    auto iter = find(local_idents.begin(), local_idents.end(), ident);
    if (iter == local_idents.end()) {
      local_idents.push_back(ident);
      return local_idents.size() - 1;
    } else {
      return distance(local_idents.begin(), iter);
    }
  }
  void next() { tables.push_back({"self"}); }
  void prev() { tables.pop_back(); }
  int size() { return tables.back().size(); }
};

static LocalIdentTable local_ident_table;

int size_local_idents() { return local_ident_table.size(); }

// int get_stack_position(const string &ident) {
//   auto iter = find(local_idents.begin(), local_idents.end(), ident);
//   if (iter == local_idents.end()) {
//     local_idents.push_back(ident);
//     return local_idents.size() - 1;
//   } else {
//     return distance(local_idents.begin(), iter);
//   }
// }

void print_code(const vector<Code> &codes);

void print_offset(int offset) {
  for (int i = 0; i < offset; i++) {
    cout << ".  ";
  }
}

struct IntLiteralNode : public Node {
  int value;
  IntLiteralNode(int value) : Node(AST_INT_LITERAL), value(value){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "IntLiteral " << value << "" << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = OP_LOAD_INT});
    codes->push_back({.ival = value});
  }
};

const string BOOL_S[] = {
    "false",
    "true",
};

struct BoolLiteralNode : public Node {
  bool value;
  BoolLiteralNode(bool value) : Node(AST_BOOL_LITERAL), value(value){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "BoolLiteral " << BOOL_S[value] << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = OP_LOAD_BOOL});
    codes->push_back({.bval = value});
  }
};

struct StringLiteralNode : public Node {
  string str;
  StringLiteralNode(const string &str) : Node(AST_BOOL_LITERAL), str(str){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "StringLiteral \"" << str << "\"" << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = OP_LOAD_STRING});
    codes->push_back({.sval = &str});
  }
};

struct IdentNode : public Node {
  string ident;
  IdentNode(const string &ident) : Node(AST_IDENT), ident(ident) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Ident " << ident << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    // cout << ident << ":" << local_ident_table.get(ident) << endl;
    codes->push_back({.op = OP_LOCAL_LOAD});
    codes->push_back({.ival = local_ident_table.get(ident)});
  }
};

OpCode to_opcode(char c) {
  switch (c) {
  case KADD:
    return OP_ADD;
  case KSUB:
    return OP_SUB;
  case KMUL:
    return OP_MUL;
  case KDIV:
    return OP_DIV;
  case KLT:
    return OP_LESS;
  case KGT:
    return OP_GREATER;
  default:
    cerr << "err: to_opecode" << endl;
    exit(1);
  }
}

struct BinopNode : public Node {
  Keyword op;
  Node *lhs, *rhs;
  BinopNode(Keyword op, Node *lhs, Node *rhs)
      : Node(AST_BINOP), op(op), lhs(lhs), rhs(rhs) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Binop " << KEYWORD_S[op] << endl;
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
  AssignNode(IdentNode *ident, Node *rhs)
      : Node(AST_ASSIGN), lhs(ident), rhs(rhs) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Assign " << lhs->ident << endl;
    rhs->print(offset + 1);
  }
  virtual void code_gen(vector<Code> *codes) {
    rhs->code_gen(codes);
    // cout << lhs->ident << ":" << local_ident_table.get(lhs->ident) << endl;
    codes->push_back({.op = OP_LOCAL_STORE});
    codes->push_back({.ival = local_ident_table.get(lhs->ident)});
  }
};

struct ExprsNode : public Node {
  Node *current;
  Node *next;
  int num;
  ExprsNode(Node *current, Node *next, int num)
      : Node(AST_STMTS), current(current), next(next), num(num) {}
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
  StmtsNode(Node *current, Node *next)
      : Node(AST_STMTS), current(current), next(next) {}
  virtual void print(int offset) {
    current->print(offset);
    next->print(offset);
  }
  virtual void code_gen(vector<Code> *codes) {
    current->code_gen(codes);
    codes->push_back({OP_POP});
    next->code_gen(codes);
  }
};

struct IfNode : public Node {
  Node *cond, *then, *els;
  IfNode(Node *cond, Node *then, Node *els)
      : Node(AST_IF), cond(cond), then(then), els(els) {}
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
    codes->push_back({OP_JUMP_IFNOT});
    codes->push_back({.ival = 0}); // dummy
    then->code_gen(codes);

    int from_then = codes->size() + 1;
    codes->push_back({OP_JUMP});
    codes->push_back({.ival = 0}); // dummy

    int to_else = codes->size();
    if (els == nullptr) {
      // nilの概念ができたらnilにする
      codes->push_back({OP_LOAD_INT});
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
  Node *self;
  FuncCallNode(const string &name, const vector<Node *> &args)
      : Node(AST_STMTS), name(name), args(args), self(nullptr) {}
  FuncCallNode(const string &name, Node *self, const vector<Node *> &args)
      : Node(AST_STMTS), name(name), args(args), self(self) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Call " << name << endl;
    for (const auto &arg : args) {
      arg->print(offset + 1);
    }
  }
  virtual void code_gen(vector<Code> *codes) {
    if (self != nullptr) {
      self->code_gen(codes);
    } else {
      codes->push_back({OP_PUT_SELF});
    }

    for (Node *arg : args) {
      arg->code_gen(codes);
    }
    codes->push_back({OP_CALL_FUNC});
    codes->push_back({.sval = &name});
    codes->push_back({.ival = (int)args.size()});
  }
};

struct FuncDefNode : public Node {
  string name;
  vector<string> args;
  Node *body;
  FuncDefNode(const string &name, const vector<string> &args, Node *body)
      : Node(AST_STMTS), name(name), args(args), body(body) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "FuncDef " << name << endl;
    body->print(offset + 1);
  }
  virtual void code_gen(vector<Code> *codes) {
    vector<Code> body_code;
    local_ident_table.next();

    for (const string &arg : args) {
      local_ident_table.get(arg);
    }
    body->code_gen(&body_code);
    // cout << "def:" << body_code.size() << endl;
    body_code.push_back({OP_RET});
    codes->push_back({OP_DEF_FUNC});
    codes->push_back({.sval = &name});
    codes->push_back({.objval = (Object *)new Func(body_code)});
    // set_func(name, new Func(body_code));
    local_ident_table.prev();

    // print_code(body_code);

    // codes->push_back({OP_LOAD_BOOL});
    // codes->push_back({.bval = true});
  }
};

struct KlassDefNode : public Node {
  string name;
  Node *body;
  KlassDefNode(const string &name, Node *body)
      : Node(AST_STMTS), name(name), body(body) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "KlassDef " << name << endl;
    body->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({OP_LOAD_CLASS});
    codes->push_back({.sval = &name});

    body->code_gen(codes);

    codes->push_back({OP_PREV_ENV});
  }
};

void Parser::take(TokenType type) {
  Token *token = get();
  if (token->type != type) {
    exit_by_unexpected(type, token);
  }
}

void Parser::take(Keyword keyword) {
  Token *token = get();
  if (token->type != TKEYWORD || token->keyword != keyword) {
    exit_by_unexpected(keyword, token);
  }
}

void Parser::consume_newlines() {
  while (next_token(TNEWLINE)) {
  }
}

Node *Parser::read_number() {
  Token *token = get();
  int val = stoi(*token->sval);
  return new IntLiteralNode(val);
}

Node *Parser::read_string() {
  Token *token = get();
  return new StringLiteralNode(*token->sval);
}

void Parser::read_exprs(vector<Node *> &args) {
  args.push_back(read_expr());
  while (next_token(KCOMMA)) {
    args.push_back(read_expr());
  }
}

Node *Parser::read_funccall() {
  Token *ident = get();
  if (next_token(KPARENL)) {
    vector<Node *> args;
    if (!next_token(KPARENR)) {
      read_exprs(args);
      take(KPARENR);
    }
    return new FuncCallNode(*ident->sval, args);
  } else {
    return new IdentNode(*ident->sval);
  }
}

Node *Parser::read_prime() {
  Node *node;
  if (is_next(TNUMBER)) {
    node = read_number();
  } else if (is_next(TIDENT)) {
    node = read_funccall();
  } else if (next_token(KTRUE)) {
    node = new BoolLiteralNode(true);
  } else if (next_token(KFALSE)) {
    node = new BoolLiteralNode(false);
  } else if (is_next(TSTRING)) {
    node = read_string();
  } else {
    exit_by_unexpected("something prime", get());
  }

  while (next_token(KDOT)) {
    Token *token = get();
    vector<Node *> args;
    // should(token, TIDENT);
    take(KPARENL);
    // read_exprs(args);
    take(KPARENR);

    if (is_next(KBRACEL)) {
      Node *block = read_block();
    }
    node = new FuncCallNode(*token->sval, node, args);
  }
  return node;
}

Node *ast_binop(Keyword op, Node *lhs, Node *rhs) {
  return new BinopNode(op, lhs, rhs);
}

Node *Parser::read_multiplicative_expr() {
  Node *node = read_prime();
  while (true) {
    if (next_token(KMUL))
      node = ast_binop(KMUL, node, read_prime());
    else if (next_token(KDIV))
      node = ast_binop(KDIV, node, read_prime());
    else
      return node;
  }
}

Node *Parser::read_additive_expr() {
  Node *node = read_multiplicative_expr();
  while (true) {
    if (next_token(KADD))
      node = ast_binop(KADD, node, read_multiplicative_expr());
    else if (next_token(KSUB))
      node = ast_binop(KSUB, node, read_multiplicative_expr());
    else
      return node;
  }
}

Node *Parser::read_comp_expr() {
  Node *node = read_additive_expr();
  if (next_token(KLT))
    return ast_binop(KLT, node, read_additive_expr());
  else if (next_token(KGT))
    return ast_binop(KGT, node, read_additive_expr());
  else
    return node;
}

Node *Parser::read_assignment_expr() {
  Token *token = get();
  if (token->type == TIDENT && next_token(KASSIGN)) {
    return new AssignNode(new IdentNode(*token->sval), read_assignment_expr());
  }
  unget();
  return read_comp_expr();
}

Node *Parser::read_expr() { return read_assignment_expr(); }

Node *Parser::read_funcdef() {
  Token *ident = get();
  vector<string> args;
  take(KPARENL);
  if (!next_token(KPARENR)) {
    Token *token = get_ident();
    args.push_back(*token->sval);
    while (is_next(KCOMMA)) {
      token = get_ident();
      args.push_back(*token->sval);
    }
    take(KPARENR);
  }
  Node *body = read_stmt();
  return new FuncDefNode(*ident->sval, args, body);
}

Node *Parser::read_klassdef() {
  Token *ident = get();
  vector<string> args;
  take(KBRACEL);
  consume_newlines();
  Node *body = read_stmts();
  consume_newlines();
  take(KBRACER);
  consume_newlines();
  return new KlassDefNode(*ident->sval, body);
}

Node *Parser::read_block() {
  take(KBRACEL);
  Node *node = read_stmts();
  take(KBRACER);
  return node;
}

Node *Parser::read_if() {
  Node *node = read_assignment_expr();
  Node *then = read_stmt();
  Node *els = next_token(KELSE) ? consume_newlines(), read_stmt() : nullptr;
  return new IfNode(node, then, els);
}

Node *Parser::read_stmt() {
  Node *node;
  if (next_token(KIF)) {
    node = read_if();
  } else if (next_token(KBRACEL)) {
    consume_newlines();
    node = read_stmts();
    consume_newlines();
    take(KBRACER);
  } else if (next_token(KFUNC)) {
    node = read_funcdef();
  } else if (next_token(KCLASS)) {
    node = read_klassdef();
  } else {
    node = read_expr();
  }
  consume_newlines();
  return node;
}

Node *Parser::read_stmts() {
  Node *node = read_stmt();
  while (!(is_eof() || is_next(KBRACER))) {
    node = new StmtsNode(node, read_stmt());
  }
  return node;
}

Node *Parser::read_toplevel() {
  Node *node = read_stmts();
  take(TEOF);
  return node;
}

Node *Parser::parse() {
  if (is_eof()) {
    exit(0);
  }

  Node *root = read_toplevel();
  return root;
}

void print_code(const vector<Code> &codes) {
  printf("---------- code ----------\n");
  for (int i = 0; i < codes.size(); i) {
    printf("%2d: ", i);
    auto op = codes[i++].op;
    switch (op) {
    case OP_PUT_ENV:
      printf("%s %d\n", OPCODE_S[op].c_str(), codes[i++].ival);
      break;
    case OP_LOAD_INT:
      printf("%s %d\n", OPCODE_S[op].c_str(), codes[i++].ival);
      break;
    case OP_LOAD_BOOL:
      printf("%s %d\n", OPCODE_S[op].c_str(), codes[i++].bval);
      break;
    case OP_LOAD_STRING:
      printf("%s %s\n", OPCODE_S[op].c_str(), codes[i++].sval->c_str());
      break;
    case OP_POP:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_LESS:
    case OP_GREATER:
      printf("%s\n", OPCODE_S[op].c_str());
      break;
    case OP_LOCAL_STORE:
      printf("%s %d\n", OPCODE_S[op].c_str(), codes[i++].ival);
      break;
    case OP_LOCAL_LOAD:
      printf("%s %d\n", OPCODE_S[op].c_str(), codes[i++].ival);
      break;
    case OP_PUT_SELF:
      printf("%s\n", OPCODE_S[op].c_str());
      break;
    case OP_JUMP:
    case OP_JUMP_IF:
    case OP_JUMP_IFNOT:
      printf("%s %d\n", OPCODE_S[op].c_str(), codes[i++].ival);
      break;
    case OP_RET:
      printf("%s\n", OPCODE_S[op].c_str());
      break;
    case OP_DEF_FUNC:
      printf("%s %s %d\n", OPCODE_S[op].c_str(), codes[i].sval->c_str(),
             codes[i + 1].ival);
      i += 2;
      break;
    case OP_CALL_FUNC:
      printf("%s %s %d\n", OPCODE_S[op].c_str(), codes[i].sval->c_str(),
             codes[i + 1].ival);
      i += 2;
      break;
    default:
      std::cerr << "hogehogehogehogheog" << endl;
      exit(1);
    }
  }
  printf("---------- code ----------\n");
}
