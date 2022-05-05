#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Node.hpp>

#include "RecastPolygonMesh.hpp"

using namespace godot;

class AdvancedNavigationServer3D : public Node
{
  GODOT_CLASS(AdvancedNavigationServer3D, Node);

 public:
  void _init();
  void _ready();

  // TODO: should get geometry
  // TODO: should get parameters
  Ref<RecastPolygonMesh> build_polygon_mesh();

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationServer3D::_ready);
  }
};
