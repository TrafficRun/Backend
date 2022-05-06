#include "core_run.h"
#include "globalvar.h"
#include "database/database.h"

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>

CoreRun::CoreRun(GameConfig& config, GameEnv& env) : 
  env(env),
  config(config)
{
  uid = boost::uuids::to_string(boost::uuids::random_generator()());
  simulate_name = config.simulate_name;
  work_dir = generate_work_dir();
}

GameEnvDetail CoreRun::detail() {
  auto detail = env.detail();
  detail.uid = uid;
  detail.simulate_name = simulate_name;
  return detail;
}

int CoreRun::run() {
  boost::filesystem::path work_dir_path(work_dir);

  // 日志初始化
  boost::filesystem::path log_file = work_dir_path / "run.log";
  logger = std::make_shared<Logging>(log_file.generic_string());

  const auto& model_config = std::find_if(global_var.models.begin(), global_var.models.end(), [&](const ModelType& item){
    return item.model_name == config.model_name;
  });

  if (model_config == global_var.models.end()) {
    std::cout << "Not Such Model." << std::endl;
    std::exit(-1);
  }
  // 添加模型的扩展
  add_exts(model_config->model_name, model_config->ext_info);
  // 模型对象
  ModelBaseType *model_obj = model_config->generate_func(config, env, logger);
  
  // 生成记录表
  boost::filesystem::path indicator_path = work_dir_path / "indicator.csv";
  table_record = std::make_shared<TableRecord>(indicator_path.generic_string(), model_config->indicator_fields);

  const auto& generator_config = std::find_if(global_var.generators.begin(), global_var.generators.end(), [&](const GeneratorType& item) {
    return item.generator_name == config.generator_name;
  });
  if (generator_config == global_var.generators.end()) {
    std::cout << "Not Such Model." << std::endl;
    std::exit(-1);
  }
  // 添加生成器的扩展
  add_exts(generator_config->generator_name, generator_config->ext_info);
  // 生成器对象
  GeneratorBaseType *generator_obj = generator_config->generate_func(config, env, logger);

  // Add To sqlite
  TaskDBRecordType record;
  auto detail = this->detail();
  record.sim_name = detail.simulate_name;
  record.model_name = config.model_name;
  record.gen_name = config.generator_name;
  record.uid = detail.uid;
  database->task->create(record);

  // 保存配置
  auto config_json = boost::json::value_from(config);
  auto config_json_string = boost::json::serialize(config_json);
  std::ofstream config_json_fp;
  config_json_fp.open((work_dir_path / "config.json").generic_string(), std::ios::out | std::ios::trunc);
  config_json_fp << config_json_string;
  config_json_fp.close();

  for (int loop_time = 0; loop_time < env.time_step; ++loop_time) {
    env.rewards.clear();
    generator_obj->generate(loop_time);
    model_obj->run(loop_time);
    env.commit();
    table_record->log(env.indicator);
  }

  // 修改运行状态
  database->task->change_status(detail.uid, TaskDBRecordStatusEnumType_FINISH);

  delete model_obj;
  delete generator_obj;
  return 0;
}

int CoreRun::add_exts(const std::string &ext_name, const std::map<std::string, generate_ext_func_type>& exts) {
  std::vector<generate_ext_func_type> ext_funs(ext_slot_type_string.size(), nullptr);

  for (int loop_ext_index = 0; loop_ext_index < ext_funs.size(); ++loop_ext_index) {
    if (exts.find(ext_slot_type_string[loop_ext_index]) != exts.end()) {
      ext_funs[loop_ext_index] = exts.at(ext_slot_type_string[loop_ext_index]);
    }
  }

  for (auto mstate : env.graph) {
    if (ext_funs[ExtSlotTypeState] != nullptr) {
      mstate->ext_slot[ext_name] = ext_funs[ExtSlotTypeState](mstate);
    }
    for (auto maction : mstate->actions) {
      if (ext_funs[ExtSlotTypeAction] != nullptr) {
        maction->ext_slot[ext_name] = ext_funs[ExtSlotTypeAction](maction);
      }
      for (auto mtran : maction->transitions) {
        if (ext_funs[ExtSlotTypeTransition] != nullptr) {
          mtran->ext_slot[ext_name] = ext_funs[ExtSlotTypeTransition](mtran);
        }
      }
    } 
  }

  if (ext_funs[ExtSlotTypeAgent] != nullptr) {
    for (auto magent : env.agents) {
      magent->ext_slot[ext_name] = ext_funs[ExtSlotTypeAgent](magent);
    }
  }

  if (ext_funs[ExtSlotTypeReward] != nullptr) {
    env.reward_ext_funs[ext_name] = ext_funs[ExtSlotTypeReward];
  }

  return 0;
}

std::string CoreRun::generate_work_dir() {
  boost::filesystem::path work_root_dir(config.work_root_dir);
  boost::filesystem::path work_dir(this->uid);
  boost::filesystem::path full_path = work_root_dir / work_dir;
  if (!boost::filesystem::exists(full_path)) {
    boost::filesystem::create_directories(full_path);
  }
  return full_path.generic_string();
}