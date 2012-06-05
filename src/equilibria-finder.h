// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_EQUILIBRIA_FINDER_H_
#define SRC_EQUILIBRIA_FINDER_H_

#include <vector>

namespace ash {

class Game;
class StrategyProfile;

class EquilibriaFinder {
 public:
  explicit EquilibriaFinder(const Game& game);
  int Find();
  void Reset();
  const std::vector<StrategyProfile>& equilibria() const;

 private:
  const Game& game_;
  std::vector<StrategyProfile> equilibria_;
};

}  // namespace ash
#endif  // SRC_EQUILIBRIA_FINDER_H_
