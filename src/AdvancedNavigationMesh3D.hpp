#pragma once

#include <Godot.hpp>
#include <Material.hpp>
#include <MeshInstance.hpp>
#include <Reference.hpp>
#include <Spatial.hpp>
#include <SpatialMaterial.hpp>

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

  void _init();
  void _ready();

  void bake();
  void clear();

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

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationMesh3D::_ready);

    register_method("bake", &AdvancedNavigationMesh3D::bake);
    register_method("clear", &AdvancedNavigationMesh3D::clear);

    // properties

    godot::register_property<AdvancedNavigationMesh3D, int>(
        "partitioning/algorithm",
        &AdvancedNavigationMesh3D::partitioning_algorithm,
        default_partitioning_algorithm,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_ENUM,
        "Watershed,Monotone,Layers");

    godot::register_property<AdvancedNavigationMesh3D, float>(
        "cell/size",
        &AdvancedNavigationMesh3D::cell_size,
        default_cell_size,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.05,99999.0");
    godot::register_property<AdvancedNavigationMesh3D, float>(
        "cell/height",
        &AdvancedNavigationMesh3D::cell_height,
        default_cell_height,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.05,99999.0");

    godot::register_property<AdvancedNavigationMesh3D, float>(
        "agent/radius",
        &AdvancedNavigationMesh3D::walkable_radius,
        default_walkable_radius,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.0,99999.0");
    godot::register_property<AdvancedNavigationMesh3D, float>(
        "agent/height",
        &AdvancedNavigationMesh3D::walkable_height,
        default_walkable_height,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.15,99999.0");
    godot::register_property<AdvancedNavigationMesh3D, float>(
        "agent/max_climb",
        &AdvancedNavigationMesh3D::walkable_climb,
        default_walkable_climb,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.0,99999.0");
    godot::register_property<AdvancedNavigationMesh3D, float>(
        "agent/max_slope",
        &AdvancedNavigationMesh3D::walkable_slope_angle,
        default_walkable_slope_angle,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.0,89.9");

    godot::register_property<AdvancedNavigationMesh3D, int>(
        "region/min_size",
        &AdvancedNavigationMesh3D::min_region_size,
        default_min_region_size,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0,99999");
    godot::register_property<AdvancedNavigationMesh3D, int>(
        "region/merge_size",
        &AdvancedNavigationMesh3D::merge_region_size,
        default_merge_region_size,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0,99999");

    godot::register_property<AdvancedNavigationMesh3D, float>(
        "polygon/max_edge_length",
        &AdvancedNavigationMesh3D::max_edge_len,
        default_max_edge_len,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.0,99999.0");
    godot::register_property<AdvancedNavigationMesh3D, float>(
        "polygon/max_edge_error",
        &AdvancedNavigationMesh3D::max_simplification_error,
        default_max_simplification_error,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0.0,99999.0");
    godot::register_property<AdvancedNavigationMesh3D, int>(
        "polygon/max_verts_per_poly",
        &AdvancedNavigationMesh3D::max_verts_per_poly,
        default_max_verts_per_poly,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "3,6");

    godot::register_property<AdvancedNavigationMesh3D, int>(
        "detail/sample_distance",
        &AdvancedNavigationMesh3D::detail_sample_dist,
        default_detail_sample_dist,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0,99999");
    godot::register_property<AdvancedNavigationMesh3D, int>(
        "detail/sample_max_error",
        &AdvancedNavigationMesh3D::detail_sample_max_error,
        default_detail_sample_max_error,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RANGE,
        "0,99999");

    godot::register_property<AdvancedNavigationMesh3D, bool>(
        "filter/low_hanging_walkable_obstacles",
        &AdvancedNavigationMesh3D::filter_low_hanging_walkable_obstacles,
        default_filter_low_hanging_walkable_obstacles);
    godot::register_property<AdvancedNavigationMesh3D, bool>(
        "filter/ledge_spans",
        &AdvancedNavigationMesh3D::filter_ledge_spans,
        default_filter_ledge_spans);
    godot::register_property<AdvancedNavigationMesh3D, bool>(
        "filter/walkable_low_height_spans",
        &AdvancedNavigationMesh3D::filter_walkable_low_height_spans,
        default_filter_walkable_low_height_spans);

    godot::register_property<AdvancedNavigationMesh3D, bool>(
        "debug/logs", &AdvancedNavigationMesh3D::logs, default_logs);
    godot::register_property<AdvancedNavigationMesh3D, bool>(
        "debug/performance_logs",
        &AdvancedNavigationMesh3D::performance_logs,
        default_performance_logs);
    godot::register_property<AdvancedNavigationMesh3D, int>(
        "debug/mesh_type",
        &AdvancedNavigationMesh3D::debug_mesh_type,
        default_debug_mesh_type,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_ENUM,
        "Recast Heightfield,Recast Poly,Recast Poly Detail,Detour Navmesh");

    // storage

    register_property<AdvancedNavigationMesh3D, Ref<DetourNavigationMesh>>(
        "navigation_mesh",
        &AdvancedNavigationMesh3D::navigation_mesh,
        Ref<DetourNavigationMesh>(),
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_STORAGE,
        GODOT_PROPERTY_HINT_NONE);
  }

 private:
  void create_debug_mesh_instance();
  void update_debug_mesh_instance(Ref<Mesh>);
  Ref<Mesh> get_debug_mesh();
  Ref<Material> create_debug_mesh_instance_material();
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
  Ref<RecastPolygonMesh> polygon_mesh{nullptr};
  Ref<DetourNavigationMesh> navigation_mesh{nullptr};
};
