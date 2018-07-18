#include "holang/string.hpp"
#include "holang.hpp"

using namespace holang;

static Value reverse_func(Value *self, Value *, int) {
  String *str = (String *)self->objval;
  std::string rev = str->str;
  std::reverse(rev.begin(), rev.end());
  return Value((Object *)new String(rev));
}

static Value to_i(Value *self, Value *, int) {
  String *str = (String *)self->objval;
  int i = stoi(str->str);
  return Value(i);
}

void String::init() {
  Klass::String.set_method("reverse", new Func((NativeFunc)reverse_func));
  Klass::String.set_method("to_i", new Func((NativeFunc)to_i));
}
