#include "holang/vm.hpp"
#include "config.hpp"
#include "holang.hpp"

using namespace holang;

Object *HolangVM::main_obj = nullptr;
std::vector<string> HolangVM::import_search_path;

void HolangVM::init_import_search_path() {
  if (import_search_path.size() != 0) {
    return;
  }

#ifdef PATH_HOLIB
  import_search_path.push_back(PATH_HOLIB);
#else
  import_search_path.push_back("/");
#endif
}

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
