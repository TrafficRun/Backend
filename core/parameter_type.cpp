#include "parameter_type.h"

extern const std::vector<std::string> parameter_base_type_string = {
  "int",
  "float",
  "string",
  "range",
  "enum",
  "bool"
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, ParameterItemType const &c) {
  boost::json::value ext_slot;
  switch (c.type) {
    case ParameterBaseType_RANGE: {
      ext_slot = boost::json::value_from(boost::any_cast<ParameterBaseTypeRangeExtType>(c.ext_slot.value()));
    }
    break;
    case ParameterBaseType_ENUM: {
      ext_slot = boost::json::value_from(boost::any_cast<ParameterBaseTypeEnumExtType>(c.ext_slot.value()));
    }
    break;
    default:
      ext_slot = boost::json::value_from(nullptr);
  }
  
  boost::json::value default_value;
  
  switch (c.type) {
    case ParameterBaseType_BOOL:
      default_value = false;
    break;
    case ParameterBaseType_ENUM:
      default_value = boost::any_cast<int>(c.default_value);
    break;
    case ParameterBaseType_FLOAT:
      default_value = boost::any_cast<double>(c.default_value);
    break;
    case ParameterBaseType_INT:
      default_value = boost::any_cast<int>(c.default_value);
    break;
    case ParameterBaseType_RANGE: {
      const auto& temp_ext = boost::any_cast<ParameterBaseTypeRangeExtType>(c.ext_slot.value());
      if (temp_ext.is_continue) {
        default_value = boost::any_cast<double>(c.default_value);
      } else {
        default_value = boost::any_cast<int>(c.default_value);
      }
    }
    break;
    case ParameterBaseType_STRING:
      default_value = boost::any_cast<std::string>(c.default_value);
    break;
  }

  jv = {
    {"name", c.name},
    {"description", c.description},
    {"type", parameter_base_type_string[c.type]},
    {"default_value", default_value},
    {"ext_slot", ext_slot}
  };
}

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ParameterBaseTypeEnumExtType& c) {
  boost::json::array rv;
  for (const auto& item : c.items) {
    rv.push_back(boost::json::string(item));
  }
  jv = rv;
}

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ParameterBaseTypeRangeExtType& c) {
  jv = {
    {"min", c.min},
    {"max", c.max},
    {"is_continue", c.is_continue}
  };
}