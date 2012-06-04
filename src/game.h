// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <string>
#include <vector>

namespace ash {

class Player {
 public:
  explicit Player(const std::string& name);
  int AddStrategy(const int s_id);
  int num_strategies() const;

 private:
  std::vector<int> strategies_;
  std::string name_;
};

class Outcome {
 public:
  Outcome(const std::string& name, const std::vector<int>& payoffs);

 private:
  std::vector<int> payoffs_;
  std::string name_;
};

typedef std::vector<int> StrategyProfile;

class Game {
 public:
  static const int kInvalidId;

  explicit Game(const std::string& name);
  int AddPlayer(const Player& p);
  int AddStrategy(const std::string& name);
  int AddOutcome(const Outcome& o);
  void SetPayoff(const StrategyProfile& profile, const int outcome);
  void SetPayoff(const int sp_id, const int outcome);
  int StrategyProfileId(const StrategyProfile& profile) const;
  const Player& player(const int id) const;
  int num_strategy_profiles() const;
  int num_players() const;
  int num_outcomes() const;

 private:
  std::vector<Player> players_;
  std::vector<Outcome> outcomes_;
  std::vector<int> payoff_indices_;
  std::vector<std::string> strategies_;
  std::string name_;
};

}  // namespace ash
#endif  // SRC_GAME_H_
