#pragma once

#include <string>
#include <vector>

#include "holang/code.hpp"
#include "holang/node.hpp"
#include "holang/object.hpp"
#include "holang/token.hpp"
#include "holang/value.hpp"

namespace holang {
int size_local_idents();
void set_func(const std::string &name, Func *func);
} // namespace holang
