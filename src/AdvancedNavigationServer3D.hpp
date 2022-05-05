#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Node.hpp>

using namespace godot;

class AdvancedNavigationServer3D : public Node
{
  GODOT_CLASS(AdvancedNavigationServer3D, Node);

 public:
  void _init(); // `_init` must exist as it is called by Godot.
  void _ready();

  // TODO: should get geometry
  // TODO: should get parameters
  // TODO: should return recast navmesh resource
  Ref<Mesh> build_navigation_mesh();

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationServer3D::_ready);
  }
};
