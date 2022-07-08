#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include "AdvancedNavigationCrowd3D.hpp"
#include "DetourCrowdAgent.hpp"

class AdvancedNavigationAgent3D : public godot::Node
{
  GODOT_CLASS(AdvancedNavigationAgent3D, godot::Node);

 public:
  static constexpr bool default_passive_movement{true};
  static constexpr float default_target_desired_distance{0.2};

 public:
  static void _register_methods();

  void _init() {}
  void _ready();

  void set_navigation_crowd(AdvancedNavigationCrowd3D*);
  void set_position(godot::Vector3);
  void set_target(godot::Vector3);

  godot::Vector3 get_position();
  godot::Vector3 get_target();

 private:
  void try_fetching_crowd();
  void try_creating_agent();

  godot::Ref<DetourCrowdAgentConfig> create_detour_crowd_agent_config() const;

  void on_navigation_crowd_changed();
  void on_new_position(godot::Vector3);
  void on_new_velocity(godot::Vector3);

 private:
  bool passive_movement{default_passive_movement};
  float target_desired_distance{default_target_desired_distance};
  // config:
  float radius{DetourCrowdAgentConfig::default_radius}; // >=0, 0 not working, so more likely >0
  float height{DetourCrowdAgentConfig::default_height}; // >0
  float max_acceleration{DetourCrowdAgentConfig::default_max_acceleration}; // >= 0
  float max_speed{DetourCrowdAgentConfig::default_max_speed}; // >= 0
  float collision_query_range{DetourCrowdAgentConfig::default_collision_query_range}; // >0
  float path_optimization_range{DetourCrowdAgentConfig::default_path_optimization_range}; // >0
  float separation_weight{DetourCrowdAgentConfig::default_separation_weight}; // >=0

 private:
  godot::Vector3 requested_position{godot::Vector3::INF};
  godot::Vector3 requested_target{godot::Vector3::INF};
  AdvancedNavigationCrowd3D* navigation_crowd_node{nullptr};
  Ref<DetourCrowd> crowd{nullptr};
  Ref<DetourCrowdAgent> agent{nullptr};
};
