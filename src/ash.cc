// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include <gflags/gflags.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <limits>
#include "./clock.h"
#include "./profiler.h"
#include "./parser.h"
#include "./game.h"
#include "./game-factory.h"
#include "./lcp.h"
#include "./lcp-factory.h"
#include "./equilibria-finder.h"

using std::cout;
using std::string;
using std::vector;
using std::numeric_limits;
using base::Clock;
using base::Profiler;
using ash::parse::Parser;
using ash::parse::StrategicGame;
using ash::Game;
using ash::GameFactory;
using ash::Lcp;
using ash::LcpFactory;
using ash::EquilibriaFinder;
using ash::StrategyProfile;

// Command-line flag for verbose output.
DEFINE_bool(verbose, false, "Verbose output");
// Command-line flag for pure strategy equilibira.
DEFINE_bool(pure, true, "Find pure strategy Nash equilibria");
// Command-line flag for mixed strategies equilibira.
DEFINE_bool(mixed, true, "Find mixed strategies Nash equilibria");
// Command-line flag for max number of equilibira to be searched for.
DEFINE_int32(maxequilibria, numeric_limits<int>::max(),
             "Maximum number of equilibira to be found (min 1)");

// The command-line usage text.
const string kUsage =  // NOLINT
  string("Usage:\n") +
         "  $ ash input.nfg\n" +
         "  input.nfg is a strategic game instance" +
         " in the Gambit outcome format";

void FindPureEquilibria(EquilibriaFinder* finder);
void FindMixedEquilibria(EquilibriaFinder* finder);

int main(int argc, char* argv[]) {
  google::SetUsageMessage(kUsage);
  // Parse command line flags and remove them from the argc and argv.
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc != 2) {
    cout << "Wrong argument number provided, use -help for help.\n"
         << kUsage << "\n";
    return 1;
  } else if (!Parser::FileSize(argv[1])) {
    cout << "File " << argv[1] << " is empty or does not exist.\n";
    return 1;
  }
  const string input_path = argv[1];
  Parser parser(input_path);
  StrategicGame parsed_game = parser.ParseStrategicGame();
  cout << "File: " << input_path << "\n";
  if (FLAGS_verbose) {
    cout << parsed_game.Str();
  }
  Game game = GameFactory::Create(parsed_game);
  EquilibriaFinder finder(game);
  if (FLAGS_pure) {
    FindPureEquilibria(&finder);
  }
  if (FLAGS_mixed) {
    FindMixedEquilibria(&finder);
  }
  return 0;
}

void FindPureEquilibria(EquilibriaFinder* finder) {
  finder->max_num_equilibria(FLAGS_maxequilibria);
  const int num_eq = finder->FindPure();
  const vector<StrategyProfile>& eqs = finder->equilibria();
  cout << "Found " << num_eq << " pure strategy Nash equilibria"
       << (num_eq ? ":" : ".");
  for (auto it = eqs.begin(); it != eqs.end(); ++it) {
    const StrategyProfile& profile = *it;
    cout << " " << profile.str(finder->game());
  }
  cout << "\nDuration: " << Clock::DiffStr(finder->duration()) << "\n";
}

void FindMixedEquilibria(EquilibriaFinder* finder) {
  finder->max_num_equilibria(FLAGS_maxequilibria);
  const int num_eq = finder->FindMixed();
  const vector<StrategyProfile>& eqs = finder->equilibria();
  cout << "Found " << num_eq << " mixed strategies Nash equilibria"
       << (num_eq ? ":" : ".");
  for (auto it = eqs.begin(); it != eqs.end(); ++it) {
    const StrategyProfile& profile = *it;
    cout << " " << profile.str(finder->game());
  }
  cout << "\nLCP duration: " << Clock::DiffStr(finder->lcp_duration());
  cout << "\nDuration: " << Clock::DiffStr(finder->duration()) << "\n";
}
