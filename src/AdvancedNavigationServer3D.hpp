#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Node.hpp>

#include "DetourNavigationMesh.hpp"
#include "InputGeometry.hpp"
#include "RecastPolygonMesh.hpp"

using namespace godot;

class AdvancedNavigationServer3D : public Node
{
  GODOT_CLASS(AdvancedNavigationServer3D, Node);

 public:
  void _init() {}

  Ref<RecastPolygonMesh> build_polygon_mesh(Ref<InputGeometry>, Ref<RecastPolygonMeshConfig>);
  Ref<DetourNavigationMesh> build_navigation_mesh(
      Ref<RecastPolygonMesh>,
      Ref<DetourNavigationMeshConfig>);

  static void _register_methods()
  {
    // TODO:
    ;
  }
};
