#include "simple_random.h"
#include "reward_generator/random_generator/random_generator.h"
#include <string>

const std::string SimpleRandomModel::model_name = "simple_random";

extern const std::string SIMPLE_RANDOM_REWARD = "Reward Number";

SimpleRandomModel::SimpleRandomModel(GameConfig& config, GameEnv& env) :
  config(config),
  env(env)
{}

int SimpleRandomModel::run(int now_time) {
  for (int loop_i = 0; loop_i < env.agent_number; ++loop_i) {
    auto magent = env.agents[loop_i];
    if (magent->state->period > now_time) continue;
    auto mstate = magent->state;
    int chosen_action_index = static_cast<int>(random_gen() % (mstate->actions.size()));
    auto maction = mstate->actions[chosen_action_index];
    SimpleRandomActionExt::get_ext(maction)->agent_number++;
    int chosen_tran_index = static_cast<int>(random_gen() % (maction->transitions.size()));
    auto mtran = maction->transitions[chosen_tran_index];
    magent->state = mtran->target;
  }

  int gain = 0;
  for (const auto& mstate : env.time_step_graph[now_time]) {
    for (const auto& maction : mstate->actions) {
      gain += std::min(SimpleRandomActionExt::get_ext(maction)->agent_number, RandomGeneratorActionExtType::get_ext(maction)->reward_number);
    }
  }

  env.indicator[SIMPLE_RANDOM_REWARD] = gain;
  return 0;
}

ModelBaseType* generate_simple_random_model(GameConfig& config, GameEnv& env, std::shared_ptr<Logging> logger) {
  logger->log("Init Simple Random Model");
  return new SimpleRandomModel(config, env);
}

extern int register_simple_random_model() {
  ModelType model_info;
  model_info.model_name = SimpleRandomModel::model_name;
  model_info.generate_func = generate_simple_random_model;
  model_info.parameters = {};
  model_info.ext_info[ext_slot_type_string[ExtSlotTypeAction]] = &SimpleRandomActionExt::create_ext;
  
  model_info.indicator_fields.push_back(
    SIMPLE_RANDOM_REWARD
  );
  register_model(model_info);
  return 0;
}

GameExtType* SimpleRandomActionExt::create_ext(const void * maction) {
  return new SimpleRandomActionExt;
}

SimpleRandomActionExt* SimpleRandomActionExt::get_ext(GameActionPtr maction) {
  return static_cast<SimpleRandomActionExt *>(maction->ext_slot[SimpleRandomModel::model_name]);
}
