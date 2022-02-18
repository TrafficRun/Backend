#ifndef _CORE_PARAMETER_TYPE_H
#define _CORE_PARAMETER_TYPE_H

#include <string>
#include <vector>
#include <map>
#include <boost/any.hpp>

class ModelBaseType {
public:
  ModelBaseType(){};
  virtual int run() {return 0;}
  virtual ~ModelBaseType();
};

enum ParameterBaseType {
  ParameterBaseType_INT = 0,
  ParameterBaseType_FLOAT = 1,
  ParameterBaseType_STRING = 2,
  ParameterBaseType_RANGE = 3,
  ParameterBaseType_ENUM = 4
};

struct ParameterBaseTypeRangeExtType {
  int max;
  int min;
  int is_continue;
};

struct ParameterBaseTypeEnumExtType {
  std::vector<std::string> items; 
};

struct ParameterItemType {
  std::string name;
  std::string description;
  ParameterBaseType type;
  boost::any default_value;
};

#endif