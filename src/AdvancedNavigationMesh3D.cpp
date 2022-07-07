#include "AdvancedNavigationMesh3D.hpp"

#include <cmath>
#include <string>

#include <Engine.hpp>
#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>
#include <SceneTree.hpp>

void AdvancedNavigationMesh3D::_register_methods()
{
  // methods

  register_method("_ready", &AdvancedNavigationMesh3D::_ready);

  register_method("bake", &AdvancedNavigationMesh3D::bake);
  register_method("bake_from_input_geometry", &AdvancedNavigationMesh3D::bake_from_input_geometry);
  register_method("clear", &AdvancedNavigationMesh3D::clear);

  register_method("get_simple_path", &AdvancedNavigationMesh3D::get_simple_path);

  // signals

  register_signal<AdvancedNavigationMesh3D>("baked");

  // properties

  register_property<AdvancedNavigationMesh3D, int>(
      "partitioning/algorithm",
      &AdvancedNavigationMesh3D::partitioning_algorithm,
      default_partitioning_algorithm,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_ENUM,
      "Watershed,Monotone,Layers");

  register_property<AdvancedNavigationMesh3D, float>(
      "cell/size",
      &AdvancedNavigationMesh3D::cell_size,
      default_cell_size,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.05,99999.0");
  register_property<AdvancedNavigationMesh3D, float>(
      "cell/height",
      &AdvancedNavigationMesh3D::cell_height,
      default_cell_height,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.05,99999.0");

  register_property<AdvancedNavigationMesh3D, float>(
      "agent/radius",
      &AdvancedNavigationMesh3D::walkable_radius,
      default_walkable_radius,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.0,99999.0");
  register_property<AdvancedNavigationMesh3D, float>(
      "agent/height",
      &AdvancedNavigationMesh3D::walkable_height,
      default_walkable_height,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.15,99999.0");
  register_property<AdvancedNavigationMesh3D, float>(
      "agent/max_climb",
      &AdvancedNavigationMesh3D::walkable_climb,
      default_walkable_climb,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.0,99999.0");
  register_property<AdvancedNavigationMesh3D, float>(
      "agent/max_slope",
      &AdvancedNavigationMesh3D::walkable_slope_angle,
      default_walkable_slope_angle,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.0,89.9");

  register_property<AdvancedNavigationMesh3D, int>(
      "region/min_size",
      &AdvancedNavigationMesh3D::min_region_size,
      default_min_region_size,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0,99999");
  register_property<AdvancedNavigationMesh3D, int>(
      "region/merge_size",
      &AdvancedNavigationMesh3D::merge_region_size,
      default_merge_region_size,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0,99999");

  register_property<AdvancedNavigationMesh3D, float>(
      "polygon/max_edge_length",
      &AdvancedNavigationMesh3D::max_edge_len,
      default_max_edge_len,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.0,99999.0");
  register_property<AdvancedNavigationMesh3D, float>(
      "polygon/max_edge_error",
      &AdvancedNavigationMesh3D::max_simplification_error,
      default_max_simplification_error,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0.0,99999.0");
  register_property<AdvancedNavigationMesh3D, int>(
      "polygon/max_verts_per_poly",
      &AdvancedNavigationMesh3D::max_verts_per_poly,
      default_max_verts_per_poly,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "3,6");

  register_property<AdvancedNavigationMesh3D, int>(
      "detail/sample_distance",
      &AdvancedNavigationMesh3D::detail_sample_dist,
      default_detail_sample_dist,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0,99999");
  register_property<AdvancedNavigationMesh3D, int>(
      "detail/sample_max_error",
      &AdvancedNavigationMesh3D::detail_sample_max_error,
      default_detail_sample_max_error,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RANGE,
      "0,99999");

  register_property<AdvancedNavigationMesh3D, bool>(
      "filter/low_hanging_walkable_obstacles",
      &AdvancedNavigationMesh3D::filter_low_hanging_walkable_obstacles,
      default_filter_low_hanging_walkable_obstacles);
  register_property<AdvancedNavigationMesh3D, bool>(
      "filter/ledge_spans",
      &AdvancedNavigationMesh3D::filter_ledge_spans,
      default_filter_ledge_spans);
  register_property<AdvancedNavigationMesh3D, bool>(
      "filter/walkable_low_height_spans",
      &AdvancedNavigationMesh3D::filter_walkable_low_height_spans,
      default_filter_walkable_low_height_spans);

  register_property<AdvancedNavigationMesh3D, bool>(
      "debug/logs", &AdvancedNavigationMesh3D::logs, default_logs);
  register_property<AdvancedNavigationMesh3D, bool>(
      "debug/performance_logs",
      &AdvancedNavigationMesh3D::performance_logs,
      default_performance_logs);
  register_property<AdvancedNavigationMesh3D, int>(
      "debug/mesh_type",
      &AdvancedNavigationMesh3D::debug_mesh_type,
      default_debug_mesh_type,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_ENUM,
      "Recast Heightfield,Recast Poly,Recast Poly Detail,Detour Navmesh");
  register_property<AdvancedNavigationMesh3D, Ref<Material>>(
      "debug/transparent_mesh_material",
      &AdvancedNavigationMesh3D::transparent_debug_mesh_material,
      Ref<Material>(),
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RESOURCE_TYPE,
      "Material");
  register_property<AdvancedNavigationMesh3D, Ref<Material>>(
      "debug/solid_mesh_material",
      &AdvancedNavigationMesh3D::solid_debug_mesh_material,
      Ref<Material>(),
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_RESOURCE_TYPE,
      "Material");

  // storage

  register_property<AdvancedNavigationMesh3D, Ref<DetourNavigationMesh>>(
      "navigation_mesh",
      &AdvancedNavigationMesh3D::navigation_mesh,
      Ref<DetourNavigationMesh>(),
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_STORAGE,
      GODOT_PROPERTY_HINT_NONE);
}

void AdvancedNavigationMesh3D::_init()
{
  if (transparent_debug_mesh_material.is_null())
  {
    transparent_debug_mesh_material = create_transparent_debug_mesh_material();
  }
  if (solid_debug_mesh_material.is_null())
  {
    solid_debug_mesh_material = create_solid_debug_mesh_material();
  }
}

void AdvancedNavigationMesh3D::_ready()
{
  if (navigation_mesh.is_valid())
  {
    auto was_deserialized = polygon_mesh.is_null();
    if (was_deserialized)
    {
      emit_signal("baked");
    }
  }
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    create_debug_mesh_instance();
    update_debug_mesh_instance(get_debug_mesh());
  }
}

void AdvancedNavigationMesh3D::bake()
{
  Ref<InputGeometry> input_geometry = Ref<InputGeometry>(InputGeometry::_new());
  auto nodes_to_parse = get_children(); // TODO: proper discovery
  input_geometry->add_nodes(nodes_to_parse);
  bake_from_input_geometry(input_geometry);
}

void AdvancedNavigationMesh3D::bake_from_input_geometry(godot::Ref<InputGeometry> input_geometry)
{
  if (input_geometry.is_null())
  {
    ERR_PRINT("'InputGeometry' is null");
    return;
  }
  auto polygon_mesh_config = create_recast_polygon_mesh_config();
  if (polygon_mesh_config.is_null())
  {
    ERR_PRINT("Failed creating 'RecastPolygonMeshConfig'");
    return;
  }
  Ref<RecastPolygonMesh> a_polygon_mesh{RecastPolygonMesh::_new()};
  if (not a_polygon_mesh->build_from_input_geometry(input_geometry, polygon_mesh_config))
  {
    ERR_PRINT("Failed building 'RecastPolygonMesh'");
    return;
  }
  auto navigation_mesh_config = create_detour_navigation_mesh_config(polygon_mesh_config);
  if (navigation_mesh_config.is_null())
  {
    ERR_PRINT("Failed creating 'DetourNavigationMeshConfig'");
    return;
  }
  Ref<DetourNavigationMesh> a_navigation_mesh{DetourNavigationMesh::_new()};
  if (not a_navigation_mesh->build_from_polygon_mesh(a_polygon_mesh, navigation_mesh_config))
  {
    ERR_PRINT("Failed building 'DetourNavigationMesh'");
    return;
  }
  polygon_mesh = a_polygon_mesh;
  navigation_mesh = a_navigation_mesh;
  if (debug_mesh_instance != nullptr)
  {
    update_debug_mesh_instance(get_debug_mesh());
  }
  emit_signal("baked");
}

void AdvancedNavigationMesh3D::clear()
{
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(Ref<Mesh>());
    navigation_mesh = Ref<DetourNavigationMesh>();
  }
}

PoolVector3Array AdvancedNavigationMesh3D::get_simple_path(
    Vector3 begin,
    Vector3 end,
    bool simplified) const
{
  PoolVector3Array result;
  if (navigation_mesh.is_null())
  {
    ERR_PRINT("Navigation mesh is not baked yet");
    return result;
  }
  return navigation_mesh->get_simple_path(begin, end, simplified);
  return result;
}

Ref<DetourNavigationMesh> AdvancedNavigationMesh3D::get_navigation_mesh()
{
  return navigation_mesh;
}

void AdvancedNavigationMesh3D::create_debug_mesh_instance()
{
  debug_mesh_instance = MeshInstance::_new();
  debug_mesh_instance->set_name("DebugNavigationMeshInstance");
  debug_mesh_instance->set_meta("advanced_navigation:debug_mesh", true);
  debug_mesh_instance->set_material_override(get_debug_mesh_material());
  add_child(debug_mesh_instance);
}

void AdvancedNavigationMesh3D::update_debug_mesh_instance(Ref<Mesh> mesh)
{
  if (debug_mesh_instance != nullptr)
  {
    debug_mesh_instance->set_mesh(mesh);
    debug_mesh_instance->set_material_override(get_debug_mesh_material());
  }
}

Ref<Mesh> AdvancedNavigationMesh3D::get_debug_mesh()
{
  switch (debug_mesh_type)
  {
    case DEBUG_MESH_TYPE_RECAST_HEIGHTFIELD:
    case DEBUG_MESH_TYPE_RECAST_POLY:
    case DEBUG_MESH_TYPE_RECAST_POLY_DETAIL:
    {
      if (polygon_mesh.is_valid())
      {
        switch (debug_mesh_type)
        {
          case DEBUG_MESH_TYPE_RECAST_HEIGHTFIELD:
            return polygon_mesh->get_height_field_mesh();
          case DEBUG_MESH_TYPE_RECAST_POLY:
            return polygon_mesh->get_poly_mesh();
          case DEBUG_MESH_TYPE_RECAST_POLY_DETAIL:
            return polygon_mesh->get_poly_mesh_detail();
        }
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

Ref<Material> AdvancedNavigationMesh3D::get_debug_mesh_material()
{
  switch (debug_mesh_type)
  {
    case DEBUG_MESH_TYPE_RECAST_HEIGHTFIELD:
      return solid_debug_mesh_material;
    case DEBUG_MESH_TYPE_RECAST_POLY:
    case DEBUG_MESH_TYPE_RECAST_POLY_DETAIL:
    case DEBUG_MESH_TYPE_DETOUR_NAVMESH:
      return transparent_debug_mesh_material;
  }
  return nullptr;
}

Ref<Material> AdvancedNavigationMesh3D::create_transparent_debug_mesh_material()
{
  Ref<SpatialMaterial> material = SpatialMaterial::_new();
  material->set_flag(SpatialMaterial::FLAG_UNSHADED, true);
  material->set_feature(SpatialMaterial::FEATURE_TRANSPARENT, true);
  material->set_albedo(Color(1.0, 0.0, 0.0, 0.4));
  material->set_render_priority(127);
  return material;
}

Ref<Material> AdvancedNavigationMesh3D::create_solid_debug_mesh_material()
{
  Ref<SpatialMaterial> material = SpatialMaterial::_new();
  material->set_albedo(Color(1.0, 0.0, 0.0, 1.0));
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
  config->pipeline_logs = logs;
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
  config->logs = logs;

  return config;
}
