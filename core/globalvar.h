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

class GlobalVar {
public:
  std::vector<ModelType> models;
};

extern GlobalVar global_var;

//TODO Achieve
extern int register_model(const ModelType& model_info);
#endif
