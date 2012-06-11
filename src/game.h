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

  int strategy(const int index) const;
  int num_strategies() const;
  const std::string& name() const;

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

class Game;

class StrategyProfile {
 public:
  StrategyProfile(const std::vector<int>& strategies);  // NOLINT
  StrategyProfile(const std::initializer_list<int>& strategies);  // NOLINT
  StrategyProfile(const int num_players, const int common_strategy_id);
  int strategy(const int player_id) const;
  int operator[](const int player_id) const;
  void strategy(const int player_id, const int strategy_id);
  int size() const;
  std::string str() const;
  std::string str(const Game& game) const;

 private:
  std::vector<int> strategies_;
};

class MixedStrategyProfile {
 public:
  MixedStrategyProfile(const int num_players);
  void SetNumStrategies(const int player, const int num_strategies);
  void AddProbability(const int player, const  int strategy, const float prob);
  float probability(const int player_id, const int strategy_id) const;
  std::string str(const Game& game) const;

 private:
  std::vector<std::vector<float> > probs_;
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
  StrategyProfile CreateProfile(const int sp_id) const;

  const std::vector<int>& payoff(const StrategyProfile& profile) const;
  const Player& player(const int id) const;
  const std::string& strategy(const int id) const;
  int num_strategy_profiles() const;
  int num_players() const;
  int num_strategies() const;
  int num_strategies(const int player_id) const;
  int num_outcomes() const;
  bool zero_sum() const;

 private:
  int StrategyProfileId(const StrategyProfile& profile) const;
  bool Valid(const StrategyProfile& profile) const;

  std::vector<Player> players_;
  std::vector<Outcome> outcomes_;
  std::vector<int> payoff_indices_;
  std::vector<std::string> strategies_;
  std::string name_;
  bool zero_sum_;
};

}  // namespace ash
#endif  // SRC_GAME_H_
