#ifndef _CORE_GAME_ENV_H
#define _CORE_GAME_ENV_H

#include <vector>
#include "commconfig.h"
#include "parameter_type.h"
#include <mutex>
#include <variant>

class GameEnvConfig {
public:
  GameEnvConfig(GameConfig& config);
  std::optional<std::string> graph_file;
  std::optional<std::string> graph_type;

  std::optional<int> time_step;
  std::optional<int> agent_number;
  std::optional<int> position_num;
};

extern int register_game_env_config();

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
  int period;
  std::vector<GameActionPtr> actions;
  GameExtType *ext_slot = nullptr;
};

class GameAgent {
public:
  GameAgent() {};
  ~GameAgent() {
    if (ext_slot != nullptr) delete ext_slot;
  };
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

struct GameSnapshotPath {
  int agent_id;
  std::vector<int> path;
  int period;
};

struct GameSnapshotResultType {
  int time_step;
  std::vector<GameSnapshotPath> agents;
  std::vector<int> rewards;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const GameSnapshotResultType &c);

// 游戏快照
class GameSnapshot {
public:
  GameSnapshot(GameEnvConfig& config);
  int commit(const std::vector<GameAgentPtr>& agents, const std::vector<GameReward>& rewards);
  std::optional<GameSnapshotResultType> get(int time_step);
private:
  std::mutex lock_mutex;
  std::vector<int> agents_position_snapshot;

  std::vector<std::vector<int>> rewards_position;
  std::vector<std::vector<GameSnapshotPath>> agent_path;
  GameEnvConfig& config;
};


struct GameGraphGridDetail {
public:
  const static std::string graph_type;
  // 高度方向区域的数量
  int height;
  // 宽度方向区域的数量
  int width;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameGraphGridDetail const &c);

struct GameEnvDetail {
public:
  // 图类型
  std::string graph_type;
  // 时间长度
  int time_step;
  // 图详细信息
  std::variant<GameGraphGridDetail> graph;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameEnvDetail const &c);

class GameEnv {
public:
  GameEnv(GameConfig& config);
  ~GameEnv();
  int commit();
  GameEnvDetail detail();

  std::vector<GameStatePtr> graph;
  std::vector<std::vector<GameStatePtr>> position_graph;
  std::vector<std::vector<GameStatePtr>> time_step_graph;

  std::vector<GameAgentPtr> agents;
  std::vector<GameReward> rewards;
  int position_num;
  int time_step;
  int agent_number;
  
  GameEnvConfig config;
  GameSnapshot *snapshot;
private:
  int read_from_grid();
  GameEnvDetail m_detail;
};



#endif
