#include "holang/object.hpp"
#include "holang.hpp"

using namespace holang;

Func *Object::find_method(const std::string &method_name) {
  auto it = methods.find(method_name);
  if (it != methods.end()) {
    return it->second;
  } else if (klass != nullptr) {
    return klass->find_method(method_name);
  } else {
    std::cerr << "method unmatch: " << method_name << std::endl;
    exit(1);
  }
}

Object *Object::find_field(const std::string &field_name) {
  auto it = fields.find(field_name);
  if (it != fields.end()) {
    return it->second;
  } else if (klass != nullptr) {
    return klass->find_field(field_name);
  } else {
    std::cerr << "Object#find_field() unmatch: " << field_name << std::endl;
    exit(1);
  }
}

Klass Klass::Int{"Int"};
Klass Klass::String{"String"};

void Klass::init() {
  std::function<Object *(const Klass)> nnn = &Klass::new_object;
  auto self = this;
  NativeFunc func = [=](Value *, Value *, int) {
    return Value(self->new_object());
  };
  methods["new"] = new Func(func);
}

Func *Value::find_method(const std::string &name) {
  switch (type) {
  case Type::OBJECT:
    return objval->find_method(name);
  case Type::INT:
    return Klass::Int.find_method(name);
  default:
    std::cerr << "find_method: " << this->to_s() << std::endl;
    exit(1);
  }
}

Object *Value::find_field(const std::string &name) {
  if (type != Type::OBJECT) {
    std::cerr << "Value#find_field(): " << this->to_s() << std::endl;
    exit(1);
  }
  return objval->find_field(name);
}
