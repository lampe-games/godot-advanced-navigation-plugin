#pragma once

#include <set>

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

  void _physics_process(float delta);

  void register_detour_crowd(godot::Ref<DetourCrowd>);
  void deregister_detour_crowd(godot::Ref<DetourCrowd>);

  // TODO: rename to get_empty_* and remove below ones once not used anymore
  godot::Ref<InputGeometry> create_empty_input_geometry() const;
  godot::Ref<RecastPolygonMeshConfig> create_empty_recast_polygon_mesh_config() const;
  godot::Ref<DetourNavigationMeshConfig> create_empty_detour_navigation_mesh_config() const;
  godot::Ref<DetourNavigationMesh> create_empty_detour_navigation_mesh() const;
  godot::Ref<DetourCrowdConfig> create_empty_detour_crowd_config() const;
  godot::Ref<DetourCrowd> create_empty_detour_crowd() const;
  godot::Ref<DetourCrowdAgentConfig> create_empty_detour_crowd_agent_config() const;

 private:
  std::set<godot::Ref<DetourCrowd>> crowds_to_update{};
};
