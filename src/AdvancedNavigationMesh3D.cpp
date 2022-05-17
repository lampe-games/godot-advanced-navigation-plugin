#include "AdvancedNavigationMesh3D.hpp"

#include <cmath>
#include <string>

#include <Engine.hpp>
#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>
#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "AdvancedNavigationServer3D.hpp"

void AdvancedNavigationMesh3D::_init()
{
  ;
}

void AdvancedNavigationMesh3D::_ready()
{
  Godot::print(
      "AdvancedNavigationMesh3D::_ready(): already baked: {0}", navigation_mesh.is_valid());
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    create_debug_mesh_instance();
    update_debug_mesh_instance(get_debug_mesh());
  }
}

void AdvancedNavigationMesh3D::bake()
{
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    auto* server =
        get_tree()->get_root()->get_node<AdvancedNavigationServer3D>("AdvancedNavigationServer3D");
    if (server == nullptr)
    {
      return;
    }
    auto nodes_to_parse = get_children();
    auto recast_polygon_mesh_config = create_recast_polygon_mesh_config();
    if (recast_polygon_mesh_config.is_null())
    {
      return;
    }
    auto a_polygon_mesh = server->build_polygon_mesh(nodes_to_parse, recast_polygon_mesh_config);
    if (a_polygon_mesh.is_null())
    {
      return;
    }
    polygon_mesh = a_polygon_mesh;
    auto detour_navigation_mesh_config =
        create_detour_navigation_mesh_config(recast_polygon_mesh_config);
    if (detour_navigation_mesh_config.is_null())
    {
      return;
    }
    auto a_navigation_mesh =
        server->build_navigation_mesh(polygon_mesh, detour_navigation_mesh_config);
    if (a_navigation_mesh.is_valid())
    {
      navigation_mesh = a_navigation_mesh;
      update_debug_mesh_instance(get_debug_mesh());
    }
    else
    {
      clear();
    }
  }
}

void AdvancedNavigationMesh3D::clear()
{
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(Ref<Mesh>());
    navigation_mesh = Ref<DetourNavigationMesh>();
  }
}

void AdvancedNavigationMesh3D::create_debug_mesh_instance()
{
  debug_mesh_instance = MeshInstance::_new();
  debug_mesh_instance->set_name("DebugNavigationMeshInstance");
  debug_mesh_instance->set_meta("advanced_navigation:debug_mesh", true);
  debug_mesh_instance->set_material_override(create_debug_mesh_instance_material());
  add_child(debug_mesh_instance);
}

void AdvancedNavigationMesh3D::update_debug_mesh_instance(Ref<Mesh> mesh)
{
  debug_mesh_instance->set_mesh(mesh);
}

Ref<Mesh> AdvancedNavigationMesh3D::get_debug_mesh()
{
  switch (debug_mesh_type)
  {
    case DEBUG_MESH_TYPE_RECAST_POLY:
    case DEBUG_MESH_TYPE_RECAST_POLY_DETAIL:
    {
      if (polygon_mesh.is_valid())
      {
        return debug_mesh_type == DEBUG_MESH_TYPE_RECAST_POLY
            ? polygon_mesh->get_poly_mesh()
            : polygon_mesh->get_poly_mesh_detail();
      }
      else if (navigation_mesh.is_valid())
      {
        WARN_PRINT("Cannot load debug mesh of given type - please rebake navigation");
      }
      return nullptr;
    }
    case DEBUG_MESH_TYPE_DETOUR_NAVMESH:
    {
      if (navigation_mesh.is_valid())
      {
        return navigation_mesh->get_detailed_mesh();
      }
      return nullptr;
    }
  }
  return nullptr;
}

