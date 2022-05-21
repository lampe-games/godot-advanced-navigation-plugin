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

  enum PolygonFlags
  {
    POLYGON_SEARCHABLE = 1,
  };

  static constexpr float DEFAULT_SERACH_BOX_EXTENTS = 100.0;
  static constexpr int MAX_POLYGONS_IN_PATH = 128;

 public:
  void _init();

  bool build_from_input_geometry(
      godot::Ref<InputGeometry>,
      godot::Ref<RecastPolygonMeshConfig>,
      godot::Ref<DetourNavigationMeshConfig>);
  bool build_from_polygon_mesh(
      godot::Ref<RecastPolygonMesh>,
      godot::Ref<DetourNavigationMeshConfig>);

  godot::Ref<godot::Mesh> get_detailed_mesh();

  godot::Vector3 get_closest_point(godot::Vector3 to_point) const;
  godot::Vector3 get_closest_point_with_extents(
      godot::Vector3 to_point,
      godot::Vector3 search_box_half_extents) const;
  godot::PoolVector3Array get_simple_path(godot::Vector3 begin, godot::Vector3 end) const;
  godot::PoolVector3Array get_simple_path_with_extents(
      godot::Vector3 begin,
      godot::Vector3 end,
      godot::Vector3 search_box_half_extents) const;

  void deserialize_detour_nav_mesh(godot::PoolByteArray);
  godot::PoolByteArray serialize_detour_nav_mesh() const;

 private:
  static std::string detour_status_to_string(dtStatus);

 private:
  std::unique_ptr<Detour::NavMesh> detour_nav_mesh{nullptr};
  std::unique_ptr<Detour::NavMeshQuery> detour_nav_mesh_query{nullptr};
  dtQueryFilter filter;
};
