#include "holang/parser.hpp"
#include "holang.hpp"
#include "holang/variable_table.hpp"
#include <iostream>
#include <map>
#include <vector>

using namespace std;
using namespace holang;

// ----- parser ----- //

Node *Parser::parse() { return read_toplevel(); }

void Parser::take(TokenType type) {
  Token *token = get();
  if (token->type != type) {
    exit_by_unexpected(type, token);
  }
}

void Parser::take(Keyword keyword) {
  Token *token = get();
  if (token->type != TokenType::KEYWORD || token->keyword != keyword) {
    exit_by_unexpected(keyword, token);
  }
}

void Parser::consume_newlines() {
  while (next_token(TokenType::NEWLINE)) {
  }
}

// ----- top level ----- //

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

Node *Parser::read_stmts() {
  Node *node = read_stmt();
  while (!(is_eof() || is_next(Keyword::BRACER))) {
    node = new StmtsNode(node, read_stmt());
  }
  return node;
}

// ----- statement ----- //

Node *Parser::read_stmt() {
  Node *node;
  if (is_next(Keyword::IF)) {
    node = read_if();
  } else if (is_next(Keyword::FUNC)) {
    node = read_funcdef();
  } else if (is_next(Keyword::CLASS)) {
    node = read_klassdef();
  } else if (is_next(Keyword::IMPORT)) {
    node = read_import();
  } else if (is_next(Keyword::BRACEL)) {
    node = read_suite();
  } else {
    node = read_expr();
  }
  return node;
}

Node *Parser::read_if() {
  take(Keyword::IF);
  Node *node = read_expr();
  Node *then = read_suite();
  Node *els = next_token(Keyword::ELSE) ? read_stmt() : nullptr;
  return new IfNode(node, then, els);
}

Node *Parser::read_funcdef() {
  take(Keyword::FUNC);
  Token *ident = get_ident();
  variable_table.next();

  take(Keyword::PARENL);
  vector<string *> params;
  read_params(&params);
  take(Keyword::PARENR);

  Node *body = read_suite();
  variable_table.prev();
  return new FuncDefNode(*ident->sval, params, body);
}

Node *Parser::read_klassdef() {
  take(Keyword::CLASS);
  Token *ident = get_ident();
  Node *body = read_suite();
  return new KlassDefNode(*ident->sval, body);
}

Node *Parser::read_import() {
  take(Keyword::IMPORT);
  Node *node = read_expr();
  return new ImportNode(node);
}

Node *Parser::read_suite() {
  Node *suite = nullptr;

  take(Keyword::BRACEL);
  consume_newlines();
  while (!is_next(Keyword::BRACER)) {
    Node *node = read_stmt();
    consume_newlines();

    if (suite != nullptr) {
      suite = new StmtsNode(suite, node);
    } else {
      suite = node;
    }
  }
  take(Keyword::BRACER);
  return suite;
}

// ----- expression ----- //

Node *Parser::read_expr() { return read_assignment_expr(); }

Node *Parser::read_assignment_expr() {
  Token *token = get();
  if (token->type == TokenType::IDENT && next_token(Keyword::ASSIGN)) {
    int index = variable_table.get(*token->sval);
    return new AssignNode(new IdentNode(*token->sval, index),
                          read_assignment_expr());
  }
  unget();
  return read_equal_expr();
}

Node *ast_binop(Keyword op, Node *lhs, Node *rhs) {
  return new BinopNode(op, lhs, rhs);
}

Node *Parser::read_equal_expr() {
  Node *node = read_comp_expr();
  if (next_token(Keyword::EQUAL))
    return ast_binop(Keyword::EQUAL, node, read_comp_expr());
  else
    return node;
}

Node *Parser::read_comp_expr() {
  Node *node = read_additive_expr();
  if (next_token(Keyword::LT))
    return ast_binop(Keyword::LT, node, read_additive_expr());
  else if (next_token(Keyword::GT))
    return ast_binop(Keyword::GT, node, read_additive_expr());
  else
    return node;
}

Node *Parser::read_multiplicative_expr() {
  Node *node = read_factor();
  while (true) {
    if (next_token(Keyword::MUL))
      node = ast_binop(Keyword::MUL, node, read_factor());
    else if (next_token(Keyword::DIV))
      node = ast_binop(Keyword::DIV, node, read_factor());
    else if (next_token(Keyword::MOD))
      node = ast_binop(Keyword::MOD, node, read_factor());
    else
      return node;
  }
}

Node *Parser::read_additive_expr() {
  Node *node = read_multiplicative_expr();
  while (true) {
    if (next_token(Keyword::ADD))
      node = ast_binop(Keyword::ADD, node, read_multiplicative_expr());
    else if (next_token(Keyword::SUB))
      node = ast_binop(Keyword::SUB, node, read_multiplicative_expr());
    else
      return node;
  }
}

Node *Parser::read_factor() {
  if (next_token(Keyword::SUB)) {
    return new SignChangeNode(read_prime_expr());
  } else {
    return read_prime_expr();
  }
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

Node *Parser::read_traier() {
  if (next_token(Keyword::DOT)) {
    return read_name_or_funccall(true);
  }
  return nullptr;
}

// ----- prime ----- //

Node *Parser::read_prime() {
  if (is_next(TokenType::NUMBER)) {
    return read_number();
  } else if (is_next(TokenType::IDENT)) {
    return read_name_or_funccall(false);
  } else if (next_token(Keyword::TRUE)) {
    return new BoolLiteralNode(true);
  } else if (next_token(Keyword::FALSE)) {
    return new BoolLiteralNode(false);
  } else if (is_next(TokenType::STRING)) {
    return read_string();
  }
  exit_by_unexpected("something prime", get());
  return nullptr;
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

Node *Parser::read_name_or_funccall(bool is_trailer) {
  Token *ident = get();
  if (next_token(Keyword::PARENL)) {
    vector<Node *> args;
    if (!next_token(Keyword::PARENR)) {
      read_exprs(args);
      take(Keyword::PARENR);
    }
    return new FuncCallNode(*ident->sval, args, is_trailer);
  } else {
    if (is_trailer) {
      return new RefFieldNode(ident->sval);
    } else {
      int index = variable_table.get(*ident->sval);
      return new IdentNode(*ident->sval, index);
    }
  }
}

Node *Parser::read_block() {
  take(Keyword::BRACEL);
  Node *node = read_stmts();
  take(Keyword::BRACER);
  return node;
}

void Parser::read_exprs(vector<Node *> &args) {
  args.push_back(read_expr());
  while (next_token(Keyword::COMMA)) {
    args.push_back(read_expr());
  }
}

void Parser::read_arglist(vector<Node *> *arglist) {
  if (is_next(Keyword::PARENR)) {
    return;
  }

  Node *arg = read_expr();
  arglist->emplace_back(arg);
  while (next_token(Keyword::COMMA)) {
    arg = read_expr();
    arglist->emplace_back(arg);
  }
}

void Parser::read_params(vector<string *> *params) {
  if (!is_next(TokenType::IDENT)) {
    return;
  }

  Token *token = get_ident();
  params->emplace_back(token->sval);
  while (next_token(Keyword::COMMA)) {
    token = get_ident();
    params->emplace_back(token->sval);
  }
}
