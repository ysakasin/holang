#pragma once

#include "holang/instruction.hpp"
#include <string>
#include <vector>

namespace holang {
class Object;
struct Func;

union Code {
  Instruction op;
  int ival;
  double dval;
  bool bval;
  std::string *sval;
  Func *funcval;
  Object *objval;
};

class CodeSequence {
public:
  CodeSequence() {}
  CodeSequence(const CodeSequence &src)
      : source_path(src.source_path), sequence(src.sequence) {}
  CodeSequence(const std::string &source_path) : source_path(source_path) {}

  void append(Instruction op) {
    Code code;
    code.op = op;
    sequence.push_back(code);
  }

  void append(int ival) {
    Code code;
    code.ival = ival;
    sequence.push_back(code);
  }

  void append(double dval) {
    Code code;
    code.dval = dval;
    sequence.push_back(code);
  }

  void append(bool bval) {
    Code code;
    code.bval = bval;
    sequence.push_back(code);
  }

  void append(std::string *sval) {
    Code code;
    code.sval = sval;
    sequence.push_back(code);
  }

  void append(Func *f) {
    Code code;
    code.funcval = f;
    sequence.push_back(code);
  }

  void append(Object *o) {
    Code code;
    code.objval = o;
    sequence.push_back(code);
  }

  std::vector<Code> get_sequence() const { return sequence; }
  size_t size() const { return sequence.size(); }
  Code &at(size_t index) { return sequence[index]; }

  const std::string source_path;

private:
  std::vector<Code> sequence;
};
} // namespace holang
