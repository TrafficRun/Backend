#include "core_run.h"
#include "globalvar.h"
#include <iostream>

CoreRun::CoreRun(GameConfig& config, GameEnv& env) : 
  env(env),
  config(config)
{

}

int CoreRun::run() {
  const auto& model_config = std::find_if(global_var.models.begin(), global_var.models.end(), [&](const ModelType& item){
    return item.model_name == config.model_name;
  });
  if (model_config == global_var.models.end()) {
    std::cout << "Not Such Model." << std::endl;
    std::exit(-1);
  }
  ModelBaseType *model_obj = model_config->generate_func(config, env);
  
  const auto& generator_config = std::find_if(global_var.generators.begin(), global_var.generators.end(), [&](const GeneratorType& item) {
    return item.generator_name == config.generator_name;
  });
  if (generator_config == global_var.generators.end()) {
    std::cout << "Not Such Model." << std::endl;
    std::exit(-1);
  }
  GeneratorBaseType *generator_obj = generator_config->generate_func(config, env);

  for (int loop_time = 0; loop_time < env.time_step; ++loop_time) {
    generator_obj->generate(loop_time);
    model_obj->run(loop_time);
  }

  delete model_obj;
  delete generator_obj;
  return 0;
}