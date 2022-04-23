#include "gameenv.h"
#include "commconfig.h"

#include <vector>
#include <mutex>
#include <algorithm>
#include <numeric>

#include "globalvar.h"

#define ALGO_JSON_STREAM_SIZE 102400

const std::string GameGraphGridDetail::graph_type = "grid";

const std::string GameGraphMapDetail::graph_type = "map";

extern int register_game_env_config() {
  std::vector<std::string> graph_type_list = {"grid"};
  ParameterBaseTypeEnumExtType graph_type_ext;
  graph_type_ext.items = graph_type_list;

  std::vector<ParameterItemType> result = {
    {"graph_file", "Graph File Path", ParameterBaseType_STRING, std::string("test.data"), {}},
    {"graph_type", "Graph Type", ParameterBaseType_ENUM, 0, graph_type_ext},
    {"time_step", "Time Step", ParameterBaseType_INT, 20, {}},
    {"agent_number", "Agent Number", ParameterBaseType_INT, 20, {}}
  };
  
  global_var.game_env_parameters = result;
  return 0;
}

GameEnvConfig::GameEnvConfig(GameConfig& config) {
  graph_file = boost::any_cast<std::string>(config.ext_config["env"]["graph_file"]);
  graph_type = boost::any_cast<std::string>(config.ext_config["env"]["graph_type"]);
  time_step = boost::any_cast<int>(config.ext_config["env"]["time_step"]);
  agent_number = boost::any_cast<int>(config.ext_config["env"]["agent_number"]);
}

GameEnv::~GameEnv() {
  for (auto item : graph) {
    delete item;
  }

  for (auto item : agents) {
    delete item;
  }

  delete snapshot;
}

GameEnv::GameEnv(GameConfig& _config):
  config(_config)
{
  time_step = config.time_step.value();
  agent_number = config.agent_number.value();
  m_detail.agent_number = agent_number;
  if (config.graph_type.value() == "grid") {
    read_from_grid();
  }

  for (int loop_i = 0; loop_i < agent_number; ++loop_i) {
     GameAgentPtr magent = new GameAgent;
     magent->state = graph[0];
     agents.push_back(magent);
  }

  snapshot = new GameSnapshot(config);
}

/** TODO
 * map文件格式内容
 * {
 *   time_step: number,
 *   node: [{
 *     x: number,
 *     y: number
 *   }],
 *   edge: [{
 *     begin_node_index: int,
 *     end_node_index: int,
 *     status: [{ // 数量和时间一致，代表每一个时刻当前边的状态
 *       reward: int,
 *       transitions: [{
 *         probability: double, // 转移概率
 *         period: int,         // 转移的时间间隔
 *         path: [int]          // 包含起点和终点的转移路径
 *       }] 
 *     }]
 *   }]
 * }
 */
