#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include "core/commconfig.h"
#include "cpp-httplib/httplib.h"

class HttpServer {
public:
  HttpServer(GameConfig& config);
  int run();
  ~HttpServer(){};
private:
  void http_get_common_config(const httplib::Request& req, httplib::Response& rsp);
  GameConfig& config;
};

#endif