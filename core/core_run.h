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
private:
  GameEnv &env;
  GameConfig& config;
};

#endif