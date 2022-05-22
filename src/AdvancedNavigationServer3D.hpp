#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Node.hpp>

#include "DetourCrowd.hpp"
#include "DetourNavigationMesh.hpp"
#include "InputGeometry.hpp"
#include "RecastPolygonMesh.hpp"

using namespace godot;

class AdvancedNavigationServer3D : public Node
{
  GODOT_CLASS(AdvancedNavigationServer3D, Node);

 public:
  static void _register_methods();

  void _init() {}

  Ref<RecastPolygonMesh> build_polygon_mesh(Ref<InputGeometry>, Ref<RecastPolygonMeshConfig>);
  Ref<DetourNavigationMesh> build_navigation_mesh(
      Ref<RecastPolygonMesh>,
      Ref<DetourNavigationMeshConfig>);

  Ref<InputGeometry> create_empty_input_geometry() const;
  Ref<RecastPolygonMeshConfig> create_empty_recast_polygon_mesh_config() const;
  Ref<DetourNavigationMeshConfig> create_empty_detour_navigation_mesh_config() const;
  Ref<DetourNavigationMesh> create_empty_detour_navigation_mesh() const;
  Ref<DetourCrowd> create_empty_detour_crowd() const;
};
