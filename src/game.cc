// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./game.h"
#include <cassert>
#include <cmath>
#include <sstream>
#include <algorithm>

using std::string;
using std::stringstream;
using std::vector;
using std::max;
using std::pow;

namespace ash {

Player::Player(const std::string& name)
    : name_(name) {}

int Player::AddStrategy(const int s_id) {
  strategies_.push_back(s_id);
  return strategies_.size() - 1;
}

int Player::strategy(const int index) const {
  assert(index >= 0 && index < static_cast<int>(strategies_.size()));
  return strategies_[index];
}

int Player::num_strategies() const {
  return strategies_.size();
}

Outcome::Outcome(const std::string& name, const std::vector<int>& payoffs)
    : payoffs_(payoffs),
      name_(name) {}

const vector<int>& Outcome::payoffs() const {
  return payoffs_;
}

StrategyProfile::StrategyProfile(const vector<int>& strategies)
    : strategies_(strategies) {}

StrategyProfile::StrategyProfile(const std::initializer_list<int>& strategies)
    : strategies_(strategies) {}

StrategyProfile::StrategyProfile(const int num_players,
                                 const int common_strategy_id)
    : strategies_(num_players, common_strategy_id) {}

int StrategyProfile::strategy(const int player_id) const {
  assert(player_id >= 0 && player_id < size());
  return strategies_[player_id];
}

int StrategyProfile::operator[](const int player_id) const {
  return strategy(player_id);
}

void StrategyProfile::strategy(const int player_id, const int strategy_id) {
  assert(player_id >= 0 && player_id < size());
  strategies_[player_id] = strategy_id;
}

int StrategyProfile::size() const {
  return strategies_.size();
}

string StrategyProfile::str() const {
  stringstream ss;
  ss << "(";
  for (int i = 0; i < size(); ++i) {
    if (i != 0) {
      ss << " ";
    }
    ss << strategy(i);
  }
  ss << ")";
  return ss.str();
}

string StrategyProfile::str(const Game& game) const {
  stringstream ss;
  ss << "(";
  for (int i = 0; i < size(); ++i) {
    if (i != 0) {
      ss << " ";
    }
    const Player& player = game.player(i);
    ss << game.strategy(player.strategy(strategy(i)));
  }
  ss << ")";
  return ss.str();
}

const int Game::kInvalidId = -1;

Game::Game(const std::string& name)
    : name_(name) {}

int Game::AddPlayer(const Player& p) {
  const int space = max(static_cast<size_t>(p.num_strategies()),
                        p.num_strategies() * payoff_indices_.size());
  payoff_indices_.resize(space, kInvalidId);
  players_.push_back(p);
  return players_.size() - 1;
}

int Game::AddStrategy(const std::string& name) {
  strategies_.push_back(name);
  return strategies_.size() - 1;
}

int Game::AddOutcome(const Outcome& o) {
  outcomes_.push_back(o);
  return outcomes_.size() - 1;
}

void Game::SetPayoff(const StrategyProfile& profile, const int outcome_id) {
  SetPayoff(StrategyProfileId(profile), outcome_id);
}

void Game::SetPayoff(const int sp_id, const int outcome_id) {
  assert(sp_id >= 0 && sp_id < num_strategy_profiles());
  payoff_indices_[sp_id] = outcome_id;
}

StrategyProfile Game::CreateProfile(const int sp_id) const {
  assert(sp_id >= 0 && sp_id < num_strategy_profiles());
  const int _num_players = num_players();
  StrategyProfile profile(_num_players, 0);
  int product = 1;
  for (int i = 0; i < _num_players; ++i) {
    const int num_player_strategies = player(i).num_strategies();
    profile.strategy(i, (sp_id / product) % num_player_strategies);
    product *= player(i).num_strategies();
  }
  return profile;
}

const Player& Game::player(const int id) const {
  assert(id >= 0 && id < num_players());
  return players_[id];
}

const string& Game::strategy(const int id) const {
  assert(id >= 0 && id < num_strategies());
  return strategies_[id];
}

const vector<int>& Game::payoff(const StrategyProfile& profile) const {
  assert(Valid(profile));
  const int sp_id = StrategyProfileId(profile);
  const vector<int>& payoffs = outcomes_[payoff_indices_[sp_id]].payoffs();
  assert(static_cast<int>(payoffs.size()) == profile.size());
  return payoffs;
}

int Game::StrategyProfileId(const StrategyProfile& profile) const {
  const int _num_players = num_players();
  assert(_num_players == static_cast<int>(profile.size()));
  int product = 1;
  int index = 0;
  for (int i = 0; i < _num_players; ++i) {
    index += profile[i] * product;
    product *= player(i).num_strategies();
  }
  return index;
}

bool Game::Valid(const StrategyProfile& profile) const {
  if (profile.size() != num_players()) {
    return false;
  }
  for (int i = 0; i < num_players(); ++i) {
    if (profile[i] < 0 || profile[i] >= player(i).num_strategies()) {
      return false;
    }
  }
  return true;
}

int Game::num_strategy_profiles() const {
  return payoff_indices_.size();
}

int Game::num_players() const {
  return players_.size();
}

int Game::num_strategies() const {
  return strategies_.size();
}

int Game::num_strategies(const int player_id) const {
  return player(player_id).num_strategies();
}

int Game::num_outcomes() const {
  return outcomes_.size();
}

}  // namespace ash
