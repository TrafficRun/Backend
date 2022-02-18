#include "cmdline.h"

#include "globalvar.h"
#include "commconfig.h"
#include "gameenv.h"

#include <boost/program_options.hpp>
#include <boost/any.hpp>

CMDLine::CMDLine() :
  cmd_options("Traffic Run")
{
  auto game_config = generate_game_config();
  auto game_env_config = generate_game_env_config();

  boost::program_options::options_description game_config_desc("Game Config");
  for (const auto& item : game_config) {
    generate_parser(game_config_desc, item);
  }
  cmd_options.add(game_config_desc);
  
  boost::program_options::options_description game_env_config_desc("Game Env Config");
  for (const auto& item : game_env_config) {
    generate_parser(game_env_config_desc, item);
  }
  cmd_options.add(game_env_config_desc);

  for (int loop_i = 0; loop_i < global_var.models.size(); ++loop_i) {
    const auto& model_item = global_var.models[loop_i];
    boost::program_options::options_description model_item_config_desc(model_item.model_name);
    for (const auto& item : model_item.parameters) {
      generate_parser(model_item_config_desc, item);
    }
    cmd_options.add(model_item_config_desc);
  }
}

int CMDLine::generate_parser(boost::program_options::options_description& options, const ParameterItemType& item) {
  switch (item.type) {
    case ParameterBaseType_INT:
      options.add_options()(item.name.c_str(), boost::program_options::value<int>()->default_value(boost::any_cast<int>(item.default_value)), item.description.c_str());
    break;
    case ParameterBaseType_FLOAT:
      options.add_options()(item.name.c_str(), boost::program_options::value<float>()->default_value(boost::any_cast<float>(item.default_value)), item.description.c_str());
    break;
    case ParameterBaseType_STRING:
      options.add_options()(item.name.c_str(),
        boost::program_options::value<std::string>()->default_value(boost::any_cast<std::string>(item.default_value)),
        item.description.c_str()
      );
    break;
    case ParameterBaseType_ENUM:
      options.add_options()(item.name.c_str(),
        boost::program_options::value<std::string>()->default_value(boost::any_cast<std::string>(item.default_value)),
        item.description.c_str()
      );
    break;
    case ParameterBaseType_RANGE:{
      auto ext_data = static_cast<ParameterBaseTypeRangeExtType*>(item.ext_slot);
      if (ext_data->is_continue) {
        options.add_options()(item.name.c_str(), boost::program_options::value<float>()->default_value(boost::any_cast<float>(item.default_value)), item.description.c_str());
      } else {
        options.add_options()(item.name.c_str(), boost::program_options::value<int>()->default_value(boost::any_cast<int>(item.default_value)), item.description.c_str());
      }
    }
    break;
    case ParameterBaseType_BOOL:
      options.add_options()(
        item.name.c_str(),
        item.description.c_str()
      );
    break;
  }
  return 0;
}