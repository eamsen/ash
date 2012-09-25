// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef SRC_LCP_FACTORY_H_
#define SRC_LCP_FACTORY_H_

#include <vector>

namespace ash {

class Game;
class Lcp;

struct LcpFactory {
  static Lcp Create(const Game& game);
  static Lcp Create(const Game& game,
                    std::vector<std::vector<int> >* compl_map,
                    std::vector<std::vector<int> >* player_vars);
};

}  // namespace ash
#endif  // SRC_LCP_FACTORY_H_
