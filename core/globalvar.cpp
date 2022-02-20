#include "globalvar.h"

extern int register_model(const ModelType& model_info) {
  global_var.models.push_back(model_info);
  return 0;
}

extern int register_generator(const GeneratorType &generator_info) {
  global_var.generators.push_back(generator_info);
  return 0;
}
