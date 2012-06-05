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
    : player_bits_((sizeof(int) * 8 - 1) / strategies.size()),
      mask_(pow(2, player_bits_)),
      id_(0) {
  for (size_t i = 0; i < strategies.size(); ++i) {
    assert(strategies[i] < static_cast<int>(mask_));
    id_ |= strategies[i] << (i * player_bits_);
  }
}

StrategyProfile::StrategyProfile(const int player_id, const int strategy_id,
                                 const int num_players)
    : player_bits_((sizeof(int) * 8 - 1) / num_players),
      mask_(pow(2, player_bits_)),
      id_(0) {
  for (int i = 0; i < num_players; ++i) {
    const int strategy = i == player_id ? strategy_id : 0;
    assert(strategy < static_cast<int>(mask_));
    id_ |= strategy << (i * player_bits_);
  }
}

StrategyProfile::StrategyProfile(const int id, const int num_players)
    : player_bits_((sizeof(int) * 8 - 1) / num_players),
      mask_(pow(2, player_bits_)),
      id_(id) {}

int StrategyProfile::strategy(const int player_id) const {
  return (id_ >> (player_id * player_bits_)) & mask_;
}

int StrategyProfile::id() const {
  return id_;
}

int StrategyProfile::size() const {
  return (sizeof(int) * 8 - 1) / player_bits_;
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
  SetPayoff(profile.id(), outcome_id);
}

void Game::SetPayoff(const int sp_id, const int outcome_id) {
  assert(sp_id >= 0 && sp_id < num_strategy_profiles());
  payoff_indices_[sp_id] = outcome_id;
}

const Player& Game::player(const int id) const {
  assert(id >= 0 && id < num_players());
  return players_[id];
}

int Game::payoff(const int player_id, const int strategy_id) const {
  assert(strategy_id >= 0 && strategy_id < player(player_id).num_strategies());
  StrategyProfile profile(player_id, strategy_id, num_players());
  return payoff(profile)[player_id];
}

const vector<int>& Game::payoff(const StrategyProfile& profile) const {
  return outcomes_[payoff_indices_[profile.id()]].payoffs();
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

}  // namespace ash
