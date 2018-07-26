#include "holang/vm.hpp"
#include "holang.hpp"

using namespace holang;

Object *HolangVM::main_obj = nullptr;

void holang::call_func_argc_zero(Value *self, Func *func) {
  if (func->type == FBUILTIN) {
    func->native(self, nullptr, 0);
  } else {
    HolangVM vm(0);
    vm.codes = &func->body;
    vm.eval();
  }
}

void holang::call_func_argc_one(Value *self, Func *func, Value *arg) {
  if (func->type == FBUILTIN) {
    func->native(self, arg, 1);
  } else {
    HolangVM vm(arg, 1);
    vm.codes = &func->body;
    vm.eval();
  }
}
