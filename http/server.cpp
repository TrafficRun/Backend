#include "server.h"
#include "core/globalvar.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "core/core_run.h"

#include "cpp-httplib/httplib.h"

#include <functional>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>

#include <boost/json.hpp>
#include <boost/any.hpp>

namespace bj = boost::json;
namespace hl = httplib;
namespace pl = std::placeholders;

const static char* http_json_mine_type = "application/json";
HttpServer::HttpServer(GameConfig& config) : config(config) {}

int HttpServer::run() {
  hl::Server serv;
  serv.set_logger(std::bind(&HttpServer::logger, this, pl::_1, pl::_2));
  serv.Get("/comm_config", std::bind(&HttpServer::http_get_common_config, this, pl::_1, pl::_2));
  serv.Get("/model_config", std::bind(&HttpServer::http_get_model_config, this, pl::_1, pl::_2));
  serv.set_default_headers({
    {"Access-Control-Allow-Origin", "*"}
  });
  serv.listen(config.server->c_str(), config.port.value());
  return 0;  
}

void HttpServer::http_get_common_config(const req_type& req, rsp_type& rsp) {
  std::cout << "Test" << std::endl;
  std::vector<ParameterItemType> temp_global_parameters = global_var.global_parameters;

  std::map<std::string, int> temp_global_parameters_map;
  for (int loop_i = 0; loop_i < temp_global_parameters.size(); ++loop_i) {
    temp_global_parameters_map[temp_global_parameters[loop_i].name] = loop_i;
  }

  temp_global_parameters[temp_global_parameters_map["server"]].default_value = config.server.value();
  temp_global_parameters[temp_global_parameters_map["online"]].default_value = config.online.value();
  temp_global_parameters[temp_global_parameters_map["port"]].default_value = config.port.value();

  bj::value result = bj::value({
    {"global", temp_global_parameters},
    {"env", global_var.game_env_parameters}
  });
  rsp.set_content(result_from(0, result), http_json_mine_type);
}

void HttpServer::http_get_model_config(const req_type& req, rsp_type& rsp) {
  std::string model_name = req.get_param_value("model_name");
  auto model_config_iter = std::find_if(global_var.models.begin(), global_var.models.end(), [&](const ModelType& item){
    return item.model_name == model_name;
  });
  if (model_config_iter == global_var.models.end()) {
    rsp.set_content(result_from(-1, bj::value_from(nullptr)), http_json_mine_type);
    return;
  }
  rsp.set_content(result_from(0, bj::value_from(model_config_iter->parameters)), http_json_mine_type);
}

void HttpServer::http_get_generator_config(const req_type& req, rsp_type& rsp) {
  std::string generator_name = req.get_param_value("generator_name");
  auto generator_config_iter = std::find_if(global_var.generators.begin(), global_var.generators.end(), [&](const GeneratorType& item) {
    return item.generator_name == generator_name;
  });
  
  if (generator_config_iter == global_var.generators.end()) {
    rsp.set_content(result_from(-1, bj::value_from(nullptr)), http_json_mine_type);
    return;
  }

  rsp.set_content(result_from(0, bj::value_from(generator_config_iter->parameters)), http_json_mine_type);
}

void HttpServer::http_post_begin_simulate(const req_type& req, rsp_type& rsp) {
  auto model_name = req.get_param_value("model_name");
  auto generator_name = req.get_param_value("generator_name");
  
  config.model_name = model_name;
  config.generator_name = generator_name;
  
  auto model_config = global_var.get_model_config(model_name);
  auto generator_config = global_var.get_generator_config(generator_name);
  
  // set model config
  std::map<std::string, boost::any> model_set_config;
  for (int loop_i = 0; loop_i < model_config.parameters.size(); ++loop_i) {
    const auto& param_item = model_config.parameters[loop_i];
    model_set_config[param_item.name] = get_param(req.get_param_value(param_item.name.c_str()), param_item);
  }
  config.ext_config[model_name] = model_set_config;

  // set generator config
  std::map<std::string, boost::any> generator_set_config;
  for (int loop_i = 0; loop_i < generator_config.parameters.size(); ++loop_i) {
    const auto& param_item = generator_config.parameters[loop_i];
    generator_set_config[param_item.name] = get_param(req.get_param_value(param_item.name.c_str()), param_item);
  }
  config.ext_config[generator_name] = generator_set_config;

  // set env config
  std::map<std::string, boost::any> env_set_config;
  const auto& env_config = global_var.game_env_parameters;
  for (int loop_i = 0; loop_i < env_config.size(); ++loop_i) {
    const auto param_item = env_config[loop_i];
    env_set_config[param_item.name] = get_param(req.get_param_value(param_item.name.c_str()), param_item);
  }
  config.ext_config["env"] = env_set_config;

  begin_simulate();
  rsp.set_content(result_from(0, nullptr), http_json_mine_type);
}

void HttpServer::logger(const req_type&req, const rsp_type& rsp) {
  // std::cout << "Req: -------\n" << req.body << std::endl;
  // std::cout << "Rsp: -------\n" << rsp.body << std::endl;
}

std::string HttpServer::result_from(int code, const bj::value& data) {
  return bj::serialize(bj::value({
    {"code", code},
    {"data", data}
  }));
}

void HttpServer::http_get_simulate_result(const req_type& req, rsp_type& rsp) {
  int time_step = std::atoi(req.get_param_value("time_step").c_str());
  auto result_data = env->snapshot->get(time_step);
  if (result_data.has_value()) {
    rsp.set_content(result_from(0, bj::value_from(result_data.value())), http_json_mine_type);
  } else {
    rsp.set_content(result_from(-1, bj::value(nullptr)), http_json_mine_type);
  }
}

HttpServer::~HttpServer() {
  delete env;
}

boost::any HttpServer::get_param(const std::string& value, const ParameterItemType& item) {
  boost::any result;
  switch (item.type) {
    case ParameterBaseType_BOOL:
      if (value == "1") {
        result = true;
      } else {
        result = false;
      }
    break;
    case ParameterBaseType_ENUM:
      result = value;
    break;
    case ParameterBaseType_FLOAT:
      result = std::atof(value.c_str());
    break;
    case ParameterBaseType_INT:
      result = std::atoi(value.c_str());
    break;
    case ParameterBaseType_STRING:
      result = value;
    break;
    case ParameterBaseType_RANGE: {
      const auto& ext_data = boost::any_cast<ParameterBaseTypeRangeExtType>(item.ext_slot);
      if (ext_data.is_continue) {
        result = std::atof(value.c_str());
      } else {
        result = std::atoi(value.c_str());
      }
    }
    break;
  }
  return result;
}

void HttpServer::begin_simulate() {
  run_thread = std::thread([&](){
    if (env != nullptr) {
      delete env;
    }
    env = new GameEnv(config);
    CoreRun core(config, *env);
    core.run(); 
  });
}
