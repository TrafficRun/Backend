#include "cmdline.h"

#include "globalvar.h"
#include "commconfig.h"
#include "gameenv.h"
#include "version.h"

#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>



CMDLine::CMDLine() :
  cmd_options(PROJECT_BUILD_INFO)
{
  auto game_config = global_var.global_parameters;
  auto game_env_config = global_var.game_env_parameters;

  boost::program_options::options_description game_config_desc("Game Config");
  for (const auto& item : game_config) {
    generate_parser(game_config_desc, item);
  }

  game_config_desc.add_options()("help,h", "Get Help");
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

  for (int loop_i = 0; loop_i < global_var.generators.size(); ++loop_i) {
    const auto& generator_item = global_var.generators[loop_i];
    boost::program_options::options_description generator_item_config_desc(generator_item.generator_name);
    for (const auto& item : generator_item.parameters) {
      generate_parser(generator_item_config_desc, item);
    }
    cmd_options.add(generator_item_config_desc);
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
    case ParameterBaseType_ENUM: {
      const auto& ext_data = boost::any_cast<ParameterBaseTypeEnumExtType>(item.ext_slot.value());
      std::string enum_list = boost::algorithm::join(ext_data.items, ", ");
      options.add_options()(item.name.c_str(),
        boost::program_options::value<std::string>()->default_value(ext_data.items[boost::any_cast<int>(item.default_value)]),
        (item.description + (boost::format(". Optional: %s")%enum_list).str()).c_str()
      );
    }
    break;
    case ParameterBaseType_RANGE: {
      auto ext_data = boost::any_cast<ParameterBaseTypeRangeExtType>(item.ext_slot.value());
      if (ext_data.is_continue) {
        options.add_options()(item.name.c_str(), boost::program_options::value<double>()->default_value(boost::any_cast<double>(item.default_value)), item.description.c_str());
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

boost::any CMDLine::get_value(const boost::program_options::variable_value& data, const ParameterItemType& item) {
  switch (item.type) {
    case ParameterBaseType_BOOL:
      return !data.empty();
    case ParameterBaseType_ENUM:
      return data.as<std:: string>();
    case ParameterBaseType_FLOAT:
      return data.as<double>();
    case ParameterBaseType_INT:
      return data.as<int>();
    case ParameterBaseType_RANGE: {
      auto ext_data = boost::any_cast<ParameterBaseTypeRangeExtType>(item.ext_slot.value());
      if (ext_data.is_continue) {
        return data.as<double>();
      } else {
        return data.as<int>();
      }
    }
    case ParameterBaseType_STRING:
      return data.as<std::string>();
  };
  return {};
}

int CMDLine::parse_cmd(int argc,char *argv[], GameConfig& config) {
  boost::program_options::variables_map var_map;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmd_options), var_map);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    std::cout << cmd_options;
    std::exit(-1);
  }

  if (var_map.count("help") >= 1) {
    std::cout << cmd_options;
    std::exit(0);
  }

  // game config
  config.online = (var_map.count("online") >= 1);
  config.port = var_map["port"].as<int>();
  config.server = var_map["server"].as<std::string>();
  config.database = var_map["database"].as<std::string>();
  config.model_name = var_map["model_name"].as<std::string>();
  config.generator_name = var_map["generator_name"].as<std::string>();

  // TODO 改成插件模式
  // game env config
  auto game_env_config_info = global_var.game_env_parameters;
  std::map<std::string, boost::any> game_env_config;
  for (const auto& param_item : game_env_config_info) {
    game_env_config[param_item.name] = get_value(var_map[param_item.name], param_item);
  }
  config.ext_config["env"] = game_env_config;

  for (const auto& model_item : global_var.models) {
    std::map<std::string, boost::any> model_config;
    for (const auto &param_item : model_item.parameters) {
      model_config[param_item.name] = get_value(var_map[param_item.name], param_item);
    }
    config.ext_config[model_item.model_name] = model_config;
  }

  for (const auto& generator_item : global_var.generators) {
    std::map<std::string, boost::any> generator_config;
    for (const auto &param_item : generator_item.parameters) {
      generator_config[param_item.name] = get_value(var_map[param_item.name], param_item);
    }
    config.ext_config[generator_item.generator_name] = generator_config;
  }
  return 0;
}
