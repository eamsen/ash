// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./lp-solver.h"
#include <lpsolve/lp_lib.h>
#include <gflags/gflags.h>
#include <cassert>
#include <vector>
#include <iostream>
#include "./lcp.h"
#include "./equation.h"

using std::vector;
using base::Clock;

DECLARE_bool(verbose);

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
  if (FLAGS_verbose) {
    for (int i = 0; i < num_vars; ++i) {
      set_col_name(lp, i + 1, const_cast<char*>(lcp_.variable(i).c_str()));
    }
  }
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
  const bool solved = result == OPTIMAL;
  if (solved) {
    solution_.resize(num_vars, 0.0);
    get_variables(lp, &solution_[0]);
    if (FLAGS_verbose) {
      std::cout << lcp_.str();
      std::cout << "lp-solve input:\n";
      write_LP(lp, stdout);
      std::cout << "\nlp-solve solution:\n";
      for (int i = 0; i < num_vars; ++i) {
        std::cout << "(" << get_col_name(lp, i + 1)
                  << " " <<  solution_[i] << ")\n";
      }
      std::cout << "\n";
    }
  }
  delete_lp(lp);
  duration_ = Clock() - beg;
  return solved;
}

bool LpSolver::AddEquation(const Equation& e, lprec* lp) {
  vector<int> vars;
  vector<double> coeffs;
  const int num_summands = e.size();
  int used_summands = 0;
  for (int su = 0; su < num_summands; ++su) {
    const int coeff = e.coefficient(su);
    if (coeff != 0) {
      vars.push_back(e.variable(su) + 1);
      coeffs.push_back(e.coefficient(su));
      ++used_summands;
    }
  }
  const int type = ConstraintType(e.type());
  const bool ok = add_constraintex(lp, used_summands, &coeffs[0], &vars[0],
                                   type, e.constant());
  assert(ok);
  return ok;
}

bool LpSolver::AddObjective(const Objective& obj, lprec* lp) {
  vector<int> vars;
  vector<double> coeffs;
  const int num_summands = obj.size();
  for (int su = 0; su < num_summands; ++su) {
    vars.push_back(obj.variable(su) + 1);
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
  solution_.clear();
  duration_ = 0;
}

const vector<double>& LpSolver::solution() const {
  return solution_;
}

Clock::Diff LpSolver::duration() const {
  return duration_;
}

}  // namespace ash
