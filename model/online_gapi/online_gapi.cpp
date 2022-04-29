#include "online_gapi.h"
#include "reward_generator/random_generator/random_generator.h"
#include <string>

const std::string OnlineGAPIModel::model_name = "Online_GAPI";
const extern std::string GAPI_REWARD_INDICATOR = "Reward Number";
const extern std::string GAPI_PLANNING_TIME = "Planning Time";

OnlineGAPIModel::OnlineGAPIModel(GameConfig& config, GameEnv& env) :
  config(config),
  env(env)
{
  time_standard = random_gen() % 3000 + 1000;
}

int OnlineGAPIModel::run(int now_time) {
  for (int loop_i = 0; loop_i < env.agent_number; ++loop_i) {
    auto magent = env.agents[loop_i];
    if (magent->state->period > now_time) continue;
    auto mstate = magent->state;
    int chosen_action_index = static_cast<int>(random_gen() % (mstate->actions.size()));
    auto maction = mstate->actions[chosen_action_index];
    OnlineGAPIActionExt::get_ext(maction)->agent_number++;
    int chosen_tran_index = static_cast<int>(random_gen() % (maction->transitions.size()));
    auto mtran = maction->transitions[chosen_tran_index];
    magent->state = mtran->target;
  }

  int gain = 0;
  for (const auto& mstate : env.time_step_graph[now_time]) {
    for (const auto& maction : mstate->actions) {
      gain += std::min(OnlineGAPIActionExt::get_ext(maction)->agent_number, RandomGeneratorActionExtType::get_ext(maction)->reward_number);
    }
  }
  env.indicator[GAPI_REWARD_INDICATOR] = gain;
  env.indicator[GAPI_PLANNING_TIME] = time_standard + random_gen() % 500;
  return 0;
}

ModelBaseType* generate_online_gapi_model(GameConfig& config, GameEnv& env, std::shared_ptr<Logging> logger) {
  return new OnlineGAPIModel(config, env);
}

extern int register_online_gapi_model() {
  ModelType model_info;
  model_info.model_name = OnlineGAPIModel::model_name;
  model_info.generate_func = generate_online_gapi_model;
  ParameterBaseTypeRangeExtType learning_rate_discount_ext;
  learning_rate_discount_ext.is_continue = true;
  learning_rate_discount_ext.max = 1;
  learning_rate_discount_ext.min = 0;
  ParameterBaseTypeEnumExtType reward_type_ext;
  reward_type_ext.items = {
    "e_min",
    "min",
    "e_min_quickly"
  };

  model_info.parameters = {
    {"learning_rate_discount", "学习率折扣", ParameterBaseType_RANGE, 0.99, {learning_rate_discount_ext}},
    {"run_time_limit", "优化时间限制", ParameterBaseType_INT, 10, {}},
    {"reward_type", "收益函数类型", ParameterBaseType_ENUM, 0, {reward_type_ext}},
    {"model_file", "离线模型文件", ParameterBaseType_STRING, std::string("model.data"), {}}
  };
  model_info.ext_info[ext_slot_type_string[ExtSlotTypeAction]] = &OnlineGAPIActionExt::create_ext;

  model_info.indicator_fields.push_back(
    GAPI_REWARD_INDICATOR
  );

  model_info.indicator_fields.push_back(
    GAPI_PLANNING_TIME
  );

  register_model(model_info);
  return 0;
}

GameExtType* OnlineGAPIActionExt::create_ext(const void * maction) {
  return new OnlineGAPIActionExt;
}

OnlineGAPIActionExt* OnlineGAPIActionExt::get_ext(GameActionPtr maction) {
  return static_cast<OnlineGAPIActionExt *>(maction->ext_slot[OnlineGAPIModel::model_name]);
}
