#include "logging.h"


Logging::Logging(const std::string& filename) {
  out_fp.open(filename, std::ios::out | std::ios::trunc);
}

Logging::~Logging() {
  out_fp.close();
}