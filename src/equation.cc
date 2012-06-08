// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./equation.h"
#include <cassert>
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

int Equation::constant() const {
  return constant_;
}

int Equation::coefficient(const int id) const {
  assert(id >= 0 && id < size());
  return coefficients_[id];
}

int Equation::variable(const int id) const {
  assert(id >= 0 && id < size());
  return variables_[id];
}

void Equation::type(const Type type) {
  type_ = type;
}

Equation::Type Equation::type() const {
  return type_;
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

}  // namespace ash
