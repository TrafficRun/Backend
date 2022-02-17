#include "gameenv.h"
#include "config.h"
#include <stdio.h>

GameEnv::GameEnv(GameConfig& config):
  config(config)
{
  timestep = config.time_step.value();
  if (config.graph_type.value() == "grid") {
    read_from_grid();
  }
}

int GameEnv::read_from_grid() {
  auto graph_fp = fopen(config.graph_file->c_str(), "r");
  if (graph_fp == NULL) {
    exit(-1);
  }

  int time_step = 0;
  int width = 0;
  int height = 0;
  int ext_action_num = 0;
  int transition_pro_num = 0;
  fscanf(graph_fp, "%d,%d,%d,%d,%d", &time_step, &width, &height, &ext_action_num, &transition_pro_num);

  int action_num = ext_action_num + 4;
  position_num = width * height;
  timestep = time_step;

  double *action_reward = new double[time_step * width * height * action_num]();
  double *action_transition_pro = new double[time_step * width * height * action_num * transition_pro_num];

  for (int loop_i = 0; loop_i < time_step * width * height * action_num; ++loop_i) {
    fscanf(graph_fp, "%lf", action_reward + loop_i);
  }

  for (int loop_i = 0; loop_i < time_step * width * height * action_num * transition_pro_num; ++loop_i) {
    fscanf(graph_fp, "%lf", action_transition_pro + loop_i);
  }
  fclose(graph_fp);
  
  for (int loop_i = 0; loop_i < timestep; ++loop_i) {
    timestep_graph.push_back({});
  }

  for (int loop_i = 0; loop_i < position_num; ++loop_i) {
    position_graph.push_back({});
  }

  // 初始化状态
  auto begin_state = new GameState();
  begin_state->state_id = 0;
  begin_state->position_id = -1;
  graph.push_back(begin_state);

  for (int loop_i = 0; loop_i < timestep; ++loop_i) {
    for (int loop_h = 0; loop_h < height; ++loop_h) {
      for (int loop_w = 0; loop_w < width; ++loop_w) {
        auto new_state = new GameState();
        new_state->state_id = graph.size();
        new_state->position_id = 1 + loop_i * width * height + loop_h * width + loop_w;
        graph.push_back(new_state);
        timestep_graph[loop_i].push_back(new_state);
        position_graph[loop_h * width + loop_w].push_back(new_state);
      }
    }
  }
  
  auto end_state = new GameState();
  end_state->state_id = graph.size();
  end_state->position_id = -1;
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

  for (int loop_time = 0; loop_time < timestep - 1; ++loop_time) {
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
          maction->action_id = mstate->actions.size();
          for (int loop_tran = 0; loop_tran < transition_pro_num; ++loop_tran) {
            if (loop_tran + loop_time + 1 >= timestep) break;
            auto mtran = new GameTransition;
            mtran->transition_id = maction->transitions.size();
            mtran->target = graph[to_position + (loop_tran + 1) * position_num];
            maction->transitions.push_back(mtran);
          }
          mstate->actions.push_back(maction);
        }

        // 额外动作
        for (int loop_ext = 0; loop_ext < ext_action_num; ++loop_ext) {
          auto maction = new GameAction;
          maction->action_id = mstate->actions.size();
          for (int loop_tran = 0; loop_tran < transition_pro_num; ++loop_tran) {
            if (loop_tran + loop_time + 1 >= timestep) break;
            auto mtran = new GameTransition;
            mtran->transition_id = maction->transitions.size();
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
      auto mstate = graph[1 + width * height * (timestep - 1) + loop_h * width + loop_w];
      auto maction = new GameAction;
      maction->action_id = 0;
      auto mtran = new GameTransition;
      mtran->transition_id = 0;
      mtran->target = graph.back();
      maction->transitions.push_back(mtran);
      mstate->actions.push_back(maction);
    }
  }

  
  return 0;
}