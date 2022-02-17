#ifndef _CORE_CONFIG_H
#define _CORE_CONFIG_H

#include <string>
#include <optional>

class GameConfig {
public:
  std::optional<std::string> graph_file;
  std::optional<std::string> graph_type;
  std::optional<int> time_step;
};

#endif