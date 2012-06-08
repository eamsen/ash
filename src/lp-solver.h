// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_LP_SOLVER_H_
#define SRC_LP_SOLVER_H_

namespace ash {

class Lcp;

class LpSolver {
 public:
  explicit LpSolver(const Lcp& lcp);
  bool Solve();

 private:
  const Lcp& lcp_;
};

}  // namespace ash
#endif  // SRC_LP_SOLVER_H_
