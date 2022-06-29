#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Node.hpp>

#include "DetourCrowd.hpp"
#include "DetourNavigationMesh.hpp"
#include "InputGeometry.hpp"
#include "RecastPolygonMesh.hpp"

class AdvancedNavigationServer3D : public godot::Node
{
  GODOT_CLASS(AdvancedNavigationServer3D, godot::Node);

 public:
  static void _register_methods();

  void _init() {}

  godot::Ref<RecastPolygonMesh> build_polygon_mesh(
      godot::Ref<InputGeometry>,
      godot::Ref<RecastPolygonMeshConfig>);
  godot::Ref<DetourNavigationMesh> build_navigation_mesh(
      godot::Ref<RecastPolygonMesh>,
      godot::Ref<DetourNavigationMeshConfig>);

  // TODO: rename to get_empty_* and remove below ones once not used anymore
  godot::Ref<InputGeometry> create_empty_input_geometry() const;
  godot::Ref<RecastPolygonMeshConfig> create_empty_recast_polygon_mesh_config() const;
  godot::Ref<DetourNavigationMeshConfig> create_empty_detour_navigation_mesh_config() const;
  godot::Ref<DetourNavigationMesh> create_empty_detour_navigation_mesh() const;
  godot::Ref<DetourCrowdConfig> create_empty_detour_crowd_config() const;
  godot::Ref<DetourCrowd> create_empty_detour_crowd() const;
  godot::Ref<DetourCrowdAgentConfig> create_empty_detour_crowd_agent_config() const;
};
