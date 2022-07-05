#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include "AdvancedNavigationMesh3D.hpp"
#include "DetourCrowdAgent.hpp"

class AdvancedNavigationAgent3D : public godot::Node
{
  GODOT_CLASS(AdvancedNavigationAgent3D, godot::Node);

 public:
  static void _register_methods();

  void _init() {}
  void _ready();

  void set_navigation_mesh(AdvancedNavigationMesh3D*);
  void set_position(godot::Vector3);
  void set_target(godot::Vector3);

  godot::Vector3 get_position();
  godot::Vector3 get_target();

 private:
  void try_fetching_crowd();
  void try_creating_agent();

  void on_navigation_mesh_baked();
  void on_new_position(godot::Vector3);
  void on_new_velocity(godot::Vector3);

 private:
  godot::Vector3 requested_position{godot::Vector3::INF};
  godot::Vector3 requested_target{godot::Vector3::INF};
  AdvancedNavigationMesh3D* navigation_mesh_node{nullptr};
  Ref<DetourCrowd> crowd{nullptr};
  Ref<DetourCrowdAgent> agent{nullptr};
};
