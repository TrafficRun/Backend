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
    {"database", "数据库文件", ParameterBaseType_STRING, std::string("data.db"), {}},
    {"model_name", "模型名字", ParameterBaseType_ENUM, 0, model_name_enum_ext},
    {"work_root_dir", "保存数据的根目录", ParameterBaseType_STRING, std::string("./work_log/"), {}},
    {"generator_name", "生成器名字", ParameterBaseType_ENUM, 0, generator_name_enum_ext},
    {"simulate_name", "仿真名", ParameterBaseType_STRING, std::string("Simulate Temp", {})}
  };
  global_var.global_parameters = result;

  return 0;
}


extern void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const GameConfig &c) {
  boost::json::object jb = boost::json::value({
    {"online", c.online},
    {"server", c.server},
    {"port", c.port},
    {"database", c.database},
    {"model_name", c.model_name},
    {"work_root_dir", c.work_root_dir},
    {"generator_name", c.generator_name},
    {"simulate_name", c.simulate_name}
  }).as_object();

  for (const auto& params : c.ext_config) {
    boost::json::object t_jb;
    for (const auto& param: params.second) {
      std::string m_typename = param.second.type().name();
      if (m_typename == typeid(true).name()) {
        t_jb[param.first] = boost::any_cast<bool>(param.second);
      } else if (m_typename == typeid(1).name()) {
        t_jb[param.first] = boost::any_cast<int>(param.second);
      } else if (m_typename == typeid(0.1).name()) {
        t_jb[param.first] = boost::any_cast<double>(param.second);
      } else {
        t_jb[param.first] = boost::any_cast<std::string>(param.second);
      }
    }
    jb[params.first] = t_jb;
  }

  jv = jb;
}