#include "server.h"
#include "core/globalvar.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "core/core_run.h"
#include "core/version.h"

#include "cpp-httplib/httplib.h"

#include <functional>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>

#include <boost/json.hpp>
#include <boost/any.hpp>

const static char* http_json_mine_type = "application/json";
HttpServer::HttpServer(GameConfig& config) : config(config) {
  serv.set_logger(std::bind(&HttpServer::logger, this, pl::_1, pl::_2));
  // 返回公共信息
  serv.Get("/comm_config", std::bind(&HttpServer::http_get_common_config, this, pl::_1, pl::_2));
  // 返回模型配置
  serv.Get("/model_config", std::bind(&HttpServer::http_get_model_config, this, pl::_1, pl::_2));
  // 返回生成器信息
  serv.Get("/generator_config", std::bind(&HttpServer::http_get_generator_config, this, pl::_1, pl::_2));
  // 开始模拟
  serv.Post("/begin_simulate", static_cast<std::function<void(const req_type&, rsp_type&)>>(std::bind(&HttpServer::http_post_begin_simulate, this, pl::_1, pl::_2)));
  // 返回模拟结果
  serv.Get("/simulate_result", std::bind(&HttpServer::http_get_simulate_result, this, pl::_1, pl::_2));
  // 获取版本信息
  serv.Get("/version", std::bind(&HttpServer::http_get_version, this, pl::_1, pl::_2));

  // 头  
  serv.set_default_headers({
    {"Access-Control-Allow-Origin", "*"},
    {"Access-Control-Allow-Methods", "POST,GET,OPTIONS,DELETE"}
  });
}

int HttpServer::run() {
  serv.listen(config.server.c_str(), config.port);
  return 0;
}

void HttpServer::http_get_version(const req_type& req, rsp_type& rsp) {
  bj::value result = bj::value({
    {"version", PROJECT_BUILD_INFO}
  });
  rsp.set_content(result_from(0, result), http_json_mine_type);
}

/* 返回公共信息
 * 输入：空
 * 返回：{
 *   global: [
 *     {
 *       "name": str,
 *       "description": str,
 *       "type": str,
 *       "default_value": str | int | float,
 *       "ext_slot": null | [str] | {
 *         min: int,
 *         max: int,
 *         is_continue: int
 *       }
 *     }, ....
 *   ],
 *   env: same above
 * }
 */
void HttpServer::http_get_common_config(const req_type& req, rsp_type& rsp) {
  std::vector<ParameterItemType> temp_global_parameters = global_var.global_parameters;

  std::map<std::string, int> temp_global_parameters_map;
  for (int loop_i = 0; loop_i < temp_global_parameters.size(); ++loop_i) {
    temp_global_parameters_map[temp_global_parameters[loop_i].name] = loop_i;
  }

  temp_global_parameters[temp_global_parameters_map["server"]].default_value = config.server;
  temp_global_parameters[temp_global_parameters_map["online"]].default_value = config.online;
  temp_global_parameters[temp_global_parameters_map["port"]].default_value = config.port;

  bj::value result = bj::value({
    {"global", temp_global_parameters},
    {"env", global_var.game_env_parameters}
  });
  rsp.set_content(result_from(0, result), http_json_mine_type);
}

/* 获取模型配置
 * 输入：{
 *   "model_name": str 
 * }
 * 输出：[
 *   same above, ...
 * ]
 */ 
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

/* 获取模型配置
 * 输入：{
 *   "generator_name": str 
 * }
 * 输出：[
 *   same above, ...
 * ]
 */ 
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

/* 开始模拟
 * 输入：{[key : str] : str | int | float}
 * 返回：{
 *   "time_step" : int,
 *   "token": str,
 *   "agent_number" : int,
 *   "graph_type" : str,
 *   "graph": {
 *     "height": int,
 *     "width": int
 *   } | null
 * }
 */
