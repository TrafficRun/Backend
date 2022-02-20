#ifndef _MODEL_SIMPLERANDOM_SIMPLERANDOM_H
#define _MODEL_SIMPLERANDOM_SIMPLERANDOM_H

#include "core/parameter_type.h"
#include "core/gameenv.h"
#include "core/commconfig.h"
#include "core/globalvar.h"

class SimpleRandomModel : public ModelBaseType {
public:
  SimpleRandomModel(GameConfig& config, GameEnv& env);
  ~SimpleRandomModel(){};
  int run(int now_time);
private:
  GameConfig& config;
  GameEnv &env;
};

#endif
