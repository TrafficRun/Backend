#ifndef _CORE_GAME_ENV_H
#define _CORE_GAME_ENV_H

#include <vector>
#include "config.h"

class GameState;
typedef GameState* GameStatePtr;

class GameExtType {
public:
  virtual ~GameExtType(){};
};

class GameTransition {
public:
  GameTransition();
  ~GameTransition();
  GameStatePtr target;
  int transition_id;
  GameExtType *ext_slot = nullptr;
};

typedef GameTransition* GameTransitionPtr;

class GameAction {
public:
  GameAction();
  ~GameAction();
  int action_id;
  std::vector<GameTransitionPtr> transitions;
  GameExtType *ext_slot = nullptr;
};

typedef GameAction* GameActionPtr;

class GameState {
public:
  GameState();
  ~GameState();
  int state_id;
  int position_id;
  std::vector<GameActionPtr> actions;
  GameExtType *ext_slot = nullptr;
};

class GameAgent {
public:
  GameAgent();
  ~GameAgent();
  GameStatePtr state;
  GameExtType *ext_slot = nullptr;
};

typedef GameAgent* GameAgentPtr;

class GameEnv {
public:
  GameEnv(GameConfig& config);
  std::vector<GameStatePtr> graph;
  std::vector<std::vector<GameStatePtr>> position_graph;
  std::vector<std::vector<GameStatePtr>> time_step_graph;

  std::vector<GameAgentPtr> agents;
  int position_num;
  int time_step;
  int agent_number;
  GameConfig& config;
private:
  int read_from_grid();
};

#endif
