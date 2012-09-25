// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef SRC_EQUILIBRIA_FINDER_H_
#define SRC_EQUILIBRIA_FINDER_H_

#include <vector>
#include "./clock.h"
#include "./game.h"

namespace ash {

class EquilibriaFinder {
 public:
  explicit EquilibriaFinder(const Game& game);
  int FindPure();
  int FindMixed();
  void Reset();
  void max_num_equilibria(const int max_num);
  int max_num_equilibria() const;
  const Game& game() const;
  const std::vector<StrategyProfile>& equilibria() const;
  const std::vector<MixedStrategyProfile>& mixed_equilibria() const;
  base::Clock::Diff duration() const;
  base::Clock::Diff lcp_duration() const;
  base::Clock::Diff lp_duration() const;

 private:
  bool NextSupports(std::vector<uint32_t>* supports) const;
  void AddMixedEquilibrium(const std::vector<double>& probs,
                           const std::vector<std::vector<int> >& player_vars);

  const Game& game_;
  std::vector<StrategyProfile> equilibria_;
  std::vector<MixedStrategyProfile> mixed_equilibria_;
  size_t max_num_equilibria_;
  base::Clock::Diff duration_;
  base::Clock::Diff lcp_duration_;
  base::Clock::Diff lp_duration_;
};

}  // namespace ash
#endif  // SRC_EQUILIBRIA_FINDER_H_
