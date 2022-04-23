#ifndef _MODEL_ONLINEGAPI_ONLINEGAPI_H
#define _MODEL_ONLINEGAPI_ONLINEGAPI_H

#include "core/parameter_type.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "core/globalvar.h"
#include <random>

const extern std::string REWARD_INDICATOR;
const extern std::string PLANNING_TIME;

class OnlineGAPIModel : public ModelBaseType {
public:
  const static std::string model_name;
  OnlineGAPIModel(GameConfig& config, GameEnv& env);
  ~OnlineGAPIModel(){};
  int run(int now_time);
private:
  int time_standard;
  GameConfig& config;
  GameEnv &env;
  std::mt19937 random_gen = std::mt19937(std::random_device()());
};

class OnlineGAPIActionExt : public GameExtType {
public:
  ~OnlineGAPIActionExt(){};
  static OnlineGAPIActionExt* get_ext(GameActionPtr maction);
  static GameExtType* create_ext(const void * maction);
  int agent_number;
};

extern int register_online_gapi_model();
#endif
