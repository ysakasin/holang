#pragma once

#include "holang/code.h"
#include <vector>

namespace holang {
struct Node {
  virtual void print(int offset){};
  virtual void code_gen(std::vector<Code> *codes) = 0;
};
} // namespace holang
