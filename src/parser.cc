// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./parser.h"
#include <cassert>
#include <fstream>
#include <sstream>

using std::string;
using std::ifstream;
using std::stringstream;
using std::vector;
using std::set;
using std::min;

namespace ash { namespace parse {

string Outcome::Str() const {
  stringstream ss;
  ss << "(" << name << " (";
  for (auto it = payoffs.begin(); it != payoffs.end(); ++it) {
    if (it != payoffs.begin()) {
      ss << " ";
    }
    ss << *it;
  }
  ss << "))";
  return ss.str();
}

string StrategicGame::Str() const {
  stringstream ss;
  ss << "(" << name;
  ss << "\n  (players ";
  for (auto it = players.begin(); it != players.end(); ++it) {
    if (it != players.begin()) {
      ss << " ";
    }
    ss << *it;
  }
  ss << ")\n  (strategies ";
  for (auto it = strategies.begin(); it != strategies.end(); ++it) {
    ss << "\n    (";
    for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
      if (it2 != it->begin()) {
        ss << " ";
      }
      ss << *it2;
    }
    ss << ")";
  }
  ss << ")\n";
  if (comment.size()) {
    ss << "  (comment " << comment << ")\n";
  }
  ss << "  (outcomes ";
  for (auto it = outcomes.begin(); it != outcomes.end(); ++it) {
    const Outcome& o = *it;
    ss << "\n    " << o.Str();
  }
  ss << ")\n  (payoff indices (";
  for (auto it = payoff_indices.begin(); it != payoff_indices.end(); ++it) {
    if (it != payoff_indices.begin()) {
      ss << " ";
    }
    ss << *it;
  }
  ss << ")))\n";
  return ss.str();
}

const char* Parser::kNumbers = "-0123456789";
const char* Parser::kWhitespace = "\n\r\t ";
const char Parser::kQuot = '\"';
const char Parser::kCurlBeg = '{';
const char Parser::kCurlEnd = '}';

size_t Parser::FileSize(const string& path) {
  ifstream stream(path.c_str());
  size_t size = 0;
  if (stream.good()) {
    stream.seekg(0, std::ios::end);
    size = stream.tellg();
    stream.seekg(0, std::ios::beg);
  }
  stream.close();
  return size;
}

void Parser::CollectNumerals(const string& content, vector<int>* numerals) {
  assert(numerals);
  size_t pos = content.find_first_of(kNumbers);
  while (pos != string::npos) {
    size_t end = content.find_first_not_of(kNumbers, pos);
    if (end == string::npos) {
      numerals->push_back(Convert<int>(content.substr(pos)));
    } else {
      numerals->push_back(Convert<int>(content.substr(pos, end - pos)));
    }
    pos = content.find_first_of(kNumbers, end);
  }
}

void Parser::CollectStrings(const string& content, vector<string>* strings) {
  assert(strings);
  string between;
  size_t pos = Between(0, kQuot, kQuot, content, &between);
  while (pos != string::npos) {
    strings->push_back(between);
    between.clear();
    if (pos < content.size()) {
      pos = Between(pos, kQuot, kQuot, content, &between);
    } else {
      pos = string::npos;
    }
  }
  // size_t pos = content.find(kQuot);
  // while (pos != string::npos) {
  //   assert(pos != string::npos);
  //   size_t end = content.find(kQuot, pos + 1);
  //   assert(end != string::npos);
  //   while (content[end - 1] == '\\') {
  //     // Escaped quotation mark, still inside the string.
  //     end = content.find(kQuot, end + 1);
  //     assert(end != string::npos);
  //   }
  //   // End of string found.
  //   strings->push_back(content.substr(pos + 1, end - pos - 1));
  //   pos = content.find(kQuot, end + 1);
  // }
}

vector<string> Parser::Split(const string& content) {
  vector<string> items;
  size_t pos = content.find_first_not_of(kWhitespace);
  while (pos != string::npos) {
    size_t end = content.find_first_of(kWhitespace, pos);
    if (end == string::npos) {
      // Last item found.
      items.push_back(content.substr(pos));
    } else {
      // Item found.
      items.push_back(content.substr(pos, end - pos));
    }
    pos = content.find_first_not_of(kWhitespace, end);
  }
  return items;
}

string Parser::StripWhitespace(const string& content) {
  size_t beg = content.find_first_not_of(kWhitespace);
  size_t end = content.find_last_not_of(kWhitespace);
  if (beg == end) {
    return "";
  }
  return content.substr(beg, end - beg + 1);
}

Parser::Parser(const string& path)
    : path_(path) {}

StrategicGame Parser::ParseStrategicGame() {
  if (content_.size() == 0) {
    ReadAll();
  }
  assert(content_.size());
  StrategicGame game;
  size_t pos = 0;
  pos = LoadHeader(pos, &game);
  pos = LoadPlayers(pos, &game);
  pos = LoadStrategies(pos, &game);
  size_t comment_end = Between(pos, kQuot, kQuot, &game.comment);
  if (comment_end != string::npos) {
    pos = comment_end;
  }
  pos = LoadOutcomes(pos, &game);
  pos = LoadPayoffIndices(pos, &game);
  return game;
}

size_t Parser::LoadHeader(const size_t pos, StrategicGame* game) const {
  assert(game);
  assert(pos < content_.size());
  size_t end = content_.find(kQuot, pos);
  assert(end != string::npos);
  stringstream ss;
  ss << content_.substr(pos, end - pos);
  string nfg;
  ss >> nfg;
  assert(nfg == "NFG");
  string version;
  ss >> version;
  assert(version == "1");
  string r;
  ss >> r;
  assert(r == "R");
  end = Between(end, kQuot, kQuot, &game->name);
  return end;
}

size_t Parser::LoadPlayers(const size_t pos, StrategicGame* game) const {
  assert(game);
  assert(pos < content_.size());
  string section;
  size_t end = Between(pos, kCurlBeg, kCurlEnd, &section);
  assert(end != string::npos);
  CollectStrings(section, &game->players);
  return end;
}

size_t Parser::LoadStrategies(const size_t pos, StrategicGame* game) const {
  assert(game);
  assert(pos < content_.size());
  string section;
  const size_t end = Between(pos, kCurlBeg, kCurlEnd, &section);
  assert(end != string::npos);
  const int num_players = game->players.size();
  game->strategies.resize(num_players);
  size_t sub_pos = 0;
  for (int i = 0; i < num_players; ++i) {
    string sub_section;
    sub_pos = Between(sub_pos, kCurlBeg, kCurlEnd, section, &sub_section);
    assert(sub_pos != string::npos);
    CollectStrings(sub_section, &game->strategies[i]);
    assert(game->strategies[i].size());
  }
  return end;
}

size_t Parser::LoadOutcomes(const size_t pos, StrategicGame* game) const {
  assert(game);
  assert(pos < content_.size());
  string section;
  const size_t end = Between(pos, kCurlBeg, kCurlEnd, &section);
  assert(end != string::npos);
  const size_t num_players = game->players.size();
  string sub_section;
  size_t sub_pos = Between(0, kCurlBeg, kCurlEnd, section, &sub_section);
  while (sub_pos != string::npos) {
    string name;
    size_t sub_sub_pos = Between(0, kQuot, kQuot, sub_section, &name);
    assert(sub_sub_pos != string::npos);
    vector<int> payoffs;
    CollectNumerals(sub_section.substr(sub_sub_pos), &payoffs);
    assert(payoffs.size() == num_players);
    game->outcomes.push_back(Outcome(name, payoffs));
    sub_section.clear();
    sub_pos = Between(sub_pos, kCurlBeg, kCurlEnd, section, &sub_section);
  }
  return end;
}

size_t Parser::LoadPayoffIndices(const size_t pos, StrategicGame* game) const {
  assert(game);
  assert(pos < content_.size());
  CollectNumerals(content_.substr(pos), &game->payoff_indices);
  return content_.size();
}

size_t Parser::Between(const size_t pos, const char open, const char close,
                       string* between) const {
  return Between(pos, open, close, content_, between);
}

size_t Parser::Between(const size_t pos, const char open, const char close,
                       const string& content, string* between) {
  assert(between);
  assert(pos != string::npos);
  if (pos >= content.size()) {
    return string::npos;
  }
  string both;
  both.push_back(open);
  both.push_back(close);
  size_t beg = content.find(open, pos);
  if (beg != string::npos) {
    int opened = 1;
    size_t end = content.find_first_of(both, beg + 1);
    while (end != string::npos) {
      while (end != string::npos && content[end - 1] == '\\') {
        // Escaped symbol, ignore it.
        end = content.find_first_of(both, end + 1);
      }
      if (end != string::npos) {
        opened -= content[end] == close;
        if (opened == 0) {
          // Structure is closed.
          *between = content.substr(beg + 1, end - beg - 1);
          return end + 1;
        }
        opened += content[end] == open;
        end = content.find_first_of(both, end + 1);
      }
    }
  }
  return string::npos;
}

void Parser::ReadAll() {
  size_t file_size = FileSize(path_);
  char* buffer = new char[file_size + 1];
  buffer[file_size] = '\0';
  ifstream stream(path_);
  assert(stream.good());
  stream.read(buffer, file_size);
  assert(static_cast<size_t>(stream.gcount()) == file_size);
  content_ = string(buffer);
}

} }  // namespace ash::parse
