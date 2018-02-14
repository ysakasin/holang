#include "./holang.hh"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

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
  LocalIdentTable() : tables(1) {}
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
  void next() { tables.push_back(vector<string>()); }
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

struct IntLiteralNode : public Node {
  int value;
  IntLiteralNode(int value) : Node(AST_INT_LITERAL), value(value){};
  virtual void print() { cout << "(IntLiteral " << value << ")"; }
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
  virtual void print() { cout << "(BoolLiteral " << BOOL_S[value] << ")"; }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = OP_LOAD_BOOL});
    codes->push_back({.bval = value});
  }
};

struct IdentNode : public Node {
  string ident;
  IdentNode(const string &ident) : Node(AST_IDENT), ident(ident) {}
  virtual void print() { cout << "(Ident " << ident << ")"; }
  virtual void code_gen(vector<Code> *codes) {
    cout << ident << ":" << local_ident_table.get(ident) << endl;
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
  virtual void print() {
    cout << "(";
    lhs->print();
    cout << KEYWORD_S[op];
    rhs->print();
    cout << ")";
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
  virtual void print() {
    cout << "(Assign " << lhs->ident << ", ";
    rhs->print();
    cout << ")";
  }
  virtual void code_gen(vector<Code> *codes) {
    rhs->code_gen(codes);
    cout << lhs->ident << ":" << local_ident_table.get(lhs->ident) << endl;
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
  virtual void print() {
    current->print();
    cout << endl;
    next->print();
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
  virtual void print() {
    current->print();
    cout << endl;
    next->print();
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
  virtual void print() {
    cout << "(If ";
    cond->print();
    cout << ", then ";
    then->print();
    cout << ", else ";
    if (els != nullptr)
      els->print();
    cout << ")";
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

    int to_else = codes->size() - 1;
    if (els == nullptr) {
      // nilの概念ができたらnilにする
      codes->push_back({OP_LOAD_INT});
      codes->push_back({.ival = 0});
    } else {
      els->code_gen(codes);
    }
    int to_end = codes->size() - 1;

    codes->at(from_if).ival = to_else;
    codes->at(from_then).ival = to_end;
  }
};

struct FuncCallNode : public Node {
  string name;
  vector<Node *> args;
  FuncCallNode(const string &name, const vector<Node *> &args)
      : Node(AST_STMTS), name(name), args(args) {}
  virtual void print() {
    cout << "(Call " << name << " ";
    for (const auto &arg : args) {
      arg->print();
      cout << ",";
    }
    cout << ")";
  }
  virtual void code_gen(vector<Code> *codes) {
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
  virtual void print() {
    cout << "(FuncDef " << name << " ";
    body->print();
    cout << ")";
  }
  virtual void code_gen(vector<Code> *codes) {
    vector<Code> body_code;
    local_ident_table.next();

    for (const string &arg : args) {
      local_ident_table.get(arg);
    }
    body->code_gen(&body_code);
    cout << "def:" << body_code.size() << endl;
    body_code.push_back({OP_RET});
    set_func(name, new Func(body_code));
    local_ident_table.prev();

    codes->push_back({OP_LOAD_BOOL});
    codes->push_back({.bval = true});
  }
};

void should(Node *node, NodeType type) {
  if (node->type != type) {
    cerr << "unexpected type _ should" << endl;
    exit(1);
  }
}
void should(Token *token, TokenType type) {
  if (token->type != type) {
    INVALID(should_token);
  }
}
bool expect(TokenType type) { return token_chain[token_head]->type == type; }
bool expect(Keyword keyword) {
  Token *token = token_chain[token_head];
  return token->type == TKEYWORD && token->keyword == keyword;
}
Token *get() { return token_chain[token_head++]; }
Token *get(TokenType type) {
  Token *token = get();
  should(token, type);
  return token;
}
void unget() { token_head--; }

void take(TokenType type) {
  Token *token = get();
  if (token->type != type) {
    cerr << "unexpected token" << endl;
    exit(1);
  }
}

void take_keyword(Keyword keyword) {
  Token *token = get();
  if (token->type != TKEYWORD || token->keyword != keyword) {
    cerr << endl;
    cerr << keyword << endl;
    print_token(token);
    INVALID(token);
  }
}

bool next_token(Keyword keyword) {
  Token *token = get();
  if (token->type == TKEYWORD && token->keyword == keyword) {
    return true;
  }
  unget();
  return false;
}

Node *read_number() {
  Token *token = get();
  int val = stoi(*token->sval);
  return new IntLiteralNode(val);
}

// Node *read_ident() {
//   Token *token = get();
//   return new IdentNode(*token->sval);
// }

Node *read_expr();

void read_exprs(vector<Node *> &args) {
  args.push_back(read_expr());
  while (next_token(KCOMMA)) {
    args.push_back(read_expr());
  }
}

Node *read_funccall() {
  Token *ident = get();
  if (next_token(KPARENL)) {
    vector<Node *> args;
    if (!next_token(KPARENR)) {
      read_exprs(args);
      take_keyword(KPARENR);
    }
    return new FuncCallNode(*ident->sval, args);
  } else {
    return new IdentNode(*ident->sval);
  }
}

Node *read_prime() {
  if (expect(TNUMBER))
    return read_number();
  else if (expect(TIDENT)) {
    return read_funccall();
  } else if (next_token(KTRUE))
    return new BoolLiteralNode(true);
  else if (next_token(KFALSE))
    return new BoolLiteralNode(false);
  else {
    cout << endl;
    print_token(get());
    INVALID(read_prime);
  }
}

Node *ast_binop(Keyword op, Node *lhs, Node *rhs) {
  return new BinopNode(op, lhs, rhs);
}

Node *read_multiplicative_expr() {
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

Node *read_additive_expr() {
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

Node *read_comp_expr() {
  Node *node = read_additive_expr();
  if (next_token(KLT))
    return ast_binop(KLT, node, read_additive_expr());
  else if (next_token(KGT))
    return ast_binop(KGT, node, read_additive_expr());
  else
    return node;
}

Node *read_assignment_expr() {
  Node *node = read_comp_expr();
  if (next_token(KASSIGN)) {
    should(node, AST_IDENT);
    node = new AssignNode((IdentNode *)node, read_assignment_expr());
  }
  return node;
}

Node *read_expr() { return read_assignment_expr(); }

void maybe(TokenType type) {
  if (expect(type))
    get();
}

Node *read_stmts();
Node *read_stmt();

Node *read_funcdef() {
  Token *ident = get();
  vector<string> args;
  take_keyword(KPARENL);
  if (!next_token(KPARENR)) {
    Token *token = get(TIDENT);
    args.push_back(*token->sval);
    while (next_token(KCOMMA)) {
      token = get(TIDENT);
      args.push_back(*token->sval);
    }
    take_keyword(KPARENR);
  }
  Node *body = read_stmt();
  return new FuncDefNode(*ident->sval, args, body);
}

Node *read_stmt() {
  maybe(TNEWLINE);
  if (next_token(KIF)) {
    Node *node = read_assignment_expr();
    maybe(TNEWLINE);
    Node *then = read_stmt();
    Node *els = next_token(KELSE) ? maybe(TNEWLINE), read_stmt() : nullptr;
    return new IfNode(node, then, els);
  } else if (next_token(KBRACEL)) {
    Node *node = read_stmts();
    maybe(TNEWLINE);
    take_keyword(KBRACER);
    maybe(TNEWLINE);
    return node;
  } else if (next_token(KFUNC)) {
    return read_funcdef();
  }
  Node *node = read_assignment_expr();
  maybe(TNEWLINE);
  return node;
}

Node *read_stmts() {
  Node *node = read_stmt();
  while (!(expect(TEOF) || expect(KBRACER))) {
    node = new StmtsNode(node, read_stmt());
  }
  return node;
}

Node *read_toplevel() {
  Node *node = read_stmts();
  take(TEOF);
  return node;
}

void print_node(Node *node) { cout << "print_node" << endl; }

Node *parse(const vector<Token *> &chain) {
  token_chain = chain;
  token_head = 0;
  if (expect(TEOF)) {
    exit(0);
  }
  // if (!expect(TNUMBER)) {
  //   cerr << "invalid parse" << endl;
  //   exit(1);
  // }
  Node *root = read_toplevel();
  root->print();
  cout << endl;
  return root;
}
