#ifndef _CORE_PARAMETER_TYPE_H
#define _CORE_PARAMETER_TYPE_H

#include <string>
#include <vector>
#include <map>
#include <boost/any.hpp>
#include <optional>

class ModelBaseType {
public:
  ModelBaseType(){};
  virtual int run(int now_time) {return 0;}
  virtual ~ModelBaseType();
};

class GeneratorBaseType {
public:
  GeneratorBaseType(){};
  virtual int generate(int now_time);
  virtual ~GeneratorBaseType();
};

enum ParameterBaseType {
  ParameterBaseType_INT = 0,
  ParameterBaseType_FLOAT = 1,
  ParameterBaseType_STRING = 2,
  ParameterBaseType_RANGE = 3,
  ParameterBaseType_ENUM = 4,
  ParameterBaseType_BOOL = 5
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
  std::optional<boost::any> ext_slot;
};

#endif