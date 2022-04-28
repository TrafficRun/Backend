#ifndef _CORE_CORE_RUN_H
#define _CORE_CORE_RUN_H

#include "globalvar.h"
#include "parameter_type.h"
#include "commconfig.h"
#include "gameenv.h"

class CoreRun {
public:
  CoreRun(GameConfig& config, GameEnv& env);
  ~CoreRun(){};
  int run();
  GameEnvDetail detail();
private:
  std::string uid;
  std::string simulate_name;
  GameEnv &env;
  GameConfig& config;
  int add_exts(const std::string &ext_name, const std::map<std::string, generate_ext_func_type>& exts);
};

#endif