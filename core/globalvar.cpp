#include "globalvar.h"

extern int register_model(const ModelType& model_info) {
  global_var.models.push_back(model_info);
  return 0;
}