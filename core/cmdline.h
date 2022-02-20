#ifndef _CORE_CMD_LINE_H
#define _CORE_CMD_LINE_H

#include "parameter_type.h"
#include "commconfig.h"
#include <boost/program_options.hpp>

class CMDLine {
public:
  CMDLine();
  int generate_cmd_line();
  int parse_cmd(int argc,char *argv[], GameConfig& config);
private:
  int generate_parser(boost::program_options::options_description& options, const ParameterItemType& item); 
  boost::program_options::options_description cmd_options;
};

#endif
