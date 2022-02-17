#ifndef _CORE_GAME_ENV_H
#define _CORE_GAME_ENV_H

#include <vector>
#include "config.h"

class GameState;
typedef GameState* GameStatePtr;

class GameTransition {
public:
  GameTransition();
  GameStatePtr target;
  int transition_id;
};

typedef GameTransition* GameTransitionPtr;

class GameAction {
public:
  GameAction();
  int action_id;
  std::vector<GameTransitionPtr> transitions;
};

typedef GameAction* GameActionPtr;

class GameState {
public:
  GameState();
  int state_id;
  int position_id;
  std::vector<GameActionPtr> actions;
};

class GameAgent {
public:
  GameAgent();
  GameStatePtr state;
};

typedef GameAgent* GameAgentPtr;

class GameEnv {
public:
  GameEnv(GameConfig& config);
  std::vector<GameStatePtr> graph;
  std::vector<std::vector<GameStatePtr>> position_graph;
  std::vector<std::vector<GameStatePtr>> timestep_graph;

  std::vector<GameAgentPtr> agents;
  int position_num;
  int timestep;
  GameConfig& config;
private:
  int read_from_grid();
};

#endif
