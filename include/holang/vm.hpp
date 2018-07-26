#pragma once

#include "holang.hpp"
#include "holang/parser.hpp"
#include "holang/string.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>

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

namespace holang {
static Value print_func(Value *, Value *args, int argc) {
  for (int i = 0; i < argc; i++) {
    std::cout << args[i].to_s();
  }
  return Value(true);
}

static Value println_func(Value *, Value *args, int argc) {
  for (int i = 0; i < argc; i++) {
    if (i != 0) {
      std::cout << " ";
    }
    std::cout << args[i].to_s();
  }
  std::cout << std::endl;
  return Value(true);
}

static Value getline_func(Value *, Value *, int) {
  std::string str;
  cin >> str;
  return Value((Object *)new String(str));
}

static Value next_func(Value *self, Value *, int) {
  return Value(self->ival + 1);
}

void call_func_argc_zero(Value *self, Func *func);

static Value times_func(Value *self, Value *args, int argc) {
  if (argc != 1) {
    std::cerr << "invalid argc" << std::endl;
    exit(1);
  }

  if (args[0].type != Type::FUNCTION) {
    std::cerr << "have to func" << std::endl;
    std::cerr << argc << std::endl;
    std::cerr << args[0].to_s() << std::endl;
    exit(1);
  }

  Func *func = args[0].funcval;
  for (int i = 0; i < self->ival; i++) {
    call_func_argc_zero(self, func);
  }
  return Value(true);
}

class HolangVM {
  using Codes = std::vector<Code>;

public:
  HolangVM(int local_val_size) {
    init_main_obj();
    if (stack == nullptr)
      stack = new Value[stack_size];
    stack_push(HolangVM::main_obj);
    sp += local_val_size;
  }

  ~HolangVM() {
    if (stack != nullptr)
      delete[] stack;
  }

  void init_main_obj() {
    if (main_obj != nullptr) {
      return;
    }
    main_obj = new Object();
    NativeFunc native = print_func;
    main_obj->set_method("print", new Func(native));
    NativeFunc native_println = println_func;
    main_obj->set_method("println", new Func(native_println));
    NativeFunc native_getline = getline_func;
    main_obj->set_method("getline", new Func(native_getline));

    NativeFunc next_native = next_func;
    Klass::Int.set_method("next", new Func(next_native));
    Klass::Int.set_method("times", new Func(times_func));
    String::init();

    main_obj->set_field("Int", &Klass::Int);
    main_obj->set_field("String", &Klass::String);
  }

