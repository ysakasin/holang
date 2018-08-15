#pragma once

#include <string>
#include <vector>

namespace holang {
class VariableTable {
private:
  class Table {
  public:
    Table(Table *prev) : vec({"self"}), prev(prev) {}
    std::pair<int, int> find(const std::string &ident, int depth = 0) const {
      auto iter = std::find(vec.begin(), vec.end(), ident);
      if (iter == vec.end()) {
        if (prev == nullptr) {
          return make_pair(-1, -1);
        } else {
          return prev->find(ident, depth + 1);
        }
      } else {
        int pos = distance(vec.begin(), iter);
        return make_pair(depth, pos);
      }
    }
    int insert(const std::string &ident) {
      vec.push_back(ident);
      return vec.size() - 1;
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
  std::pair<int, int> find(const std::string &ident) const {
    return current->find(ident);
  }
  void insert(const std::string &ident) { current->insert(ident); }
  std::pair<int, int> insert_if_absent(const std::string &ident) {
    auto res = find(ident);
    if (res.first >= 0) {
      return res;
    } else {
      int pos = current->insert(ident);
      return make_pair(0, pos);
    }
  }
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
