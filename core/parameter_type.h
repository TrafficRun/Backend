#ifndef _CORE_PARAMETER_TYPE_H
#define _CORE_PARAMETER_TYPE_H

#include <string>
#include <vector>
#include <map>
#include <boost/any.hpp>
#include <optional>
#include <boost/json.hpp>

class ModelBaseType {
public:
  ModelBaseType(){};
  virtual int run(int now_time) { return 0; }
  virtual ~ModelBaseType() {};
};

class GeneratorBaseType {
public:
  GeneratorBaseType(){};
  virtual int generate(int now_time) { return 0; };
  virtual ~GeneratorBaseType() {};
};

enum ParameterBaseType {
  ParameterBaseType_INT = 0,
  ParameterBaseType_FLOAT = 1,
  ParameterBaseType_STRING = 2,
  ParameterBaseType_RANGE = 3,
  ParameterBaseType_ENUM = 4,
  ParameterBaseType_BOOL = 5
};

extern const std::vector<std::string> parameter_base_type_string;

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

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, ParameterItemType const &c);
extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ParameterBaseTypeRangeExtType& c);
extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ParameterBaseTypeEnumExtType& c);

#endif
