#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class RecastPolygonMeshConfig : public godot::Resource
{
  GODOT_CLASS(RecastPolygonMeshConfig, godot::Resource);

 public:
  void _init();

  static constexpr float default_cell_size{0.3};
  static constexpr float default_cell_height{0.2};
  static constexpr float default_walkable_slope_angle{70}; // 45
  static constexpr int default_walkable_height{5}; // 10
  static constexpr int default_walkable_climb{1}; // 4
  static constexpr int default_walkable_radius{2};
  static constexpr int default_max_edge_len{40};
  static constexpr float default_max_simplification_error{1.3};
  static constexpr int default_min_region_area{64};
  static constexpr int default_merge_region_area{20 * 20};
  static constexpr int default_max_verts_per_poly{6};
  static constexpr float default_detail_sample_dist{1.8};
  static constexpr float default_detail_sample_max_error{1.0};

  static void _register_methods()
  {
    // TODO: make range hints

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
  }

 public:
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
};
