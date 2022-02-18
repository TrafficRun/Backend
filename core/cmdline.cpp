#include "cmdline.h"

#include "globalvar.h"
#include "commconfig.h"
#include "gameenv.h"

#include <boost/program_options.hpp>

CMDLine::CMDLine() :
  cmd_options("Traffic Run")
{
  auto game_config = generate_game_config();
  auto game_env_config = generate_game_env_config();

  boost::program_options::options_description game_config_desc("Game Config");
  for (const auto& item : game_config) {
    generate_parser(game_config_desc, item);
  }
  cmd_options.add(game_config_desc);
  
  boost::program_options::options_description game_env_config_desc("Game Env Config");
  
}