Ref<Material> AdvancedNavigationMesh3D::create_debug_mesh_instance_material()
{
  Ref<SpatialMaterial> material = SpatialMaterial::_new();
  material->set_flag(SpatialMaterial::FLAG_UNSHADED, true);
  material->set_feature(SpatialMaterial::FEATURE_TRANSPARENT, true);
  material->set_flag(SpatialMaterial::FLAG_SRGB_VERTEX_COLOR, true);
  material->set_flag(SpatialMaterial::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
  material->set_albedo(Color(1.0, 0.0, 0.0, 0.4));
  return material;
}

Ref<RecastPolygonMeshConfig> AdvancedNavigationMesh3D::create_recast_polygon_mesh_config()
{
  Ref<RecastPolygonMeshConfig> config =
      Ref<RecastPolygonMeshConfig>(RecastPolygonMeshConfig::_new());

  config->cell_size = cell_size;
  config->cell_height = cell_height;
  config->walkable_slope_angle = walkable_slope_angle;
  config->walkable_height = static_cast<int>(std::ceil(walkable_height / cell_height)); // calc
  config->walkable_climb = static_cast<int>(std::floor(walkable_climb / cell_height)); // calc
  config->walkable_radius = static_cast<int>(std::ceil(walkable_radius / cell_size)); // calc
  config->max_edge_len = static_cast<int>(std::floor(max_edge_len / cell_size)); // calc
  config->max_simplification_error = max_simplification_error;
  config->min_region_area = min_region_size * min_region_size; // calc
  config->merge_region_area = merge_region_size * merge_region_size; // calc
  config->max_verts_per_poly = max_verts_per_poly;
  config->detail_sample_dist = static_cast<float>(detail_sample_dist) * cell_size; // calc
  config->detail_sample_dist = config->detail_sample_dist < 0.9 ? 0 : config->detail_sample_dist;
  config->detail_sample_max_error = static_cast<float>(detail_sample_max_error) * cell_height;
  config->partitioning_algorithm = partitioning_algorithm;
  config->filter_low_hanging_walkable_obstacles = filter_low_hanging_walkable_obstacles;
  config->filter_ledge_spans = filter_ledge_spans;
  config->filter_walkable_low_height_spans = filter_walkable_low_height_spans;
  config->pipeline_logs = pipeline_logs;
  config->performance_logs = performance_logs;

  // warnings issued in case of conversions:

  if (not godot::Math::is_equal_approx(
          static_cast<float>(config->walkable_height) * cell_height, walkable_height))
  {
    WARN_PRINT(
        String("'walkable_height' has been changed from {0} to {1} to meet recast requirements")
            .format(Array::make(
                walkable_height, static_cast<float>(config->walkable_height) * cell_height)));
  }
  if (not godot::Math::is_equal_approx(
          static_cast<float>(config->walkable_climb) * cell_height, walkable_climb))
  {
    WARN_PRINT(
        String("'walkable_climb' has been changed from {0} to {1} to meet recast requirements")
            .format(Array::make(
                walkable_climb, static_cast<float>(config->walkable_climb) * cell_height)));
  }
  if (not godot::Math::is_equal_approx(
          static_cast<float>(config->walkable_radius) * cell_size, walkable_radius))
  {
    WARN_PRINT(
        String("'walkable_radius' has been changed from {0} to {1} to meet recast requirements")
            .format(Array::make(
                walkable_radius, static_cast<float>(config->walkable_radius) * cell_size)));
  }
  if (not godot::Math::is_equal_approx(
          static_cast<float>(config->max_edge_len) * cell_size, max_edge_len))
  {
    WARN_PRINT(String("'max_edge_len' has been changed from {0} to {1} to meet recast requirements")
                   .format(Array::make(
                       walkable_radius, static_cast<float>(config->walkable_radius) * cell_size)));
  }
  if (godot::Math::is_equal_approx(config->detail_sample_dist, 0.0) and detail_sample_dist > 0)
  {
    WARN_PRINT(
        String("'detail_sample_dist' has been changed from {0} to {1} to meet recast requirements")
            .format(Array::make(detail_sample_dist, 0)));
  }

  // errors issued in case of limits exceeded:

  if (config->walkable_height < 3)
  {
    ERR_PRINT(
        String("walkable_height must be greater than or equal to {0}")
            .format(Array::make(static_cast<float>(config->walkable_height) * cell_height * 3)));
    return nullptr;
  }

  return config;
}

Ref<DetourNavigationMeshConfig> AdvancedNavigationMesh3D::create_detour_navigation_mesh_config(
    Ref<RecastPolygonMeshConfig> recast_polygon_mesh_config)
{
  Ref<DetourNavigationMeshConfig> config =
      Ref<DetourNavigationMeshConfig>(DetourNavigationMeshConfig::_new());

  config->cell_size = recast_polygon_mesh_config->cell_size;
  config->cell_height = recast_polygon_mesh_config->cell_height;
  config->walkable_height =
      static_cast<float>(recast_polygon_mesh_config->walkable_height) * config->cell_height;
  config->walkable_climb =
      static_cast<float>(recast_polygon_mesh_config->walkable_climb) * config->cell_height;
  config->walkable_radius =
      static_cast<float>(recast_polygon_mesh_config->walkable_radius) * config->cell_size;
  config->build_bv_tree = false;

  return config;
}