int GameEnv::read_from_map() {
  auto graph_fp = fopen(config.graph_file->c_str(), "r");
  if (graph_fp == NULL) {
    exit(-1);
  }

  boost::json::stream_parser json_stm;
  json_stm.reset();
  char *json_buff = (char *)malloc(sizeof(char) * ALGO_JSON_STREAM_SIZE);
  size_t json_buff_read_size = 0;
  while ((json_buff_read_size = fread(json_buff, sizeof(char), ALGO_JSON_STREAM_SIZE, graph_fp)) != 0) {
    json_stm.write(json_buff, json_buff_read_size/sizeof(char));
  }
  json_stm.finish();
  free(json_buff);
  fclose(graph_fp);

  boost::json::object map_json_obj = json_stm.release().as_object();
  boost::json::array nodes = map_json_obj["nodes"].as_array();
  boost::json::array edges = map_json_obj["edges"].as_array();
  GameGraphMapDetail m_map_detail;
  for (const auto& edge: edges) {
    GameGraphMapDetail::EdgeDescType edge_desc;
    edge_desc.begin_node_index = edge.as_object().at("begin_node_index").as_int64();
    edge_desc.end_node_index = edge.as_object().at("begin_node_index").as_int64();
    m_map_detail.edges.push_back(edge_desc);
  }

  for (const auto& node: nodes) {
    GameGraphMapDetail::NodeDescType node_desc;
    node_desc.x = node.as_object().at("x").as_int64();
    node_desc.y = node.as_object().at("y").as_int64();
    m_map_detail.nodes.push_back(node_desc);
  }

  m_detail.graph_type = GameGraphMapDetail::graph_type;
  m_detail.time_step = time_step;
  m_detail.agent_number = agent_number;
  m_detail.graph = m_map_detail;

  position_num = nodes.size();

  // 开始节点
  GameStatePtr begin_state = new GameState();
  begin_state->state_id = 0;
  begin_state->position_id = -1;
  begin_state->period = -1;
  graph.push_back(begin_state);

  time_step_graph = std::vector<std::vector<GameStatePtr>>(time_step, std::vector<GameStatePtr>());
  position_graph = std::vector<std::vector<GameStatePtr>>(nodes.size(), std::vector<GameStatePtr>());

  for (int loop_time = 0; loop_time < time_step; ++loop_time) {
    for (int loop_node = 0; loop_node < nodes.size(); ++loop_node) {
      auto state = new GameState;
      state->state_id = graph.size();
      state->position_id = loop_node;
      state->period = loop_time;

      graph.push_back(state);
      time_step_graph[loop_time].push_back(state);
      position_graph[loop_node].push_back(state);
    }
  }

  // 结束节点
  GameStatePtr end_state = new GameState;
  end_state->state_id = graph.size();
  end_state->position_id = -1;
  end_state->period = -1;
  graph.push_back(end_state);

  // 添加边
  // 起点
  for (int loop_node = 0; loop_node < nodes.size(); ++loop_node) {
    auto begin_node_action = new GameAction;
    begin_node_action->action_id = graph[0]->actions.size();
    auto transition = new GameTransition;
    transition->transition_id = 0;
    transition->target = time_step_graph[0][loop_node];
    begin_node_action->transitions.push_back(transition);
    graph[0]->actions.push_back(begin_node_action);
  }

  // 中间部件
  for (const auto& edge : edges) {
    int begin_node_index = edge.at("begin_node_index").as_int64();
    int end_node_index = edge.at("end_node_index").as_int64();
    
    for (int loop_time = 0; loop_time < time_step - 1; ++loop_time) {
      auto m_state = graph[1 + loop_time * position_num + begin_node_index];
      auto m_edge_status_json_obj = edge.at("status").as_array()[loop_time].as_object();

      auto m_action = new GameAction;
      m_action->action_id = m_state->actions.size();
      m_state->actions.push_back(m_action);
      
      for (auto trans : edge.at("transitions").as_array()) {
        auto mtran = new GameTransition;
        mtran->transition_id = m_action->transitions.size();
        int period = trans.at("period").as_int64();
        if (loop_time + period >= time_step) continue;
        int end_time_step = loop_time + period;
        auto m_end_state = graph[1 + end_time_step * position_num + end_node_index];
        mtran->target = m_end_state;
        m_action->transitions.push_back(mtran);
      }
    }
  }

  // 最后一个时间片
  for (int loop_node = 0; loop_node < position_num; ++loop_node) {
    auto m_state = graph[1 + loop_node + position_num * (time_step - 1)];
    auto m_action = new GameAction;
    m_action->action_id = m_state->actions.size();
    m_state->actions.push_back(m_action);

    auto m_tran = new GameTransition;
    m_tran->transition_id = 0;
    m_tran->target = graph.back();
    m_action->transitions.push_back(m_tran);
  }

  return 0;
}

GameRewardPtr GameEnv::create_reward() {
  GameRewardPtr result = std::make_shared<GameReward>();
  for (const auto& item : reward_ext_funs) {
    result->ext_slot[item.first] = item.second(result.get());
  }
  this->rewards.push_back(result);
  return result;
}

int GameEnv::commit() {
  snapshot->commit(agents, rewards, indicator);
  return 0;
}

