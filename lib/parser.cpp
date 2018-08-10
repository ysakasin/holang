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

void Parser::consume_newlines() {
  while (next_token(TokenType::NewLine)) {
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

// ----- statement ----- //

Node *Parser::read_stmt() {
  Node *node;
  if (is_next(TokenType::If)) {
    node = read_if();
  } else if (is_next(TokenType::Func)) {
    node = read_funcdef();
  } else if (is_next(TokenType::Class)) {
    node = read_klassdef();
  } else if (is_next(TokenType::Import)) {
    node = read_import();
  } else if (is_next(TokenType::While)) {
    node = read_while();
  } else if (is_next(TokenType::BraseL)) {
    node = read_suite();
  } else {
    node = read_expr();
  }
  return node;
}

Node *Parser::read_if() {
  take(TokenType::If);
  Node *node = read_expr();
  Node *then = read_suite();
  Node *els = next_token(TokenType::Else) ? read_stmt() : nullptr;
  return new IfNode(node, then, els);
}

Node *Parser::read_funcdef() {
  take(TokenType::Func);
  Token *ident = get_ident();
  variable_table.next();

  take(TokenType::ParenL);
  vector<string *> params;
  read_params(&params);
  take(TokenType::ParenR);

  Node *body = read_suite();
  variable_table.prev();
  return new FuncDefNode(ident->str, params, body);
}

Node *Parser::read_klassdef() {
  take(TokenType::Class);
  Token *ident = get_ident();
  Node *body = read_suite();
  return new KlassDefNode(ident->str, body);
}

Node *Parser::read_import() {
  take(TokenType::Import);
  Node *node = read_expr();
  return new ImportNode(node);
}

Node *Parser::read_while() {
  take(TokenType::While);
  Node *node = read_expr();
  Node *body = read_suite();
  return new WhileNode(node, body);
}

Node *Parser::read_suite() {
  Node *suite = nullptr;

  take(TokenType::BraseL);
  consume_newlines();
  while (!is_next(TokenType::BraseR)) {
    Node *node = read_stmt();
    consume_newlines();

    if (suite != nullptr) {
      suite = new StmtsNode(suite, node);
    } else {
      suite = node;
    }
  }
  take(TokenType::BraseR);
  return suite;
}

// ----- expression ----- //

Node *Parser::read_expr() { return read_assignment_expr(); }

Node *Parser::read_assignment_expr() {
  Token *token = get();
  if (token->type == TokenType::Ident && next_token(TokenType::Assign)) {
    int index = variable_table.get(token->str);
    return new AssignNode(new IdentNode(token->str, index),
                          read_assignment_expr());
  }
  unget();
  return read_equal_expr();
}

Node *ast_binop(TokenType op, Node *lhs, Node *rhs) {
  return new BinopNode(op, lhs, rhs);
}

Node *Parser::read_equal_expr() {
  Node *node = read_comp_expr();
  if (next_token(TokenType::Equal))
    return ast_binop(TokenType::Equal, node, read_comp_expr());
  else
    return node;
}

Node *Parser::read_comp_expr() {
  Node *node = read_additive_expr();
  if (next_token(TokenType::LessThan))
    return ast_binop(TokenType::LessThan, node, read_additive_expr());
  else if (next_token(TokenType::GreaterThan))
    return ast_binop(TokenType::GreaterThan, node, read_additive_expr());
  else
    return node;
}

Node *Parser::read_multiplicative_expr() {
  Node *node = read_factor();
  while (true) {
    if (next_token(TokenType::Mul))
      node = ast_binop(TokenType::Mul, node, read_factor());
    else if (next_token(TokenType::Div))
      node = ast_binop(TokenType::Div, node, read_factor());
    else if (next_token(TokenType::Mod))
      node = ast_binop(TokenType::Mod, node, read_factor());
    else
      return node;
  }
}

Node *Parser::read_additive_expr() {
  Node *node = read_multiplicative_expr();
  while (true) {
    if (next_token(TokenType::Plus))
      node = ast_binop(TokenType::Plus, node, read_multiplicative_expr());
    else if (next_token(TokenType::Minus))
      node = ast_binop(TokenType::Minus, node, read_multiplicative_expr());
    else
      return node;
  }
}

Node *Parser::read_factor() {
  if (next_token(TokenType::Minus)) {
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
  if (next_token(TokenType::Dot)) {
    return read_name_or_funccall(true);
  }
  return nullptr;
}

// ----- prime ----- //

Node *Parser::read_prime() {
  if (is_next(TokenType::Integer)) {
    return read_number();
  } else if (is_next(TokenType::Ident)) {
    return read_name_or_funccall(false);
  } else if (next_token(TokenType::True)) {
    return new BoolLiteralNode(true);
  } else if (next_token(TokenType::False)) {
    return new BoolLiteralNode(false);
  } else if (is_next(TokenType::String)) {
    return read_string();
  }
  exit_by_unexpected("something prime", get());
  return nullptr;
}

Node *Parser::read_number() {
  Token *token = get();
  return new IntLiteralNode(token->i);
}

Node *Parser::read_string() {
  Token *token = get();
  return new StringLiteralNode(token->str);
}

Node *Parser::read_name_or_funccall(bool is_trailer) {
  Token *ident = get();
  if (next_token(TokenType::ParenL)) {
    vector<Node *> args;
    if (!next_token(TokenType::ParenR)) {
      read_exprs(args);
      take(TokenType::ParenR);
    }
    if (is_next(TokenType::BraseL)) {
      args.push_back(read_block());
    }
    return new FuncCallNode(ident->str, args, is_trailer);
  } else {
    if (is_trailer) {
      return new RefFieldNode(ident->str);
    } else {
      int index = variable_table.get(ident->str);
      return new IdentNode(ident->str, index);
    }
  }
}

Node *Parser::read_block() {
  Node *suite = nullptr;
  vector<string *> params;

  take(TokenType::BraseL);
  if (next_token(TokenType::VertialBar)) {
    read_params(&params);
    take(TokenType::VertialBar);
  }

  variable_table.next();
  for (auto *str : params) {
    variable_table.get(*str);
  }

  consume_newlines();
  while (!is_next(TokenType::BraseR)) {
    Node *node = read_stmt();
    consume_newlines();

    if (suite != nullptr) {
      suite = new StmtsNode(suite, node);
    } else {
      suite = node;
    }
  }
  take(TokenType::BraseR);

  variable_table.prev();
  return new LambdaNode(params, suite);
}

void Parser::read_exprs(vector<Node *> &args) {
  args.push_back(read_expr());
  while (next_token(TokenType::Comma)) {
    args.push_back(read_expr());
  }
}

void Parser::read_arglist(vector<Node *> *arglist) {
  if (is_next(TokenType::ParenR)) {
    return;
  }

  Node *arg = read_expr();
  arglist->emplace_back(arg);
  while (next_token(TokenType::Comma)) {
    arg = read_expr();
    arglist->emplace_back(arg);
  }
}

void Parser::read_params(vector<string *> *params) {
  if (!is_next(TokenType::Ident)) {
    return;
  }

  Token *token = get_ident();
  params->emplace_back(&token->str);
  while (next_token(TokenType::Comma)) {
    token = get_ident();
    params->emplace_back(&token->str);
  }
}
