#pragma once

#include <iostream>
#include <map>
#include <string>

class Klass;
struct Func;

class Object {
public:
  Klass *klass;
  std::map<std::string, Func *> methods;

public:
  Func *find_method(const std::string &method_name);
  void set_method(const std::string &name, Func *func) {
    methods.emplace(name, func);
  }
  virtual const std::string to_s() { return "<Object>"; }
};

class Klass : public Object {
  std::string name;

public:
  Klass(std::string name) : name(name) {}
  Klass(const char name[]) : name(name) {}
  static Klass Int;
  static Klass String;
};
