// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lcp.h"
#include <sstream>

using std::string;
using std::stringstream;

namespace ash {

Equation::Equation(const Type type, const int constant)
    : type_(type),
      constant_(constant) {}

int Equation::AddSummand(const int coefficient, const std::string& variable) {
  assert(variable.size());
  assert(coefficients_.size() == variables_.size());
  coefficients_.push_back(coefficient);
  variables_.push_back(variable);
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
    ss << variables_[i];
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

int Lcp::AddEquation(const Equation& e) {
  equations_.push_back(e);
  return equations_.size() - 1;
}

int Lcp::AddComplEquations(const Equation& e1, const Equation& e2) {
  compl_equations_.push_back(e1);
  compl_equations_.push_back(e2);
  return compl_equations_.size() - 2;
}

int Lcp::num_linear() const {
  return equations_.size();
}

int Lcp::num_complementary() const {
  return compl_equations_.size() / 2;
}

string Lcp::str() const {
  stringstream ss;
  for (auto it = equations_.begin(); it != equations_.end(); ++it) {
    ss << it->str() << "\n";
  }
  assert(num_complementary() % 2 == 0);
  for (auto it = compl_equations_.begin(); it != compl_equations_.end(); ++it) {
    ss << it->str() << "  v  ";
    ss << (++it)->str() << "\n";
  }
  return ss.str();
}

}  // namespace ash
