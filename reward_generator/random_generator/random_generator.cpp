#include "random_generator.h"

#include "core/parameter_type.h"
#include "core/globalvar.h"

const std::string RandomGenerator::generator_name = "random_generator";

RandomGenerator::RandomGenerator(GameConfig& config, GameEnv& env) :
  config(config),
  env(env)
{
  reward_number = boost::any_cast<int>(config.ext_config[generator_name]["reward_number"]);
}

int RandomGenerator::generate(int now_time) {
  for (int loop_i = 0; loop_i < reward_number; ++loop_i) {
    GameRewardPtr reward_item = env.create_reward();
    int status_id = 1 + env.position_num * now_time + static_cast<int>((random_gen()%env.position_num));
    reward_item->state = env.graph[status_id];
    const auto& maction = env.graph[status_id]->actions[random_gen()%(env.graph[status_id]->actions.size())];
    RandomGeneratorActionExtType::get_ext(maction)->reward_number++;
  }
  return 0;
}

GeneratorBaseType *generate_random_generator(GameConfig& config, GameEnv& env) {
  return new RandomGenerator(config, env);
}

extern int register_random_generator() {
  GeneratorType generator_info;
  generator_info.generator_name = RandomGenerator::generator_name;
  generator_info.generate_func = generate_random_generator;
  generator_info.parameters = {
    {"reward_number", "reward number", ParameterBaseType_INT, 10, {}}
  };
  generator_info.ext_info[ext_slot_type_string[ExtSlotTypeAction]] = RandomGeneratorActionExtType::create_ext;
  register_generator(generator_info);
  return 0;
}

RandomGeneratorActionExtType* RandomGeneratorActionExtType::get_ext(GameActionPtr maction) {
  return static_cast<RandomGeneratorActionExtType *>(maction->ext_slot[RandomGenerator::generator_name]);
}

GameExtType* RandomGeneratorActionExtType::create_ext(const void * maction) {
  return new RandomGeneratorActionExtType;
}
