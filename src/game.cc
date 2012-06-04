// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./game.h"
#include <cassert>
#include <algorithm>

using std::string;
using std::vector;
using std::max;

namespace ash {

Player::Player(const std::string& name)
    : name_(name) {}

int Player::AddStrategy(const int s_id) {
  strategies_.push_back(s_id);
  return strategies_.size() - 1;
}

int Player::num_strategies() const {
  return strategies_.size();
}

Outcome::Outcome(const std::string& name, const std::vector<int>& payoffs)
    : payoffs_(payoffs),
      name_(name) {}

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

void Game::SetPayoff(const StrategyProfile& profile, const int outcome) {
  SetPayoff(StrategyProfileId(profile), outcome);
}

void Game::SetPayoff(const int sp_id, const int outcome_id) {
  assert(sp_id >= 0 && sp_id < num_strategy_profiles());
  payoff_indices_[sp_id] = outcome_id;
}

const Player& Game::player(const int id) const {
  assert(id >= 0 && id < num_players());
  return players_[id];
}

int Game::num_strategy_profiles() const {
  return payoff_indices_.size();
}

int Game::num_players() const {
  return players_.size();
}

int Game::num_outcomes() const {
  return outcomes_.size();
}

int Game::StrategyProfileId(const StrategyProfile& profile) const {
  int product = 1;
  int index = 0;
  for (size_t i = 0; i < profile.size(); ++i) {
    index += profile[i] * product;
    product *= strategies_[i].size();
  }
  return index;
}

}  // namespace ash
