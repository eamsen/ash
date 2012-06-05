// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
#include "../game.h"

using ash::StrategyProfile;
using ash::Player;
using ash::Outcome;
using ash::Game;

using std::vector;
using std::set;
using std::string;
using std::cout;
using std::endl;
using std::ofstream;

using ::testing::ElementsAre;
using ::testing::Contains;
using ::testing::Not;
// using ::testing::WhenSortedBy;

struct GameTest : public ::testing::Test {
  GameTest()
      : game1("game1") {}

  void SetUp() {
  }

  void TearDown() {
    // /tmp/* is cleared automatically on reboot.
  }

  Game game1;
};

TEST_F(GameTest, GameConstruction) {
  Game game("3 Player RPS");
  Player p1("p1");
  Player p2("p2");
  Player p3("p3");

  EXPECT_EQ(0, game.AddStrategy("rock"));
  EXPECT_EQ(1, game.AddStrategy("paper"));
  EXPECT_EQ(2, game.AddStrategy("scissors"));

  EXPECT_EQ(0, p1.AddStrategy(0));
  EXPECT_EQ(1, p1.AddStrategy(1));
  EXPECT_EQ(2, p1.AddStrategy(2));
  EXPECT_EQ(0, p2.AddStrategy(0));
  EXPECT_EQ(1, p2.AddStrategy(1));
  EXPECT_EQ(2, p2.AddStrategy(2));
  EXPECT_EQ(0, p3.AddStrategy(0));
  EXPECT_EQ(1, p3.AddStrategy(1));
  EXPECT_EQ(2, p3.AddStrategy(2));

  EXPECT_EQ(0, game.AddPlayer(p1));
  EXPECT_EQ(1, game.AddPlayer(p2));
  EXPECT_EQ(2, game.AddPlayer(p2));

  Outcome o0("null", {0, 0, 0});
  Outcome o1("p1 wins", {1, -1, -1});
  Outcome o2("p2 wins", {-1, 1, -1});
  Outcome o3("p3 wins", {-1, -1, 1});
  EXPECT_EQ(0, game.AddOutcome(o0));
  EXPECT_EQ(1, game.AddOutcome(o1));
  EXPECT_EQ(2, game.AddOutcome(o2));
  EXPECT_EQ(3, game.AddOutcome(o3));

  game.SetPayoff({0, 0, 0}, 0);
  game.SetPayoff({1, 0, 0}, 1);
  game.SetPayoff({2, 0, 0}, 0);
  game.SetPayoff({0, 1, 0}, 2);
  game.SetPayoff({1, 1, 0}, 0);
  game.SetPayoff({2, 1, 0}, 0);
  game.SetPayoff({0, 2, 0}, 0);
  game.SetPayoff({1, 2, 0}, 0);
  game.SetPayoff({2, 2, 0}, 3);
  game.SetPayoff({0, 0, 1}, 3);
  game.SetPayoff({1, 0, 1}, 0);
  game.SetPayoff({2, 0, 1}, 0);
  game.SetPayoff({0, 1, 1}, 0);
  game.SetPayoff({1, 1, 1}, 0);
  game.SetPayoff({2, 1, 1}, 1);
  game.SetPayoff({0, 2, 1}, 0);
  game.SetPayoff({1, 2, 1}, 2);
  game.SetPayoff({2, 2, 1}, 0);
  game.SetPayoff({0, 0, 2}, 0);
  game.SetPayoff({1, 0, 2}, 0);
  game.SetPayoff({2, 0, 2}, 2);
  game.SetPayoff({0, 1, 2}, 0);
  game.SetPayoff({1, 1, 2}, 3);
  game.SetPayoff({2, 1, 2}, 0);
  game.SetPayoff({0, 2, 2}, 1);
  game.SetPayoff({1, 2, 2}, 0);
  game.SetPayoff({2, 2, 2}, 0);

  EXPECT_THAT(game.payoff({0, 0, 0}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({1, 0, 0}), ElementsAre(1, -1, -1));
  EXPECT_THAT(game.payoff({2, 0, 0}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({0, 1, 0}), ElementsAre(-1, 1, -1));
  EXPECT_THAT(game.payoff({1, 1, 0}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({2, 1, 0}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({0, 2, 0}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({1, 2, 0}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({2, 2, 0}), ElementsAre(-1, -1, 1));
  EXPECT_THAT(game.payoff({0, 0, 1}), ElementsAre(-1, -1, 1));
  EXPECT_THAT(game.payoff({1, 0, 1}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({2, 0, 1}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({0, 1, 1}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({1, 1, 1}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({2, 1, 1}), ElementsAre(1, -1, -1));
  EXPECT_THAT(game.payoff({0, 2, 1}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({1, 2, 1}), ElementsAre(-1, 1, -1));
  EXPECT_THAT(game.payoff({2, 2, 1}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({0, 0, 2}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({1, 0, 2}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({2, 0, 2}), ElementsAre(-1, 1, -1));
  EXPECT_THAT(game.payoff({0, 1, 2}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({1, 1, 2}), ElementsAre(-1, -1, 1));
  EXPECT_THAT(game.payoff({2, 1, 2}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({0, 2, 2}), ElementsAre(1, -1, -1));
  EXPECT_THAT(game.payoff({1, 2, 2}), ElementsAre(0, 0, 0));
  EXPECT_THAT(game.payoff({2, 2, 2}), ElementsAre(0, 0, 0));
}

TEST_F(GameTest, StrategyProfile) {
  vector<int> strategies = {0, 1, 2};
  StrategyProfile profile(strategies);
  ASSERT_EQ(3, profile.size());
  EXPECT_EQ(0, profile[0]);
  EXPECT_EQ(1, profile[1]);
  EXPECT_EQ(2, profile[2]);
}
