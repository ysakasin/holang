#pragma once

#include "holang.h"
#include "holang/string.h"

#include <cstring>
#include <iostream>

/*
# Stack layout

5:[                ]
4:[                ] <- sp
3:[ var            ]
2:[ foo            ]
2:[ local variable ]
1:[ local variable ]
0:[ self           ] <- ep
*/

Value print_func(Value *, Value *args, int argc) {
  std::cout << "print!!!!!!:";
  for (int i = 0; i < argc; i++) {
    std::cout << args[i].to_s() << ":";
  }
  std::cout << std::endl;
  return Value(true);
}

Value next_func(Value *self, Value *, int) { return Value(self->ival + 1); }

Value reverse_func(Value *self, Value *, int) {
  String *str = (String *)self->objval;
  std::string rev = str->str;
  std::reverse(rev.begin(), rev.end());
  return Value((Object *)new String(rev));
}

class HolangVM {
  using Codes = std::vector<Code>;

public:
  HolangVM(int local_val_size) {
    stack = new Value[stack_size];
    main_obj = new Object();
    stack_push(main_obj);
    sp += local_val_size;
    NativeFunc native = print_func;
    main_obj->set_method("print", new Func(native));

    // env["print"] = new Func(native);

    // klass_env["Int"] = new Klass("Int");
    NativeFunc next_native = next_func;
    Klass::Int.set_method("next", new Func(next_native));
    Klass::String.set_method("reverse", new Func((NativeFunc)reverse_func));

    main_obj->set_field("Int", &Klass::Int);
    main_obj->set_field("String", &Klass::String);
  }
  void eval() {
    while (pc < codes->size()) {
      auto op = take_code().op;
      switch (op) {
      case OP_ADD:
        binop_add();
        break;
      case OP_SUB:
        binop_sub();
        break;
      case OP_LESS:
        binop_less();
        break;
      case OP_POP:
        sp--;
        break;
      case OP_LOAD_INT:
        load_int();
        break;
      case OP_LOAD_BOOL:
        load_bool();
        break;
      case OP_LOAD_STRING:
        load_string();
        break;
      case OP_LOCAL_LOAD:
        local_load();
        break;
      case OP_LOCAL_STORE:
        local_store();
        break;
      case OP_DEF_FUNC:
        def_func();
        break;
      case OP_CALL_FUNC:
        call_func();
        break;
      case OP_RET:
        func_ret();
        break;
      case OP_PUT_SELF:
        put_self();
        break;
      case OP_JUMP:
        jump();
        break;
      case OP_JUMP_IF:
        jump_if();
        break;
      case OP_JUMP_IFNOT:
        jump_ifnot();
        break;
      case OP_LOAD_CLASS:
        load_class();
        break;
      case OP_PREV_ENV:
        prev_env();
        break;
      default:
        std::cerr << "not implemented: " << OPCODE_S[op] << std::endl;
        exit(1);
      }
    }
  }

