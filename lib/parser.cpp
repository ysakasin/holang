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

void exit_by_unsupported(const string &func) {
  cerr << func << " are not supported yet." << endl;
  exit(1);
}

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
  IntLiteralNode(int value) : value(value){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "IntLiteral " << value << "" << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = Instruction::LOAD_INT});
    codes->push_back({.ival = value});
  }
};

const string BOOL_S[] = {
    "false",
    "true",
};

struct BoolLiteralNode : public Node {
  bool value;
  BoolLiteralNode(bool value) : value(value){};
  virtual void print(int offset) {
    print_offset(offset);
    cout << "BoolLiteral " << BOOL_S[value] << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    codes->push_back({.op = Instruction::LOAD_BOOL});
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
    codes->push_back({.op = Instruction::LOAD_STRING});
    codes->push_back({.sval = &str});
  }
};

struct IdentNode : public Node {
  string ident;
  IdentNode(const string &ident) : ident(ident) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Ident " << ident << endl;
  }
  virtual void code_gen(vector<Code> *codes) {
    // cout << ident << ":" << local_ident_table.get(ident) << endl;
    codes->push_back({.op = Instruction::LOCAL_LOAD});
    codes->push_back({.ival = local_ident_table.get(ident)});
  }
};

Instruction to_opcode(char c) {
  switch (c) {
  case KADD:
    return Instruction::ADD;
  case KSUB:
    return Instruction::SUB;
  case KMUL:
    return Instruction::MUL;
  case KDIV:
    return Instruction::DIV;
  case KLT:
    return Instruction::LESS;
  case KGT:
    return Instruction::GREATER;
  default:
    cerr << "err: to_opecode" << endl;
    exit(1);
  }
}

struct BinopNode : public Node {
  Keyword op;
  Node *lhs, *rhs;
  BinopNode(Keyword op, Node *lhs, Node *rhs)
      : op(op), lhs(lhs), rhs(rhs) {}
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
      : lhs(ident), rhs(rhs) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Assign " << lhs->ident << endl;
    rhs->print(offset + 1);
  }
  virtual void code_gen(vector<Code> *codes) {
    rhs->code_gen(codes);
    // cout << lhs->ident << ":" << local_ident_table.get(lhs->ident) << endl;
    codes->push_back({.op = Instruction::LOCAL_STORE});
    codes->push_back({.ival = local_ident_table.get(lhs->ident)});
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
  StmtsNode(Node *current, Node *next)
      : current(current), next(next) {}
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
      codes->push_back({Instruction::LOAD_INT});
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
    local_ident_table.next();

    for (const string *param : params) {
      local_ident_table.get(*param);
    }
    body->code_gen(&body_code);
    // cout << "def:" << body_code.size() << endl;
    body_code.push_back({Instruction::RET});
    codes->push_back({Instruction::DEF_FUNC});
    codes->push_back({.sval = &name});
    codes->push_back({.objval = (Object *)new Func(body_code)});
    // set_func(name, new Func(body_code));
    local_ident_table.prev();

    // print_code(body_code);

    // codes->push_back({Instruction::LOAD_BOOL});
    // codes->push_back({.bval = true});
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
  PrimeExprNode(Node *prime, Node *traier)
      : prime(prime), traier(traier) {}
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
  Node *traier;
  RefFieldNode(string *field, Node *traier)
      : field(field), traier(traier) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "." << *field << endl;
    traier->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    exit_by_unsupported("PrimeExprNode#code_gen()");
  }
};

struct SendNode : public Node {
  vector<Node *> args;
  Node *traier;
  SendNode(const vector<Node *> &args, Node *traier)
      : args(args), traier(traier) {}
  virtual void print(int offset) {
    print_offset(offset);
    cout << "Call" << endl;
    // TODO: print args
    traier->print(offset + 1);
  }

