#ifndef _CORE_GAME_ENV_H
#define _CORE_GAME_ENV_H

#include <vector>
#include "commconfig.h"
#include "parameter_type.h"

class GameEnvConfig {
public:
  GameEnvConfig(GameConfig& config);
  std::optional<std::string> graph_file;
  std::optional<std::string> graph_type;

  std::optional<int> time_step;
  std::optional<int> agent_number;
};

extern std::vector<ParameterItemType> generate_game_env_config();

class GameState;
typedef GameState* GameStatePtr;

class GameExtType {
public:
  virtual ~GameExtType(){};
};

class GameTransition {
public:
  GameTransition(){};
  ~GameTransition() {
    if (ext_slot != nullptr) {
      delete ext_slot;
    }
  };
  GameStatePtr target;
  int transition_id;
  GameExtType *ext_slot = nullptr;
};

typedef GameTransition* GameTransitionPtr;

class GameAction {
public:
  GameAction(){};
  ~GameAction() {
    for (auto item : transitions) {
      delete item;
    }

    if (ext_slot != nullptr) {
      delete ext_slot;
    }
  };
  int action_id;
  std::vector<GameTransitionPtr> transitions;
  GameExtType *ext_slot = nullptr;
};

typedef GameAction* GameActionPtr;

class GameState {
public:
  GameState(){};
  ~GameState() {
    for (auto item : actions) {
      delete item;
    }
    if (ext_slot != nullptr) {
      delete ext_slot;
    }
  };
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

class GameReward {
public:
  GameReward() {};
  ~GameReward() {
    if (ext_slot != nullptr) {
      delete ext_slot;
    }
  };
  GameStatePtr state;
  GameExtType *ext_slot = nullptr;
};

class GameEnv {
public:
  GameEnv(GameConfig& config);
  ~GameEnv() {
    for (auto item : graph) {
      delete item;
    }
  }
  std::vector<GameStatePtr> graph;
  std::vector<std::vector<GameStatePtr>> position_graph;
  std::vector<std::vector<GameStatePtr>> time_step_graph;

  std::vector<GameAgentPtr> agents;
  std::vector<GameReward> rewards;
  int position_num;
  int time_step;
  int agent_number;
  GameEnvConfig config;
private:
  int read_from_grid();
};

#endif