int GameEnv::read_from_grid() {
  auto graph_fp = fopen(config.graph_file->c_str(), "r");
  if (graph_fp == NULL) {
    exit(-1);
  }

  GameGraphGridDetail m_graph_detail;
  m_detail.graph_type = GameGraphGridDetail::graph_type;

  int width = 0;
  int height = 0;
  int ext_action_num = 0;
  int transition_pro_num = 0;
  fscanf(graph_fp, "%d,%d,%d,%d,%d", &time_step, &width, &height, &ext_action_num, &transition_pro_num);

  m_graph_detail.height = height;
  m_graph_detail.width = width;
  m_detail.time_step = time_step;
  int action_num = ext_action_num + 4;
  position_num = width * height;
  config.position_num = position_num;

  double *action_reward = new double[time_step * width * height * action_num]();
  double *action_transition_pro = new double[time_step * width * height * action_num * transition_pro_num];

  for (int loop_i = 0; loop_i < time_step * width * height * action_num; ++loop_i) {
    fscanf(graph_fp, "%lf", action_reward + loop_i);
  }

  for (int loop_i = 0; loop_i < time_step * width * height * action_num * transition_pro_num; ++loop_i) {
    fscanf(graph_fp, "%lf", action_transition_pro + loop_i);
  }
  delete []action_reward;
  delete []action_transition_pro;
  fclose(graph_fp);

  for (int loop_i = 0; loop_i < time_step; ++loop_i) {
    time_step_graph.push_back({});
  }

  for (int loop_i = 0; loop_i < position_num; ++loop_i) {
    position_graph.push_back({});
  }

  // 初始化状态
  auto begin_state = new GameState();
  begin_state->state_id = 0;
  begin_state->position_id = -1;
  begin_state->period = -1;
  graph.push_back(begin_state);

  for (int loop_i = 0; loop_i < time_step; ++loop_i) {
    for (int loop_h = 0; loop_h < height; ++loop_h) {
      for (int loop_w = 0; loop_w < width; ++loop_w) {
        auto new_state = new GameState();
        new_state->state_id = static_cast<int>(graph.size());
        new_state->position_id = 1 + loop_h * width + loop_w;
        new_state->period = loop_i;

        graph.push_back(new_state);
        time_step_graph[loop_i].push_back(new_state);
        position_graph[loop_h * width + loop_w].push_back(new_state);
      }
    }
  }

  auto end_state = new GameState();
  end_state->state_id = static_cast<int>(graph.size());
  end_state->position_id = -1;
  end_state->period = std::numeric_limits<int>::max();
  graph.push_back(end_state);

  // 初始化动作和转移
  for (int loop_h = 0; loop_h < height; ++loop_h) {
    for (int loop_w = 0; loop_w < width; ++loop_w) {
      auto begin_state_action = new GameAction();
      begin_state_action->action_id = 0;
      auto mtran = new GameTransition();
      mtran->transition_id = 0;
      mtran->target = graph[loop_h * width + loop_w + 1];
      begin_state_action->transitions.push_back(mtran);
      begin_state->actions.push_back(begin_state_action);
    }
  }

  for (int loop_time = 0; loop_time < time_step - 1; ++loop_time) {
    for (int loop_h = 0; loop_h < height; ++loop_h) {
      for (int loop_w = 0; loop_w < width; ++loop_w) {
        auto mstate = graph[1 + loop_w + loop_h * width + loop_time * width * height];
        // 四方位 上下左右
        for (int loop_direct = 0; loop_direct < 4; ++loop_direct) {
          if (loop_direct == 0 && loop_h == 0) continue;
          if (loop_direct == 1 && loop_h == height - 1) continue;
          if (loop_direct == 2 && loop_w == 0) continue;
          if (loop_direct == 3 && loop_w == width - 1) continue;
          int to_position = mstate->state_id;
          switch (loop_direct) {
            case 0: to_position -= width; break;
            case 1: to_position += width; break;
            case 2: to_position -= 1; break;
            case 3: to_position += 1; break;
          }
          auto maction = new GameAction;
          maction->action_id = static_cast<int>(mstate->actions.size());
          for (int loop_tran = 0; loop_tran < transition_pro_num; ++loop_tran) {
            if (loop_tran + loop_time + 1 >= time_step) break;
            auto mtran = new GameTransition;
            mtran->transition_id = static_cast<int>(maction->transitions.size());
            mtran->target = graph[to_position + (loop_tran + 1) * position_num];
            maction->transitions.push_back(mtran);
          }
          mstate->actions.push_back(maction);
        }

        // 额外动作
        for (int loop_ext = 0; loop_ext < ext_action_num; ++loop_ext) {
          auto maction = new GameAction;
          maction->action_id = static_cast<int>(mstate->actions.size());
          for (int loop_tran = 0; loop_tran < transition_pro_num; ++loop_tran) {
            if (loop_tran + loop_time + 1 >= time_step) break;
            auto mtran = new GameTransition;
            mtran->transition_id = static_cast<int>(maction->transitions.size());
            mtran->target = graph[mstate->state_id + (1 + loop_tran) * width * height];
            maction->transitions.push_back(mtran);
          }
          mstate->actions.push_back(maction);
        }
      }
    }
  }

  // 最后一层
  for (int loop_h = 0; loop_h < height; ++loop_h) {
    for (int loop_w = 0; loop_w < width; ++loop_w) {
      auto mstate = graph[1 + width * height * (time_step - 1) + loop_h * width + loop_w];
      auto maction = new GameAction;
      maction->action_id = 0;
      auto mtran = new GameTransition;
      mtran->transition_id = 0;
      mtran->target = graph.back();
      maction->transitions.push_back(mtran);
      mstate->actions.push_back(maction);
    }
  }

  m_detail.graph = m_graph_detail;
  return 0;
}

