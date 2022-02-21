#include "server.h"
#include "core/globalvar.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "cpp-httplib/httplib.h"

#include <functional>

HttpServer::HttpServer(GameConfig& config) : config(config) {}

int HttpServer::run() {
  httplib::Server serv;
  serv.Get("/comm_config", std::bind(&HttpServer::http_get_common_config, this, std::placeholders::_1, std::placeholders::_2));
  serv.listen(config.server->c_str(), config.port.value());
  return 0;  
}

void HttpServer::http_get_common_config(const httplib::Request& req, httplib::Response& rsp) {
  rsp.set_content("Hello World!", "text/plain");
}