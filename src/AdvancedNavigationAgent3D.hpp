#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include "AdvancedNavigationMesh3D.hpp"

class AdvancedNavigationAgent3D : public godot::Node
{
  GODOT_CLASS(AdvancedNavigationAgent3D, godot::Node);

 public:
  static void _register_methods();

  void _init();
  void _ready();

  void set_navigation_mesh(AdvancedNavigationMesh3D*);

 private:
  void try_creating_agent();

  void on_navigation_mesh_baked();

 private:
  AdvancedNavigationMesh3D* navigation_mesh_node{nullptr};
};
