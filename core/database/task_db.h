#ifndef __CORE_DATABASE_TASK_DB_H
#define __CORE_DATABASE_TASK_DB_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <memory>
#include <optional>
#include <vector>

#include <boost/json.hpp>

struct TaskDBRecordType {
  int sid;
  std::string sim_name;
  std::string uid;
  std::string model_name;
  std::string gen_name;
  int status;
  int create_time;
  int finish_time;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const TaskDBRecordType &c);

enum TaskDBRecordStatusEnumType {
  TaskDBRecordStatusEnumType_RUNNING = 0,
  TaskDBRecordStatusEnumType_FINISH = 1,
  TaskDBRecordStatusEnumType_ERROR = 2 
};

namespace sql = SQLite;

class TaskDB {
public:
  TaskDB(const std::string& db_file_name);
  std::optional<TaskDBRecordType> create(const TaskDBRecordType& record);
  int change_status(const std::string &uid, TaskDBRecordStatusEnumType status);
  std::optional<TaskDBRecordType> find(const std::string &uid);
  std::vector<TaskDBRecordType> get_all();

  ~TaskDB() {};
private:
  std::shared_ptr<sql::Database> db_con;
  TaskDBRecordType taskRecordWapper(const sql::Statement& query);
};

#endif
