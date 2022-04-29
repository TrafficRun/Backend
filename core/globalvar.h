#ifndef _CORE_GLOBAL_VAR_H
#define _CORE_GLOBAL_VAR_H

#include "parameter_type.h"
#include "commconfig.h"
#include "gameenv.h"
#include "logging/logging.h"

#include <functional>

typedef std::function<ModelBaseType*(GameConfig& config, GameEnv& env, std::shared_ptr<Logging>)> generate_model_func_type;

enum ExtSlotType {
  ExtSlotTypeAgent = 0,
  ExtSlotTypeReward = 1,
  ExtSlotTypeState = 2,
  ExtSlotTypeAction = 3,
  ExtSlotTypeTransition = 4
};

extern const std::vector<std::string> ext_slot_type_string;

struct ModelType {
  std::string model_name;
  std::vector<ParameterItemType> parameters;
  generate_model_func_type generate_func;
  std::map<std::string, generate_ext_func_type> ext_info;
  std::vector<std::string> indicator_fields;
};

typedef std::function<GeneratorBaseType*(GameConfig& config, GameEnv& env, std::shared_ptr<Logging>)> generate_generator_fun_type;

struct GeneratorType {
  std::string generator_name;
  std::vector<ParameterItemType> parameters;
  generate_generator_fun_type generate_func;
  std::map<std::string, generate_ext_func_type> ext_info;
};

class GlobalVar {
public:
  std::vector<ModelType> models;
  std::vector<GeneratorType> generators;
  std::vector<ParameterItemType> global_parameters;
  std::vector<ParameterItemType> game_env_parameters;
  const ModelType &get_model_config(const std::string& model_name);
  const GeneratorType& get_generator_config(const std::string& generator_name);
};

extern GlobalVar global_var;

extern int register_model(const ModelType& model_info);
extern int register_generator(const GeneratorType &generator_info);

#endif
