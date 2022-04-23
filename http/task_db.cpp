#include "task_db.h"


TaskDB::TaskDB(const std::string& db_file_name) {
  db_con = std::make_shared<sql::Database>(
    db_file_name,
    sql::OPEN_READWRITE | sql::OPEN_CREATE
  );
  
  // create table if not exist
  sql::Transaction trans(*db_con);
  db_con->exec(
  "create table if not exists `task`("
    "sid integer not null primary key autoincrement,"
    "sim_name text not null,"
    "uid text not null,"
    "model_name text not null,"
    "gen_name text not null,"
    "status integer not null,"
    "create_time integer not null,"
    "finish_time integer"
  ");");
  trans.commit();
}

std::optional<TaskDBRecordType> TaskDB::create(const TaskDBRecordType& record) {
  sql::Statement exec_query(*db_con, 
    "INSERT INTO `task` (sim_name, uid, model_name, gen_name, status, create_time, finish_time) VALUES"
    "(@sim_name, @uid, @model_name, @gen_name, @status, @create_time, @finish_time);"
  );
  exec_query.bind("@sim_name", record.sim_name);
  exec_query.bind("@uid", record.uid);
  exec_query.bind("@model_name", record.model_name);
  exec_query.bind("@gen_name", record.gen_name);
  exec_query.bind("@status", TaskDBRecordStatusEnumType_RUNNING);
  exec_query.bind("@create_time", std::time(NULL));
  exec_query.bind("@finish_time", 0);
  exec_query.exec();
  
  return find(record.uid);
}

std::optional<TaskDBRecordType> TaskDB::find(const std::string &uid) {
  sql::Statement find_query(*db_con, 
    "SELECT * FROM `task` WHERE uid = @uid LIMIT 1;"
  );
  find_query.bind("@uid", uid);
  find_query.executeStep();
  if (find_query.hasRow()) {
    return {};
  }
  return taskRecordWapper(find_query);
}

std::vector<TaskDBRecordType> TaskDB::get_all() {
  sql::Statement get_all_query(*db_con,
    "SELECT * FROM `task`;"
  );
  std::vector<TaskDBRecordType> result;
  while (get_all_query.executeStep()) {
    result.push_back(taskRecordWapper(get_all_query));
  }
  return result;
}


TaskDBRecordType TaskDB::taskRecordWapper(const sql::Statement& query) {
  TaskDBRecordType result;
  result.sid = query.getColumn("sid");
  result.sim_name = query.getColumn("sim_name").getString();
  result.uid = query.getColumn("uid").getString();
  result.model_name = query.getColumn("model_name").getString();
  result.gen_name = query.getColumn("gen_name").getString();
  result.status = query.getColumn("status");
  result.create_time = query.getColumn("create_time");
  result.finish_time = query.getColumn("finish_time");
  return result;
}