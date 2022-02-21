#include "server.h"
#include "core/globalvar.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "cpp-httplib/httplib.h"

#include <functional>
#include <string>

#include <boost/json.hpp>

const static std::string http_json_mine_type = "application/json";

HttpServer::HttpServer(GameConfig& config) : config(config) {}

int HttpServer::run() {
  httplib::Server serv;
  serv.Get("/comm_config", std::bind(&HttpServer::http_get_common_config, this, std::placeholders::_1, std::placeholders::_2));
  serv.listen(config.server->c_str(), config.port.value());
  return 0;  
}

void HttpServer::http_get_common_config(const httplib::Request& req, httplib::Response& rsp) {
  boost::json::value result = boost::json::value({
    {"global", global_var.global_parameters},
    {"env", global_var.game_env_parameters}
  });
  rsp.set_content(boost::json::serialize(result), http_json_mine_type.c_str());
}
