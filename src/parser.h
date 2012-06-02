// Copyright 2012 Eugen Sawin <sawine@me73.com>
#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

#include <cassert>
#include <vector>
#include <set>
#include <string>

namespace ash { namespace parse {

// Gambit Outcome format.
struct Outcome {
  Outcome(const std::string& name, const std::vector<int>& payoffs)
      : name(name),
        payoffs(payoffs) {}

  std::string Str() const;

  std::string name;
  std::vector<int> payoffs;
};

// Gambit strategic game format, the outcome version.
struct StrategicGame {
  std::string Str() const;

  std::string name;
  std::vector<std::string> players;
  std::vector<std::vector<std::string> > strategies;
  std::string comment;
  std::vector<Outcome> outcomes;
  std::vector<int> payoff_indices;
};

class Parser {
 public:
  static const char* kNumbers;
  static const char* kWhitespace;
  static const char kQuot;
  static const char kCurlBeg;
  static const char kCurlEnd;

  // Converts a value from one type to another.
  template<typename To, typename From>
  static To Convert(const From& from) {
    std::stringstream ss;
    ss << from;
    To to;
    ss >> to;
    return to;
  }

  // Returns the file size of given path in bytes.
  static size_t FileSize(const std::string& path);

  static void CollectNumerals(const std::string& content,
                              std::vector<int>* numerals);

  static void CollectStrings(const std::string& content,
                             std::vector<std::string>* strings);

  // Splits the given string at whitespaces.
  static std::vector<std::string> Split(const std::string& content);

  // Strips all whitespace characters at beginning and end of given string.
  static std::string StripWhitespace(const std::string& content);

  // Does the same as the other function but on the given content string,
  // instead of the whole cached file.
  static size_t Between(const size_t beg, const char open, const char close,
                        const std::string& content, std::string* between);

  // Initialised the parser with given path.
  explicit Parser(const std::string& path);

  // Parses a strategic game in outcome format.
  StrategicGame ParseStrategicGame();

 private:
  size_t LoadHeader(const size_t beg, StrategicGame* game) const;
  size_t LoadPlayers(const size_t beg, StrategicGame* game) const;
  size_t LoadStrategies(const size_t beg, StrategicGame* game) const;
  size_t LoadOutcomes(const size_t beg, StrategicGame* game) const;
  size_t LoadPayoffIndices(const size_t beg, StrategicGame* game) const;

  // Loads the content between given symbols open and close into the string
  // between. Backslash-escaped close symbols are ignored and will be contained
  // within the string. Nested structures are handled correctly.
  // Returns the position past the close symbol, if found and npos otherwise.
  size_t Between(const size_t beg, const char open, const char close,
                 std::string* between) const;

  // Reads the whole file into parser cache.
  void ReadAll();

  std::string path_;
  std::string content_;
};

} }  // namespace ash::parse
#endif  // SRC_PARSER_H_
