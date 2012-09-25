// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./game-factory.h"
#include <string>
#include <vector>
#include "./parser.h"
#include "./game.h"

using std::string;
using std::vector;

namespace ash {

Game GameFactory::Create(const parse::StrategicGame& parsed_game) {
  Game game(parsed_game.name);
  // Adding players and their strategies.
  const int num_parsed_players = parsed_game.players.size();
  for (int i = 0; i < num_parsed_players; ++i) {
    const string& player_name = parsed_game.players[i];
    Player player(player_name);
    const int num_parsed_strategies = parsed_game.strategies[i].size();
    for (int j = 0; j < num_parsed_strategies; ++j) {
      const string& strategy_name = parsed_game.strategies[i][j];
      const int s_id = game.AddStrategy(strategy_name);
      player.AddStrategy(s_id);
    }
    assert(player.num_strategies() == num_parsed_strategies);
    int p_id = game.AddPlayer(player);
    assert(p_id == game.num_players() - 1);
  }
  assert(game.num_players() == num_parsed_players);
  // Adding outcomes.
  Outcome null_outcome("null", vector<int>(game.num_players(), 0));
  game.AddOutcome(null_outcome);
  const int num_parsed_outcomes = parsed_game.outcomes.size();
  for (int i = 0; i < num_parsed_outcomes; ++i) {
    const parse::Outcome& o = parsed_game.outcomes[i];
    Outcome outcome(o.name, o.payoffs);
    int o_id = game.AddOutcome(outcome);
    assert(o_id == game.num_outcomes() - 1);
  }
  assert(game.num_outcomes() == num_parsed_outcomes + 1);
  // Adding strategies and their payoffs.
  const int num_profiles = game.num_strategy_profiles();
  assert(num_profiles == static_cast<int>(parsed_game.payoff_indices.size()));
  for (int i = 0; i < num_profiles; ++i) {
    const int outcome_id = parsed_game.payoff_indices[i];
    game.SetPayoff(i, outcome_id);
  }
  return game;
}

}  // namespace ash
