// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_LCP_H_
#define SRC_LCP_H_

#include <cassert>
#include <string>
#include <vector>

namespace ash {

class Equation {
 public:
  enum Type { kEqual, kLessEqual, kLess, kGreaterEqual, kGreater };

  Equation(const Type type, const int constant);
  int AddSummand(const int coefficient, const std::string& variable);
  void type(const Type type);
  std::string str() const;
  int size() const;
 
 private:
  Type type_;
  int constant_;
  std::vector<int> coefficients_;
  std::vector<std::string> variables_;
};

class Lcp {
 public:
  int AddEquation(const Equation& e);
  int AddComplEquations(const Equation& e1, const Equation& e2);
  int num_linear() const;
  int num_complementary() const;
  std::string str() const;

 private:
  std::vector<Equation> equations_;
  std::vector<Equation> compl_equations_;
};

}  // namespace ash
#endif  // SRC_LCP_H_

