#ifndef _REWARD_GENERATOR_RANDOM_GENERATOR_RANDOM_GENERATOR_H
#define _REWARD_GENERATOR_RANDOM_GENERATOR_RANDOM_GENERATOR_H

#include "core/commconfig.h"
#include "core/gameenv.h"
#include "core/parameter_type.h"

#include <string>
#include <random>

class RandomGenerator : public GeneratorBaseType {
public:
  RandomGenerator(GameConfig& config, GameEnv& env);
  ~RandomGenerator(){};
  int generate(int now_time);

  static const std::string generator_name;
private:
  GameConfig& config;
  GameEnv& env;
  int reward_number;
  std::mt19937 random_gen = std::mt19937(std::random_device()());
};

class RandomGeneratorActionExtType : public GameExtType {
public:
  ~RandomGeneratorActionExtType(){};
  int reward_number;
  static RandomGeneratorActionExtType* get_ext(GameActionPtr maction);
  static GameExtType* create_ext(const void * maction);
};

extern int register_random_generator();
#endif