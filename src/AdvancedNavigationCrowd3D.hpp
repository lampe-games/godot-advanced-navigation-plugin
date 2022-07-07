#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include "AdvancedNavigationMesh3D.hpp"

class AdvancedNavigationCrowd3D : public godot::Node
{
  GODOT_CLASS(AdvancedNavigationCrowd3D, godot::Node);

 public:
  static void _register_methods();

  void _init() {}
  void _ready();

  void set_navigation_mesh(AdvancedNavigationMesh3D*);

  godot::Ref<DetourCrowd> get_crowd();

 private:
  void try_creating_crowd();

  void on_navigation_mesh_baked();

 private:
  AdvancedNavigationMesh3D* navigation_mesh_node{nullptr};
  Ref<DetourCrowd> crowd{nullptr};
};
