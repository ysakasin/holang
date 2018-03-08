#pragma once

#include "./object.hh"

class String : public Object {
public:
  String(const std::string &str) : str(str) { klass = &Klass::String; }
  virtual const std::string to_s() { return str; }

private:
  const std::string str;
};
