#ifndef _CORE_LOGGING_LOGGING_H
#define _CORE_LOGGING_LOGGING_H

#include <string>
#include <fstream>
#include <chrono>

#include "fmt/core.h"
#include "fmt/chrono.h"

class Logging {
public:
  Logging(const std::string& filename);
  template<typename ...Types>
  int log(const std::string& fmt_string, const Types&... args) {
    out_fp << fmt::format("[{:%Y-%m-%d %H:%M:%S}] ", fmt::localtime(std::chrono::system_clock::now()));
    out_fp << fmt::format(fmt_string, args...) << std::endl;
    out_fp.flush();
    return 0;
  }
  ~Logging();
private:
  std::ofstream out_fp;
};

#endif
