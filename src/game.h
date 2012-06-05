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
  const std::vector<int>& payoffs() const;

 private:
  std::vector<int> payoffs_;
  std::string name_;
};

class StrategyProfile {
 public:
  StrategyProfile(const std::vector<int>& strategies);
  StrategyProfile(const int player_id, const int strategy_id,
                  const int num_players);
  StrategyProfile(const int id, const int num_players);
  int strategy(const int player_id) const;
  int id() const;
  int size() const;
  std::string str() const;

 private:
  int player_bits_;
  uint32_t mask_;
  uint32_t id_;
};

class Game {
 public:
  static const int kInvalidId;

  explicit Game(const std::string& name);
  int AddPlayer(const Player& p);
  int AddStrategy(const std::string& name);
  int AddOutcome(const Outcome& o);
  void SetPayoff(const StrategyProfile& profile, const int outcome_id);
  void SetPayoff(const int sp_id, const int outcome_id);

  int payoff(const int player_id, const int strategy_id) const;
  const std::vector<int>& payoff(const StrategyProfile& profile) const;
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