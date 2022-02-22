#ifndef _CORE_GLOBAL_VAR_H
#define _CORE_GLOBAL_VAR_H

#include "parameter_type.h"
#include "commconfig.h"
#include "gameenv.h"

typedef ModelBaseType* (*generate_model_func_type)(GameConfig& config, GameEnv& env);

struct ModelType {
  std::string model_name;
  std::vector<ParameterItemType> parameters;
  generate_model_func_type generate_func;
};

typedef GeneratorBaseType* (*generate_generator_fun_type)(GameConfig& config, GameEnv& env);

struct GeneratorType {
  std::string generator_name;
  std::vector<ParameterItemType> parameters;
  generate_generator_fun_type generate_func;
};

class GlobalVar {
public:
  std::vector<ModelType> models;
  std::vector<GeneratorType> generators;
  std::vector<ParameterItemType> global_parameters;
  std::vector<ParameterItemType> game_env_parameters;
};

extern GlobalVar global_var;

extern int register_model(const ModelType& model_info);
extern int register_generator(const GeneratorType &generator_info);

#endif
