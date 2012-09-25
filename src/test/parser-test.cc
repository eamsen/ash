// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
#include "../parser.h"

using ash::parse::Outcome;
using ash::parse::StrategicGame;
using ash::parse::Parser;

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

DEFINE_bool(verbose, false, "Verbose output");

struct ParserTest : public ::testing::Test {
  void SetUp() {
    nfg1 = string("NFG 1 R ") +
      " \"3 Player RPS\" { \"player 1\" \"player 2\" \"player 3\" }\n\n" +
      "{ { \"rock\" \"paper\" \"scissors\" }\n" +
      "{ \"rock\" \"paper\" \"scissors\" }\n" +
      "{ \"rock\" \"paper\" \"scissors\" }\n}" +
      "\"This is a comment.\"\n\n" +
      "{\n{ \"player 1 wins\" 1, -1, -1 }\n" +
      "\n{ \"player 2 wins\" -1, 1, -1 }\n" +
      "\n{ \"player 3 wins\" -1, -1, 1 }\n}\n" +
      "0 1 0 2 0 0 0 0 3 3 0 0 0 0 1 0 2 0 0 0 2 0 3 0 1 0 0\n";
    nfg1_path = "/tmp/ash-parser-test-nfg1.xml";
    ofstream nfg1_stream(nfg1_path.c_str());
    nfg1_stream.write(nfg1.c_str(), nfg1.size());
    nfg1_stream.close();
  }

  void TearDown() {
    // /tmp/* is cleared automatically on reboot.
  }

  string nfg1;
  string nfg1_path;
};

TEST_F(ParserTest, CollectNumerals) {
  {
    string numerals_str = "";
    vector<int> numerals;
    Parser::CollectNumerals(numerals_str, &numerals);
    EXPECT_THAT(numerals, ElementsAre());
  }
  {
    string numerals_str = "\n1 -1 1 1   -2 \n\t -1 2";
    vector<int> numerals;
    Parser::CollectNumerals(numerals_str, &numerals);
    EXPECT_THAT(numerals, ElementsAre(1, -1, 1, 1, -2, -1, 2));
  }
  {
    string numerals_str = "-1 ,-1 1, 1   -2, \n\t -1 2";
    vector<int> numerals;
    Parser::CollectNumerals(numerals_str, &numerals);
    EXPECT_THAT(numerals, ElementsAre(-1, -1, 1, 1, -2, -1, 2));
  }
  {
    string numerals_str = "{{\"something\" 1, -1, -1 }}";
    vector<int> numerals;
    Parser::CollectNumerals(numerals_str, &numerals);
    EXPECT_THAT(numerals, ElementsAre(1, -1, -1));
  }

}

TEST_F(ParserTest, Between) {
  {
    string str = "";
    string between;
    size_t pos = Parser::Between(0, '\"', '\"', str, &between);
    ASSERT_EQ(string::npos, pos);
    EXPECT_EQ("", between);
  }
  {
    string str = "nothing here \"but here \\\"quoted\\\" end\"";
    string between;
    size_t pos = Parser::Between(0, '\"', '\"', str, &between);
    ASSERT_NE(string::npos, pos);
    EXPECT_EQ("but here \\\"quoted\\\" end", between);
  }
  {
    string str = "{ { \"a\" \"b\" } { \"c\" \"d\" } } { \"d\" }";
    string between;
    size_t pos = Parser::Between(0, '{', '}', str, &between);
    ASSERT_NE(string::npos, pos);
    EXPECT_EQ(" { \"a\" \"b\" } { \"c\" \"d\" } ", between);
  } 
  {
    string str = string("something else here {{ \"a\" 1, -1, -1 }") +
      "{ \"b\" -1, 1, -1 }}";
    string between;
    size_t pos = Parser::Between(4, '{', '}', str, &between);
    ASSERT_NE(string::npos, pos);
    EXPECT_EQ("{ \"a\" 1, -1, -1 }{ \"b\" -1, 1, -1 }", between);
  }
} 


