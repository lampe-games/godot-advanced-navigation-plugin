#pragma once

#include <Godot.hpp>
#include <Material.hpp>
#include <MeshInstance.hpp>
#include <Reference.hpp>
#include <Spatial.hpp>
#include <SpatialMaterial.hpp>

#include "DetourCrowd.hpp"
#include "DetourNavigationMesh.hpp"
#include "RecastPolygonMesh.hpp"

using namespace godot;

class AdvancedNavigationMesh3D : public Spatial
{
  GODOT_CLASS(AdvancedNavigationMesh3D, Spatial);

 public:
  enum DebugMeshType
  {
    DEBUG_MESH_TYPE_RECAST_HEIGHTFIELD = 0,
    DEBUG_MESH_TYPE_RECAST_POLY,
    DEBUG_MESH_TYPE_RECAST_POLY_DETAIL,
    DEBUG_MESH_TYPE_DETOUR_NAVMESH,
  };

  static void _register_methods();

  void _init();
  void _ready();

  void bake();
  void bake_from_input_geometry(godot::Ref<InputGeometry>);
  void clear();

  godot::PoolVector3Array get_simple_path(godot::Vector3 begin, godot::Vector3 end, bool simplified)
      const;

  godot::Ref<DetourCrowd> get_crowd();

  // rasterization:
  static constexpr float default_cell_size{0.3};
  static constexpr float default_cell_height{0.2};
  // agent:
  static constexpr float default_walkable_slope_angle{45};
  static constexpr float default_walkable_height{2.0};
  static constexpr float default_walkable_climb{0.8};
  static constexpr float default_walkable_radius{0.6};
  // region:
  static constexpr int default_min_region_size{8};
  static constexpr int default_merge_region_size{20};
  // polygonization:
  static constexpr float default_max_edge_len{12};
  static constexpr float default_max_simplification_error{1.3};
  static constexpr int default_max_verts_per_poly{6};
  // detail mesh:
  static constexpr int default_detail_sample_dist{6};
  static constexpr int default_detail_sample_max_error{1};
  // partitioning:
  static constexpr int default_partitioning_algorithm{
      RecastPolygonMeshConfig::PARTITIONING_ALGORITHM_WATERSHED};
  // filtering:
  static constexpr bool default_filter_low_hanging_walkable_obstacles{true};
  static constexpr bool default_filter_ledge_spans{true};
  static constexpr bool default_filter_walkable_low_height_spans{true};
  // debug:
  static constexpr bool default_logs{false};
  static constexpr bool default_performance_logs{false};
  static constexpr int default_debug_mesh_type{DEBUG_MESH_TYPE_DETOUR_NAVMESH};

 private:
  void create_crowd();
  void create_debug_mesh_instance();
  void update_debug_mesh_instance(Ref<Mesh>);
  Ref<Mesh> get_debug_mesh();
  Ref<Material> get_debug_mesh_material();
  Ref<Material> create_transparent_debug_mesh_material();
  Ref<Material> create_solid_debug_mesh_material();
  Ref<RecastPolygonMeshConfig> create_recast_polygon_mesh_config();
  Ref<DetourNavigationMeshConfig> create_detour_navigation_mesh_config(
      Ref<RecastPolygonMeshConfig>);

 private:
  float cell_size{default_cell_size};
  float cell_height{default_cell_height};
  float walkable_slope_angle{default_walkable_slope_angle};
  float walkable_height{default_walkable_height};
  float walkable_climb{default_walkable_climb};
  float walkable_radius{default_walkable_radius};
  float max_edge_len{default_max_edge_len};
  float max_simplification_error{default_max_simplification_error};
  int min_region_size{default_min_region_size};
  int merge_region_size{default_merge_region_size};
  int max_verts_per_poly{default_max_verts_per_poly};
  int detail_sample_dist{default_detail_sample_dist};
  int detail_sample_max_error{default_detail_sample_max_error};
  int partitioning_algorithm{default_partitioning_algorithm};
  bool filter_low_hanging_walkable_obstacles{default_filter_low_hanging_walkable_obstacles};
  bool filter_ledge_spans{default_filter_ledge_spans};
  bool filter_walkable_low_height_spans{default_filter_walkable_low_height_spans};
  bool logs{default_logs};
  bool performance_logs{default_performance_logs};
  int debug_mesh_type{default_debug_mesh_type};

 private:
  MeshInstance* debug_mesh_instance{nullptr};
  Ref<Material> transparent_debug_mesh_material{nullptr};
  Ref<Material> solid_debug_mesh_material{nullptr};
  Ref<RecastPolygonMesh> polygon_mesh{nullptr};
  Ref<DetourNavigationMesh> navigation_mesh{nullptr};
  Ref<DetourCrowd> crowd{nullptr};
};
