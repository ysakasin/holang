#include "./holang.hh"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

#define DEF_BINOP_FUNC(name, op)                                               \
  void binop_##name(vector<Value> *stack) {                                    \
    Value rhs = stack->back();                                                 \
    stack->pop_back();                                                         \
                                                                               \
    Value lhs = stack->back();                                                 \
    stack->pop_back();                                                         \
    if (lhs.type == VINT && rhs.type == VINT) {                                \
      stack->push_back(Value({VINT, .ival = lhs.ival op rhs.ival}));           \
    } else if (lhs.type == VINT && rhs.type == VDOUBLE) {                      \
      stack->push_back(Value({VDOUBLE, .dval = lhs.ival op rhs.dval}));        \
    } else if (lhs.type == VDOUBLE && rhs.type == VINT) {                      \
      stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.ival}));        \
    } else if (lhs.type == VDOUBLE && rhs.type == VDOUBLE) {                   \
      stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.dval}));        \
    } else {                                                                   \
      cerr << "can not cal " #op << endl;                                      \
      exit(1);                                                                 \
    }                                                                          \
  }

DEF_BINOP_FUNC(add, +)
DEF_BINOP_FUNC(sub, -)
DEF_BINOP_FUNC(mul, *)
DEF_BINOP_FUNC(div, /)

// void binop_sub(vector<Value> *stack) {
//   Value rhs = stack->back();
//   stack->pop_back();

//   Value lhs = stack->back();
//   stack->pop_back();

//   stack->push_back(Value({VINT, lhs.ival - rhs.ival}));
// }

// void binop_mul(vector<Value> *stack) {
//   Value rhs = stack->back();
//   stack->pop_back();

//   Value lhs = stack->back();
//   stack->pop_back();

//   stack->push_back(Value({VINT, lhs.ival * rhs.ival}));
// }

// void binop_div(vector<Value> *stack) {
//   Value rhs = stack->back();
//   stack->pop_back();

//   Value lhs = stack->back();
//   stack->pop_back();

//   stack->push_back(Value({VINT, lhs.ival / rhs.ival}));
// }

void binop_less(vector<Value> *stack) {
  Value rhs = stack->back();
  stack->pop_back();

  Value lhs = stack->back();
  stack->pop_back();

  stack->push_back(Value{VBOOL, lhs.ival < rhs.ival});
}

void binop_greater(vector<Value> *stack) {
  Value rhs = stack->back();
  stack->pop_back();

  Value lhs = stack->back();
  stack->pop_back();

  stack->push_back(Value({VBOOL, lhs.ival > rhs.ival}));
}

map<string, Func *> env;

Value print_func(Value *args, int argc) {
  cout << "print!!!!!!:";
  for (int i = 0; i < argc; i++) {
    cout << args[i].to_s() << ":";
  }
  cout << endl;
  return Value({VBOOL, true});
}

void init_vm() {
  std::function<Value(Value *, int)> native = print_func;
  env["print"] = new Func(native);
}
void set_func(const string &name, Func *func) { env[name] = func; }

using Codes = vector<Code>;
vector<pair<Codes *, int>> prev_code;
vector<int> prev_ep;

void eval(vector<Code> *codes) {
  init_vm();
  vector<Value> stack;
  int ep = 0;
  // cout << "start eval" << endl;
  // cout << "------------------------------" << endl;
  for (int i = 0; i < codes->size(); i++) {
    // cout << OPCODE_S[codes->at(i).op] << endl;
    switch (codes->at(i).op) {
    case OP_PUT_ENV:
      ep = stack.size();
      stack.resize(stack.size() + codes->at(++i).ival);
      break;
    case OP_LOAD_INT:
      stack.push_back(Value({VINT, codes->at(++i).ival}));
      break;
    case OP_LOAD_BOOL:
      stack.push_back(Value({VBOOL, codes->at(++i).bval}));
      break;
    case OP_POP:
      stack.pop_back();
      break;
    case OP_ADD:
      binop_add(&stack);
      break;
    case OP_SUB:
      binop_sub(&stack);
      break;
    case OP_MUL:
      binop_mul(&stack);
      break;
    case OP_DIV:
      binop_div(&stack);
      break;
    case OP_LESS:
      binop_less(&stack);
      break;
    case OP_GREATER:
      binop_greater(&stack);
      break;
    case OP_LOCAL_STORE:
      stack[ep + codes->at(++i).ival] = stack.back();
      break;
    case OP_LOCAL_LOAD:
      stack.push_back(stack[ep + codes->at(++i).ival]);
      break;
    case OP_JUMP:
      i = codes->at(i + 1).ival;
      break;
    case OP_JUMP_IF:
      if (stack.back().bval)
        i = codes->at(i + 1).ival;
      stack.pop_back();
      break;
    case OP_JUMP_IFNOT:
      if (!stack.back().bval)
        i = codes->at(i + 1).ival;
      stack.pop_back();
      break;
    case OP_RET:
      i = prev_code.back().second;
      codes = prev_code.back().first;
      stack[ep] = stack.back();
      stack.resize(ep + 1);
      ep = prev_ep.back();
      prev_code.pop_back();
      prev_ep.pop_back();
      break;
    case OP_CALL_FUNC:
      auto func = env[*codes->at(++i).sval];
      int argc = codes->at(++i).ival;
      Value ret;
      if (func->type == FBUILTIN) {
        ret = func->native(&stack.back() - argc + 1, argc);
        stack.resize(stack.size() - argc);
        stack.push_back(ret);
      } else {
        prev_code.push_back({codes, i});
        prev_ep.push_back(ep);
        ep = stack.size() - argc;
        i = -1;
        codes = &func->body;
      }
      break;
    }
  }

  cout << "stack status:" << endl;
  for (auto last : stack) {
    cout << last.to_s() << endl;
  }
}
