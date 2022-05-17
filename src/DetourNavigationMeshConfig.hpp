#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class DetourNavigationMeshConfig : public godot::Resource
{
  GODOT_CLASS(DetourNavigationMeshConfig, godot::Resource);

 public:
  void _init() {}

  static constexpr float default_cell_size{0.3};
  static constexpr float default_cell_height{0.2};
  static constexpr float default_walkable_height{2.0};
  static constexpr float default_walkable_climb{0.8};
  static constexpr float default_walkable_radius{0.6};
  static constexpr bool default_build_bv_tree{false};
  static constexpr bool default_logs{false};

  static void _register_methods()
  {
    // TODO: make range hints

    godot::register_property<DetourNavigationMeshConfig, float>(
        "cell/size", &DetourNavigationMeshConfig::cell_size, default_cell_size);
    godot::register_property<DetourNavigationMeshConfig, float>(
        "cell/height", &DetourNavigationMeshConfig::cell_height, default_cell_height);

    godot::register_property<DetourNavigationMeshConfig, float>(
        "agent/radius", &DetourNavigationMeshConfig::walkable_radius, default_walkable_radius);
    godot::register_property<DetourNavigationMeshConfig, float>(
        "agent/height", &DetourNavigationMeshConfig::walkable_height, default_walkable_height);
    godot::register_property<DetourNavigationMeshConfig, float>(
        "agent/max_climb", &DetourNavigationMeshConfig::walkable_climb, default_walkable_climb);
    godot::register_property<DetourNavigationMeshConfig, bool>(
        "bv_tree/enabled", &DetourNavigationMeshConfig::build_bv_tree, default_build_bv_tree);
    godot::register_property<DetourNavigationMeshConfig, bool>(
        "debug/logs", &DetourNavigationMeshConfig::logs, default_logs);
  }

 public:
  float cell_size{default_cell_size};
  float cell_height{default_cell_height};
  float walkable_height{default_walkable_height};
  float walkable_climb{default_walkable_climb};
  float walkable_radius{default_walkable_radius};
  bool build_bv_tree{default_build_bv_tree};
  bool logs{default_logs};
};
