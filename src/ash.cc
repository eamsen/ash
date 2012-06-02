// Copyright 2012 Eugen Sawin <sawine@me73.com>
#include <gflags/gflags.h>
#include <cassert>
#include <iostream>
#include "./parser.h"
#include "./clock.h"
#include "./profiler.h"

using std::cout;
using std::string;
using ash::parse::Parser;
using ash::parse::StrategicGame;
using base::Clock;
using base::Profiler;

// Flag for verbose output.
DEFINE_bool(verbose, false, "Verbose output");

// The command-line usage text.
const string kUsage =  // NOLINT
  string("Usage:\n") +
         "  $ ash input.nfg\n" +
         "  input.nfg is a strategic game instance" +
         " in the Gambit outcome format";

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
  return 0;
}
