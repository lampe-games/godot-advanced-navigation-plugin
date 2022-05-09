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

  Ref<RecastPolygonMesh> build_polygon_mesh(
      Array& nodes_to_parse_geometry_from,
      Ref<RecastPolygonMeshConfig>);

  Array get_triangles_from_geometry(Array& nodes_to_parse_geometry_from);

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationServer3D::_ready);
  }
};
