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
