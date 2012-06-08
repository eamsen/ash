// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lcp.h"
#include <sstream>

using std::string;
using std::stringstream;

namespace ash {

Equation::Equation(const Type type, const int constant)
    : type_(type),
      constant_(constant) {}

int Equation::AddSummand(const int coefficient, const int var_id) {
  assert(coefficients_.size() == variables_.size());
  coefficients_.push_back(coefficient);
  variables_.push_back(var_id);
  return variables_.size() - 1;
}

void Equation::type(const Type type) {
  type_ = type;
}

int Equation::size() const {
  assert(variables_.size() == coefficients_.size());
  return variables_.size();
}

string Equation::str() const {
  stringstream ss;
  for (int i = 0; i < size(); ++i) {
    if (i != 0) {
      ss << " ";
    }
    if (coefficients_[i] >= 0) {
      ss << "+";
    }
    if (coefficients_[i] != 1) {
      ss << coefficients_[i] << "*";
    }
    ss << "v" << variables_[i];
  }
  if (type_ == kEqual) { ss << " = "; }
  else if (type_ == kLessEqual) { ss << " <= "; }
  else if (type_ == kLess) { ss << " < "; }
  else if (type_ == kGreaterEqual) { ss << " >= "; }
  else if (type_ == kGreater) { ss << " > "; }
  else if (true) { ss << " ? "; }
  ss << constant_ << ";";
  return ss.str();
}

Objective::Objective(const Type type)
    : Equation(kEqual, 0),
      type_(type) {}

string Objective::str() const {
  stringstream ss;
  if (type_ == kMin) {
    ss << "min: ";
  } else {
    ss << "max: ";
  }
  for (int i = 0; i < size(); ++i) {
    if (i != 0) {
      ss << " ";
    }
    if (coefficients_[i] >= 0) {
      ss << "+";
    }
    if (coefficients_[i] != 1) {
      ss << coefficients_[i] << "*";
    }
    ss << "v" << variables_[i];
  }
  ss << ";";
  return ss.str();
}

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

const Objective& Lcp::objective(const int id) const {
  assert(id >= 0 && id < num_objectives());
  return objectives_[id];
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
