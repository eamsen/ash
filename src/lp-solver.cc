// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lp-solver.h"
#include <lpsolve/lp_lib.h>
#include <cassert>
#include <vector>
#include "./lcp.h"

using std::vector;

namespace ash {

LpSolver::LpSolver(const Lcp& lcp)
    : lcp_(lcp) {}

bool LpSolver::Solve() {
  const int num_vars = lcp_.num_variables();
  lprec* lp = make_lp(0, num_vars);
  assert(lp);
  // vector<int> vars(num_variables, 0);
  // vector<float> coefficients(num_variables, 0);
  set_add_rowmode(lp, true);
  const int num_linear = lcp_.num_linear();
  for (int e = 0; e < num_linear; ++e) {
    vector<int> vars;
    vector<double> coeffs;
    const Equation& eq = lcp_.equation(e);
    const int num_summands = eq.size();
    for (int su = 0; su < num_summands; ++su) {
      vars.push_back(eq.variable(su));
      coeffs.push_back(eq.coefficient(su));
    }
    const int type = ConstraintType(eq.type());
    const bool ok = add_constraintex(lp, num_summands, &coeffs[0], &vars[0],
                                     type, eq.constant());
    assert(ok);
  }
  delete_lp(lp);
  return false;
}

int LpSolver::ConstraintType(const Equation::Type type) {
  if (type == Equation::kEqual) {
    return EQ;
  } else if (type == Equation::kGreaterEqual) {
    return GE;
  } else if (type == Equation::kLessEqual) {
    return LE;
  }
  assert(false && "Constraint type is not supported");
}

}  // namespace ash
