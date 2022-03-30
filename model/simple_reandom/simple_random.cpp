#include "simple_random.h"

#include <string>

const static std::string model_name = "simple_random";

SimpleRandomModel::SimpleRandomModel(GameConfig& config, GameEnv& env) :
  config(config),
  env(env)
{}

int SimpleRandomModel::run(int now_time) {
  for (int loop_i = 0; loop_i < env.agent_number; ++loop_i) {
    auto magent = env.agents[loop_i];
    if ((magent->state->position_id - 1) / env.position_num > now_time) continue;
    auto mstate = magent->state;
    int chosen_action_index = static_cast<int>(random_gen() % (mstate->actions.size()));
    auto maction = mstate->actions[chosen_action_index];
    int chosen_tran_index = static_cast<int>(random_gen() % (maction->transitions.size()));
    auto mtran = maction->transitions[chosen_tran_index];
    magent->state = mtran->target;
  }
  return 0;
}

ModelBaseType* generate_simple_random_model(GameConfig& config, GameEnv& env) {
  return new SimpleRandomModel(config, env);
}

extern int register_simple_random_model() {
  ModelType model_info;
  model_info.model_name = model_name;
  model_info.generate_func = generate_simple_random_model;
  model_info.parameters = {};
  register_model(model_info);
  return 0;
}