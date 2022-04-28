#include "database.h"

std::shared_ptr<DataBase> database = nullptr;

extern void database_init(const std::string& db_file) {
  database = std::make_shared<DataBase>(db_file);
}

DataBase::DataBase(const std::string& db_file) {
  this->task = std::make_shared<TaskDB>(db_file);
}