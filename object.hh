#pragma once

#include <iostream>
#include <map>
#include <string>

#include "./holang.hh"

class Klass;

class Object {
  Klass *klass;
  std::map<std::string, Func *> methods;

public:
  Func *find_method(const std::string &method_name);
  void set_method(const std::string &name, Func *func) {
    methods.emplace(name, func);
  }
};

class Klass : public Object {
  std::string name;

public:
  Klass(std::string name) : name(name) {}
  Klass(const char name[]) : name(name) {}
  static Klass Int;
};
