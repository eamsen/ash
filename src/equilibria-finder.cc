// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./equilibria-finder.h"
#include <cassert>
#include "./game.h"

using std::string;
using std::vector;

namespace ash {

EquilibriaFinder::EquilibriaFinder(const Game& game)
    : game_(game) {}

int EquilibriaFinder::Find() {
  int total_strategies = 0;
  const int num_players = game_.num_players();
  vector<vector<bool> > is_strictly_dominated(num_players);
  for (int p = 0; p < num_players; ++p) {
    const Player& player = game_.player(p);
    is_strictly_dominated[p].resize(player.num_strategies(), false);
    total_strategies += player.num_strategies();
  }
  int num_dominated = 0;
  while (total_strategies - num_dominated > num_players) {
    for (int p1 = 0; p1 < num_players; ++p1) {
      const Player& player1 = game_.player(p1);
      for (int s1 = 0; s1 < player1.num_strategies(); ++s1) {
        const vector<int>& payoffs1 = game_.payoffs(p1, s1);
        for (int s2 = 0; s2 < player1.num_strategies(); ++s2) {
          const vector<int>& payoffs2 = game_.payoffs(p1, s2);
          const size_t payoffs_size = payoffs1.size();
          assert(payoffs_size == payoffs2.size());
          size_t i = 0;
          while (i < payoffs_size && payoffs1[i] < payoffs2[i]) {
            ++i;
          }
          if (i == payoffs_size) {
            // s1 is strictly dominated by s2.
            is_strictly_dominated[p1][s1] = true;
            break;
          }
        }
      }
    }
  }
  return 0;
}

const vector<StrategyProfile>& EquilibriaFinder::equilibria() const {
  return equilibria_;
}

}  // namespace ash
