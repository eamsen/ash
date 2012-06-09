// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lp-solver.h"
#include <lpsolve/lp_lib.h>
#include <cassert>
#include <vector>
#include "./lcp.h"
#include "./equation.h"

using std::vector;
using base::Clock;

namespace ash {

static int ConstraintType(const Equation::Type type) {
  if (type == Equation::kEqual) {
    return EQ;
  } else if (type == Equation::kGreaterEqual) {
    return GE;
  } else if (type == Equation::kLessEqual) {
    return LE;
  }
  assert(false && "Constraint type is not supported");
}

LpSolver::LpSolver(const Lcp& lcp)
    : lcp_(lcp) {
  Reset();
}

bool LpSolver::Solve() {
  Reset();
  Clock beg;
  const int num_vars = lcp_.num_variables();
  lprec* lp = make_lp(0, num_vars);
  assert(lp);
  set_add_rowmode(lp, true);
  const int num_linear = lcp_.num_linear();
  for (int e = 0; e < num_linear; ++e) {
    const Equation& eq = lcp_.equation(e);
    AddEquation(eq, lp);
  }
  const int num_compl = lcp_.num_complementary();
  for (int e = 0; e < num_compl; ++e) {
    const Equation& eq = lcp_.selected_equation(e);
    AddEquation(eq, lp);
  }
  set_add_rowmode(lp, false);
  if (lcp_.has_objective()) {
    const Objective& obj = lcp_.selected_objective();
    AddObjective(obj, lp);
  }
  set_verbose(lp, NEUTRAL);
  const int result = solve(lp);
  delete_lp(lp);
  duration_ = Clock() - beg;
  return result == OPTIMAL;
}

bool LpSolver::AddEquation(const Equation& e, lprec* lp) {
  vector<int> vars;
  vector<double> coeffs;
  const int num_summands = e.size();
  for (int su = 0; su < num_summands; ++su) {
    vars.push_back(e.variable(su));
    coeffs.push_back(e.coefficient(su));
  }
  const int type = ConstraintType(e.type());
  const bool ok = add_constraintex(lp, num_summands, &coeffs[0], &vars[0],
                                   type, e.constant());
  assert(ok);
  return ok;
}

bool LpSolver::AddObjective(const Objective& obj, lprec* lp) {
  vector<int> vars;
  vector<double> coeffs;
  const int num_summands = obj.size();
  for (int su = 0; su < num_summands; ++su) {
    vars.push_back(obj.variable(su));
    coeffs.push_back(obj.coefficient(su));
  }
  const bool ok = set_obj_fnex(lp, num_summands, &coeffs[0], &vars[0]);
  assert(ok);
  if (obj.type() == Objective::kMin) {
    set_minim(lp);
  } else {
    set_maxim(lp);
  }
  return ok;
}

void LpSolver::Reset() {
  duration_ = 0;
}

}  // namespace ash
