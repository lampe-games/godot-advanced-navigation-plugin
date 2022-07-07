#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include "AdvancedNavigationMesh3D.hpp"

class AdvancedNavigationCrowd3D : public godot::Node
{
  GODOT_CLASS(AdvancedNavigationCrowd3D, godot::Node);

 public:
  static constexpr float default_max_agent_radius{0.6};
  static constexpr int default_max_agents{25};

 public:
  static void _register_methods();

  void _init() {}
  void _ready();

  void set_navigation_mesh(AdvancedNavigationMesh3D*);

  godot::Ref<DetourCrowd> get_crowd();

 private:
  void try_creating_crowd();
  godot::Ref<DetourCrowdConfig> create_detour_crowd_config();

  void on_navigation_mesh_baked();

 private:
  float max_agent_radius{default_max_agent_radius}; // >0
  int max_agents{default_max_agents}; // >=1

 private:
  AdvancedNavigationMesh3D* navigation_mesh_node{nullptr};
  Ref<DetourCrowd> crowd{nullptr};
};
