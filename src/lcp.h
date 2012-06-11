// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_LCP_H_
#define SRC_LCP_H_

#include <string>
#include <vector>
#include "./equation.h"

namespace ash {

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

  bool has_objective() const;

  const std::string& variable(const int id) const;
  const Objective& objective(const int id) const;
  const Objective& selected_objective() const;
  const Equation& equation(const int id) const;
  const Equation& selected_equation(const int id) const;

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