  void print_stack() {
    std::cout << "--- print stack ---" << std::endl;
    printf("%2d:\t\t<- sp\n", sp);
    for (int i = sp - 1; i >= 0; i--) {
      printf("%2d: ", i);
      std::cout << stack[i].to_s();
      if (i == ep) {
        std::cout << "\t<- ep";
      }
      std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
  }

private:
  void binop_add() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == VINT && rhs.type == VINT) {
      stack_push(lhs.ival + rhs.ival);
      // } else if (lhs.type == VINT && rhs.type == VDOUBLE) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.ival op rhs.dval}));
      // } else if (lhs.type == VDOUBLE && rhs.type == VINT) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.ival}));
      // } else if (lhs.type == VDOUBLE && rhs.type == VDOUBLE) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.dval}));
    } else {
      std::cerr << "can not cal +" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void binop_sub() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == VINT && rhs.type == VINT) {
      stack_push(lhs.ival - rhs.ival);
      // } else if (lhs.type == VINT && rhs.type == VDOUBLE) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.ival op rhs.dval}));
      // } else if (lhs.type == VDOUBLE && rhs.type == VINT) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.ival}));
      // } else if (lhs.type == VDOUBLE && rhs.type == VDOUBLE) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.dval}));
    } else {
      std::cerr << "can not cal -" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void binop_less() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == VINT && rhs.type == VINT) {
      stack_push(lhs.ival < rhs.ival);
      // } else if (lhs.type == VINT && rhs.type == VDOUBLE) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.ival op rhs.dval}));
      // } else if (lhs.type == VDOUBLE && rhs.type == VINT) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.ival}));
      // } else if (lhs.type == VDOUBLE && rhs.type == VDOUBLE) {
      //   stack->push_back(Value({VDOUBLE, .dval = lhs.dval op rhs.dval}));
    } else {
      std::cerr << "can not cal <" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void load_int() {
    int i = take_code().ival;
    stack_push(i);
  }
  void load_bool() {
    bool b = take_code().bval;
    stack_push(b);
  }
  void load_string() {
    std::string *str = take_code().sval;
    stack_push(new String(*str));
  }
  void local_load() {
    int offset = take_code().ival;
    stack_push(stack[ep + offset]);
  }
  void local_store() {
    int offset = take_code().ival;
    auto v = stack_pop();
    stack[ep + offset] = v;
  }
  void def_func() {
    auto *self = stack[ep].objval;
    const std::string &name = *take_code().sval;
    Func *obj = (Func *)take_code().objval;
    self->set_method(name, obj);
    stack_push(true);
  }
  void call_func() {
    std::string *func_name = take_code().sval;
    int argc = take_code().ival;
    Value *self = &stack[sp - argc - 1];
    auto func = self->find_method(*func_name);

    Value ret;
    if (func->type == FBUILTIN) {
      ret = func->native(self, &stack[sp - argc], argc);
      sp = sp - argc - 1;
      stack_push(ret);
    } else {
      save_current_codes();
      prev_ep.push_back(ep);

      codes = &func->body;
      pc = 0;
      ep = sp - argc - 1;
    }
  }
  void func_ret() {
    auto r = stack_pop();
    sp = ep;
    stack_push(r);

    ep = prev_ep.back();
    prev_ep.pop_back();
    load_prev_codes();
  }
  void put_self() { stack_push(stack[ep]); }
  void jump() {
    auto to = take_code();
    pc = to.ival;
  }
  void jump_if() {
    auto cond = stack_pop();
    auto to = take_code();
    if (cond.bval) {
      pc = to.ival;
    }
  }
  void jump_ifnot() {
    auto cond = stack_pop();
    auto to = take_code();
    if (!cond.bval) {
      pc = to.ival;
    }
  }
  void load_class() {
    const std::string *klass_name = take_code().sval;
    auto *self = stack[ep].objval;
    auto it = self->fields.find(*klass_name);
    Klass *klass;
    if (it == self->fields.end()) {
      klass = new Klass(*klass_name);
      self->set_field(*klass_name, klass);
    } else {
      klass = (Klass *)it->second;
    }
    stack_push(klass);

    save_ep();
    ep = sp - 1;
  }

  void prev_env() {
    sp = ep + 1;
    load_ep();
  }

  void stack_push(int x) { stack_push(Value(x)); }
  void stack_push(double x) { stack_push(Value(x)); }
  void stack_push(bool x) { stack_push(Value(x)); }
  void stack_push(Object *x) { stack_push(Value(x)); }
  void stack_push(const Value &val) {
    reserve_stack();
    stack[sp++] = val;
  }

  Value stack_pop() { return stack[--sp]; }

  Code take_code() { return codes->at(pc++); }
  void save_current_codes() { prev_code.push_back({codes, pc}); }
  void save_ep() { prev_ep.push_back(ep); }
  void load_prev_codes() {
    auto prev = prev_code.back();
    prev_code.pop_back();
    codes = prev.first;
    pc = prev.second;
  }
  void load_ep() {
    ep = prev_ep.back();
    prev_ep.pop_back();
  }

private:
  void reserve_stack() {
    if (sp >= stack_size) {
      auto new_size = stack_size * 2;
      auto *new_stack = new Value[new_size];
      if (new_stack == nullptr) {
        std::cerr << "allocation error" << std::endl;
        exit(1);
      }
      std::memcpy(new_stack, stack, sizeof(Value) * stack_size);
      delete[] stack;
      stack = new_stack;
      stack_size = new_size;
    }
  }

public:
  Codes *codes;

private:
  int pc = 0; // program counter
  Value *stack;
  int sp = 0; // stack pointer
  int ep = 0; // env pointer
  int stack_size = 1024;
  Object *main_obj;
  std::vector<int> prev_ep;
  std::vector<std::pair<Codes *, int>> prev_code;
};
