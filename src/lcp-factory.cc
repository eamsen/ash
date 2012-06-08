// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./lcp-factory.h"
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
  return Create(game, NULL);
}

Lcp LcpFactory::Create(const Game& game, vector<vector<int> >* _compl_map) {
  Lcp lcp;
  const bool zero_sum_game = game.zero_sum();
  const int num_players = game.num_players();
  vector<vector<int> > compl_map(num_players);
  vector<vector<int> > player_vars(num_players);
  for (int p = 0; p < num_players; ++p) {
    const Player& player = game.player(p);
    const int payoff_var_id = lcp.AddVariable(CreatePlayerPayoffVar(p));
    if (zero_sum_game) {
      Objective obj(Objective::kMin);
      obj.AddSummand(1, payoff_var_id);
      const int obj_id = lcp.AddObjective(obj);
      assert(obj_id == p);
    }
    const int num_player_strategies = player.num_strategies();
    compl_map[p].resize(num_player_strategies, Game::kInvalidId);
    player_vars[p].resize(num_player_strategies, Game::kInvalidId);
    for (int s = 0; s < num_player_strategies; ++s) {
      if (player_vars[p][s] == Game::kInvalidId) {
        player_vars[p][s] = lcp.AddVariable(CreatePlayerMixedVar(p, s));
      }
      const int var_id = player_vars[p][s];
      Equation e(Equation::kGreaterEqual, 0);
      e.AddSummand(1, var_id);
      lcp.AddEquation(e);
      Equation e2(Equation::kGreaterEqual, 0);
      e2.AddSummand(1, payoff_var_id);
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
          if (player_vars[p2].empty()) {
            player_vars[p2].resize(game.num_strategies(p2), Game::kInvalidId);
          }
          if (player_vars[p2][profile[p2]] == Game::kInvalidId) {
            player_vars[p2][profile[p2]] =
              lcp.AddVariable(CreatePlayerMixedVar(p2, profile[p2]));
          }
          e2.AddSummand(-1 * p_payoff, player_vars[p2][profile[p2]]);
        }
      }
      lcp.AddEquation(e2);
      if (!zero_sum_game) {
        e.type(Equation::kEqual);
        e2.type(Equation::kEqual);
        int compl_id = lcp.AddComplEquations(e, e2);
        assert(compl_map[p][s] == Game::kInvalidId);
        compl_map[p][s] = compl_id;
      }
    }
    Equation e(Equation::kEqual, 1);
    for (auto it = player_vars[p].begin(), end = player_vars[p].end();
         it != end; ++it) {
      e.AddSummand(1, *it);
    }
    lcp.AddEquation(e);
    // player_vars[p].swap(vars);
  }
  if (_compl_map) {
    compl_map.swap(*_compl_map);
  }
  return lcp;
}

}  // namespace ash
