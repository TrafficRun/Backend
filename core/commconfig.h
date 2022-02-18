#ifndef _CORE_CONFIG_H
#define _CORE_CONFIG_H

#include <string>
#include <optional>
#include <vector>

#include "parameter_type.h"

class GameConfig {
public:
  bool online;
  std::optional<std::string> server;
  std::optional<int> port;
};

// TODO Achieve
extern std::vector<ParameterItemType>  generate_game_config();
#endif
