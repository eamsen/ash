// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lcp-factory.h"
#include <lpsolve/lp_lib.h>
#include <string>
#include <vector>
#include <sstream>
#include "./lcp.h"
#include "./game.h"

using std::string;
using std::vector;
using std::stringstream;

namespace ash {

Lcp LcpFactory::Create(const Game& game) {
  Lcp lcp;
  vector<vector<string> > player_vars(game.num_players());
  for (int p = 0; p < game.num_players(); ++p) {
    const Player& player = game.player(p);
    assert(p <= 'z' - static_cast<int>('a'));
    string player_var;
    player_var += 'a' + p;
    vector<string> vars;
    for (int s = 0; s < player.num_strategies(); ++s) {
      stringstream var;
      var << player_var << s;
      Equation e(Equation::kGreaterEqual, 0);
      e.AddSummand(1, var.str());
      lcp.AddEquation(e);
      vars.push_back(var.str());
    }
    Equation e(Equation::kEqual, 1);
    for (auto it = vars.begin(); it != vars.end(); ++it) {
      e.AddSummand(1, *it);
    }
    lcp.AddEquation(e);
    player_vars[p].swap(vars);
  }

  for (int p = 0; p < game.num_players(); ++p) {
    const Player& player = game.player(p);
    for (int s = 0; s < player.num_strategies(); ++s) {
      vector<int> profile(game.num_players(), 0);
      profile[p] = s;
      for (int p2 = 0; p2 < game.num_players(); ++p2) {
        if (p2 == p) {
          continue;
        }
        const Player& player2 = game.player(p2);
        for (int s2 = 0; s2 < player2.num_strategies(); ++s2) {
          profile[p2] = s2;
        }
        profile[p2] = 0;
      }
    }
  }
  return lcp;
}

}  // namespace ash
