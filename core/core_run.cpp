#include "core_run.h"
#include "globalvar.h"

#include <algorithm>
#include <numeric>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

CoreRun::CoreRun(GameConfig& config, GameEnv& env) : 
  env(env),
  config(config)
{
  uid = boost::uuids::to_string(boost::uuids::random_generator()());
  simulate_name = config.simulate_name;
}

int CoreRun::run() {
  const auto& model_config = std::find_if(global_var.models.begin(), global_var.models.end(), [&](const ModelType& item){
    return item.model_name == config.model_name;
  });
  if (model_config == global_var.models.end()) {
    std::cout << "Not Such Model." << std::endl;
    std::exit(-1);
  }
  // 添加模型的扩展
  add_exts(model_config->model_name, model_config->ext_info);
  // 模型对象
  ModelBaseType *model_obj = model_config->generate_func(config, env);

  const auto& generator_config = std::find_if(global_var.generators.begin(), global_var.generators.end(), [&](const GeneratorType& item) {
    return item.generator_name == config.generator_name;
  });
  if (generator_config == global_var.generators.end()) {
    std::cout << "Not Such Model." << std::endl;
    std::exit(-1);
  }
  // 添加生成器的扩展
  add_exts(generator_config->generator_name, generator_config->ext_info);
  // 生成器对象
  GeneratorBaseType *generator_obj = generator_config->generate_func(config, env);

  for (int loop_time = 0; loop_time < env.time_step; ++loop_time) {
    env.rewards.clear();
    generator_obj->generate(loop_time);
    model_obj->run(loop_time);
    env.commit();
  }

  delete model_obj;
  delete generator_obj;
  return 0;
}

int CoreRun::add_exts(const std::string &ext_name, const std::map<std::string, generate_ext_func_type>& exts) {
  std::vector<generate_ext_func_type> ext_funs(ext_slot_type_string.size(), nullptr);

  for (int loop_ext_index = 0; loop_ext_index < ext_funs.size(); ++loop_ext_index) {
    if (exts.find(ext_slot_type_string[loop_ext_index]) != exts.end()) {
      ext_funs[loop_ext_index] = exts.at(ext_slot_type_string[loop_ext_index]);
    }
  }

  for (auto mstate : env.graph) {
    if (ext_funs[ExtSlotTypeState] != nullptr) {
      mstate->ext_slot[ext_name] = ext_funs[ExtSlotTypeState](mstate);
    }
    for (auto maction : mstate->actions) {
      if (ext_funs[ExtSlotTypeAction] != nullptr) {
        maction->ext_slot[ext_name] = ext_funs[ExtSlotTypeAction](maction);
      }
      for (auto mtran : maction->transitions) {
        if (ext_funs[ExtSlotTypeTransition] != nullptr) {
          mtran->ext_slot[ext_name] = ext_funs[ExtSlotTypeTransition](mtran);
        }
      }
    } 
  }

  if (ext_funs[ExtSlotTypeAgent] != nullptr) {
    for (auto magent : env.agents) {
      magent->ext_slot[ext_name] = ext_funs[ExtSlotTypeAgent](magent);
    }
  }

  if (ext_funs[ExtSlotTypeReward] != nullptr) {
    env.reward_ext_funs[ext_name] = ext_funs[ExtSlotTypeReward];
  }

  return 0;
}