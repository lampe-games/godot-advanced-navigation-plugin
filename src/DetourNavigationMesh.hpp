#pragma once

#include <memory>

#include <Godot.hpp>
#include <PoolArrays.hpp>
#include <Ref.hpp>
#include <Resource.hpp>

#include "DetourNavigationMeshConfig.hpp"
#include "DetourWrappers.hpp"
#include "RecastPolygonMesh.hpp"

class DetourNavigationMesh : public godot::Resource
{
  GODOT_CLASS(DetourNavigationMesh, godot::Resource);

 public:
  static void _register_methods();

  void _init() {}

  bool build_from_input_geometry(
      godot::Ref<InputGeometry>,
      godot::Ref<RecastPolygonMeshConfig>,
      godot::Ref<DetourNavigationMeshConfig>);
  bool build_from_polygon_mesh(
      godot::Ref<RecastPolygonMesh>,
      godot::Ref<DetourNavigationMeshConfig>);

  godot::Ref<godot::Mesh> get_detailed_mesh();

  void deserialize_detour_nav_mesh(godot::PoolByteArray);
  godot::PoolByteArray serialize_detour_nav_mesh() const;

 private:
  std::unique_ptr<Detour::NavMesh> detour_nav_mesh{nullptr};
};
