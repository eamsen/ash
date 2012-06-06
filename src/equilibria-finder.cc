// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./equilibria-finder.h"
#include <cassert>
#include <limits>
#include <algorithm>
#include "./game.h"

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
        // Switch the strategy for one player.
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
  return 0;
}

void EquilibriaFinder::Reset() {
  equilibria_.clear();
  duration_ = 0;
}

void EquilibriaFinder::max_num_equilibria(const int max_num) {
  max_num_equilibria_ = max(0, max_num);
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

Clock::Diff EquilibriaFinder::duration() const {
  return duration_;
}

}  // namespace ash
