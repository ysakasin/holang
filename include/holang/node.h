#pragma once

#include "holang/code.h"
#include <vector>

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
  virtual void print(int offset){};
  virtual void code_gen(std::vector<Code> *codes) = 0;
  Node(NodeType type) : type(type){};
};
