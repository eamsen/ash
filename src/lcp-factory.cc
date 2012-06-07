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

string CreatePlayerMixedVar(const int player_id, const int strategy_id) {
  assert(player_id <= 'z' - static_cast<int>('a'));
  stringstream ss;
  ss << char('a' + player_id) << strategy_id;
  return ss.str();
}

string CreatePlayerPayoffVar(const int player_id) {
  assert(player_id <= 'Z' - static_cast<int>('A'));
  stringstream ss;
  ss << char('A' + player_id);
  return ss.str();
}

Lcp LcpFactory::Create(const Game& game) {
  Lcp lcp;
  const int num_players = game.num_players();
  vector<vector<string> > player_vars(num_players);
  for (int p = 0; p < num_players; ++p) {
    const Player& player = game.player(p);
    vector<string> vars;
    const int num_player_strategies = player.num_strategies();
    for (int s = 0; s < num_player_strategies; ++s) {
      const string var = CreatePlayerMixedVar(p, s);
      Equation e(Equation::kGreaterEqual, 0);
      e.AddSummand(1, var);
      const int e_id = lcp.AddEquation(e);
      vars.push_back(var);

      const string var2 = CreatePlayerPayoffVar(p);
      Equation e2(Equation::kGreaterEqual, 0);
      e2.AddSummand(1, var2);
      const int num_strategy_profiles = game.num_strategy_profiles();
      for (int sp = 0; sp < num_strategy_profiles; ++sp) {
        const StrategyProfile profile = game.CreateProfile(sp);
        if (profile[p] != s) {
          continue;
        }
        const int p_payoff = game.payoff(profile)[p];
        for (int p2 = 0; p2 < num_players; ++p2) {
          if (p2 == p) {
            continue;
          }
          const string var3 = CreatePlayerMixedVar(p2, profile[p2]);
          e2.AddSummand(-1 * p_payoff, var3);
        }
        const int e2_id = lcp.AddEquation(e2);
        lcp.SetComplementary(e_id, e2_id);
      }
    }
    Equation e(Equation::kEqual, 1);
    for (auto it = vars.begin(), end = vars.end(); it != end; ++it) {
      e.AddSummand(1, *it);
    }
    lcp.AddEquation(e);
    player_vars[p].swap(vars);
  }
  Equation e(Equation::kEqual, 0);
  e.AddSummand(1, "null");
  lcp.AddEquation(e);
  return lcp;
}

}  // namespace ash
