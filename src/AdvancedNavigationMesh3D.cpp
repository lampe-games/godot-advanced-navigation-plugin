#include "AdvancedNavigationMesh3D.hpp"

#include <string>

#include <Engine.hpp>
#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>
#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "AdvancedNavigationServer3D.hpp"

void AdvancedNavigationMesh3D::_init()
{
  if (config.is_null())
  {
    config = Ref<RecastPolygonMeshConfig>(RecastPolygonMeshConfig::_new());
  }
}

void AdvancedNavigationMesh3D::_ready()
{
  Godot::print("AdvancedNavigationMesh3D::_ready(): baked: {0}", detour_navigation_mesh.is_valid());
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    create_debug_mesh_instance();
    update_debug_mesh_instance(load_debug_mesh());
  }
}

void AdvancedNavigationMesh3D::bake()
{
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(create_debug_mesh());
    detour_navigation_mesh = Ref<DetourNavigationMesh>(DetourNavigationMesh::_new());
  }
}

void AdvancedNavigationMesh3D::clear()
{
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(Ref<Mesh>());
    detour_navigation_mesh = Ref<DetourNavigationMesh>();
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

Ref<Mesh> AdvancedNavigationMesh3D::create_debug_mesh()
{
  auto* server =
      get_tree()->get_root()->get_node<AdvancedNavigationServer3D>("AdvancedNavigationServer3D");
  if (server == nullptr or config.is_null())
  {
    return nullptr;
  }
  auto nodes_to_parse = get_children();
  return server->build_polygon_mesh(nodes_to_parse, config)->get_poly_mesh_detail();
}

Ref<Mesh> AdvancedNavigationMesh3D::load_debug_mesh()
{
  if (detour_navigation_mesh.is_null())
  {
    return nullptr;
  }
  return create_debug_mesh();
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
