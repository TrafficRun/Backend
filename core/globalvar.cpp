#include "globalvar.h"

#include <algorithm>
#include <numeric>

GlobalVar global_var;

const std::vector<std::string> ext_slot_type_string = {
  "agent",
  "reward",
  "state",
  "action",
  "transition"
};

extern int register_model(const ModelType& model_info) {
  global_var.models.push_back(model_info);
  return 0;
}

extern int register_generator(const GeneratorType &generator_info) {
  global_var.generators.push_back(generator_info);
  return 0;
}

const ModelType & GlobalVar::get_model_config(const std::string& model_name) {
  return *std::find_if(models.begin(), models.end(), [&](const ModelType& item) {
    return item.model_name == model_name;
  });  
}

const GeneratorType& GlobalVar::get_generator_config(const std::string& generator_name) {
  return *std::find_if(generators.begin(), generators.end(), [&](const GeneratorType& item) {
    return item.generator_name == generator_name;
  });
}