GameSnapshot::GameSnapshot(GameEnvConfig& config) :
  config(config)
{
  agents_position_snapshot = std::vector<int>(config.agent_number.value(), -1);  
}

int GameSnapshot::commit(
  const std::vector<GameAgentPtr> &agents,
  const std::vector<GameRewardPtr> &rewards,
  const std::map<std::string, double>& indicator) {
  std::lock_guard<std::mutex> lock(lock_mutex);
  this->indicators.push_back(indicator);
  std::vector<int> new_agent_position(agents.size(), 0);

  std::transform(agents.begin(), agents.end(), new_agent_position.begin(), [](const GameAgentPtr& agent_item){
    return agent_item->state->state_id;
  });

  std::vector<GameSnapshotPath> new_agent_path;

  for (int loop_i = 0; loop_i < config.agent_number; ++loop_i) {
    if (new_agent_position[loop_i] != agents_position_snapshot[loop_i]) {
      if (agents_position_snapshot[loop_i] == -1) {
        new_agent_path.push_back({
          loop_i,
          {new_agent_position[loop_i] - 1, new_agent_position[loop_i] - 1},
          1
        });
      } else {
        new_agent_path.push_back({
          loop_i,
          {(agents_position_snapshot[loop_i] - 1) % config.position_num.value(), (new_agent_position[loop_i] - 1) % config.position_num.value()},
          ((new_agent_position[loop_i] - 1) / config.position_num.value() - (agents_position_snapshot[loop_i] - 1) / config.position_num.value())
        });
      }
    }
  }
  agents_position_snapshot = new_agent_position;

  this->agent_path.push_back(new_agent_path);

  std::vector<int> new_rewards_position(rewards.size(), 0);
  std::transform(rewards.begin(), rewards.end(), new_rewards_position.begin(), [](const GameRewardPtr& item) {
    return item->state->position_id;
  });

  rewards_position.push_back(new_rewards_position);
  return 0;
}

std::optional<GameSnapshotResultType> GameSnapshot::get(int time_step) {
  if (rewards_position.size() <= time_step || agent_path.size() <= time_step) {
    return std::nullopt;
  }
  GameSnapshotResultType result;
  result.time_step = time_step;
  result.agents = agent_path[time_step];
  result.rewards = rewards_position[time_step];
  result.indicator = this->indicators[time_step];
  return result;
}


GameEnvDetail GameEnv::detail() {
  return m_detail;
}

extern void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const GameSnapshotResultType &c) {
  boost::json::array agent;
  for (int loop_i = 0; loop_i < c.agents.size(); ++loop_i) {
    agent.push_back({
      {"agent_id", c.agents[loop_i].agent_id},
      {"path", c.agents[loop_i].path},
      {"period", c.agents[loop_i].period}
    });
  }

  jv = boost::json::value({
    {"time_step", c.time_step},
    {"agents", agent},
    {"rewards", c.rewards},
    {"indicator", c.indicator}
  });
}

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameGraphGridDetail const &c) {
  jv = boost::json::value({
    {"height", c.height},
    {"width", c.width}
  });
}

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameEnvDetail const &c) {
  boost::json::value graph = boost::json::value_from(std::get<GameGraphGridDetail>(c.graph));
  jv = boost::json::value({
    {"time_step", c.time_step},
    {"graph_type", c.graph_type},
    {"graph", graph},
    {"agent_number", c.agent_number}
  });
}

extern void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, GameGraphMapDetail const &c) {
  boost::json::array nodes;
  boost::json::array edges;
  for (const auto& node : c.nodes) {
    nodes.push_back(boost::json::value({
      {"x", node.x},
      {"y", node.y}
    }));
  }

  for (const auto& edge : c.edges) {
    edges.push_back(boost::json::value({
      {"begin_node_index", edge.begin_node_index},
      {"end_node_index", edge.end_node_index}
    }));
  }

  jv = boost::json::value({
    {"nodes", nodes},
    {"edges", edges}
  });
}
