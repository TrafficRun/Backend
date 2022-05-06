#ifndef __CORE_LOGGING_TABLE_REOCRD
#define __CORE_LOGGING_TABLE_REOCRD

#include <string>
#include <vector>
#include <fstream>
#include <map>

class TableRecord {
public:
  TableRecord(const std::string &filename, const std::vector<std::string>& fields);
  int log(const std::map<std::string, double>& indicator);
  ~TableRecord();
private:
  std::ofstream m_out_fp;
  std::vector<std::string> m_fields;
};

#endif