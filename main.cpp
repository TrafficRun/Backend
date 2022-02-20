#include <iostream>

#include "model/simple_reandom/simple_random.h"
#include "reward_generator/random_generator/random_generator.h"

#include "core/cmdline.h"
#include "core/core_run.h"

int main (int argc, char *argv[]) {
  register_random_generator();
  register_simple_random_model();
  CMDLine cmd;
  GameConfig config;

  cmd.parse_cmd(argc, argv, config);
  GameEnv env(config);
  CoreRun core(config, env);
  core.run();
  return 0;
}