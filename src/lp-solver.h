// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_LP_SOLVER_H_
#define SRC_LP_SOLVER_H_

#include "./equation.h"

namespace ash {

class Lcp;

class LpSolver {
 public:
  static int ConstraintType(const Equation::Type type);

  explicit LpSolver(const Lcp& lcp);
  bool Solve();

 private:
  const Lcp& lcp_;
};

}  // namespace ash
#endif  // SRC_LP_SOLVER_H_
