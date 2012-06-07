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
    if (coefficients[i] >= 0) {
      ss << "+";
    }
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
  else if (true) { ss << " ? "; }
  ss << constant << ";";
  return ss.str();
}

int Lcp::AddEquation(const Equation& e) {
  equations_.push_back(e);
  return equations_.size() - 1;
}

int Lcp::SetComplementary(const int eq_id1, const int eq_id2) {
  assert(eq_id1 >= 0 && eq_id1 < num_linear());
  assert(eq_id2 >= 0 && eq_id2 < num_linear());
  complementary_.push_back(eq_id1);
  complementary_.push_back(eq_id2);
  return complementary_.size() - 2;
}

int Lcp::num_linear() const {
  return equations_.size();
}

string Lcp::str() const {
  stringstream ss;
  for (auto it = equations_.begin(); it != equations_.end(); ++it) {
    ss << it->str() << "\n";
  }
  const int num_compl = complementary_.size();
  assert(num_compl % 2 == 0);
  for (int i = 0; i < num_compl; ++i) {
    ss << equations_[complementary_[i]].str();
    ss << "  v  " << equations_[complementary_[++i]].str() << "\n";
  }
  return ss.str();
}

}  // namespace ash
