#include "./lp-solver.h"
#include <lpsolve/lp_lib.h>
#include "./lcp.h"

namespace ash {

LpSolver::LpSolver(const Lcp& lcp)
    : lcp_(lcp) {}

bool LpSolver::Solve() {
  lprec* lp;
  return false;
}

}  // namespace ash
