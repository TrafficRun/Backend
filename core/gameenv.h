#ifndef _CORE_GAME_ENV_H
#define _CORE_GAME_ENV_H

#include <vector>
#include "commconfig.h"
#include "parameter_type.h"
#include <mutex>
#include <variant>
#include <map>
#include <functional>

// 环境配置对象
class GameEnvConfig {
public:
  GameEnvConfig(GameConfig& config);
  std::string graph_file;
  std::string graph_type;

  int time_step;
  int agent_number;
  int position_num;
};

// 注册环境配置
extern int register_game_env_config();

class GameState;
typedef GameState* GameStatePtr;

// 游戏状态智能体等对象的额外扩展插槽
class GameExtType {
public:
  virtual ~GameExtType(){};
};

typedef std::function<GameExtType*(const void * item)> generate_ext_func_type;

// 转移对象
class GameTransition {
public:
  GameTransition(){};
  ~GameTransition() {
    for (const auto &ext : ext_slot) {
      delete ext.second;
    }
  };
  // 转移到的位置
  GameStatePtr target;
  // 当前转移的ID，在action上面的编号
  int transition_id;
  std::map<std::string, GameExtType *> ext_slot;
};

// 转移的指针
typedef GameTransition* GameTransitionPtr;

// 动作对象
class GameAction {
public:
  GameAction(){};
  ~GameAction() {
    for (auto item : transitions) {
      delete item;
    }

    for (const auto &ext : ext_slot) {
      delete ext.second;
    }
  };
  // 动作ID，动作在state下的编号
  int action_id;
  // 转移集合
  std::vector<GameTransitionPtr> transitions;
  std::map<std::string, GameExtType *> ext_slot;
};

// 动作的指针
typedef GameAction* GameActionPtr;

// 状态对象
class GameState {
public:
  GameState(){};
  ~GameState() {
    for (auto item : actions) {
      delete item;
    }
    for (const auto &ext : ext_slot) {
      delete ext.second;
    }
  };
  // 状态ID，唯一
  int state_id;
  // 位置ID，不唯一，不同时间下可能出现同一位置的不同状态
  int position_id;
  // 当前所处的时间片
  int period;
  // 当前状态下的动作集合
  std::vector<GameActionPtr> actions;
  std::map<std::string, GameExtType *> ext_slot;
};

// 智能体对象
class GameAgent {
public:
  GameAgent() {};
  ~GameAgent() {
    for (const auto &ext : ext_slot) {
      delete ext.second;
    }
  };
  // 智能体所处的状态
  GameStatePtr state;
  std::map<std::string, GameExtType *> ext_slot;
};

typedef GameAgent* GameAgentPtr;

// 收益对象
class GameReward {
public:
  GameReward() {};
  ~GameReward() {
    for (const auto &ext : ext_slot) {
      delete ext.second;
    }
  };
  // 收益所处的状态
  GameStatePtr state;
  std::map<std::string, GameExtType *> ext_slot;
};

typedef std::shared_ptr<GameReward> GameRewardPtr;

// 快照路径
struct GameSnapshotPath {
  // 智能体ID
  int agent_id;
  // 路径
  std::vector<int> path;
  // 花费的时间片
  int period;
};

// 快照的结果，包含了一个时间片的各种信息
struct GameSnapshotResultType {
  // 时间片
  int time_step;
  // 路径规划
  std::vector<GameSnapshotPath> agents;
  // 收益集合
  std::vector<int> rewards;
  // 当前时间下的收益
  std::map<std::string, double> indicator;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const GameSnapshotResultType &c);

// 游戏快照
class GameSnapshot {
public:
  GameSnapshot(GameEnvConfig& config);
  int commit(const std::vector<GameAgentPtr>& agents, const std::vector<GameRewardPtr>& rewards, const std::map<std::string, double>& indicator);
  std::optional<GameSnapshotResultType> get(int time_step);
private:
  std::mutex lock_mutex;
  std::vector<int> agents_position_snapshot;

  std::vector<std::vector<int>> rewards_position;
  std::vector<std::vector<GameSnapshotPath>> agent_path;
  std::vector<std::map<std::string, double>> indicators;
  GameEnvConfig& config;
};

// 真实地图的表述
struct GameGraphMapDetail {
  // 节点的结构体
  typedef struct {
    int x;
    int y;
  } NodeDescType;
  // 边的描述
  typedef struct {
    int begin_node_index;
    int end_node_index;
  } EdgeDescType;
public:
  const static std::string graph_type;
  std::vector<NodeDescType> nodes;
  std::vector<EdgeDescType> edges;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameGraphMapDetail const &c);

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
  // 别名
  std::string simulate_name;
  // UID
  std::string uid;
  // 时间长度
  int time_step;
  // 智能体数量
  int agent_number;
  // 图详细信息
  std::variant<GameGraphGridDetail, GameGraphMapDetail> graph;
};

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameEnvDetail const &c);

class GameEnv {
public:
  GameEnv(GameConfig& config);
  ~GameEnv();
  int commit();
  GameEnvDetail detail();
  GameRewardPtr create_reward();

  std::vector<GameStatePtr> graph;
  std::vector<std::vector<GameStatePtr>> position_graph;
  std::vector<std::vector<GameStatePtr>> time_step_graph;

  std::vector<GameAgentPtr> agents;
  std::vector<GameRewardPtr> rewards;
  std::map<std::string, double> indicator;
  int position_num;
  int time_step;
  int agent_number;

  GameEnvConfig config;
  GameSnapshot *snapshot;

  std::map<std::string, generate_ext_func_type> reward_ext_funs;
private:
  int read_from_grid();
  int read_from_map();
  GameEnvDetail m_detail;
};



#endif
