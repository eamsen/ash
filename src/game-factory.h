// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_GAME_FACTORY_H_
#define SRC_GAME_FACTORY_H_

namespace ash {

namespace parse { class StrategicGame; }

class Game;

struct GameFactory {
  static Game Create(const parse::StrategicGame& parsed_game);
};

}  // namespace ash
#endif  // SRC_GAME_FACTORY_H_
