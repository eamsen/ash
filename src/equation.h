// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_EQUATION_H_
#define SRC_EQUATION_H_

#include <string>
#include <vector>

namespace ash {

class Equation {
 public:
  enum Type { kEqual, kLessEqual, kLess, kGreaterEqual, kGreater };

  Equation(const Type type, const int constant);
  int AddSummand(const int coefficient, const int var_id);
  void type(const Type type);
  Type type() const;
  int constant() const;
  int coefficient(const int id) const;
  int variable(const int id) const;
  std::string str() const;
  int size() const;

 protected:
  Type type_;
  int constant_;
  std::vector<int> coefficients_;
  std::vector<int> variables_;
};

class Objective : public Equation {
 public:
  enum Type { kMin, kMax };

  explicit Objective(const Type type);
  Type type() const;
  std::string str() const;

 private:
  Type type_;
};

}  // namespace ash
#endif  // SRC_EQUATION_H_
