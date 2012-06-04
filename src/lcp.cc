// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lcp.h"
#include <sstream>

using std::string;
using std::stringstream;

namespace ash {

int Equation::size() const {
  assert(variables.size() == coefficients.size());
  return variables.size();
}

string Equation::str() const {
  stringstream ss; 
  for (int i = 0; i < size(); ++i) {
    if (i != 0) {
      ss << " ";
    }
    ss << (coefficients[i] >= 0 ? "+ " : "- ");
    if (coefficients[i] != 1) {
      ss << coefficients[i] << "*";
    }
    ss << variables[i];
  }
  if (type == kEqual) { ss << " = "; }
  else if (type == kLessEqual) { ss << " <= "; }
  else if (type == kLess) { ss << " < "; }
  else if (type == kGreaterEqual) { ss << " >= "; }
  else if (type == kGreater) { ss << " > "; }
  else { ss << " ? "; }

  ss << constant << ";";
  return ss.str();
}

int Lcp::AddEquation(const Equation& e) {
  equations_.push_back(e);
  return equations_.size() - 1;
}

string Lcp::str() const {
  stringstream ss;
  for (auto it = equations_.begin(); it != equations_.end(); ++it) {
    ss << it->str() << "\n";
  }
  return ss.str();
}

}  // namespace ash
