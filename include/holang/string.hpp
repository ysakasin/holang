#pragma once

#include "holang/object.h"

namespace holang {
class String : public Object {
public:
  String(const std::string &str) : str(str) { klass = &Klass::String; }
  virtual const std::string to_s() { return str; }

  const std::string str;
};
} // namespace holang
