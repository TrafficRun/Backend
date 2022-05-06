#ifndef _CORE_CONFIG_H
#define _CORE_CONFIG_H

#include <string>
#include <vector>
#include <map>

#include "parameter_type.h"

#include <boost/any.hpp>

class GameConfig {
public:
  GameConfig() {};
  GameConfig(const GameConfig& pc) = default;
  bool online;
  std::string server;
  int port;
  std::string database;
  std::string model_name;
  std::string generator_name;
  std::string simulate_name;
  std::string work_root_dir;

  std::map<std::string, std::map<std::string, boost::any>> ext_config;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const GameConfig &c);

extern int register_game_config();
#endif
