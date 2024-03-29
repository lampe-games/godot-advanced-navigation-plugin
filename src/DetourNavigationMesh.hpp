#pragma once

#include <memory>

#include <Godot.hpp>
#include <PoolArrays.hpp>
#include <Ref.hpp>
#include <Resource.hpp>

#include "DetourCrowdConfig.hpp"
#include "DetourNavigationMeshConfig.hpp"
#include "DetourWrappers.hpp"
#include "RecastPolygonMesh.hpp"

class DetourCrowd;

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
  static constexpr int MAX_POLYGONS_IN_PATH = 256; // 256 is an UB in Crowd module

 public:
  void _init();

  bool build_from_input_geometry(
      godot::Ref<InputGeometry>,
      godot::Ref<RecastPolygonMeshConfig>,
      godot::Ref<DetourNavigationMeshConfig>);
  bool build_from_polygon_mesh(
      godot::Ref<RecastPolygonMesh>,
      godot::Ref<DetourNavigationMeshConfig>);

  godot::Ref<DetourCrowd> create_crowd(godot::Ref<DetourCrowdConfig>) const;

  godot::Ref<godot::Mesh> get_detailed_mesh();

  godot::Vector3 get_closest_point(godot::Vector3 to_point) const;
  godot::Vector3 get_closest_point_with_extents(
      godot::Vector3 to_point,
      godot::Vector3 search_box_half_extents) const;
  std::tuple<godot::Vector3, dtPolyRef> get_closest_point_and_poly_with_extents_quiet(
      godot::Vector3 to_point,
      godot::Vector3 search_box_half_extents) const;
  godot::PoolVector3Array get_simple_path(godot::Vector3 begin, godot::Vector3 end, bool simplified)
      const;
  godot::PoolVector3Array get_simple_path_with_extents(
      godot::Vector3 begin,
      godot::Vector3 end,
      bool simplified,
      godot::Vector3 search_box_half_extents) const;
  godot::PoolVector3Array get_detailed_path(godot::Vector3 begin, godot::Vector3 end) const;

  std::shared_ptr<detour::NavMesh> get_detour_nav_mesh();

  void deserialize_detour_nav_mesh(godot::PoolByteArray);
  godot::PoolByteArray serialize_detour_nav_mesh() const;

 private:
  static std::string detour_status_to_string(dtStatus);

 private:
  std::shared_ptr<detour::NavMesh> detour_nav_mesh{nullptr};
  std::unique_ptr<detour::NavMeshQuery> detour_nav_mesh_query{nullptr};
  dtQueryFilter filter;
};
