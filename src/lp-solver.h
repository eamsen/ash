// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_LP_SOLVER_H_
#define SRC_LP_SOLVER_H_

#include <vector>
#include "./clock.h"

struct _lprec;
typedef _lprec lprec;

namespace ash {

class Lcp;
class Equation;
class Objective;

class LpSolver {
 public:
  explicit LpSolver(const Lcp& lcp);
  bool Solve();
  bool AddEquation(const Equation& e, lprec* lp);
  bool AddObjective(const Objective& obj, lprec* lp);
  void Reset();

  const std::vector<double>& solution() const;
  base::Clock::Diff duration() const;

 private:
  const Lcp& lcp_;
  std::vector<double> solution_;
  base::Clock::Diff duration_;
};

}  // namespace ash
#endif  // SRC_LP_SOLVER_H_
