#include "commconfig.h"
#include "globalvar.h"

#include <algorithm>
#include <numeric>

extern int register_game_config() {
  std::vector<ParameterItemType> result;
  std::vector<std::string> models_name = std::accumulate(
    global_var.models.begin(), 
    global_var.models.end(), 
    std::vector<std::string>(), [](std::vector<std::string> &data, const ModelType& a){
      data.push_back(a.model_name);
      return data;
  });
  ParameterBaseTypeEnumExtType model_name_enum_ext;
  model_name_enum_ext.items = models_name;
  
  std::vector<std::string> generators_name = std::accumulate(
    global_var.generators.begin(),
    global_var.generators.end(),
    std::vector<std::string>(),
    [](std::vector<std::string>& data, const GeneratorType& a) {
      data.push_back(a.generator_name);
      return data;
    }
  );

  ParameterBaseTypeEnumExtType generator_name_enum_ext;
  generator_name_enum_ext.items = generators_name;

  result = {
    {"online", "whether web", ParameterBaseType_BOOL, {}, {}},
    {"server", "server name", ParameterBaseType_STRING, std::string("127.0.0.1"), {}},
    {"port", "listen port", ParameterBaseType_INT, 53434, {}},
    {"database", "数据库文件", ParameterBaseType_STRING, "data.db", {}},
    {"model_name", "模型名字", ParameterBaseType_ENUM, 0, model_name_enum_ext},
    {"generator_name", "生成器名字", ParameterBaseType_ENUM, 0, generator_name_enum_ext}
  };
  global_var.global_parameters = result;

  return 0;
}
