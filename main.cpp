#include <iostream>

#include "model/simple_reandom/simple_random.h"
#include "model/online_gapi/online_gapi.h"
#include "reward_generator/random_generator/random_generator.h"

#include "core/cmdline.h"
#include "core/core_run.h"
#include "http/server.h"

int main (int argc, char *argv[]) {
  register_random_generator();
  register_online_gapi_model();
  register_simple_random_model();
  register_game_env_config();
  register_game_config();

  CMDLine cmd;
  GameConfig config;

  cmd.parse_cmd(argc, argv, config);

  if (config.online) {
    HttpServer serv(config);
    serv.run();
    printf("Run Over? \n");
    return 0;
  }

  GameEnv env(config);
  CoreRun core(config, env);
  core.run();
  return 0;
}
