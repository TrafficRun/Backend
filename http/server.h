#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#define CPPHTTPLIB_NO_EXCEPTIONS

#include "core/commconfig.h"
#include "core/gameenv.h"
#include "cpp-httplib/httplib.h"

#include <boost/json.hpp>
#include <boost/any.hpp>
#include <thread>

class GameRunHandle {
public:
  GameRunHandle(GameConfig &config);
  GameEnvDetail get_env_detail();
  std::optional<GameSnapshotResultType> get_snapshot(int time_step);
  bool is_finish();
private:
  std::shared_ptr<GameEnv> m_env;
  std::shared_ptr<GameConfig> m_config;
  std::thread m_run_handle;
  bool m_is_finish = false;
};

class HttpServer {
public:
  HttpServer(GameConfig& config);
  int run();
  ~HttpServer();
private:
  typedef httplib::Request req_type;
  typedef httplib::Response rsp_type;
  void http_get_common_config(const req_type& req, rsp_type& rsp);
  void http_get_model_config(const req_type& req, rsp_type& rsp);
  void http_get_generator_config(const req_type& req, rsp_type& rsp);
  void http_post_begin_simulate(const req_type& req, rsp_type& rsp);
  void http_get_simulate_result(const req_type& req, rsp_type& rsp);
  void http_get_version(const req_type& req, rsp_type& rsp);

  void logger(const req_type&req, const rsp_type& rsp);
  boost::any get_param(const std::string& value, const ParameterItemType& item);
  std::string result_from(int code, const boost::json::value& data);
  GameConfig& config;
  std::map<std::string, std::shared_ptr<GameRunHandle>> run_handles;
};

#endif