  void eval() {
    while (pc < codes->size()) {
      auto op = take_code().op;
      switch (op) {
      case Instruction::ADD:
        binop_add();
        break;
      case Instruction::SUB:
        binop_sub();
        break;
      case Instruction::MUL:
        binop_mul();
        break;
      case Instruction::DIV:
        binop_div();
        break;
      case Instruction::MOD:
        binop_mod();
        break;
      case Instruction::LESS:
        binop_less();
        break;
      case Instruction::GREATER:
        binop_greater();
        break;
      case Instruction::EQUAL:
        binop_equal();
        break;
      case Instruction::POP:
        sp--;
        break;
      case Instruction::PUT_INT:
        put_int();
        break;
      case Instruction::PUT_BOOL:
        put_bool();
        break;
      case Instruction::PUT_STRING:
        put_string();
        break;
      case Instruction::PUT_LAMBDA:
        put_lambda();
        break;
      case Instruction::LOAD_LOCAL:
        load_local();
        break;
      case Instruction::STORE_LOCAL:
        store_local();
        break;
      case Instruction::DEF_FUNC:
        def_func();
        break;
      case Instruction::CALL_FUNC:
        call_func();
        break;
      case Instruction::RET:
        func_ret();
        break;
      case Instruction::PUT_SELF:
        put_self();
        break;
      case Instruction::JUMP:
        jump();
        break;
      case Instruction::JUMP_IF:
        jump_if();
        break;
      case Instruction::JUMP_IFNOT:
        jump_ifnot();
        break;
      case Instruction::LOAD_CLASS:
        load_class();
        break;
      case Instruction::PREV_ENV:
        prev_env();
        break;
      case Instruction::LOAD_OBJ_FIELD:
        load_obj_field();
        break;
      case Instruction::IMPORT:
        import();
        break;
      default:
        std::cerr << "not implemented: " << op << std::endl;
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
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival + rhs.ival);
      // } else if (lhs.type == Type::INT && rhs.type == Type::DOUBLE)
      // {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.ival op
      //   rhs.dval}));
      // } else if (lhs.type == Type::DOUBLE && rhs.type == Type::INT)
      // {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.dval op
      //   rhs.ival}));
      // } else if (lhs.type == Type::DOUBLE && rhs.type ==
      // Type::DOUBLE) {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.dval op
      //   rhs.dval}));
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
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival - rhs.ival);
      // } else if (lhs.type == Type::INT && rhs.type == Type::DOUBLE)
      // {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.ival op
      //   rhs.dval}));
      // } else if (lhs.type == Type::DOUBLE && rhs.type == Type::INT)
      // {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.dval op
      //   rhs.ival}));
      // } else if (lhs.type == Type::DOUBLE && rhs.type ==
      // Type::DOUBLE) {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.dval op
      //   rhs.dval}));
    } else {
      std::cerr << "can not cal -" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void binop_mul() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival * rhs.ival);
    } else {
      std::cerr << "can not cal *" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void binop_div() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival / rhs.ival);
    } else {
      std::cerr << "can not cal /" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void binop_mod() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival % rhs.ival);
    } else {
      std::cerr << "can not cal %" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }
  void binop_less() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival < rhs.ival);
      // } else if (lhs.type == Type::INT && rhs.type == Type::DOUBLE)
      // {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.ival op
      //   rhs.dval}));
      // } else if (lhs.type == Type::DOUBLE && rhs.type == Type::INT)
      // {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.dval op
      //   rhs.ival}));
      // } else if (lhs.type == Type::DOUBLE && rhs.type ==
      // Type::DOUBLE) {
      //   stack->push_back(Value({Type::DOUBLE, .dval = lhs.dval op
      //   rhs.dval}));
    } else {
      std::cerr << "can not cal <" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }

  void binop_greater() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival > rhs.ival);
    } else {
      std::cerr << "can not cal >" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }

  void binop_equal() {
    auto rhs = stack_pop();
    auto lhs = stack_pop();
    if (lhs.type == Type::INT && rhs.type == Type::INT) {
      stack_push(lhs.ival == rhs.ival);
    } else {
      std::cerr << "can not cal ==" << std::endl;
      std::cerr << rhs.to_s() << std::endl;
      std::cerr << lhs.to_s() << std::endl;
      exit(1);
    }
  }

  // put_int number
  // [] -> [val]
  void put_int() {
    int i = take_code().ival;
    stack_push(i);
  }

  // put_bool boolean
  // [] -> [val]
  void put_bool() {
    bool b = take_code().bval;
    stack_push(b);
  }

  // put_string string_ptr
  // [] -> [val]
  void put_string() {
    std::string *str = take_code().sval;
    stack_push(new String(*str));
  }

  // put_lambda lambda_ptr
  // [] -> [val]
  void put_lambda() {
    Func *lambda = take_code().funcval;
    stack_push(lambda);
  }

  // load_local index
  // [] -> [val]
  void load_local() {
    int offset = take_code().ival;
    stack_push(stack[ep + offset]);
  }

  // store_local index
  // [val] -> [val]
  void store_local() {
    int offset = take_code().ival;
    auto v = stack_top();
    stack[ep + offset] = v;
  }

  // def_func func_name, func_obj
  // [] -> [true]
  void def_func() {
    auto *self = stack[ep].objval;
    const std::string &name = *take_code().sval;
    Func *obj = (Func *)take_code().objval;
    self->set_method(name, obj);
    stack_push(true);
  }

  // call_func func_name, argc
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

    if (prev_ep.empty()) {
      pc = std::numeric_limits<int>::max();
      return;
    }

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

  void load_obj_field() {
    const std::string &field = *take_code().sval;
    Value val = stack_pop();
    stack_push(val.find_field(field));
  }

  // import
  // [str] -> []
  void import() {
    Value target = stack_pop();
    std::string path = target.to_s();
    std::ifstream ifs(path);
    if (ifs.fail()) {
      std::cerr << path << ": Not found." << std::endl;
      exit(1);
    }
    std::istreambuf_iterator<char> it(ifs);
    std::istreambuf_iterator<char> last;
    std::string source_code(it, last);

    std::vector<Token *> token_chain;
    lex(source_code, token_chain);

    holang::Parser parser(token_chain);
    Node *root = parser.parse();
    std::vector<Code> *other_codes = new std::vector<Code>();
    root->code_gen(other_codes);
    other_codes->push_back({Instruction::RET});
    auto self = stack[ep];
    stack_push(self);

    for (int i = 0; i < parser.toplevel_val_size(); i++) {
      stack_push(0);
    }
    prev_ep.push_back(ep);
    save_current_codes();

    codes = other_codes;
    pc = 0;
    ep = sp - parser.toplevel_val_size() - 1;
  }

  void stack_push(int x) { stack_push(Value(x)); }
  void stack_push(double x) { stack_push(Value(x)); }
  void stack_push(bool x) { stack_push(Value(x)); }
  void stack_push(Object *x) { stack_push(Value(x)); }
  void stack_push(Func *x) { stack_push(Value(x)); }
  void stack_push(const Value &val) {
    reserve_stack();
    stack[sp++] = val;
  }

  Value stack_pop() { return stack[--sp]; }
  Value stack_top() { return stack[sp - 1]; }

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
  Value *stack = nullptr;
  int sp = 0; // stack pointer
  int ep = 0; // env pointer
  int stack_size = 1024;
  static Object *main_obj;
  std::vector<int> prev_ep;
  std::vector<std::pair<Codes *, int>> prev_code;
};
} // namespace holang
