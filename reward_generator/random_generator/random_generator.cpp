#include "random_generator.h"

#include "core/parameter_type.h"
#include "core/globalvar.h"

static const std::string generator_name = "random_generator";

RandomGenerator::RandomGenerator(GameConfig& config, GameEnv& env) :
  config(config),
  env(env)
{
  
}

int RandomGenerator::run(int now_time) {
  env.rewards.clear();
  for (int loop_i = 0; loop_i < reward_number; ++loop_i) {
    GameReward reward_item;
    int status_id = 1 + env.position_num * now_time + static_cast<int>((random_gen()%env.position_num));
    reward_item.state = env.graph[status_id];
    env.rewards.push_back(reward_item);
  }
  return 0;
}

GeneratorBaseType *generate_random_generator(GameConfig& config, GameEnv& env) {
  return new RandomGenerator(config, env);
}

extern int register_random_generator() {
  GeneratorType generator_info;
  generator_info.generator_name = generator_name;
  generator_info.generate_func = generate_random_generator;
  generator_info.parameters = {
    {"reward_number", "reward number", ParameterBaseType_INT, 10, {}}
  };
  register_generator(generator_info);
  return 0;
}
