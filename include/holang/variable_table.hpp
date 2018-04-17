#pragma once

#include <string>
#include <vector>

namespace holang {
class VariableTable {
private:
  class Table {
  public:
    Table(Table *prev) : vec({"self"}), prev(prev) {}
    int get(const std::string &ident) {
      auto iter = find(vec.begin(), vec.end(), ident);
      if (iter == vec.end()) {
        vec.push_back(ident);
        return vec.size() - 1;
      } else {
        return distance(vec.begin(), iter);
      }
    }
    Table *get_prev() { return prev; }
    int size() { return vec.size(); }

  private:
    std::vector<const std::string> vec;
    Table *prev;
  };

public:
  VariableTable() : current(new Table(nullptr)) {}
  ~VariableTable() {
    while (current != nullptr) {
      Table *trash = current;
      current = current->get_prev();
      delete trash;
    }
  }
  int get(const std::string &ident) { return current->get(ident); }
  void next() { current = new Table(current); }
  void prev() {
    Table *trash = current;
    current = current->get_prev();
    delete trash;
  }
  int size() { return current->size(); }

private:
  Table *current;
};
} // namespace holang
