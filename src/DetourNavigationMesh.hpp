#pragma once

#include <memory>

#include <Godot.hpp>
#include <Ref.hpp>
#include <Resource.hpp>

#include "DetourWrappers.hpp"
#include "RecastPolygonMesh.hpp"

class DetourNavigationMesh : public godot::Resource
{
  GODOT_CLASS(DetourNavigationMesh, godot::Resource);

 public:
  void _init() {}

  // TODO: config
  bool build_from_polygon_mesh(godot::Ref<RecastPolygonMesh>);

  godot::Ref<godot::Mesh> get_detailed_mesh();

  static void _register_methods()
  {
    godot::register_property<DetourNavigationMesh, bool>(
        "dummy",
        &DetourNavigationMesh::dummy,
        false,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_STORAGE,
        GODOT_PROPERTY_HINT_NONE);
  }

 private:
  bool dummy{false};
  std::unique_ptr<Detour::NavMesh> detour_nav_mesh{nullptr};
};
