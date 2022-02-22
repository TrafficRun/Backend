#include "server.h"
#include "core/globalvar.h"
#include "core/gameenv.h"
#include "core/commconfig.h"

#include "cpp-httplib/httplib.h"

#include <functional>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>

#include <boost/json.hpp>

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
  rsp.set_content(bj::serialize(result), http_json_mine_type);
}

void HttpServer::http_get_model_config(const req_type& req, rsp_type& rsp) {
  std::string model_name = req.get_param_value("model_name");
  auto model_config_iter = std::find_if(global_var.models.begin(), global_var.models.end(), [&](const ModelType& item){
    return item.model_name == model_name;
  });
  if (model_config_iter == global_var.models.end()) {
    rsp.set_content(bj::serialize(bj::value({
      {"code", -1},
      {"data", nullptr}
    })) , http_json_mine_type);
    return;
  }
  rsp.set_content(bj::serialize(bj::value({
    {"code", 0},
    {"data", model_config_iter->parameters}
  })), http_json_mine_type);
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
