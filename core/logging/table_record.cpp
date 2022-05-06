#include "table_record.h"
#include <fstream>


TableRecord::TableRecord(const std::string &filename, const std::vector<std::string>& fields) {
  m_out_fp.open(filename, std::ios::out | std::ios::trunc);
  m_fields = fields;
  if (fields.size() == 0) return;
  for (int loop_i = 0; loop_i < fields.size() - 1; ++loop_i) {
    m_out_fp << fields[loop_i] << ",";
  }
  m_out_fp << fields[fields.size() - 1] << std::endl;;
  m_out_fp.flush();
}

int TableRecord::log(const std::map<std::string, double>& indicator) {
  if (m_fields.size() == 0) return 0;
  for (int loop_i = 0; loop_i < m_fields.size() - 1; ++loop_i) {
    m_out_fp << indicator.at(m_fields[loop_i]) << ",";
  }
  m_out_fp << indicator.at(m_fields[m_fields.size() - 1]) << std::endl;
  m_out_fp.flush();
  return 0;
}

TableRecord::~TableRecord() {
  m_out_fp.close();
}