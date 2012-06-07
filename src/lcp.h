// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_LCP_H_
#define SRC_LCP_H_

#include <cassert>
#include <string>
#include <vector>

namespace ash {

struct Equation {
  enum Type { kEqual, kLessEqual, kLess, kGreaterEqual, kGreater };

  Equation(const Type type, const int constant)
      : type(type),
        constant(constant) {}

  int AddSummand(const int coefficient, const std::string& variable) {
    assert(variable.size());
    assert(coefficients.size() == variables.size());
    coefficients.push_back(coefficient);
    variables.push_back(variable);
    return variable.size() - 1;
  }

  std::string str() const;
  int size() const;

  Type type;
  int constant;
  std::vector<int> coefficients;
  std::vector<std::string> variables;
  bool slack;
};

class Lcp {
 public:
  int AddEquation(const Equation& e);
  int SetComplementary(const int eq1_id, const int eq2_id);
  int num_linear() const;
  std::string str() const;

 private:
  std::vector<Equation> equations_;
  std::vector<int> complementary_;
};

}  // namespace ash
#endif  // SRC_LCP_H_