TEST_F(ParserTest, CollectStrings) {
  {
    string str = "";
    vector<string> strings;
    Parser::CollectStrings(str, &strings);
    EXPECT_THAT(strings, ElementsAre());
  }
  {
    string str = string("\"something one\" \"something two\"") +
      " \"something \\\"quoted\\\"\"";
    vector<string> strings;
    Parser::CollectStrings(str, &strings);
    EXPECT_EQ("something one", strings[0]);
    EXPECT_EQ("something two", strings[1]);
    EXPECT_EQ("something \\\"quoted\\\"", strings[2]);
  }
} 

TEST_F(ParserTest, Split) {
  {
    string s = "";
    vector<string> items = Parser::Split(s);
    EXPECT_EQ(vector<string>(), items);
  } 
  {
    string s = "a";
    vector<string> items = Parser::Split(s);
    EXPECT_EQ(vector<string>({"a"}), items);
  } 
  {
    string s = "c1 c2";
    vector<string> items = Parser::Split(s);
    EXPECT_EQ(vector<string>({"c1", "c2"}), items);
  } 
  {
    string s = "a b c d";
    vector<string> items = Parser::Split(s);
    EXPECT_EQ(vector<string>({"a", "b", "c", "d"}), items);
  }
  {
    string s = " a b c d ";
    vector<string> items = Parser::Split(s);
    EXPECT_EQ(vector<string>({"a", "b", "c", "d"}), items);
  }
}

TEST_F(ParserTest, ParseStrategicGame) {
  Parser parser(nfg1_path);
  StrategicGame game = parser.ParseStrategicGame();
  EXPECT_EQ("3 Player RPS", game.name);
  // Check players.
  ASSERT_EQ(3, game.players.size());
  EXPECT_EQ("player 1", game.players[0]);
  EXPECT_EQ("player 2", game.players[1]);
  EXPECT_EQ("player 3", game.players[2]);
  // Check strategies.
  ASSERT_EQ(3, game.strategies.size());
  ASSERT_EQ(3, game.strategies[0].size());
  ASSERT_EQ(3, game.strategies[1].size());
  ASSERT_EQ(3, game.strategies[2].size());
  EXPECT_EQ("rock", game.strategies[0][0]);
  EXPECT_EQ("rock", game.strategies[1][0]);
  EXPECT_EQ("rock", game.strategies[2][0]);
  EXPECT_EQ("paper", game.strategies[0][1]);
  EXPECT_EQ("paper", game.strategies[1][1]);
  EXPECT_EQ("paper", game.strategies[2][1]);
  EXPECT_EQ("scissors", game.strategies[0][2]);
  EXPECT_EQ("scissors", game.strategies[1][2]);
  EXPECT_EQ("scissors", game.strategies[2][2]);
  // Check comment.
  EXPECT_EQ("This is a comment.", game.comment);
  // Check outcomes.
  ASSERT_EQ(3, game.outcomes.size());
  EXPECT_EQ("player 1 wins", game.outcomes[0].name);
  EXPECT_EQ("player 2 wins", game.outcomes[1].name);
  EXPECT_EQ("player 3 wins", game.outcomes[2].name);
  EXPECT_THAT(game.outcomes[0].payoffs, ElementsAre(1, -1, -1));
  EXPECT_THAT(game.outcomes[1].payoffs, ElementsAre(-1, 1, -1));
  EXPECT_THAT(game.outcomes[2].payoffs, ElementsAre(-1, -1, 1));
  // Check payoff indices.
  ASSERT_EQ(27, game.payoff_indices.size());
  vector<int> indices1(game.payoff_indices.begin(),
                       game.payoff_indices.begin() + 10);
  vector<int> indices2(game.payoff_indices.begin() + 10,
                       game.payoff_indices.begin() + 20);
  vector<int> indices3(game.payoff_indices.begin() + 20,
                       game.payoff_indices.begin() + 27);
  EXPECT_THAT(indices1, ElementsAre(0, 1, 0, 2, 0, 0, 0, 0, 3, 3));
  EXPECT_THAT(indices2, ElementsAre(0, 0, 0, 0, 1, 0, 2, 0, 0, 0));
  EXPECT_THAT(indices3, ElementsAre(2, 0, 3, 0, 1, 0, 0));
}
