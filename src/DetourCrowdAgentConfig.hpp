#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class DetourCrowdAgentConfig : public godot::Resource
{
  GODOT_CLASS(DetourCrowdAgentConfig, godot::Resource);

 public:
  void _init() {}

  static constexpr float default_radius{0.6};
  static constexpr float default_height{2.0};
  static constexpr float default_max_acceleration{0.6};
  static constexpr float default_max_speed{1.2};
  static constexpr float default_collision_query_range{default_radius * 8.0};
  static constexpr float default_path_optimization_range{default_radius * 30.0};
  static constexpr float default_separation_weight{0.1};
  static constexpr bool default_anticipate_turns{true};
  static constexpr bool default_obstacle_avoidance{true};
  static constexpr bool default_separation{false};
  static constexpr bool default_optimize_path_visibility{true};
  static constexpr bool default_optimize_path_topology{true};

  static void _register_methods()
  {
    // TODO: add hints
    godot::register_property<DetourCrowdAgentConfig, float>(
        "radius", &DetourCrowdAgentConfig::radius, default_radius);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "height", &DetourCrowdAgentConfig::height, default_height);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "max_acceleration", &DetourCrowdAgentConfig::max_acceleration, default_max_acceleration);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "max_speed", &DetourCrowdAgentConfig::max_speed, default_max_speed);
    godot::register_property<DetourCrowdAgentConfig, bool>(
        "anticipate_turns", &DetourCrowdAgentConfig::anticipate_turns, default_anticipate_turns);
    godot::register_property<DetourCrowdAgentConfig, bool>(
        "obstacle_avoidance",
        &DetourCrowdAgentConfig::obstacle_avoidance,
        default_obstacle_avoidance);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "collision_query_range",
        &DetourCrowdAgentConfig::collision_query_range,
        default_collision_query_range);
    godot::register_property<DetourCrowdAgentConfig, bool>(
        "optimize_path_visibility",
        &DetourCrowdAgentConfig::optimize_path_visibility,
        default_optimize_path_visibility);
    godot::register_property<DetourCrowdAgentConfig, bool>(
        "optimize_path_topology",
        &DetourCrowdAgentConfig::optimize_path_topology,
        default_optimize_path_topology);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "path_optimization_range",
        &DetourCrowdAgentConfig::path_optimization_range,
        default_path_optimization_range);
    godot::register_property<DetourCrowdAgentConfig, bool>(
        "separation", &DetourCrowdAgentConfig::separation, default_separation);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "separation_weight", &DetourCrowdAgentConfig::separation_weight, default_separation_weight);
  }

 public:
  float radius{default_radius}; // >=0, 0 not working, so more likely >0
  float height{default_height}; // >0
  float max_acceleration{default_max_acceleration}; // >= 0
  float max_speed{default_max_speed}; // >= 0
  float collision_query_range{default_collision_query_range}; // >0
  float path_optimization_range{default_path_optimization_range}; // >0
  float separation_weight{default_separation_weight}; // >=0
  bool anticipate_turns{default_anticipate_turns};
  bool obstacle_avoidance{default_obstacle_avoidance};
  bool separation{default_separation};
  bool optimize_path_visibility{default_optimize_path_visibility};
  bool optimize_path_topology{default_optimize_path_topology};
};
