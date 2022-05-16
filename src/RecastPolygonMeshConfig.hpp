#pragma once

#include <Godot.hpp>
#include <Resource.hpp>
#include <Vector3.hpp>

class RecastPolygonMeshConfig : public godot::Resource
{
  GODOT_CLASS(RecastPolygonMeshConfig, godot::Resource);

 public:
  enum PartitioningAlgorithm
  {
    PARTITIONING_ALGORITHM_WATERSHED = 0,
    PARTITIONING_ALGORITHM_MONOTONE,
    PARTITIONING_ALGORITHM_LAYERS,
  };

  void _init();

  // rcConfig
  static constexpr float default_cell_size{0.3};
  static constexpr float default_cell_height{0.2};
  static constexpr float default_walkable_slope_angle{70};
  static constexpr int default_walkable_height{5};
  static constexpr int default_walkable_climb{1};
  static constexpr int default_walkable_radius{2};
  static constexpr int default_max_edge_len{40};
  static constexpr float default_max_simplification_error{1.3};
  static constexpr int default_min_region_area{64};
  static constexpr int default_merge_region_area{20 * 20};
  static constexpr int default_max_verts_per_poly{6};
  static constexpr float default_detail_sample_dist{1.8};
  static constexpr float default_detail_sample_max_error{1.0};
  static constexpr bool default_custom_aabb_enabled{false};
  // other
  static constexpr int default_partitioning_algorithm{PARTITIONING_ALGORITHM_WATERSHED};
  static constexpr bool default_filter_low_hanging_walkable_obstacles{true};
  static constexpr bool default_filter_ledge_spans{true};
  static constexpr bool default_filter_walkable_low_height_spans{true};
  static constexpr bool default_pipeline_logs{false};
  static constexpr bool default_performance_logs{false};

  static void _register_methods()
  {
    // TODO: make range hints

    godot::register_property<RecastPolygonMeshConfig, int>(
        "partitioning/algorithm",
        &RecastPolygonMeshConfig::partitioning_algorithm,
        default_partitioning_algorithm,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_ENUM,
        "Watershed,Monotone,Layers");

    godot::register_property<RecastPolygonMeshConfig, float>(
        "cell/size", &RecastPolygonMeshConfig::cell_size, default_cell_size);
    godot::register_property<RecastPolygonMeshConfig, float>(
        "cell/height", &RecastPolygonMeshConfig::cell_height, default_cell_height);

    godot::register_property<RecastPolygonMeshConfig, int>(
        "agent/radius", &RecastPolygonMeshConfig::walkable_radius, default_walkable_radius);
    godot::register_property<RecastPolygonMeshConfig, int>(
        "agent/height", &RecastPolygonMeshConfig::walkable_height, default_walkable_height);
    godot::register_property<RecastPolygonMeshConfig, int>(
        "agent/max_climb", &RecastPolygonMeshConfig::walkable_climb, default_walkable_climb);
    godot::register_property<RecastPolygonMeshConfig, float>(
        "agent/max_slope",
        &RecastPolygonMeshConfig::walkable_slope_angle,
        default_walkable_slope_angle);

    godot::register_property<RecastPolygonMeshConfig, int>(
        "edge/max_legth", &RecastPolygonMeshConfig::max_edge_len, default_max_edge_len);
    godot::register_property<RecastPolygonMeshConfig, float>(
        "edge/max_error",
        &RecastPolygonMeshConfig::max_simplification_error,
        default_max_simplification_error);

    godot::register_property<RecastPolygonMeshConfig, int>(
        "region/min_area", &RecastPolygonMeshConfig::min_region_area, default_min_region_area);
    godot::register_property<RecastPolygonMeshConfig, int>(
        "region/merge_area",
        &RecastPolygonMeshConfig::merge_region_area,
        default_merge_region_area);

    godot::register_property<RecastPolygonMeshConfig, int>(
        "polygon/max_verts_per_poly",
        &RecastPolygonMeshConfig::max_verts_per_poly,
        default_max_verts_per_poly);

    godot::register_property<RecastPolygonMeshConfig, float>(
        "detail/sample_distance",
        &RecastPolygonMeshConfig::detail_sample_dist,
        default_detail_sample_dist);
    godot::register_property<RecastPolygonMeshConfig, float>(
        "detail/sample_max_error",
        &RecastPolygonMeshConfig::detail_sample_max_error,
        default_detail_sample_max_error);

    godot::register_property<RecastPolygonMeshConfig, bool>(
        "filter/low_hanging_walkable_obstacles",
        &RecastPolygonMeshConfig::filter_low_hanging_walkable_obstacles,
        default_filter_low_hanging_walkable_obstacles);
    godot::register_property<RecastPolygonMeshConfig, bool>(
        "filter/ledge_spans",
        &RecastPolygonMeshConfig::filter_ledge_spans,
        default_filter_ledge_spans);
    godot::register_property<RecastPolygonMeshConfig, bool>(
        "filter/walkable_low_height_spans",
        &RecastPolygonMeshConfig::filter_walkable_low_height_spans,
        default_filter_walkable_low_height_spans);

    godot::register_property<RecastPolygonMeshConfig, bool>(
        "custom_aabb_enabled/enabled",
        &RecastPolygonMeshConfig::custom_aabb_enabled,
        default_custom_aabb_enabled);
    godot::register_property<RecastPolygonMeshConfig, godot::Vector3>(
        "custom_aabb_enabled/bmin", &RecastPolygonMeshConfig::bmin, godot::Vector3(-10, -10, -10));
    godot::register_property<RecastPolygonMeshConfig, godot::Vector3>(
        "custom_aabb_enabled/bmax", &RecastPolygonMeshConfig::bmax, godot::Vector3(10, 10, 10));

    godot::register_property<RecastPolygonMeshConfig, bool>(
        "debug/pipeline_logs", &RecastPolygonMeshConfig::pipeline_logs, default_pipeline_logs);
    godot::register_property<RecastPolygonMeshConfig, bool>(
        "debug/performance_logs",
        &RecastPolygonMeshConfig::performance_logs,
        default_performance_logs);
  }