void HttpServer::http_post_begin_simulate(const req_type& req, rsp_type& rsp) {
  auto model_name = req.get_param_value("model_name");
  auto generator_name = req.get_param_value("generator_name");
  GameConfig fconfig = config;

  // set global config
  fconfig.model_name = model_name;
  fconfig.generator_name = generator_name;
  
  auto model_config = global_var.get_model_config(model_name);
  auto generator_config = global_var.get_generator_config(generator_name);
  
  // set model config
  std::map<std::string, boost::any> model_set_config;
  for (int loop_i = 0; loop_i < model_config.parameters.size(); ++loop_i) {
    const auto& param_item = model_config.parameters[loop_i];
    model_set_config[param_item.name] = get_param(req.get_param_value(param_item.name.c_str()), param_item);
  }
  fconfig.ext_config[model_name] = model_set_config;

  // set generator config
  std::map<std::string, boost::any> generator_set_config;
  for (int loop_i = 0; loop_i < generator_config.parameters.size(); ++loop_i) {
    const auto& param_item = generator_config.parameters[loop_i];
    generator_set_config[param_item.name] = get_param(req.get_param_value(param_item.name.c_str()), param_item);
  }
  fconfig.ext_config[generator_name] = generator_set_config;

  // set env config
  std::map<std::string, boost::any> env_set_config;
  const auto& env_config = global_var.game_env_parameters;
  for (int loop_i = 0; loop_i < env_config.size(); ++loop_i) {
    const auto param_item = env_config[loop_i];
    env_set_config[param_item.name] = get_param(req.get_param_value(param_item.name.c_str()), param_item);
  }
  fconfig.ext_config["env"] = env_set_config;

  auto run_handle = std::make_shared<GameRunHandle>(fconfig);
  run_handles[run_handle->get_env_detail().uid] = run_handle;

  auto returnJsonObj = boost::json::value_from(run_handle->get_env_detail()).as_object();

  rsp.set_content(result_from(0, returnJsonObj), http_json_mine_type);
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

/* 获取模拟结果
 * 输入 {
 *  "time_step": int,
 *  "token": str
 * }
 * 输出 {
 *   "time_step": int,
 *   "agents": [{
 *     "agent_id": int,
 *     "path": [int, ...],
 *     "period": int
 *   }],
 *   "rewards": [int, ...],
 *   "gain": double
 * }
 */
void HttpServer::http_get_simulate_result(const req_type& req, rsp_type& rsp) {
  int time_step = std::atoi(req.get_param_value("time_step").c_str());
  std::string run_token = req.get_param_value("uid");
  if (run_handles.find(run_token) != run_handles.end()) {
    const auto& run_handle = run_handles[run_token];
    auto result_data = run_handle->get_snapshot(time_step);
    if (result_data.has_value()) {
      rsp.set_content(result_from(0, bj::value_from(result_data.value())), http_json_mine_type);
      return;
    }
  }
  rsp.set_content(result_from(-1, bj::value(nullptr)), http_json_mine_type);
}

HttpServer::~HttpServer() {}

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
      const auto& ext_data = boost::any_cast<ParameterBaseTypeRangeExtType>(item.ext_slot.value());
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

GameRunHandle::GameRunHandle(GameConfig &config) {
  m_config = std::make_shared<GameConfig>(config);
  m_env = std::make_shared<GameEnv>(config);
  m_core = std::make_shared<CoreRun>(*m_config, *m_env);
  m_run_handle = std::thread([&](){
    m_core->run();
    m_is_finish = true;
  });
}

GameEnvDetail GameRunHandle::get_env_detail() {
  auto detail = m_env->detail();
  detail.uid = m_core->uid;
  detail.simulate_name = m_core->simulate_name;
  return detail;
}

bool GameRunHandle::is_finish() {
  return m_is_finish;
}

std::optional<GameSnapshotResultType> GameRunHandle::get_snapshot(int time_step) {
  return m_env->snapshot->get(time_step);
}
