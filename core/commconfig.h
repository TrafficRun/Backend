#ifndef _CORE_CONFIG_H
#define _CORE_CONFIG_H

#include <string>
#include <optional>
#include <vector>
#include <map>

#include "parameter_type.h"

#include <boost/any.hpp>

class GameConfig {
public:
  GameConfig() {};
  GameConfig(const GameConfig& pc) = default;
  std::optional<bool> online;
  std::optional<std::string> server;
  std::optional<int> port;
  std::optional<std::string> model_name;
  std::optional<std::string> generator_name;

  std::map<std::string, std::map<std::string, boost::any>> ext_config;
};

extern int register_game_config();
#endif