 public:
  // rcConfig
  float cell_size{default_cell_size}; // [Limit: > 0] [Units: wu]
  float cell_height{default_cell_height}; // [Limit: > 0] [Units: wu]
  float walkable_slope_angle{
      default_walkable_slope_angle}; // [Limits: 0 <= value < 90] [Units: Degrees]
  int walkable_height{default_walkable_height}; // [Limit: >= 3] [Units: vx]
  int walkable_climb{default_walkable_climb}; // [Limit: >=0] [Units: vx]
  int walkable_radius{default_walkable_radius}; // [Limit: >=0] [Units: vx]
  int max_edge_len{default_max_edge_len}; // [Limit: >=0] [Units: vx]
  float max_simplification_error{default_max_simplification_error}; // [Limit: >=0] [Units: vx]
  int min_region_area{default_min_region_area}; // [Limit: >=0] [Units: vx]
  int merge_region_area{default_merge_region_area}; // [Limit: >=0] [Units: vx]
  int max_verts_per_poly{default_max_verts_per_poly}; // [Limit: >= 3]
  float detail_sample_dist{default_detail_sample_dist}; // [Limits: 0 or >= 0.9] [Units: wu]
  float detail_sample_max_error{default_detail_sample_max_error}; // [Limit: >=0] [Units: wu]
  bool custom_aabb_enabled{default_custom_aabb_enabled};
  godot::Vector3 bmin;
  godot::Vector3 bmax;
  // other
  int partitioning_algorithm{default_partitioning_algorithm};
  bool filter_low_hanging_walkable_obstacles{default_filter_low_hanging_walkable_obstacles};
  bool filter_ledge_spans{default_filter_ledge_spans};
  bool filter_walkable_low_height_spans{default_filter_walkable_low_height_spans};
  bool pipeline_logs{default_pipeline_logs};
  bool performance_logs{default_performance_logs};
};
