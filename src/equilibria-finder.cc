// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./equilibria-finder.h"
#include <cassert>
#include <limits>
#include <algorithm>
#include "./game.h"
#include "./lcp.h"
#include "./lcp-factory.h"
#include "./lp-solver.h"

using std::string;
using std::vector;
using std::numeric_limits;
using std::max;
using base::Clock;

namespace ash {

EquilibriaFinder::EquilibriaFinder(const Game& game)
    : game_(game),
      max_num_equilibria_(numeric_limits<int>::max()) {
  Reset();
}

int EquilibriaFinder::FindPure() {
  Reset();
  Clock beg;
  const int num_players = game_.num_players();
  const int num_profiles = game_.num_strategy_profiles();
  for (int sp = 0; sp < num_profiles; ++sp) {
    StrategyProfile profile = game_.CreateProfile(sp);
    const vector<int>& payoff = game_.payoff(profile);
    bool equilibrium = true;
    for (int p = 0; p < num_players && equilibrium; ++p) {
      // Remember the original strategy played.
      const int org_s = profile.strategy(p);
      const Player& player = game_.player(p);
      const int num_strategies = player.num_strategies();
      for (int s = 0; s < num_strategies && equilibrium; ++s) {
        // Switch the strategy for player p to strategy s.
        profile.strategy(p, s);
        const vector<int>& payoff2 = game_.payoff(profile);
        if (payoff2[p] > payoff[p]) {
          // Player p increases payoff by switching to strategy s, therefore the
          // strategy profile is not a Nash equilibrium.
          equilibrium = false;
        }
      }
      // Revert the tested strategy back for player p.
      profile.strategy(p, org_s);
    }
    if (equilibrium) {
      equilibria_.push_back(profile);
      if (equilibria_.size() >= max_num_equilibria_) {
        break;
      }
    }
  }
  duration_ = Clock() - beg;
  return equilibria_.size();
}

int EquilibriaFinder::FindMixed() {
  Reset();
  Clock beg;
  vector<vector<int> > compl_map;
  vector<vector<int> > player_vars;
  Lcp lcp = LcpFactory::Create(game_, &compl_map, &player_vars);
  lcp_duration_ = Clock() - beg;
  const int num_players = game_.num_players();
  assert(static_cast<int>(compl_map.size()) == num_players);
  if (game_.zero_sum()) {
    // Zero-sum game.
    for (int p = 0; p < num_players; ++p) {
      lcp.SelectObjective(p);
      LpSolver lp_solver(lcp);
      const bool solved = lp_solver.Solve();
      lp_duration_ += lp_solver.duration();
      if (solved) {
        AddMixedEquilibrium(lp_solver.solution(), player_vars);
        if (equilibria_.size() >= max_num_equilibria_) {
          break;
        }
      }
    }
  } else {
    // Non-zero-sum game.
    vector<uint32_t> supports(num_players, 1u);
    supports[0] = 0u;  // We use this only to kick off the permutations.
    while (NextSupports(&supports)) {
      for (int p = 0; p < num_players; ++p) {
        const int num_strategies = game_.num_strategies(p);
        assert(static_cast<int>(compl_map[p].size()) == num_strategies);
        for (int s = 0; s < num_strategies; ++s) {
          const int compl_fun_id = compl_map[p][s];
          if (compl_fun_id != Lcp::kInvalidId) {
            const uint32_t mask = s + 1u;
            const int supported = (supports[p] & mask) != 0u;
            lcp.SelectEquation(compl_fun_id, !supported, supported);
          }
        }
      }
      Clock lp_beg;
      LpSolver lp_solver(lcp);
      const bool solved = lp_solver.Solve();
      lp_duration_ += Clock() - lp_beg;
      if (solved) {
        AddMixedEquilibrium(lp_solver.solution(), player_vars);
        if (equilibria_.size() >= max_num_equilibria_) {
          break;
        }
      }
    }
  }
  duration_ = Clock() - beg;
  return mixed_equilibria_.size();
}

bool EquilibriaFinder::NextSupports(vector<uint32_t>* supports) const {
  assert(supports && static_cast<int>(supports->size()) == game_.num_players());
  const int num_players = game_.num_players();
  int i = 0u;
  while (i < num_players &&
         (*supports)[i] == pow(2, game_.num_strategies(i)) - 1) {
    (*supports)[i++] = 1u;
  }
  if (i < num_players) {
    ++(*supports)[i];
    return true;
  }
  return false;
}

void EquilibriaFinder::AddMixedEquilibrium(const vector<double>& probs,
                                     const vector<vector<int> >& player_vars) {
  const int num_players = player_vars.size();
  MixedStrategyProfile profile(num_players);
  for (int p = 0; p < num_players; ++p) {
    const vector<int>& strategies = player_vars[p];
    const int num_strategies = strategies.size();
    profile.SetNumStrategies(p, num_strategies);
    for (int s = 0; s < num_strategies; ++s) {
      profile.AddProbability(p, s, probs[player_vars[p][s]]);
    }
  }
  mixed_equilibria_.push_back(profile);
}

void EquilibriaFinder::Reset() {
  equilibria_.clear();
  mixed_equilibria_.clear();
  duration_ = 0;
  lcp_duration_ = 0;
  lp_duration_ = 0;
}

void EquilibriaFinder::max_num_equilibria(const int max_num) {
  max_num_equilibria_ = max(1, max_num);
}

int EquilibriaFinder::max_num_equilibria() const {
  return max_num_equilibria_;
}

const Game& EquilibriaFinder::game() const {
  return game_;
}

const vector<StrategyProfile>& EquilibriaFinder::equilibria() const {
  return equilibria_;
}

const vector<MixedStrategyProfile>& EquilibriaFinder::mixed_equilibria() const {
  return mixed_equilibria_;
}

Clock::Diff EquilibriaFinder::duration() const {
  return duration_;
}

Clock::Diff EquilibriaFinder::lcp_duration() const {
  return lcp_duration_;
}

Clock::Diff EquilibriaFinder::lp_duration() const {
  return lp_duration_;
}

}  // namespace ash
