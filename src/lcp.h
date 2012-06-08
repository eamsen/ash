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
  int AddSummand(const int coefficient, const int var_id);
  void type(const Type type);
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

  Objective(const Type type);
  std::string str() const;

 private:
  Type type_;
};

class Lcp {
 public:
  static const int kInvalidId;

  Lcp();
  int AddVariable(const std::string& var);
  int AddEquation(const Equation& e);
  int AddComplEquations(const Equation& e1, const Equation& e2);
  int AddObjective(const Objective& o);
  void SelectEquation(const int id, const bool e1, const bool e2);
  void SelectObjective(const int id);
  void UnselectObjective();
  const Objective& objective(const int id) const;
  int num_linear() const;
  int num_complementary() const;
  int num_objectives() const;
  int num_variables() const;
  std::string str() const;

 private:
  std::vector<std::string> variables_;
  std::vector<Equation> equations_;
  std::vector<Equation> compl_equations_;
  std::vector<bool> compl_active_;
  std::vector<Objective> objectives_;
  int active_objective_;
};

}  // namespace ash
#endif  // SRC_LCP_H_

