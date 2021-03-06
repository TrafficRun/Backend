#ifndef _MODEL_SIMPLERANDOM_SIMPLERANDOM_H
#define _MODEL_SIMPLERANDOM_SIMPLERANDOM_H

#include "core/parameter_type.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "core/globalvar.h"
#include <random>

extern const std::string SIMPLE_RANDOM_REWARD;

class SimpleRandomModel : public ModelBaseType {
public:
  const static std::string model_name;
  SimpleRandomModel(GameConfig& config, GameEnv& env);
  ~SimpleRandomModel(){};
  int run(int now_time);
private:
  GameConfig& config;
  GameEnv &env;
  std::mt19937 random_gen = std::mt19937(std::random_device()());
};

class SimpleRandomActionExt : public GameExtType {
public:
  ~SimpleRandomActionExt(){};
  static SimpleRandomActionExt* get_ext(GameActionPtr maction);
  static GameExtType* create_ext(const void * maction);
  int agent_number;
};

extern int register_simple_random_model();
#endif
