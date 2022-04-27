#ifndef __CORE_DATABASE_DATABASE_H
#define __CORE_DATABASE_DATABASE_H

#include "task_db.h"

class DataBase {
public:
  DataBase(const std::string & db_file);
  std::shared_ptr<TaskDB> task;
};

extern std::shared_ptr<DataBase> database;
extern void database_init(const std::string& db_file);

#endif