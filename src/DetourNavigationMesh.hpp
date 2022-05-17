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
  void _init() {}

  bool build_from_polygon_mesh(
      godot::Ref<RecastPolygonMesh>,
      godot::Ref<DetourNavigationMeshConfig>);

  godot::Ref<godot::Mesh> get_detailed_mesh();

  void deserialize_detour_nav_mesh(godot::PoolByteArray);
  godot::PoolByteArray serialize_detour_nav_mesh() const;

  static void _register_methods()
  {
    godot::register_property<DetourNavigationMesh, godot::PoolByteArray>(
        "serialized_detour_navigation_mesh",
        &DetourNavigationMesh::deserialize_detour_nav_mesh,
        &DetourNavigationMesh::serialize_detour_nav_mesh,
        godot::PoolByteArray(),
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_STORAGE,
        GODOT_PROPERTY_HINT_NONE);
  }

 private:
  std::unique_ptr<Detour::NavMesh> detour_nav_mesh{nullptr};
};