  virtual void code_gen(vector<Code> *codes) {
    for (Node *arg : args) {
      arg->code_gen(codes);
    }
    exit_by_unsupported("SendNode#code_gen()");
    exit(1);
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

Node *Parser::read_name_or_funccall(bool is_trailer) {
  Token *ident = get();
  if (next_token(KPARENL)) {
    vector<Node *> args;
    if (!next_token(KPARENR)) {
      read_exprs(args);
      take(KPARENR);
    }
    return new FuncCallNode(*ident->sval, args, is_trailer);
  } else {
    return new IdentNode(*ident->sval);
  }
}

Node *Parser::read_ident() {
  Token *token = get_ident();
  return new IdentNode(*token->sval);
}

Node *Parser::read_prime() {
  if (is_next(TNUMBER)) {
    return read_number();
  } else if (is_next(TIDENT)) {
    return read_name_or_funccall(false);
  } else if (next_token(KTRUE)) {
    return new BoolLiteralNode(true);
  } else if (next_token(KFALSE)) {
    return new BoolLiteralNode(false);
  } else if (is_next(TSTRING)) {
    return read_string();
  }
  exit_by_unexpected("something prime", get());
  return nullptr;
}

void Parser::read_arglist(vector<Node *> *arglist) {
  if (is_next(KPARENR)) {
    return;
  }

  Node *arg = read_expr();
  arglist->emplace_back(arg);
  while (next_token(KCOMMA)) {
    arg = read_expr();
    arglist->emplace_back(arg);
  }
}

Node *Parser::read_traier() {
  if (next_token(KDOT)) {
    return read_name_or_funccall(true);
  }
  return nullptr;
}

Node *Parser::read_prime_expr() {
  Node *node = read_prime();
  while (true) {
    Node *traier = read_traier();
    if (traier == nullptr) {
      break;
    }
    node = new PrimeExprNode(node, traier);
  }
  return node;
}

Node *ast_binop(Keyword op, Node *lhs, Node *rhs) {
  return new BinopNode(op, lhs, rhs);
}

Node *Parser::read_factor() {
  if (next_token(KSUB)) {
    return new SignChangeNode(read_prime_expr());
  } else {
    return read_prime_expr();
  }
}

Node *Parser::read_multiplicative_expr() {
  Node *node = read_factor();
  while (true) {
    if (next_token(KMUL))
      node = ast_binop(KMUL, node, read_factor());
    else if (next_token(KDIV))
      node = ast_binop(KDIV, node, read_factor());
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

void Parser::read_params(vector<string *> *params) {
  if (!is_next(TIDENT)) {
    return;
  }

  Token *token = get_ident();
  params->emplace_back(token->sval);
  while (next_token(KCOMMA)) {
    token = get_ident();
    params->emplace_back(token->sval);
  }
}

Node *Parser::read_funcdef() {
  take(KFUNC);
  Token *ident = get_ident();

  take(KPARENL);
  vector<string *> params;
  read_params(&params);
  take(KPARENR);

  Node *body = read_suite();
  return new FuncDefNode(*ident->sval, params, body);
}

Node *Parser::read_klassdef() {
  take(KCLASS);
  Token *ident = get_ident();
  Node *body = read_suite();
  return new KlassDefNode(*ident->sval, body);
}

Node *Parser::read_block() {
  take(KBRACEL);
  Node *node = read_stmts();
  take(KBRACER);
  return node;
}

Node *Parser::read_if() {
  take(KIF);
  Node *node = read_expr();
  Node *then = read_suite();
  Node *els = next_token(KELSE) ? read_stmt() : nullptr;
  return new IfNode(node, then, els);
}

Node *Parser::read_suite() {
  Node *suite = nullptr;

  take(KBRACEL);
  consume_newlines();
  while (!is_next(KBRACER)) {
    Node *node = read_stmt();
    consume_newlines();

    if (suite != nullptr) {
      suite = new StmtsNode(suite, node);
    } else {
      suite = node;
    }
  }
  take(KBRACER);
  return suite;
}

Node *Parser::read_stmt() {
  Node *node;
  if (is_next(KIF)) {
    node = read_if();
  } else if (is_next(KFUNC)) {
    node = read_funcdef();
  } else if (is_next(KCLASS)) {
    node = read_klassdef();
  } else if (is_next(KBRACEL)) {
    node = read_suite();
  } else {
    node = read_expr();
  }
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
  Node *root = nullptr;

  while (!is_eof()) {
    Node *node = read_stmt();
    consume_newlines();

    if (root != nullptr) {
      root = new StmtsNode(root, node);
    } else {
      root = node;
    }
  }
  return root;
}

Node *Parser::parse() { return read_toplevel(); }

void print_code(const vector<Code> &codes) {
  printf("---------- code ----------\n");
  for (int i = 0; i < codes.size();) {
    printf("%2d: ", i);
    auto op = codes[i++].op;
    const char *op_str = OPCODE_S[static_cast<int>(op)].c_str();
    switch (op) {
    case Instruction::PUT_ENV:
      printf("%s %d\n", op_str, codes[i++].ival);
      break;
    case Instruction::LOAD_INT:
      printf("%s %d\n", op_str, codes[i++].ival);
      break;
    case Instruction::LOAD_BOOL:
      printf("%s %d\n", op_str, codes[i++].bval);
      break;
    case Instruction::LOAD_STRING:
      printf("%s %s\n", op_str, codes[i++].sval->c_str());
      break;
    case Instruction::POP:
    case Instruction::ADD:
    case Instruction::SUB:
    case Instruction::MUL:
    case Instruction::DIV:
    case Instruction::LESS:
    case Instruction::GREATER:
      printf("%s\n", op_str);
      break;
    case Instruction::LOCAL_STORE:
      printf("%s %d\n", op_str, codes[i++].ival);
      break;
    case Instruction::LOCAL_LOAD:
      printf("%s %d\n", op_str, codes[i++].ival);
      break;
    case Instruction::PUT_SELF:
      printf("%s\n", op_str);
      break;
    case Instruction::JUMP:
    case Instruction::JUMP_IF:
    case Instruction::JUMP_IFNOT:
      printf("%s %d\n", op_str, codes[i++].ival);
      break;
    case Instruction::RET:
      printf("%s\n", op_str);
      break;
    case Instruction::DEF_FUNC:
      printf("%s %s %d\n", op_str, codes[i].sval->c_str(),
             codes[i + 1].ival);
      i += 2;
      break;
    case Instruction::CALL_FUNC:
      printf("%s %s %d\n", op_str, codes[i].sval->c_str(),
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
