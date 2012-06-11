// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include "./game.h"
#include <cassert>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <numeric>

using std::string;
using std::stringstream;
using std::vector;
using std::max;
using std::pow;
using std::accumulate;

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

const string& Player::name() const {
  return name_;
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
  ss << "(pure profile";
  for (int i = 0; i < size(); ++i) {
    const Player& player = game.player(i);
    ss << "\n  (" << player.name()
       << " " << game.strategy(player.strategy(strategy(i))) << ")";
  }
  ss << ")";
  return ss.str();
}

MixedStrategyProfile::MixedStrategyProfile(const int num_players)
    : probs_(num_players) {}

void MixedStrategyProfile::SetNumStrategies(const int player_id,
                                            const int num_strategies) {
  assert(player_id >= 0 && player_id < static_cast<int>(probs_.size()));
  assert(num_strategies >= 0 &&
         num_strategies >= static_cast<int>(probs_[player_id].size()));
  probs_[player_id].resize(num_strategies, 0.0f);
}

void MixedStrategyProfile::AddProbability(const int player_id,
                                          const int strategy_id,
                                          const float prob) {
  assert(player_id >= 0 && player_id < static_cast<int>(probs_.size()));
  assert(strategy_id >= 0 &&
         strategy_id < static_cast<int>(probs_[player_id].size()));
  probs_[player_id][strategy_id] = prob;
}

float MixedStrategyProfile::probability(const int player_id,
                                        const int strategy_id) const {
  assert(player_id >= 0 && player_id < static_cast<int>(probs_.size()));
  assert(strategy_id >= 0 &&
         strategy_id < static_cast<int>(probs_[player_id].size()));
  return probs_[player_id][strategy_id];
}

string MixedStrategyProfile::str(const Game& game) const {
  stringstream ss;
  ss << "(mixed profile ";
  for (size_t p = 0; p < probs_.size(); ++p) {
    const vector<float>& strategy_probs = probs_[p];
    const Player& player = game.player(p);
    ss << "\n  (" << player.name();
    for (size_t s = 0; s < strategy_probs.size(); ++s) {
      const string& strategy = game.strategy(player.strategy(s));
      ss << " (" << strategy_probs[s] << " " << strategy << ")";
    }
    ss << ")";
  }
  ss << ")";
  return ss.str();
}

const int Game::kInvalidId = -1;

Game::Game(const std::string& name)
    : name_(name),
      zero_sum_(true) {}

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
  zero_sum_ = zero_sum_ &&
              accumulate(o.payoffs().begin(), o.payoffs().end(), 0) == 0;
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

bool Game::zero_sum() const {
  return zero_sum_;
}

}  // namespace ash
