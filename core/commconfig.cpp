#include "commconfig.h"
#include "globalvar.h"

#include <algorithm>
#include <numeric>

extern std::vector<ParameterItemType>  generate_game_config() {
  std::vector<ParameterItemType> result;
  std::vector<std::string> models_name = std::accumulate(
    global_var.models.begin(), 
    global_var.models.begin(), 
    std::vector<std::string>(), [](std::vector<std::string> &data, const ModelType& a){
      data.push_back(a.model_name);
      return data;
  });
  auto enum_ext = new ParameterBaseTypeEnumExtType;
  enum_ext->items = models_name;
  result = {
    {"online", "whether web", ParameterBaseType_BOOL, {}, nullptr},
    {"server", "server name", ParameterBaseType_STRING, "127.0.0.1", nullptr},
    {"port", "listen port", ParameterBaseType_INT, 53434, nullptr},
    {"model_type", "模型列表", ParameterBaseType_ENUM, 1, enum_ext}
  };
  return result;
}