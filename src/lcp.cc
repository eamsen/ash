// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lcp.h"
#include <cassert>
#include <sstream>

using std::string;
using std::stringstream;

namespace ash {

const int Lcp::kInvalidId = -1;

Lcp::Lcp()
    : active_objective_(kInvalidId) {}

int Lcp::AddVariable(const string& var) {
  variables_.push_back(var);
  return variables_.size() - 1;
}

int Lcp::AddEquation(const Equation& e) {
  equations_.push_back(e);
  return equations_.size() - 1;
}

int Lcp::AddComplEquations(const Equation& e1, const Equation& e2) {
  compl_equations_.push_back(e1);
  compl_equations_.push_back(e2);
  compl_active_.push_back(true);
  compl_active_.push_back(true);
  return compl_equations_.size() - 2;
}

void Lcp::SelectEquation(const int id, const bool e1, const bool e2) {
  assert(id >= 0 && id < num_complementary() * 2 - 1);
  compl_active_[id] = e1;
  compl_active_[id + 1] = e2;
  assert(e1 != e2);
}

int Lcp::AddObjective(const Objective& o) {
  objectives_.push_back(o);
  return objectives_.size() - 1;
}

void Lcp::SelectObjective(const int id) {
  assert(id >= 0 && id < num_objectives());
  active_objective_ = id;
}

void Lcp::UnselectObjective() {
  active_objective_ = kInvalidId;
}

bool Lcp::has_objective() const {
  return active_objective_ != kInvalidId;
}

const Objective& Lcp::objective(const int id) const {
  assert(id >= 0 && id < num_objectives());
  return objectives_[id];
}

const Objective& Lcp::selected_objective() const {
  assert(active_objective_ != kInvalidId);
  return objective(active_objective_);
}

const Equation& Lcp::equation(const int id) const {
  assert(id >= 0 && id < num_linear());
  return equations_[id];
}

const Equation& Lcp::selected_equation(const int id) const {
  assert(id >= 0 && id < num_complementary());
  const int index = id * 2;
  assert(compl_active_[index] != compl_active_[index + 1]);
  if (compl_active_[index]) {
    return compl_equations_[index];
  }
  return compl_equations_[index + 1];
}


int Lcp::num_linear() const {
  return equations_.size();
}

int Lcp::num_complementary() const {
  return compl_equations_.size() / 2;
}

int Lcp::num_objectives() const {
  return objectives_.size();
}

int Lcp::num_variables() const {
  return variables_.size();
}

string Lcp::str() const {
  stringstream ss;
  if (active_objective_ != kInvalidId) {
    ss << objective(active_objective_).str() << "\n";
  } else {
    ss << "min: ;\n";
  }
  for (auto it = equations_.begin(); it != equations_.end(); ++it) {
    ss << it->str() << "\n";
  }
  assert(compl_equations_.size() % 2 == 0);
  for (size_t i = 0; i < compl_equations_.size(); ++i) {
    if (compl_active_[i]) {
      ss << compl_equations_[i].str();
    }
    if (compl_active_[i++] && compl_active_[i]) {
      ss << "  v  ";
    }
    if (compl_active_[i]) {
      ss << compl_equations_[i].str();
    }
    ss << "\n";
  }
  return ss.str();
}

}  // namespace ash
