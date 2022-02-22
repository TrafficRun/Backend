#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#define CPPHTTPLIB_NO_EXCEPTIONS

#include "core/commconfig.h"
#include "cpp-httplib/httplib.h"
#include <boost/json.hpp>

class HttpServer {
public:
  HttpServer(GameConfig& config);
  int run();
  ~HttpServer(){};
private:
  typedef httplib::Request req_type;
  typedef httplib::Response rsp_type;
  void http_get_common_config(const req_type& req, rsp_type& rsp);
  void http_get_model_config(const req_type& req, rsp_type& rsp);
  void logger(const req_type&req, const rsp_type& rsp);
  std::string result_from(int code, const boost::json::value& data);
  GameConfig& config;
};

#endif